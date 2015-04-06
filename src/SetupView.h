/****************************************************************************************************
*	SetupView.h																						*
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

#ifndef __SETUPVIEW_H__
#define __SETUPVIEW_H__

#include "View.h"
#include "Slider.h"

#define maxstars 501

//---------------------------------------------------------------------------------------------------

class SetupView : public BView
{
	public:
	
		SetupView(BRect frame, const char *name, int32* ioNumStars, int32* ioRadiusStars, int32* ioSpeedStars, int32* ioAngleStars);
		void AttachedToWindow();
		void MessageReceived(BMessage *msg);
		
	private:
		BSlider* AngleSlider;		// slider bar for selecting the star's angle
		BSlider* RadiusSlider;		// slider bar for selecting the star's radius
		BSlider* StarSlider;		// slider bar for selecting the number of stars 
		BSlider* SpeedSlider;		// slider bar for selecting the star's speed
		// used to pass number, radius, speed and angle from the
		//   SetupView constructor to the slider bars 
		int32* mNumStars;			// number of stars
		int32* mRadiusStars;		// radius of stars
		int32* mSpeedStars;			// speed of stars
		int32* mAngleStars;			// angle of stars
};

//---------------------------------------------------------------------------------------------------

#endif
