/****************************************************************************************************
*	SetupView.cpp																					*
*																									*
*	Version:	1.2																					*
*	Author: 	David Enderson																		*
*	E-mail:		neelix@festing.org																	*
*	Webpage:	http://neelix.festing.org															*
*	This source code is released under the GPL														*
*---------------------------------------------------------------------------------------------------*
* See the rest of the comments in Space.cpp															*
*																									*
* My biggest thanks goes to Stefan Arentz (stefan.arentz@soze.com).  I couldn't figure out how to	*
* get my slider bars to work correctly, so I used his SetupView.cpp and SetupView.h files and then	*
* changed them to fit my needs.																		*
****************************************************************************************************/

#include <stdio.h>
#include <be/support/SupportDefs.h>
#include <be/interface/StringView.h>
#include <be/interface/Slider.h>
#include <be/interface/View.h>
#include <be/interface/Font.h>
#include "SetupView.h"

//---------------------------------------------------------------------------------------------------

SetupView::SetupView(BRect frame, const char *name, int32* ioNumStars, int32* ioRadiusStars, int32* ioSpeedStars, int32* ioAngleStars)
	:	BView(frame, name, 0, B_FOLLOW_ALL),
		mNumStars(ioNumStars), mRadiusStars(ioRadiusStars), mSpeedStars(ioSpeedStars), mAngleStars(ioAngleStars)
{
	AngleSlider = NULL;
	StarSlider = NULL;
	RadiusSlider = NULL;
	SpeedSlider = NULL;	
}

//---------------------------------------------------------------------------------------------------

void SetupView::AttachedToWindow()
{
	// Use the same background color as the screensaver does
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	// Adds text to appear in the screensaver window	
	this->AddChild(new BStringView(BRect(10, 0, 240, 11), NULL, "Space: version 1.2, by David Enderson"));
	this->AddChild(new BStringView(BRect(10, 12, 240, 23), NULL, "email:   neelix@festing.org  "));
	this->AddChild(new BStringView(BRect(10, 24, 240, 35), NULL, "webpage: http://neelix.festing.org"));

//	this->AddChild(new BStringView(BRect(10, 47, 240, 58), NULL, "In the making of this, I used bits of code by"));
//	this->AddChild(new BStringView(BRect(10, 60, 240, 71), NULL, "Be, Inc., Urban Lindeskog, and Stefan Arentz."));

	// Adds slider specifying angle of view
	AngleSlider = new BSlider(BRect(10,38,240,88), "anglestars", "Angle of Stars", new BMessage('angl'), 0, 627, B_TRIANGLE_THUMB);
	AngleSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	AngleSlider->SetLimitLabels("0", "359");
	AngleSlider->SetValue(*mAngleStars);
	AngleSlider->SetHashMarkCount(15);
	AngleSlider->SetTarget(this);
	this->AddChild(AngleSlider);

	// Adds slider specifying number of stars
	StarSlider = new BSlider(BRect(10,89,240,139), "numofstars", "Number of Stars", new BMessage('nums'), 1, maxstars, B_TRIANGLE_THUMB);
	StarSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	StarSlider->SetLimitLabels("1", "500");
	StarSlider->SetValue(*mNumStars);
	StarSlider->SetHashMarkCount(11);
	StarSlider->SetTarget(this);
	this->AddChild(StarSlider);

	// Adds slider specifying the radius of the stars
	// I know my "Diameter" terminology is inconsistent with the rest of the program
	// but I think it makes it less confusing for the user.
	RadiusSlider = new BSlider(BRect(10,140,240,190), "radiusofstars", "Diameter of Stars", new BMessage('diam'), 1, 18, B_TRIANGLE_THUMB);
	RadiusSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	RadiusSlider->SetLimitLabels("Tiny", "Huge");
	RadiusSlider->SetValue(*mRadiusStars);
	RadiusSlider->SetHashMarkCount(11);
	RadiusSlider->SetTarget(this);
	this->AddChild(RadiusSlider);

	// Sets how fast the stars pass you
	SpeedSlider = new BSlider(BRect(10,191,240,241), "speedofstars", "Warp Speed", new BMessage('sped'), 1, 850, B_TRIANGLE_THUMB);
	SpeedSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	SpeedSlider->SetLimitLabels("Start Pushing", "Chased by Borg");
	SpeedSlider->SetValue(*mSpeedStars);
	SpeedSlider->SetHashMarkCount(11);
	SpeedSlider->SetTarget(this);
	this->AddChild(SpeedSlider);
}

//---------------------------------------------------------------------------------------------------

void SetupView::MessageReceived(BMessage *msg)
{	
	switch(msg->what)
	{
		case 'angl':
			*mAngleStars = AngleSlider->Value();
			break;
		case 'nums':
			*mNumStars = StarSlider->Value();
			// display the actual number in text here someday
			break;
		case 'diam':
			*mRadiusStars = RadiusSlider->Value();
			break;
		case 'sped':
			*mSpeedStars = SpeedSlider->Value();
			break;
		default :
			BView::MessageReceived(msg);
			break;
	}
}