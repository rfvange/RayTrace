/* ***********************************************************************
   *									 *
   *		        Video Graphics Array Driver                      *
   *									 *
   *		 Original Material by Christopher D. Watkins		 *
   *									 *
   *			     'C' conversion by				 *
   *                            Larry Sharp				 *
   *									 *
   ***********************************************************************



   Plot		- place pixel to screen
   ClearPallete	- clear palette register
   SetPalette	- set palette register
   InitPalette1	- 64 levels of grey, red, green and blue
   InitPalette2	- 7 colors with 35 intensities each - use with Pixel
   CyclePalette - cycle through palette
   Circle	- circle draw routine
   Draw		- line draw routine
   InitGraphics - initialize graphics
   WaitForKey	- wait for key press
   ExitGraphics - sound and wait for keypress before exiting graphics
   Title	- set up text screen colors
*/

#pragma once

#include "bob_math.hpp"
#include <vector>

#define MaxXres 320
#define MaxYres 200
#define MaxX (MaxXres-1)
#define MaxY (MaxYres-1)

void PreCalc();

void Swap(int *first, int *second);

void WaitForKey();

/* **********************************************************************
   *                                                                    *
   *             Three Dimensional Plotting Routines                    *
   *								        *
   **********************************************************************

   InitPlotting       - rotation and tilt angles
   InitPerspective    - observer location and distances
   MapCoordinates     - maps 3D space onto the 2D screen
   CartesianPlot      - plot a cartesian system point
   CylindricalPlot3D  - plot a cylindrical system point
   SphericalPlot3D    - plot a spherical system point
   DrawLine3D	      - plots a line from 3D coordinates
*/


void InitPlotting(int Ang, int Tlt);

void TestPlot();

void Plot(int x, int y, Byte color);

void Circle(Word x, Word y, Word radius, Byte color);

void Draw(int xx1, int yy1, int xx2, int yy2, Byte color);

void InitPerspective(Boolean Perspective, float x, float y, float z, float m);

void MapCoordinates(float X, float Y, float Z, int *Xp, int *Yp);

void CartesianPlot3D(float X, float Y, float Z, Byte Color);

void CylindricalPlot3D(float Rho, float Theta, float Z, Byte Color);

void SphericalPlot3D(float R, float Theta, float Phi, Byte Color);

void DrawLine3D(TDA Pnt1, TDA Pnt2, Byte Color);

/* ***********************************************************************
   *									 *
   *                  	         Pixel					 *
   *									 *
   ***********************************************************************

   PutPixel - plots pixel
   GetPixel - gets pixel

   Color 1 - Blue
	 2 - Green
	 3 - Cyan
	 4 - Red
	 5 - Magenta
	 6 - Brown/Yellow
	 7 - Gray Scale

   Intensity levels (0..35) for each color
*/

#define MaxCol   7
#define MaxInten 35

/* ***********************************************************************
   *									 *
   *             Setup of Coordinate Axes and Color Palette		 *
   *									 *
   ***********************************************************************

   PutAxisAndPalette - toggle for Axis and Palette
   AxisAndPalette    - places Axis and Color Palette on screen
*/

void DisplayAxis();
