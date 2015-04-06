/****************************************************************************************************
*	Space.h																							*
*																									*
*	Version:	1.2																					*
*	Author: 	David Enderson																		*
*	E-mail:		neelix@festing.org																	*
*	Webpage:	http://neelix.festing.org															*
*	This source code is released under the GPL														*
*---------------------------------------------------------------------------------------------------*
* All comments regarding this program are in Space.cpp												*
****************************************************************************************************/

#include <ScreenSaver.h>
#include <time.h>
#include "SetupView.h"

//---------------------------------------------------------------------------------------------------

class Space : public BScreenSaver
{
public:
	Space(BMessage *message, image_id id);
	status_t SaveState(BMessage *into) const;
	void StartConfig(BView *view);
	status_t StartSaver(BView *v, bool preview);
	void Draw(BView *v, int32 frame);
	SetupView *configview;

	// I know this is a LOT of global variables.  In most cases I wouldn't make this many, 
	// but in this case, it saves on processor time not to have to define them and calculate
	// them EACH pass, which happens on average every 40000 to 50000 microseconds.  So please
	// bear with them--for efficiency sake.
	double radius[maxstars];	// instantaneous radius of the star in pixels, used for drawing
	double currx[maxstars];		// current x coordinate for the star
	double curry[maxstars];		// current y coordinate for the star
	double currz[maxstars];		// current z coordinate for the star

	double locx[maxstars];		// final screen coordinates for the star
	double locy[maxstars];
	double width, height;		// the width & height of the screen
	double halfheight;			// half the height of the current screen
	double halfwidth;			// half the width of the current screen

	// these times are in microseconds
	bigtime_t lasttime;			// the time it was last time a star was spawned in the draw statement
	bigtime_t currtime;			// the current time.  Needs to be reset during every function call
	bigtime_t birth[maxstars];	// time of birth for each star

	int32 usermax;				// the number of stars to display, set by user
	int32 oldusermax;			// used to check to see if the user changed usermax and if so, to restart screensaver
	int32 mainradius;			// the overall radius of the stars
	int32 oldmainradius;		// used to check to see if the user changed mainradius and if so, to restart screensaver
	int32 speedstars;			// the speed of the stars
	int32 oldspeedstars;		// used to check to see if the user changed speedstars and if so, to restart screensaver
	int32 anglestars;			// the angle of the stars flying by
	int32 oldanglestars;		// used to check to see if the user changed anglestars and if so, to restart screensaver
	float realspeed;			// the speed value actually used in calculations.  Global to save one more
								// calculation every time it runs.  I make more variables global than necessary
								// for this reason.  I think a few more bytes of memory is a fair price to pay
								// to make it a little smoother.

	double	tempx,				// transposed x coordinate of the star to be viewed
			tempz,				// transposed z coordinate of the star to be viewed
			temphyp,			// direct distance from the star to the center of the x-z coordinate plane in meters
			temphyp3,			// line of sight distance from eye to star, used for fading
			theta,				// angle of star from center of coordinate universe
								//   also used later in the program to figure the angle of view in
								//   radians that a star's radius at current distance takes up
			realangle;			// anglestars / 100.  It's the real angle of rotated view in radians
};
