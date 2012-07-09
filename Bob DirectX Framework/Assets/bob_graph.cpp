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

#include <math.h>
#include "bob_graph.hpp"

#define MaxXres 320
#define MaxYres 200
#define MaxX (MaxXres-1)
#define MaxY (MaxYres-1)

int   CentreX, CentreY;
int   Angl, Tilt;
float CosA, SinA;
float CosB, SinB;
float CosACosB, SinASinB;
float CosASinB, SinACosB;
int XRes, YRes;

float   Mx, My, Mz, ds;
std::vector<MyCode> codeList;

Boolean PerspectivePlot;

Word PreCalcY[MaxY+1];

void Swap(int *first, int *second)
{
  int temp;

  temp=*first;
  *first=*second;
  *second=temp;
}
/*
void WaitForKey()
{
  char k;

  while(!(k=getch()));
}
*/

void Plot(int x, int y, Byte color)
{
	MyCode mycode;
	mycode.draw_funct = point;
	mycode.a = x; mycode.b = y; mycode.c = color;
	codeList.push_back(mycode);
}

void Circle(Word x, Word y, Word radius, Byte color)
{
  int a, af, b, bf, target, r2;

  target=0;
  a=radius;
  b=0;
  r2=Sqr(radius);
  while(a>=b)
  {
    b=Round(sqrt((float)(r2-Sqr(a))));
    Swap(&target,&b);
    while(b<target)
    {
      af=(120*a) / 100;
      bf=(120*b) / 100;
      Plot(x+af, y+b, color);
      Plot(x+bf, y+a, color);
      Plot(x-af, y+b, color);
      Plot(x-bf, y+a, color);
      Plot(x-af, y-b, color);
      Plot(x-bf, y-a, color);
      Plot(x+af, y-b, color);
      Plot(x+bf, y-a, color);
      ++b;
    }
    --a;
  }
}

void Draw(int xx1, int yy1, int xx2, int yy2, Byte color)
{
  int LgDelta, ShDelta, Cycle, LgStep, ShStep, dtotal;

  LgDelta=xx2-xx1;
  ShDelta=yy2-yy1;
  if(LgDelta<0)
  {
    LgDelta=-LgDelta;
    LgStep=-1;
  }
  else
    LgStep=1;
  if(ShDelta<0)
  {
    ShDelta=-ShDelta;
    ShStep=-1;
  }
  else
    ShStep=1;
  if(ShDelta<LgDelta)
  {
    Cycle=LgDelta >> 1;
    while(xx1 != xx2)
    {
      Plot(xx1, yy1, color);
      Cycle+=ShDelta;
      if(Cycle>LgDelta)
      {
	Cycle-=LgDelta;
	yy1+=ShStep;
      }
      xx1+=LgStep;
    }
    Plot(xx1, yy1, color);
  }
  else
  {
    Cycle=ShDelta >> 1;
    Swap(&LgDelta, &ShDelta);
    Swap(&LgStep, &ShStep);
    while(yy1 != yy2)
    {
      Plot(xx1, yy1, color);
      Cycle+=ShDelta;
      if(Cycle>LgDelta)
      {
	Cycle-=LgDelta;
	xx1+=ShStep;
      }
      yy1+=LgStep;
    }
    Plot(xx1, yy1, color);
  }
}


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

void InitPlotting(int Ang, int Tlt)
{
  CentreX=MaxX >> 1;
  CentreY=MaxY >> 1;
  Angl=Ang;
  Tilt=Tlt;
  CosA=CosD(Angl);
  SinA=SinD(Angl);
  CosB=CosD(Tilt);
  SinB=SinD(Tilt);
  CosACosB=CosA*CosB;
  SinASinB=SinA*SinB;
  CosASinB=CosA*SinB;
  SinACosB=SinA*CosB;
}

void TestPlot()
{
	MyCode mycode;
	mycode.a = 100; mycode.b = 200;
	mycode.draw_funct = circle;
	codeList.push_back(mycode);
}


void InitPerspective(Boolean Perspective, float x, float y, float z, float m)
{
  PerspectivePlot=Perspective;
  Mx=x;
  My=y;
  Mz=z;
  ds=m;
}

void MapCoordinates(float X, float Y, float Z, int *Xp, int *Yp)
{
  float Xt, Yt, Zt;

  Xt=(Mx+X*CosA-Y*SinA);
  Yt=(My+X*SinASinB+Y*CosASinB+Z*CosB);
  if(PerspectivePlot)
  {
    Zt=Mz+X*SinACosB+Y*CosACosB-Z*SinB;
    *Xp=CentreX+Round(ds*Xt/Zt);
    *Yp=CentreY-Round(ds*Yt/Zt);
  }
  else
  {
    *Xp=CentreX+Round(Xt);
    *Yp=CentreY-Round(Yt);
  }
}

void CartesianPlot3D(float X, float Y, float Z, Byte Color)
{
  int Xp, Yp;

  MapCoordinates(X, Y, Z, &Xp, &Yp);
  Plot(Xp, Yp, Color);
}

void CylindricalPlot3D(float Rho, float Theta, float Z, Byte Color)
{
  float X, Y;

  Theta=Radians(Theta);
  X=Rho*cos(Theta);
  Y=Rho*sin(Theta);
  CartesianPlot3D(X, Y, Z, Color);
}

void SphericalPlot3D(float R, float Theta, float Phi, Byte Color)
{
  float X, Y, Z;

  Theta=Radians(Theta);
  Phi=Radians(Phi);
  X=R*sin(Theta)*cos(Phi);
  Y=R*sin(Theta)*sin(Phi);
  Z=R*cos(Theta);
  CartesianPlot3D(X, Y, Z, Color);
}

void DrawLine3D(TDA Pnt1, TDA Pnt2, Byte Color)
{
  int   Xp1, Yp1;
  int   Xp2, Yp2;
  float x1, y1, z1;
  float x2, y2, z2;

  UnVec(Pnt1, &x1, &y1, &z1);
  UnVec(Pnt2, &x2, &y2, &z2);
  MapCoordinates(x1, y1, z1, &Xp1, &Yp1);
  MapCoordinates(x2, y2, z2, &Xp2, &Yp2);
  Draw(Xp1, Yp1, Xp2, Yp2, Color);
}

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

void DisplayAxis()
{
  int x, y, z;

  for(x=-100; x<101; x++)
  {
    CartesianPlot3D(x, 0, 0, 35);
    CartesianPlot3D(100, 0, 0, 251);
  }
  for(y=-100; y<101; y++)
  {
    CartesianPlot3D(0, y, 0, 71);
    CartesianPlot3D(0, 100, 0,251);
  }
  for(z=-100; z<101; z++)
  {
    CartesianPlot3D(0, 0, z, 107);
    CartesianPlot3D(0, 0, 100, 251);
  }
}

