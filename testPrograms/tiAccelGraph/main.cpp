#include <MAUtil/Moblet.h>
#include <madmath.h>
#include <MAUtil/BluetoothDiscovery.h>
#include <MAUtil/Vector.h>
#include <ma.h>
#include <maheap.h>
#include <mavsprintf.h>

using namespace MAUtil;

#define LOG(a, ...) maPanic(1, a)

#define TEST_LTZ(call) { int _res = (call); if(_res < 0) { LOG("@%i: %i (%s)\n", __LINE__, _res, #call); return; } }
#define TEST_ONE(call) { int _res = (call); if(_res != 1) { LOG("@%i: %i (%s)\n", __LINE__, _res, #call); return; } }
#define TEST_ZERO(call) { int _res = (call); if(_res != 0) { LOG("@%i: %i (%s)\n", __LINE__, _res, #call); return; } }

static const MAUUID sAccelDataUUID = {{0xF000AA11, 0x04514000, 0xB0000000, 0x00000000}};
static const MAUUID sAccelConfUUID = {{0xF000AA12, 0x04514000, 0xB0000000, 0x00000000}};
static const MAUUID sAccelPeriodUUID = {{0xF000AA13, 0x04514000, 0xB0000000, 0x00000000}};

static const MAUUID sClientCharConfigUUID = {{0x00002902, 0x00001000, 0x80000080, 0x5F9B34FB}};

/**
 * A Moblet is a high-level class that defines the
 * behaviour of a MoSync program.
 */
class MyMoblet: public Moblet, public BluetoothDeviceDiscoveryListener {
private:

	BluetoothDiscoverer mBtDisc;
	MAHandle mDevice;
	bool mConnected;

public:
	void clearTop() {
		maSetColor(0);
		maFillRect(0,0,300,50);
	}

	void drawText(const char* text) {
		clearTop();
		maSetColor(0xffffffff);
		maDrawText(0,0, text);
		maUpdateScreen();
	}

	MyMoblet() {
#if 1
		//Calculate the font size
		MAExtent ex = maGetScrSize();
		//int screenWidth = EXTENT_X(ex);
		int screenHeight = EXTENT_Y(ex);
		int fontSize = screenHeight / 40;

		//Here we load a default font, which is used for the first collumn
		MAHandle mDefaultFont = maFontLoadDefault(FONT_TYPE_MONOSPACE, 0, fontSize);

		//Check if it's implemented on the current platform
		if(-1 == mDefaultFont)
		{
			maPanic(0, "Device fonts is only available on Android, iOS and WP7.");
		}
		maFontSetCurrent(mDefaultFont);
#endif
		drawText("Discovering...");
#if 1
		//static const MABtAddr sDeviceAddress = {{0xBC,0x6A,0x29,0xAC,0x16,0xE6}};
		static const MABtAddr sDeviceAddress = {{0xBC,0x6A,0x29,0xAB,0x39,0x69}};
#define DEVICE_ADDRESS sDeviceAddress
#endif

#ifdef DEVICE_ADDRESS
		mDevice = maGattConnect(&DEVICE_ADDRESS);
		if(mDevice < 0) {
			maPanic(1, "maGattConnect failed");
		}
		drawText("Connecting...");
#else
		int res = mBtDisc.startDeviceDiscovery(this, true, MA_BTDD_LOW_ENERGY);
		if(res < 0) {
			maPanic(1, "Discovery failed to start!");
		}
#endif
	}

	void btNewDevice(const BtDevice& dev) {
		if(mDevice != 0)
			return;
		int res = mBtDisc.cancel();
		if(res < 0) {
			maPanic(1, "cancel failed");
		}
		//printf("Connecting...\n");
		mDevice = maGattConnect(&dev.address);
		//printf("maGattConnect: %i\n", mDevice);
		if(mDevice < 0) {
			maPanic(1, "maGattConnect failed");
		}
		drawText("Connecting...");
	}

	void btDeviceDiscoveryFinished(int state) {
		//printf("Device Discovery Finished: %i\n", state);
	}

	struct MyGattChar {
		MAGattChar c;
		Vector<MAGattDesc> d;
	};

	struct MyGattService {
		MAGattService s;
		Vector<MyGattChar> c;
	};
	Vector<MyGattService> mServices;

	// this is the number of MAGATT_EVENT_DESC_READ events we will expect.
	int mNamedChars;

	// this is the number of MAGATT_EVENT_DESC_READ events we have received.
	int mDescReadCount;

	int mCharChangedCount;
	int mCharWriteCount;

	MyGattChar* mAccelDataChar;
	MyGattChar* mAccelConfChar;
	MyGattChar* mAccelPeriodChar;
	MAGattDesc* mAccelNotificationDesc;

	void customEvent(const MAEvent& event) {
		if(event.type != EVENT_TYPE_GATT)
			return;
		//printf("event.gatt.device: %i\n", event.gatt.device);
		MAASSERT(event.gatt.device == mDevice);
		switch(event.gatt.type) {
		case MAGATT_EVENT_CONNECT:
			//printf("connect: %i %i\n", event.gatt.status, event.gatt.connected);
			if(event.gatt.connected && !mConnected) {
				TEST_ONE(maGattFetchServices(mDevice));
				//TEST_ONE(maGattFetchRssi(mDevice));
			}
			mConnected = event.gatt.connected != 0;
			mAccelDataChar = mAccelConfChar = mAccelPeriodChar = NULL;
			mCharChangedCount = 0;
			mCharWriteCount = 0;
			drawText("Connected...");
			break;
		case MAGATT_EVENT_SERVICES:
			//printf("services: %i\n", event.gatt.status);
			dumpServices();
			break;
		case MAGATT_EVENT_DESC_READ:
			{
				//MAGattDesc* d = (MAGattDesc*)event.gatt.data;
				mDescReadCount++;
				//printf("DescRead %i %i\n", event.gatt.status, d->len);
			}
			break;
		case MAGATT_EVENT_CHAR_WRITE:
			//printf("CharWrite %i\n", event.gatt.status);
			mCharWriteCount++;
			if(mCharWriteCount == 2)
			{
				// request notifications
				{
					static char noteBytes[] = {0x01,0x00};
					MAGattDesc& d(*mAccelNotificationDesc);
					d.value = noteBytes;
					d.len = sizeof(noteBytes);
					TEST_ONE(maGattDescWrite(&d));
				}
				// maybe this will work.
				TEST_ONE(maGattNotification(&mAccelDataChar->c, 1));
				//printf("dNotification requested.\n");
			}
			break;
		case MAGATT_EVENT_DESC_WRITE:
			//printf("DescWrite %i\n", event.gatt.status);
			break;
		case MAGATT_EVENT_CHAR_CHANGED:
			{
				MAGattChar* c = (MAGattChar*)event.gatt.data;
				mCharChangedCount++;
				if(c == &mAccelDataChar->c) {
					// print accelerometer data
					MAASSERT(c->len == 3);
					char v[3];
					c->value = v;
					TEST_ZERO(maGattCharValue(c));
					sensorEvent(v);
					//printf("%02x %02x %02x\n", v[0], v[1], v[2]);
				} else {
					//printf("CharChanged %i %i %i\n", event.gatt.status, c->len, mCharChangedCount);
				}
			}
			break;
		//default:
			//printf("Unkown GATT event: %i\n", event.gatt.type);
		}
	}

	void runTimerEvent() {
		TEST_ONE(maGattFetchCharValue(&mAccelDataChar->c));
	}

	typedef unsigned short uint16;

	void dumpServices() {
		int sCount = maGattServiceCount(mDevice);
		TEST_LTZ(sCount);
		//printf("%i services.\n", sCount);
		mServices.resize(sCount);
		mNamedChars = 0;
		mDescReadCount = 0;
		// do services.
		for(int i=0; i<sCount; i++) {
			MyGattService& ms(mServices[i]);
			MAGattService& s(ms.s);
			s.device = mDevice;
			s.serviceIndex = i;
			s.inclCount = -1;
			s.charCount = -1;
			TEST_ZERO(maGattService(&s));
			//const int* a = s.uuid.i;
			//printf("s%2i: i%i c%i %08X%08X%08X%08X\n", i, s.inclCount, s.charCount, a[0], a[1], a[2], a[3]);

			// do characteristics.
			ms.c.resize(s.charCount);
			for(int j=0; j<s.charCount; j++) {
				bool isAccelDataChar = false;
				MyGattChar& mc(ms.c[j]);
				MAGattChar& c(mc.c);
				c.device = mDevice;
				c.serviceIndex = i;
				c.charIndex = j;
				TEST_ZERO(maGattChar(&c));
				//const int* b = c.uuid.i;
				//printf("c%2i: d%i %08X%08X%08X%08X\n", j, c.descCount, b[0], b[1], b[2], b[3]);
				if(!memcmp(&c.uuid, &sAccelDataUUID, sizeof(MAUUID))) {
					mAccelDataChar = &mc;
					//printf("AccelDataChar found.\n");
					isAccelDataChar = true;
				}
				if(!memcmp(&c.uuid, &sAccelConfUUID, sizeof(MAUUID))) {
					mAccelConfChar = &mc;
					//printf("AccelConfChar found.\n");
				}
				if(!memcmp(&c.uuid, &sAccelPeriodUUID, sizeof(MAUUID))) {
					mAccelPeriodChar = &mc;
					//printf("AccelPeriodChar found.\n");
				}
				// do descriptors.
				mc.d.resize(c.descCount);
				for(int k=0; k<c.descCount; k++) {
					MAGattDesc& d(mc.d[k]);
					d.device = mDevice;
					d.serviceIndex = i;
					d.charIndex = j;
					d.descIndex = k;
					TEST_ZERO(maGattDesc(&d));
					//const int* e = d.uuid.i;
					//printf("d%2i: %08X%08X%08X%08X\n", k, e[0], e[1], e[2], e[3]);

					if(isAccelDataChar and !memcmp(&d.uuid, &sClientCharConfigUUID, sizeof(MAUUID))) {
						mAccelNotificationDesc = &d;
						//printf("AccelNoteDesc found.\n");
					}
				}
			}
		}
		MAASSERT(mAccelDataChar && mAccelConfChar && mAccelPeriodChar && mAccelNotificationDesc);

		startAccelerometer();
	}

	void startAccelerometer() {
		// set period
		if(1)
		{
			static char period = 10;	// in centiseconds.
			MAGattChar& c(mAccelPeriodChar->c);
			c.value = &period;
			c.len = 1;
			TEST_ONE(maGattCharWrite(&c));
			// we should get a MAGATT_EVENT_CHAR_WRITE soon.
		}
		// enable sensor
		{
			static char startCommand = 1;
			MAGattChar& c(mAccelConfChar->c);
			c.value = &startCommand;
			c.len = 1;
			TEST_ONE(maGattCharWrite(&c));
			// we should get a MAGATT_EVENT_CHAR_WRITE soon.
		}
		// write to descriptor once char writes are complete.
	}

	/**
	 * Called when a key is pressed.
	 */
	void keyPressEvent(int keyCode, int nativeCode) {
		if (MAK_BACK == keyCode || MAK_0 == keyCode) {
			// Call close to exit the application.
			close();
		}
	}

	struct SP {
		char v[3];
	};

	Vector<SP> mSP;

	void sensorEvent(char* v) {
		// add new data.
		SP sp;
		memcpy(sp.v, v, 3);
		mSP.add(sp);

		// draw background.
		MAExtent ss = maGetScrSize();
		int w = EXTENT_X(ss);
		int h = EXTENT_Y(ss);
		maSetColor(0);
		maFillRect(0,0,w,h);

		{
			static int sCount = 0;
			char buf[32];
			sCount++;
			sprintf(buf, "%i", sCount);
			maSetColor(0xffffffff);
			maDrawText(0,0, buf);
		}

		// draw graph.
		static const int colors[] = {
			0xffff0000,	// red
			0xff00ff00,	// green
			0xff0000ff,	// blue
		};
		for(int j=0; j<3; j++) {
			int y;
			maSetColor(colors[j]);
			for(int i=mSP.size()-1; i>0; i--) {
				int newY = ((((int)(signed char)mSP[i].v[j]) * h) / 256) + h/2;
				if(i < mSP.size()-1) {
					maLine(w-i*2-2, y, w-i*2, newY);
				}
				y = newY;
			}
		}
		// todo: if we have performance problems, turn the array into a circular fifo.
		while(mSP.size() > w/2) {
			mSP.remove(0);
		}
		maUpdateScreen();
	}
};

/**
 * Entry point of the program. The MAMain function
 * needs to be declared as extern "C".
 */
extern "C" int MAMain() {
	Moblet::run(new MyMoblet());
	return 0;
}
