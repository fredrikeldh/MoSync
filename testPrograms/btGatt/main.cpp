
#include <MAUtil/Moblet.h>
#include <MAUtil/BluetoothDiscovery.h>
#include <MAUtil/Vector.h>
#include <wchar.h>
#include <conprint.h>
#include <mavsprintf.h>
#include <mastring.h>
#include <MAUtil/String.h>
#include <ma.h>
#include <maheap.h>

using namespace MAUtil;

#define TEST_LTZ(call) { int _res = (call); if(_res < 0) { printf("@%i: %i (%s)\n", __LINE__, _res, #call); return; } }
#define TEST_ONE(call) { int _res = (call); if(_res != 1) { printf("@%i: %i (%s)\n", __LINE__, _res, #call); return; } }
#define TEST_ZERO(call) { int _res = (call); if(_res != 0) { printf("@%i: %i (%s)\n", __LINE__, _res, #call); return; } }

class MyMoblet : public Moblet, BluetoothDeviceDiscoveryListener
{
public:
	MyMoblet()
	{
		mConnected = false;
#if 1
		//Calculate the font size
		MAExtent ex = maGetScrSize();
		//int screenWidth = EXTENT_X(ex);
		int screenHeight = EXTENT_Y(ex);
		int fontSize = screenHeight / 40;

		//Here we load a default font, which is used for the first collumn
		mDefaultFont = maFontLoadDefault(FONT_TYPE_MONOSPACE, 0, fontSize);

		//Check if it's implemented on the current platform
		if(-1 == mDefaultFont)
		{
			maPanic(0, "Device fonts is only available on Android, iOS and WP7.");
		}
		maFontSetCurrent(mDefaultFont);
#endif

		// no need to do this; all moblets are already custom listeners by default.
		//addCustomEventListener(this);

#if 1
		//static const MABtAddr sDeviceAddress = {{0xBC,0x6A,0x29,0xAC,0x16,0xE6}};
		static const MABtAddr sDeviceAddress = {{0xBC,0x6A,0x29,0xAB,0x39,0x69}};
#define DEVICE_ADDRESS sDeviceAddress
#endif

#ifdef DEVICE_ADDRESS
		printf("Connecting...\n");
		mDevice = maGattConnect(&DEVICE_ADDRESS);
		printf("maGattConnect: %i\n", mDevice);
		if(mDevice < 0) {
			printf("maGattConnect failed: %i\n", mDevice);
		}
#else
		printf("Starting discovery...\n");

		int res = mBtDisc.startDeviceDiscovery(this, true, MA_BTDD_LOW_ENERGY);
		if(res < 0) {
			printf("Discovery failed to start: %i\n", res);
		}
#endif
	}

	void btNewDevice(const BtDevice& dev) {
		const byte* a = dev.address.a;
		printf("Device found: %02X:%02X:%02X:%02X:%02X:%02X %s\n",
			a[0], a[1], a[2], a[3], a[4], a[5],
			dev.name.c_str());
#if 0
		int res = mBtDisc.cancel();
		if(res < 0) {
			printf("cancel failed: %i\n", res);
			return;
		}
		printf("Connecting...\n");
		mDevice = maGattConnect(&dev.address);
		printf("maGattConnect: %i\n", mDevice);
		if(mHandle < 0) {
			printf("maGattConnect failed: %i\n", mDevice);
		}
#endif
	}

	void btDeviceDiscoveryFinished(int state) {
		printf("Device Discovery Finished: %i\n", state);
	}

	struct MyGattService {
		MAGattService s;
		Vector<MAGattChar> c;
	};
	Vector<MyGattService> mServices;

	// this is the number of MAGATT_EVENT_CHAR_READ events we will expect.
	int mNamedServices;

	// this is the number of MAGATT_EVENT_CHAR_READ events we have received.
	int mCharReadCount;

	void customEvent(const MAEvent& event) {
		if(event.type != EVENT_TYPE_GATT)
			return;
		//printf("event.gatt.device: %i\n", event.gatt.device);
		MAASSERT(event.gatt.device == mDevice);
		switch(event.gatt.type) {
		case MAGATT_EVENT_CONNECT:
			printf("connect: %i %i\n", event.gatt.status, event.gatt.connected);
			if(event.gatt.connected && !mConnected) {
				TEST_ONE(maGattFetchServices(mDevice));
				//TEST_ONE(maGattFetchRssi(mDevice));
			}
			mConnected = event.gatt.connected != 0;
			break;
		case MAGATT_EVENT_RSSI:
			printf("rssi: %i\n", event.gatt.rssi);
			break;
		case MAGATT_EVENT_SERVICES:
			printf("services: %i\n", event.gatt.status);
			dumpServices();
			break;
		case MAGATT_EVENT_CHAR_READ:
			mCharReadCount++;
			printf("CharRead %i\n", event.gatt.status);
			if(mCharReadCount == mNamedServices && mNamedServices > 0) {
				dumpServiceNames();
				mNamedServices = 0;
			}
			break;
		default:
			printf("Unkown GATT event: %i\n", event.gatt.type);
		}
	}

	typedef unsigned short uint16;

	void dumpServices() {
		int sCount = maGattServiceCount(mDevice);
		TEST_LTZ(sCount);
		printf("%i services.\n", sCount);
		mServices.resize(sCount);
		mNamedServices = 0;
		mCharReadCount = 0;
		for(int i=0; i<sCount; i++) {
			MAGattService& s(mServices[i].s);
			s.device = mDevice;
			s.serviceIndex = i;
			s.inclCount = -1;
			s.charCount = -1;
			TEST_ZERO(maGattService(&s));
			const int* a = s.uuid.i;
			printf("s%2i: i%i c%i %08X%08X%08X%08X\n", i, s.inclCount, s.charCount, a[0], a[1], a[2], a[3]);
			// search service's descriptors for one where (uint16)uuid[0] == 0x2901.
			// its value should be an UTF-8 string describing the service.
			mServices[i].c.resize(s.charCount);
			for(int j=0; j<s.charCount; j++) {
				MAGattChar& c(mServices[i].c[j]);
				c.device = mDevice;
				c.serviceIndex = i;
				c.charIndex = j;
				TEST_ZERO(maGattChar(&c));
				const int* b = c.uuid.i;
				printf("c%2i: %08X%08X%08X%08X\n", j, b[0], b[1], b[2], b[3]);
				if((uint16)c.uuid.i[0] == 0x2901) {
					TEST_ONE(maGattFetchCharValue(&c));
					mNamedServices++;
				}
			}
		}
		printf("%i named services.\n", mNamedServices);
	}

	void dumpServiceNames() {
		for(int i=0; i<mServices.size(); i++) {
			MAGattService& s(mServices[i].s);
			const int* a = s.uuid.i;
			printf("%2i: %08X%08X%08X%08X\n", i, a[0], a[1], a[2], a[3]);
			MAASSERT(mServices[i].c.size() == s.charCount);
			for(int j=0; j<s.charCount; j++) {
				MAGattChar& c(mServices[i].c[j]);
				char buf[c.len+1];
				c.value = buf;
				TEST_ZERO(maGattCharValue(&c));
				buf[c.len] = 0;	// just to be safe.
				printf("%s\n", buf);
			}
		}
	}

	void keyPressEvent(int keyCode, int nativeCode)
	{
		if (MAK_BACK == keyCode || MAK_0 == keyCode)
		{
			close();
		}
	}

	void pointerMoveEvent(MAPoint2d point)
	{
	}

	void pointerPressEvent(MAPoint2d point)
	{
	}
private:
	MAHandle mDefaultFont;
	BluetoothDiscoverer mBtDisc;
	MAHandle mDevice;
	bool mConnected;
};

extern "C" int MAMain()
{
	Moblet::run(new MyMoblet());
	return 0;
}
