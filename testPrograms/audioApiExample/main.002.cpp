#include <MAUtil/Moblet.h>
#include <conprint.h>
#include <maassert.h>

#include "MAHeaders.h"
using namespace MAUtil;

#define TEST(func) do { int res = (func); if(res < 0) {\
	char buf[64];\
	sprintf(buf, ":%i: %i", __LINE__, res);\
	maSetColor(0);\
	maDrawText(mCenterX/2, mCenterY/2, buf);\
	FREEZE;\
	} } while(0)

class MyMoblet : public Moblet
{
public:

	MyMoblet()
	{
		// Setup screen
		mWindowWidth = EXTENT_X(maGetScrSize());
		mWindowHeight = EXTENT_Y(maGetScrSize());
		mCenterX = mWindowWidth/2;
		mCenterY = mWindowHeight/2;

		fillScreen();
		maUpdateScreen();

		// Init sounds
		TEST(mData = maAudioDataCreateFromResource(
			NULL,
			RES_SOUND_1,
			0,
			maGetDataSize(RES_SOUND_1),
			0));
		TEST(mInstance = maAudioInstanceCreate(mData));
	}

	void fillScreen()
	{
		maSetColor(0xff0000);
		maFillRect(0, 0, mWindowWidth, mCenterY);

		maSetColor(0x00ff00);
		maFillRect(0, mCenterY, mWindowWidth, mCenterY);
	}

	void keyPressEvent(int keyCode, int nativeCode)
	{
		if (MAK_BACK == keyCode || MAK_0 == keyCode)
		{
			// Call close to exit the application.
			close();
		}
	}

	void pointerPressEvent(MAPoint2d point)
	{
		fillScreen();
		maSetColor(0x000000);

		// Play on Green, Pause on Red
		if (point.y > mCenterY)
		{
			maDrawText(mCenterX-mCenterX/2, mCenterY + mCenterY/2, ">> Playing >>");
			TEST(maAudioPlay(mInstance));
		}
		else
		{
			maDrawText(mCenterX-mCenterX/2, mCenterY/2, "|| Paused ||");
			TEST(maAudioPause(mInstance));
		}
		maUpdateScreen();
	}

	int mWindowWidth;
	int mWindowHeight;
	int mCenterX;
	int mCenterY;
	MAAudioData mData;
    MAAudioInstance mInstance;
};

extern "C" int MAMain()
{
	Moblet::run(new MyMoblet());
	return 0;
}
