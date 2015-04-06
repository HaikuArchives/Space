/****************************************************************************************************
*	Space.cpp																						*
*																									*
*	Version:	1.2																					*
*	Author: 	David Enderson																		*
*	E-mail:		neelix@festing.org																	*
*	Webpage:	http://neelix.festing.org															*
*	This source code is released under the GPL														*
*---------------------------------------------------------------------------------------------------*
* I've always wanted to fly in space.  So I loved Windows 95's "Flying Through Space" on sight and	*
* used it exclusively. When I switched to the BeOS, I was disappointed no one had made such a thing *
* for Be, until it occured to me: "If nobody else will, why don't you?"  It took me a few days		*
* total of coding spread out over 2 weeks, but it's finally functional and beautiful.				*
*																									*
* On A Side Note: I sometimes wasted large quantities of time staring at the Windows screen saver,	*
* hoping a star would fly directly at me.  But I was always disappointed.  In THIS program, they	*
* will!! :) )																						*
*																									*
* Future plans for Space:																			*
*	math optimizations																				*
*	report the EXACT number of stars chosen to the viewer in preferences							*
*	other things I haven't thought of yet															*
*																									*
* As I did with my BinaryClock screensaver, I used the skeleton of Urban Lindeskog's (ubbe@rit.se)	*
* MySpots screensaver, which he got from the sample code Be Inc. included with the operating system.*
* If you installed the optional stuff, it's at: /boot/optional/sample-code/screen_savers/Spots		*
*																									*
* My biggest thanks goes to Stefan Arentz (stefan.arentz@soze.com).  I couldn't figure out how to	*
* get my slider bars to work correctly, so I used his SetupView.cpp and SetupView.h files and then	*
* changed them to fit my needs.																		*
*																									*
* If you have advice on any of this or adapt this program in any way, PLEASE email me the code.		*
* It would help me greatly!																			*
*---------------------------------------------------------------------------------------------------*
* HOW THIS WORKS:																					*
* This screen saver creates a three-dimensional cube of space.  It puts stars in that space, and	*
* moves them toward the viewer.  The size the stars appear, their speed, and their location are all *
* calculated correctly and accurately.  Which, in my humble opinion, makes this many steps ahead of *
* the Windows' 95 ScreenSaver, which was a total kludge.  I needed to use numbers as a frame of     *
* reference, so this cube of space is 33000 meters deep (currz), 24750 meters high (curry), and     *
* 30000 meters wide (currx).																		*
*																									*
* The stars' speed is calculated in meters/microsecond.  When stars leave the field of view, they	*
* are redrawn in the back of the cube at a random depth between 30000 and 33000.  This is to keep	*
* it from looking entirely the same, and to spread out the stars when they get all bunched up		*
* together.  Each time the you view a star, it is translated to temporary coordinates and viewed	*
* there.  That was the best way I came up with for viewing the angle.  If you have a better one,	*
* I'd like to see it.			 																	*
****************************************************************************************************/
#include "Space.h"
#include <StringView.h>
#include <stdlib.h>
#include <math.h>
#include <Slider.h>

//---------------------------------------------------------------------------------------------------

extern "C" _EXPORT BScreenSaver *instantiate_screen_saver(BMessage *message, image_id image)
{
	return new Space(message, image);
}

//---------------------------------------------------------------------------------------------------

Space::Space(BMessage *message, image_id image)
 : BScreenSaver(message, image)
{
	// retrieves number of stars
	if (message->FindInt32("nofstars", &usermax) != B_OK) {
		usermax = 150;
	}
	// retrieves radius of stars
	if (message->FindInt32("rofstars", &mainradius) != B_OK) {
		mainradius = 5;
	}
	// retrieves speed of stars
	if (message->FindInt32("sofstars", &speedstars) != B_OK) {
		speedstars = 50;
	}
	// retrieves angle of stars  (in radians * 100)
	if (message->FindInt32("aofstars", &anglestars) != B_OK) {
		anglestars = 0;
	}
}

//---------------------------------------------------------------------------------------------------

status_t Space::SaveState(BMessage *into) const
{
	// saves the number, radius, speed, and angle of stars
	into->AddInt32("nofstars", usermax);
	into->AddInt32("rofstars", mainradius);
	into->AddInt32("sofstars", speedstars);
	into->AddInt32("aofstars", anglestars);
	return B_OK;
}

//---------------------------------------------------------------------------------------------------

void Space::StartConfig(BView *view)
{
	configview = new SetupView(view->Bounds(), "setup", &usermax, &mainradius, &speedstars, &anglestars);
	view->AddChild(configview);
}

//---------------------------------------------------------------------------------------------------

status_t Space::StartSaver(BView *view, bool preview)
{
	// sets variables that check to see if parameters changed
	oldusermax = usermax;
	oldmainradius = mainradius;
	oldspeedstars = speedstars;
	oldanglestars = anglestars;
	
	currtime = real_time_clock_usecs();

	rgb_color maincolor = {255, 255, 255, 0};	// sets star's initial color using temporary variable
	rgb_color backcolor = {0, 0, 0, 0};			// background color for the screen. I assume it will always be black

	view->SetHighColor(maincolor);
	view->SetLowColor(backcolor);

	SetTickSize(0);		// I did 0 because that's what Be's documentation recommended 
						// as I understand it, this setting gives the screen saver lowest priority,
						// but runs it all the time when nothing else is happening

	for (int a=0; a<usermax; a++) {
		currx[a]=(random()%33000)-16500;	// in meters
		curry[a]=(random()%24750)-12375;	// in meters
		currz[a]=(random()%30000)-15000;	// in meters
		birth[a]=currtime;			
		radius[a]=0.5;
	}

	// here a bunch of figuring is done that only needs to be done once
	width = view->Bounds().IntegerWidth();
	height = view->Bounds().IntegerHeight();
	halfwidth = width/2;
	halfheight = height/2;
	realangle = (double)anglestars/100;
	realspeed=(float)speedstars/20000;		// in the program's reference zone, this is in meters/usecs
											//   that the star travels, also known as it's speed
	
	/* Fill the screen with black on the first frame: */ 
	view->SetLowColor( 0, 0, 0 ); 
	view->FillRect( view->Bounds(), B_SOLID_LOW ); 		
	view->SetViewColor(backcolor);
	view->Invalidate();

	lasttime=currtime;

	return B_OK;
}

//---------------------------------------------------------------------------------------------------

void Space::Draw(BView *view, int32 frame)
{
	// gets the system's current time in epoch microseconds
	currtime = real_time_clock_usecs();
	
	// main loop that iterates through all the stars
	for (int a=0; a<usermax; a++) {
		// erases star from screen
		// this IF statement is needed because an ellipse with a radius of <1 make a box two by two pixels 
		if (radius[a]<1)
			view->FillRect(BRect(locx[a],locy[a],locx[a],locy[a]),B_SOLID_LOW);
		else
			view->FillEllipse(BPoint(locx[a],locy[a]),radius[a],radius[a],B_SOLID_LOW);

		// subtracts distance traveled
		currz[a]=currz[a]-((currtime-birth[a])*realspeed);
		// records the system time at which the last speed calculation was made
		birth[a]=currtime;

		// checks to see if star moved out of our known universe
		if (currz[a]<-15000) {
			currx[a]=(random()%33000)-16500;	// in meters
			curry[a]=(random()%24750)-12375;	// in meters
			currz[a]=(random()%3000)+15000;		// in meters
			birth[a]=currtime;					// sets time of star birth
		}

		// transposes the stars in a 2D plane, which works since I'm only handling X and Z.
		// I got this from a book entitled: Black Art of 3D Game Programming, by Andre LaMothe
		tempx=currx[a]*cos(realangle) - currz[a]*sin(realangle);
		tempz=currx[a]*sin(realangle) + currz[a]*cos(realangle); 
		tempz+=15000;   // so I don't have to do this later in the display formula
		
		// figures temphyp3, which is the actual sight distance to a star
		temphyp3=sqrt((tempx*tempx)+(curry[a]*curry[a])+(tempz*tempz));

		// .4636/halfwidth = number of radians for 1 pixel
		theta=atan2(mainradius,temphyp3);
		// figures the display radius of the star
		radius[a]=theta/(.4636/halfwidth);

		// figures the actual screen coordinates based off the fake rotation 
		locx[a]=(1+(atan2(tempx,tempz)/.4636))*halfwidth;
		locy[a]=(1+(atan2(curry[a],tempz)/.3588))*halfheight;

		// draws star on screen
		// this IF statement is needed because an ellipse with a radius of <1 make a box two by two pixels 
		if (radius[a]<1)
			// fades the stars in from black to white over 13000 meters
			if (temphyp3>20000) {
				int temp;
				if (temphyp3<33000)
					temp = (int)((33000-temphyp3)/51);
				else
					temp = 0;
				view->SetHighColor(temp,temp,temp);
				view->FillRect(BRect(locx[a],locy[a],locx[a],locy[a]),B_SOLID_HIGH);
				view->SetHighColor(255,255,255);
			}	
			else
				view->FillRect(BRect(locx[a],locy[a],locx[a],locy[a]),B_SOLID_HIGH);
		else if (radius[a]<1.5) {
			// I'm taking an upper level COBOL class at Union College (www.ucollege.edu)
			// so I thought I'd throw in an easter egg comment here and show how my next
			// line of C++ code would look in COBOL.
			//---------------------------------------------
			// DATA DIVISION.
			//     05 temp  pic 9(3).
			//* Later, in some procedure
			// COMPUTE temp ROUNDED = 510 * (radius - 1);  // I don't know how to do arrays yet in COBOL
			int temp = (int)(510 * (radius[a] - 1));
			view->SetHighColor(temp,temp,temp);
			view->FillRect(BRect(locx[a],locy[a],locx[a]+1,locy[a]+1),B_SOLID_HIGH);
			view->SetHighColor(255,255,255);			
			view->FillRect(BRect(locx[a],locy[a],locx[a],locy[a]),B_SOLID_HIGH);
		}
		else if (radius[a]<2) {
			int temp = (int)(510 * (radius[a] - 1.5));
			view->SetHighColor(temp,temp,temp);
			view->FillRect(BRect(locx[a]-1,locy[a]-1,locx[a]+1,locy[a]+1),B_SOLID_HIGH);
			view->SetHighColor(255,255,255);			
			view->FillRect(BRect(locx[a],locy[a],locx[a]+1,locy[a]+1),B_SOLID_HIGH);
		}
		else if (radius[a]<2.5) {
			int temp = (int)(510 * (radius[a] - 2));
			view->SetHighColor(temp,temp,temp);
			view->FillEllipse(BPoint(locx[a],locy[a]),radius[a],radius[a],B_SOLID_HIGH);
			view->SetHighColor(255,255,255);			
			view->FillRect(BRect(locx[a]-1,locy[a]-1,locx[a]+1,locy[a]+1),B_SOLID_HIGH);
		}
		else
			view->FillEllipse(BPoint(locx[a],locy[a]),radius[a],radius[a],B_SOLID_HIGH);
	}
	
	// checks to see if the user changed the usermax number
	if ((oldusermax!=usermax)||(oldspeedstars!=speedstars)||(oldmainradius!=mainradius)||(oldanglestars!=anglestars))
		StartSaver(view, TRUE);	
}
