/* ************************************************************************
   *									  *
   *		       	    Mathematical Functions                        *
   *									  *
   *		 Original Material by Christopher D. Watkins		  *
   *									  *
   *			     'C' conversion by				  *
   *                            Larry Sharp				  *
   *									  *
   ************************************************************************

   Radians   - converts degrees to radians
   Degrees   - converts radians to degrees
   CosD	     - cosine in degrees
   SinD	     - sine in degrees
   Power     - power a^n
   Log       - log base 10
   Exp10     - exp base 10
   Sign      - negative=-1  positive=1  null=0
   IntSign   - negative=-1  positive=1  null=0
   IntSqrt   - integer square root
   IntPower  - integer power a^n
*/

#pragma once

#define Ln10 2.30258509299405E+000
#define Pi 3.1415927
#define PiOver180 1.74532925199433E-002
#define PiUnder180 5.72957795130823E+001

typedef enum { f, t } Boolean;
typedef enum { point, line, circle } Draw_Funct;
typedef unsigned char      Byte;
typedef unsigned int       Word;

typedef struct {
	Draw_Funct draw_funct;
	int a;
	int b;
	int c;
	int d;
} MyCode;

int Round(float x);

int Trunc(float x);

float Frac(float x);

float SqrFP(float x);

int Sqr(int x);

float Radians(float Angle);

float Degrees(float Angle);

float CosD(float Angle);

float SinD(float Angle);

float Power(float Base, int Exponent);

float Log(float x);

float Exp10(float x);

float Sign(float x);

int IntSign(int x);

int IntSqrt(int x);

int IntPower(int Base, int Exponent);

/* ***********************************************************************
   *									 *
   *			Vector And Matrix Routines			 *
   *									 *
   ***********************************************************************

   Vec             - Make Vector
   VecInt	   - Make Integer Vector
   UnVec	   - Get Components of vector
   UnVecInt	   - Get Components of Integer Vector
   VecDot	   - Vector Dot Product
   VecCross	   - Vector Cross Product
   VecLen	   - Vector Length
   VecNormalize	   - Vector Normalize
   VecMatxMult	   - Vector Matrix Multiply
   VecSub	   - Vector Subtraction
   VecSubInt	   - Vector Subtraction Integer
   VecAdd	   - Vector Addition
   VecAdd3	   - Vector Addition
   VecCopy	   - Vector Copy
   VecLinComb	   - Vector Linear Combination
   VecScalMult	   - Vector Scalar Multiple
   VecScalMultI    - Vector Scalar Multiple
   VecScalMultInt  - Vector Scalar Multiple and Rounding
   VecAddScalMult  - Vector Add Scalar Multiple
   VecNull	   - Vector Null
   VecNullInt	   - Vector Null Integer
   VecElemMult	   - Vector Element Multiply
*/

typedef float TDA[3];
typedef int   TDIA[3];
typedef float FDA[4];
typedef float Matx4x4[4][4];

void Vec(float r, float s, float t, TDA A);

void VecInt(int r, int s, int t, TDIA A);

void UnVec(TDA A, float *r, float *s, float *t);

void UnVecInt(TDIA A, int *r, int *s, int *t);

float VecDot(TDA A, TDA B);

void VecCross(TDA A, TDA B, TDA C);

float VecLen(TDA A);

void VecNormalize(TDA A);

void VecMatxMult(FDA A, Matx4x4 Matrix, FDA B);

void VecSub(TDA A, TDA B, TDA C);

void VecSubInt(TDIA A, TDIA B, TDA C);

void VecAdd(TDA A, TDA B, TDA C);

void VecAdd3(TDA A, TDA B, TDA C, TDA D);

void VecCopy(TDA A, TDA B);

void VecLinComb(float r, TDA A, float s, TDA B, TDA C);

void VecScalMult(float r, TDA A, TDA B);

void VecScalMultI(float r, TDIA A, TDA B);

void VecScalMultInt(float r, TDA A, TDIA B);

void VecAddScalMult(float r, TDA A, TDA B, TDA C);

void VecNull(TDA A);

void VecNullInt(TDIA A);

void VecElemMult(float r, TDA A, TDA B, TDA C);

/* ***********************************************************************
   *									 *
   *			Affine Transformation Routines			 *
   *									 *
   ***********************************************************************

   ZeroMatrix		- zeros the elements of a 4x4 matrix
   Translate3D		- make translation matrix
   Scale3D		- make scaling matrix
   Rotate3D		- make rotation matrix
   ZeroAllMatricies	- zeros all matricies used in transformation
   Multiply3DMatricies	- multiply 2 4x4 matricies
   PrepareMatrix	- prepare the transformation matrix (Tm=S*R*T)
   PrepareInvMatrix	- prepare the inverse transformation matrix
   Transform		- multipy a vertex by the transformation matrix
*/

void ZeroMatrix(Matx4x4 A);

void Translate3D(float tx, float ty, float tz, Matx4x4 A);

void Scale3D(float sx, float sy, float sz, Matx4x4 A);

void Rotate3D(int m, float Theta, Matx4x4 A);

void Multiply3DMatricies(Matx4x4 A, Matx4x4 B, Matx4x4 C);

void MatCopy(Matx4x4 a, Matx4x4 b);

void PrepareMatrix(float Tx, float Ty, float Tz,
		   float Sx, float Sy, float Sz,
		   float Rx, float Ry, float Rz,
		   Matx4x4 XForm);

void PrepareInvMatrix(float Tx, float Ty, float Tz,
		 float Sx, float Sy, float Sz,
		 float Rx, float Ry, float Rz,
		 Matx4x4 XForm);

void Transform(TDA A, Matx4x4 M, TDA B);




