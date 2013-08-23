/*
 * main.h
 *
 *  Created on: 16-08-2013
 *      Author: Adrian Swarcewicz
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <ma.h>
#include <madmath.h>
#include <matime.h>
#include <mavsprintf.h>
#include <MAUtil/Moblet.h>
#include <NativeUI/Widgets.h>
#include <NativeUI/WidgetUtil.h>

using namespace MAUtil;
using namespace NativeUI;

/**
 * Moblet to be used as a template for a Native UI application.
 */
class NativeUIMoblet: public Moblet, public ButtonListener
{
public:
	NativeUIMoblet();
	virtual ~NativeUIMoblet();
	void createUI();
	void keyPressEvent(int keyCode, int nativeCode);
	virtual void buttonClicked(Widget* button);

private:
	unsigned char* mandelbrotFractal(const int width, const int height,
			const double xMin, const double xMax, const double yMin,
			const double yMax, const int maxIteration);
	Screen* mScreen;			//A Native UI screen
	VerticalLayout* mLayout;	//A Native UI layout
	Button* mButton;			//A Native UI button
};

/**
 * Main function that is called when the program starts.
 */
extern "C" int MAMain()
{
	Moblet::run(new NativeUIMoblet());
	return 0;
}
#endif /* MAIN_H_ */
