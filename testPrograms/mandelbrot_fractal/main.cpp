/*
* main.cpp
*
*  Created on: 16-08-2013
*      Author: Adrian Swarcewicz
*/

#include <ma.h>
#include <madmath.h>
#include <mavsprintf.h>

static unsigned char* mandelbrotFractal(const int width,
	const int height, const double xMin, const double xMax,
	const double yMin, const double yMax, const int maxIteration)
{
	unsigned char color[4];
	unsigned char *image = new unsigned char[width * height * 4];

	double pixelWidth = (xMax - xMin) / width;
	double pixelHeight = (yMax - yMin) / height;

	for (int iY = 0; iY < height; iY++)
	{
		double Cy = yMin + iY * pixelHeight;
		if (fabs(Cy) < pixelHeight / 2)
		{
			Cy = 0.0;
		}
		for (int iX = 0; iX < width; iX++)
		{
			double Cx = xMin + iX * pixelWidth;
			double Zx = 0.0;
			double Zy = 0.0;
			double Zx2 = Zx * Zx;
			double Zy2 = Zy * Zy;
			double ER2 = 2 * 2;

			int iteration = 0;
			for (iteration = 0; iteration < maxIteration && ((Zx2 + Zy2) < ER2);
					iteration++)
			{
				Zy = 2 * Zx * Zy + Cy;
				Zx = Zx2 - Zy2 + Cx;
				Zx2 = Zx * Zx;
				Zy2 = Zy * Zy;
			};
			/*  inside set */
			if (iteration == maxIteration)
			{
				color[0] = 255;
				color[1] = 0;
				color[2] = 0;
				color[3] = 255;
			}
			else
			{ /* outer white part */
				color[0] = 255;
				color[1] = 255;
				color[2] = 255;
				color[3] = 255;
			};
			image[iX * 4 + 0 + iY * width * 3] = color[0];
			image[iX * 4 + 1 + iY * width * 3] = color[1];
			image[iX * 4 + 2 + iY * width * 3] = color[2];
			image[iX * 4 + 3 + iY * width * 3] = color[3];
		}
	}

	return image;
}

int MAMain()
{
	int startTime = maGetMilliSecondCount();
	unsigned char* image = mandelbrotFractal(300, 300, -2.5, 1.5, -1.5, 1.5,
			500);
	int endTime = maGetMilliSecondCount();
	lprintfln("Mandelbrot fractal calculated in %dms", endTime - startTime);
	delete image;
	return 0;
}
