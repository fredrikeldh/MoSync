/*
Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

/** \file main.cpp
*
* This example shows how to retrieve accelerometer data and
* use it in a meaningful manner (rotate 3d objects).
* The 3d rendering is in a seperate module so if you dare
* you can try and plug in your own 3d renderer and see
* how your 3d world looks from different angles using
* the accelerometer. To do this you just implement the
* Renderer interface and the two functions 'init' and
* 'render'. Look at the default renderer implementation
* called SimpleRenderer to see how it is done.
*
* \author Niklas Nummelin
*
*/

#include <MAUtil/GLMoblet.h>
#include <madmath.h>
#include "SimpleRenderer.h"
#include <MAUtil/BluetoothDiscovery.h>
#include <MAUtil/Vector.h>
#include <ma.h>
#include <maheap.h>

using namespace MAUtil;

#define LOG(a, ...) maPanic(1, a)

#define TEST_LTZ(call) { int _res = (call); if(_res < 0) { LOG("@%i: %i (%s)\n", __LINE__, _res, #call); return; } }
#define TEST_ONE(call) { int _res = (call); if(_res != 1) { LOG("@%i: %i (%s)\n", __LINE__, _res, #call); return; } }
#define TEST_ZERO(call) { int _res = (call); if(_res != 0) { LOG("@%i: %i (%s)\n", __LINE__, _res, #call); return; } }

static const MAUUID sAccelDataUUID = {{0xF000AA11, 0x04514000, 0xB0000000, 0x00000000}};
static const MAUUID sAccelConfUUID = {{0xF000AA12, 0x04514000, 0xB0000000, 0x00000000}};
static const MAUUID sAccelPeriodUUID = {{0xF000AA13, 0x04514000, 0xB0000000, 0x00000000}};

static const MAUUID sClientCharConfigUUID = {{0x00002902, 0x00001000, 0x80000080, 0x5F9B34FB}};

// A simple struct that represents
// a vector in 3-dimensional space.
struct Vector3 {
	Vector3() :
		x(0.0), y(0.0), z(0.0)
	{
	}
	Vector3(float _x, float _y, float _z) :
		x(_x), y(_y), z(_z)
	{
	}

	float x, y, z;
};

// A simple low pass filter used to
// smoothen the noisy accelerometer
// data.
struct LowPassFilter {
	LowPassFilter() :
		// this constant sets the cutoff for the filter.
		// It must be a value between 0 and 1, where
		// 0 means no filtering (everything is passed through)
		// and 1 that no signal is passed through.
		a(0.80f)
	{
		b = 1.0f - a;
	}

	Vector3 filter(const Vector3& in) {
		previousState.x = (in.x * b) + (previousState.x * a);
		previousState.y = (in.y * b) + (previousState.y * a);
		previousState.z = (in.z * b) + (previousState.z * a);
		return previousState;
	}

	float a, b;
	Vector3 previousState;
};

/**
 * A Moblet is a high-level class that defines the
 * behaviour of a MoSync program.
 */
class MyMoblet: public GLMoblet, public BluetoothDeviceDiscoveryListener {
private:
	Renderer* mRenderer;
	LowPassFilter mFilter;
	Vector3 mRotation;

	BluetoothDiscoverer mBtDisc;
	MAHandle mDevice;
	bool mConnected;

public:
	/**
	 * Initialize the application in the constructor.
	 */
	MyMoblet() :
		GLMoblet(GLMoblet::GL1) {

		int res = mBtDisc.startDeviceDiscovery(this, true, MA_BTDD_LOW_ENERGY);
		if(res < 0) {
			maPanic(1, "Discovery failed to start!");
		}
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
		MAASSERT(mAccelDataChar && mAccelConfChar && mAccelPeriodChar);

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

	void init() {
		// Make an instance of the SimpleRenderer class that just
		// renders a simple finite plane (quad).
		mRenderer = new SimpleRenderer();
		mRenderer->init(EXTENT_X(maGetScrSize()), EXTENT_Y(maGetScrSize()));
	}

	// Function that uses trigonometric math to convert accelerometer and compass
	// data into euler angles expressed as radians. This simple method won't always
	// give correct results (complex complex math may fix that for you).
	Vector3 convertAccelerometerAndCompassDataToRadians(const Vector3& a, float compass) {
		float g = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
		return Vector3(atan2(a.y, -a.z), asin(a.x / -g), compass);
	}

	// Convert euler angles from radians to degrees.
	float convertRadiansToDegrees(float radians) {
		return (radians * 360.0f / (2.0f * M_PI));
	}

	void sensorEvent(char* v) {
		float f[3];
		for(int i=0; i<3; i++) {
			f[i] = ((float)v[i]) / 64;
		}

		// Filter the accelerometer gravity vector.
		Vector3 vf = mFilter.filter(Vector3(f[0], f[1], f[2]));

		// And calculate the rotations. We don't pass the compass angle, just the
		// accelerometer gravity vector.
		mRotation = convertAccelerometerAndCompassDataToRadians(vf, 0.0f);

		// Set the rotation.
		mRenderer->setRotation(
			convertRadiansToDegrees(mRotation.x),
			convertRadiansToDegrees(mRotation.y),
			convertRadiansToDegrees(mRotation.z)
			);
	}

	void draw() {
		mRenderer->draw();
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
