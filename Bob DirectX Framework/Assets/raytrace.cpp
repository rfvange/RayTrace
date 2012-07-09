/* ***********************************************************************
   *									 *
   *		        Recursive Ray Tracing Program			 *
   *									 *
   *			  Renders Reflective Objects			 *
   *									 *
   *				Program by				 *
   *			   Christopher D. Watkins			 *
   *									 *
   *			     'C' conversion by				 *
   *				Larry Sharp				 *
   *									 *
   ***********************************************************************
*/

#include "stdio.h"
#include "stdlib.h"
#include <assert.h>
//#include "dos.h"
//#include "conio.h"
//#include "alloc.h"
//#include "math.h"
//#include "string.h"
#include "bob_math.hpp"
#include "bob_graph.hpp"

/* ***********************************************************************
   *									 *
   *			Declare Constants and Variables			 *
   *									 *
   ***********************************************************************
*/

#include "raytrace.hpp"
#define ReflectiveLamps t
#define DistEffect 0.3

Name OutputFile; 		/* Stats */
float XPitch, YPitch;

TDA LoclWgt;			/* Environment */
TDA ReflWgt;
TDA MinWgt;
TDA MaxWgt;
int MaxDepth;

float FocalLength;	       	/* Observer */
TDA ObsPos;
float ObsRotate;
float ObsTilt;
TDA ViewDir;
TDA U, V;

TDA HorCol;			/* Sky Horizon to Zenith Coloration */
TDA ZenCol;

TDA Tile1;			/* Checkerboard tiling coloration */
TDA Tile2;
float Tile;

extern int XRes, YRes;



#define MaxMaterial 10		/* Maximum Types of Materials */

typedef struct {
  Name MType;
  Name Textur;
  TDA  AmbRfl;
  TDA  DifRfl;
  TDA  SpcRfl;
  float Gloss;
} MaterialList;

MaterialList Matl[MaxMaterial+1];

TDA AmbIntens;

#define MaxTexture 6

#define Smooth 	  1
#define Checker   2
#define Grit 	  3
#define Marble 	  4
#define Wood 	  5
#define Sheetrock 6

#define MaxShapeType 9

#define Ground 0

#define Lamp          1
#define Triangle      2
#define Parallelogram 3
#define Circles       4
#define Ring          5
#define Sphere 	      6
#define Quadratic     7
#define Cone 	      8
#define Cylinder      9

#define MaxLamp 	  15
#define MaxTriangle 	  30
#define MaxParallelogram  40
#define MaxCircles 	  20
#define MaxRing 	  20
#define MaxSphere 	  30
#define MaxQuadratic 	   5
#define MaxCone 	  15
#define MaxCylinder 	  15

typedef struct {
  int MtlNum;
  int TexNum;
} GroundList;

typedef struct {
  TDA Loc;
  float Rad;
  float RadSqr;
  TDA Intens;
} LampList;

typedef struct {
  TDA Loc;
  TDA v1;
  TDA v2;
  TDA Norm;
  float NdotLoc;
  int MtlNum;
  int TexNum;
} TriangleList;

typedef struct {
  TDA Loc;
  TDA v1;
  TDA v2;
  TDA Norm;
  float NdotLoc;
  int MtlNum;
  int TexNum;
}  ParallelogramList;

typedef struct {
  TDA Loc;
  TDA v1;
  TDA v2;
  TDA Norm;
  float NdotLoc;
  float Radius;
  int MtlNum;
  int TexNum;
}  CircleList;

typedef struct {
  TDA Loc;
  TDA v1;
  TDA v2;
  TDA Norm;
  float NdotLoc;
  float Rad1;
  float Rad2;
  int MtlNum;
  int TexNum;
}  RingList;

typedef struct {
  TDA Loc;
  float Rad;
  float RadSqr;
  int MtlNum;
  int TexNum;
} SphereList;

typedef struct {
  TDA Loc;
  TDA Direct;
  float cos1;
  float sin1;
  float cos2;
  float sin2;
  float c1;
  float c2;
  float c3;
  float c4;
  float c5;
  float xmin;
  float xmax;
  float ymin;
  float ymax;
  float zmin;
  float zmax;
  int MtlNum;
  int TexNum;
} QuadraticList;

typedef QuadraticList ConeList;

typedef QuadraticList CylinderList;

int ObjCnt[MaxShapeType+1];

GroundList Gnd;
LampList Lmp[MaxLamp+1];
TriangleList Tri[MaxTriangle+1];
ParallelogramList Para[MaxParallelogram+1];
CircleList Cir[MaxCircles+1];
RingList Rng[MaxRing+1];
SphereList Sphr[MaxSphere+1];
QuadraticList Quad[MaxQuadratic+1];
ConeList Con[MaxCone+1];
CylinderList Cyl[MaxCylinder+1];

FILE *DiskFile;
FILE *TextDiskFile;

/* ***********************************************************************
   *									 *
   *			    Clear all Variables				 *
   *									 *
   ***********************************************************************

   ClearMemory - clear all variables

*/

void ClearQuadratic(QuadraticList *List)
{
  VecNull(List->Loc);
  VecNull(List->Direct);
  List->cos1=0.0;
  List->sin1=0.0;
  List->cos2=0.0;
  List->sin2=0.0;
  List->c1=0.0;
  List->c2=0.0;
  List->c3=0.0;
  List->c4=0.0;
  List->c5=0.0;
  List->xmin=0.0;
  List->xmax=0.0;
  List->ymin=0.0;
  List->ymax=0.0;
  List->zmin=0.0;
  List->zmax=0.0;
  List->MtlNum=0;
  List->TexNum=0;
}

void ClearMemory()
{
  int i;

  strcpy(OutputFile, "");
  XRes=0;
  YRes=0;
  XPitch=0.0;
  YPitch=0.0;
  VecNull(LoclWgt);
  VecNull(ReflWgt);
  VecNull(MinWgt);
  VecNull(MaxWgt);
  MaxDepth=0;
  FocalLength=0.0;
  VecNull(ObsPos);
  ObsRotate=0.0;
  ObsTilt=0.0;
  VecNull(ViewDir);
  VecNull(U);
  VecNull(V);
  VecNull(HorCol);
  VecNull(ZenCol);
  VecNull(Tile1);
  VecNull(Tile2);
  Tile=0.0;
  for(i=0; i<=MaxMaterial; i++)
  {
    strcpy(Matl[i].MType, "");
    strcpy(Matl[i].Textur, "");
    VecNull(Matl[i].AmbRfl);
    VecNull(Matl[i].DifRfl);
    VecNull(Matl[i].SpcRfl);
    Matl[i].Gloss=0.0;
  }

  VecNull(AmbIntens);
  ObjCnt[Ground]=1;
  for(i=1; i<=MaxShapeType; i++)
    ObjCnt[i]=0;
  Gnd.MtlNum=0;
  Gnd.TexNum=0;
  for(i=0; i<=MaxLamp; i++)
  {
    VecNull(Lmp[i].Loc);
    Lmp[i].Rad=0.0;
    Lmp[i].RadSqr=0.0;
    VecNull(Lmp[i].Intens);
  }
  for(i=0; i<=MaxTriangle; i++)
  {
    VecNull(Tri[i].Loc);
    VecNull(Tri[i].v1);
    VecNull(Tri[i].v2);
    VecNull(Tri[i].Norm);
    Tri[i].NdotLoc=0.0;
    Tri[i].MtlNum=0;
    Tri[i].TexNum=0;
  }
  for(i=0; i<=MaxParallelogram; i++)
  {
    VecNull(Para[i].Loc);
    VecNull(Para[i].v1);
    VecNull(Para[i].v2);
    VecNull(Para[i].Norm);
    Para[i].NdotLoc=0.0;
    Para[i].MtlNum=0;
    Para[i].TexNum=0;
  }
  for(i=0; i<=MaxCircles; i++)
  {
    VecNull(Cir[i].Loc);
    VecNull(Cir[i].v1);
    VecNull(Cir[i].v2);
    VecNull(Cir[i].Norm);
    Cir[i].NdotLoc=0.0;
    Cir[i].Radius=0.0;
    Cir[i].MtlNum=0;
    Cir[i].TexNum=0;
  }
  for(i=0; i<=MaxRing; i++)
  {
    VecNull(Rng[i].Loc);
    VecNull(Rng[i].v1);
    VecNull(Rng[i].v2);
    VecNull(Rng[i].Norm);
    Rng[i].NdotLoc=0.0;
    Rng[i].Rad1=0.0;
    Rng[i].Rad2=0.0;
    Rng[i].MtlNum=0;
    Rng[i].TexNum=0;
  }
  for(i=0; i<=MaxSphere; i++)
  {
    VecNull(Sphr[i].Loc);
    Sphr[i].Rad=0.0;
    Sphr[i].RadSqr=0.0;
    Sphr[i].MtlNum=0;
    Sphr[i].TexNum=0;
  }
  for(i=0; i<=MaxQuadratic; i++)
    ClearQuadratic(&Quad[i]);
  for(i=0; i<=MaxCone; i++)
    ClearQuadratic(&Con[i]);
  for(i=0; i<=MaxCylinder; i++)
    ClearQuadratic(&Cyl[i]);
}

/* ***********************************************************************
   *									 *
   *			     Load an *.RT File				 *
   *									 *
   ***********************************************************************
*/

int MtlCount;	/* Number of Materials Loaded */

void GetViewDir(float Angl, float Tilt, TDA View, TDA U, TDA V)
{
  float Phi, Theta;
  float x, y, z;

  Phi=Radians(Angl);
  Theta=Radians(Tilt);
  x=cos(Theta)*sin(Phi);
  y=cos(Theta)*cos(Phi);
  z=-sin(Theta);
  Vec(x, y, z, View);
  x=cos(Phi);
  y=-sin(Phi);
  z=0.0;
  Vec(x, y, z, U);
  x=sin(Theta)*sin(Phi);
  y=sin(Theta)*cos(Phi);
  z=cos(Theta);
  Vec(x, y, z, V);
}

char Buf1[256], Buf2[256], Buf3[256], Buf4[256], Buf5[256];
int dummy;
Name MtlName;

void Clear_Buffers()
{
  strset(Buf1, 0);
  strset(Buf2, 0);
  strset(Buf3, 0);
  strset(Buf4, 0);
  strset(Buf5, 0);
}

void LoadTDA(TDA A)
{
  Clear_Buffers();
  fscanf(TextDiskFile, "%s %s %s %s %s", Buf1, Buf2, Buf3, Buf4, Buf5);
  A[0]=atof(Buf3);
  A[1]=atof(Buf4);
  A[2]=atof(Buf5);
}

void LoadReal(float *a)
{
  Clear_Buffers();
  fscanf(TextDiskFile, "%s %s %s", Buf1, Buf2, Buf3);
  *a=atof(Buf3);
}

void LoadInteger(int *a)
{
  Clear_Buffers();
  fscanf(TextDiskFile, "%s %s %s", Buf1, Buf2, Buf3);
  *a=atof(Buf3);
}

void LoadText(Name a)
{
  Clear_Buffers();
  fscanf(TextDiskFile, "%s %s %s", Buf1, Buf2, Buf3);
  strcpy(a, Buf3);
}

void GetMatlNum(char Mat[], int *MatNum)
{
  int i;

  for(i=1; i<=MtlCount; i++)
  {
    if(!(strcmp(Matl[i].MType, Mat)))
      *MatNum=i;
  }
}

void GetTexNum(char Tex[], int *TexNum)
{
  if(!(strcmp(Tex, "SMOOTH")))
    *TexNum=Smooth;
  else
  {
    if(!(strcmp(Tex, "CHECKER")))
      *TexNum=Checker;
    else
    {
      if(!(strcmp(Tex, "GRIT")))
	*TexNum=Grit;
      else
      {
	if(!(strcmp(Tex, "MARBLE")))
	  *TexNum=Marble;
	else
	{
	  if(!(strcmp(Tex, "WOOD")))
	    *TexNum=Wood;
	  else
	  {
	    if(!(strcmp(Tex, "SHEETROCK")))
	      *TexNum=Sheetrock;
	  }
	}
      }
    }
  }
}

void OrientQuadratic(QuadraticList *List)
{
  float Temp;
  TDA NewDirect;

  VecNormalize(List->Direct);
  Temp=SqrFP(List->Direct[0])+SqrFP(List->Direct[2]);
  if(Temp==0.0)
    List->cos1=1.0;
  else
    List->cos1=List->Direct[2]/sqrt(Temp);
  List->sin1=sqrt(1.0-SqrFP(List->cos1));
  NewDirect[0]=List->Direct[0]*List->cos1-List->Direct[2]*List->sin1;
  NewDirect[1]=List->Direct[1];
  NewDirect[2]=List->Direct[0]*List->sin1+List->Direct[2]*List->cos1;
  Temp=SqrFP(NewDirect[1])+SqrFP(NewDirect[2]);
  if(Temp==0.0)
    List->cos2=1.0;
  else
    List->cos2=NewDirect[1]/sqrt(Temp);
  List->sin2=sqrt(1.0-SqrFP(List->cos2));
}

void GetData(Name FileName)
{
  float Rad, Hgt;
  TDA   ShapeLoc, TempLoc, vec1, vec2, vec3;
  int   MtlNumber, TexNumber;
  int TDF;

  MtlCount=0;
  TextDiskFile=fopen(FileName, "r");
  TDF=fileno(TextDiskFile);
  do
  {
    Clear_Buffers();
    fscanf(TextDiskFile, "%s", Buf1);
	assert(!ferror(TextDiskFile));
/*    if(ferror(TextDiskFile))
    {
      printf("Error!!!!!\n");
      getch();
      exit(1);
    }*/
    if(!(strcmp(Buf1, "STATS")))
    {
      LoadText(OutputFile);
      LoadInteger(&XRes);
      LoadInteger(&YRes);
      LoadReal(&XPitch);
      LoadReal(&YPitch);
    }
    if(!(strcmp(Buf1, "ENVIRONMENT")))
    {
      LoadTDA(LoclWgt);
      LoadTDA(ReflWgt);
      LoadTDA(MinWgt);
      LoadTDA(MaxWgt);
      LoadInteger(&MaxDepth);
    }
    if(!(strcmp(Buf1, "OBSERVER")))
    {
      LoadReal(&FocalLength);
      LoadTDA(ObsPos);
      LoadReal(&ObsRotate);
      LoadReal(&ObsTilt);
      GetViewDir(ObsRotate, ObsTilt, ViewDir, U, V);
    }
    if(!(strcmp(Buf1, "SKY")))
    {
      LoadTDA(HorCol);
      LoadTDA(ZenCol);
    }
    if(!(strcmp(Buf1, "CHECK")))
    {
      LoadTDA(Tile1);
      LoadTDA(Tile2);
      LoadReal(&Tile);
    }
    if(!(strcmp(Buf1, "MATERIAL")))
    {
      MtlCount+=1;
      LoadText(Matl[MtlCount].MType);
      LoadText(Matl[MtlCount].Textur);
      LoadTDA(Matl[MtlCount].AmbRfl);
      LoadTDA(Matl[MtlCount].DifRfl);
      LoadTDA(Matl[MtlCount].SpcRfl);
      LoadReal(&Matl[MtlCount].Gloss);
    }
    if(!(strcmp(Buf1, "AMBIENTLIGHT")))
      LoadTDA(AmbIntens);
    if(!(strcmp(Buf1, "GROUND")))
    {
      LoadText(MtlName);
      GetMatlNum(MtlName, &Gnd.MtlNum);
      GetTexNum(Matl[Gnd.MtlNum].Textur, &Gnd.TexNum);
    }
    if(!(strcmp(Buf1, "LAMP")))
    {
      ObjCnt[Lamp]+=1;
      LoadTDA(Lmp[ObjCnt[Lamp]].Loc);
      LoadReal(&Lmp[ObjCnt[Lamp]].Rad);
      Lmp[ObjCnt[Lamp]].RadSqr=SqrFP(Lmp[ObjCnt[Lamp]].Rad);
      LoadTDA(Lmp[ObjCnt[Lamp]].Intens);
    }
    if(!(strcmp(Buf1, "TRIANGLE")))
    {
      ObjCnt[Triangle]+=1;
      LoadTDA(Tri[ObjCnt[Triangle]].Loc);
      LoadTDA(Tri[ObjCnt[Triangle]].v1);
      LoadTDA(Tri[ObjCnt[Triangle]].v2);
      VecCross(Tri[ObjCnt[Triangle]].v1, Tri[ObjCnt[Triangle]].v2, Tri[ObjCnt[Triangle]].Norm);
      VecNormalize(Tri[ObjCnt[Triangle]].Norm);
      Tri[ObjCnt[Triangle]].NdotLoc=VecDot(Tri[ObjCnt[Triangle]].Norm, Tri[ObjCnt[Triangle]].Loc);
      LoadText(MtlName);
      GetMatlNum(MtlName, &Tri[ObjCnt[Triangle]].MtlNum);
      GetTexNum(Matl[Tri[ObjCnt[Triangle]].MtlNum].Textur, &Tri[ObjCnt[Triangle]].TexNum);
    }
    if(!(strcmp(Buf1, "PARALLELOGRAM")))
    {
      ObjCnt[Parallelogram]+=1;
      LoadTDA(Para[ObjCnt[Parallelogram]].Loc);
      LoadTDA(Para[ObjCnt[Parallelogram]].v1);
      LoadTDA(Para[ObjCnt[Parallelogram]].v2);
      VecCross(Para[ObjCnt[Parallelogram]].v1, Para[ObjCnt[Parallelogram]].v2, Para[ObjCnt[Parallelogram]].Norm);
      VecNormalize(Para[ObjCnt[Parallelogram]].Norm);
      Para[ObjCnt[Parallelogram]].NdotLoc=VecDot(Para[ObjCnt[Parallelogram]].Norm, Para[ObjCnt[Parallelogram]].Loc);
      LoadText(MtlName);
      GetMatlNum(MtlName, &Para[ObjCnt[Parallelogram]].MtlNum);
      GetTexNum(Matl[Para[ObjCnt[Parallelogram]].MtlNum].Textur, &Para[ObjCnt[Parallelogram]].TexNum);
    }
    if(!(strcmp(Buf1, "CIRCLE")))
    {
      ObjCnt[Circles]+=1;
      LoadTDA(Cir[ObjCnt[Circles]].Loc);
      LoadTDA(Cir[ObjCnt[Circles]].v1);
      VecNormalize(Cir[ObjCnt[Circles]].v1);
      LoadTDA(Cir[ObjCnt[Circles]].v2);
      VecNormalize(Cir[ObjCnt[Circles]].v2);
      VecCross(Cir[ObjCnt[Circles]].v1, Cir[ObjCnt[Circles]].v2, Cir[ObjCnt[Circles]].Norm);
      VecNormalize(Cir[ObjCnt[Circles]].Norm);
      Cir[ObjCnt[Circles]].NdotLoc=VecDot(Cir[ObjCnt[Circles]].Norm, Cir[ObjCnt[Circles]].Loc);
      LoadReal(&Cir[ObjCnt[Circles]].Radius);
      LoadText(MtlName);
      GetMatlNum(MtlName, &Cir[ObjCnt[Circles]].MtlNum);
      GetTexNum(Matl[Cir[ObjCnt[Circles]].MtlNum].Textur, &Cir[ObjCnt[Circles]].TexNum);
    }
    if(!(strcmp(Buf1, "RING")))
    {
      ObjCnt[Ring]+=1;
      LoadTDA(Rng[ObjCnt[Ring]].Loc);
      LoadTDA(Rng[ObjCnt[Ring]].v1);
      VecNormalize(Rng[ObjCnt[Ring]].v1);
      LoadTDA(Rng[ObjCnt[Ring]].v2);
      VecNormalize(Rng[ObjCnt[Ring]].v2);
      VecCross(Rng[ObjCnt[Ring]].v1, Rng[ObjCnt[Ring]].v2, Rng[ObjCnt[Ring]].Norm);
      VecNormalize(Rng[ObjCnt[Ring]].Norm);
      Rng[ObjCnt[Ring]].NdotLoc=VecDot(Rng[ObjCnt[Ring]].Norm, Rng[ObjCnt[Ring]].Loc);
      LoadReal(&Rng[ObjCnt[Ring]].Rad1);
      LoadReal(&Rng[ObjCnt[Ring]].Rad2);
      LoadText(MtlName);
      GetMatlNum(MtlName, &Rng[ObjCnt[Ring]].MtlNum);
      GetTexNum(Matl[Rng[ObjCnt[Ring]].MtlNum].Textur, &Rng[ObjCnt[Ring]].TexNum);
    }
    if(!(strcmp(Buf1, "SPHERE")))
    {
      ObjCnt[Sphere]+=1;
      LoadTDA(Sphr[ObjCnt[Sphere]].Loc);
      LoadReal(&Sphr[ObjCnt[Sphere]].Rad);
      Sphr[ObjCnt[Sphere]].RadSqr=SqrFP(Sphr[ObjCnt[Sphere]].Rad);
      LoadText(MtlName);
      GetMatlNum(MtlName, &Sphr[ObjCnt[Sphere]].MtlNum);
      GetTexNum(Matl[Sphr[ObjCnt[Sphere]].MtlNum].Textur, &Sphr[ObjCnt[Sphere]].TexNum);
    }
    if(!(strcmp(Buf1, "QUADRATIC")))
    {
      ObjCnt[Quadratic]+=1;
      LoadTDA(Quad[ObjCnt[Quadratic]].Loc);
      LoadTDA(Quad[ObjCnt[Quadratic]].Direct);
      OrientQuadratic(&Quad[ObjCnt[Quadratic]]);
      LoadReal(&Quad[ObjCnt[Quadratic]].c1);
      LoadReal(&Quad[ObjCnt[Quadratic]].c2);
      LoadReal(&Quad[ObjCnt[Quadratic]].c3);
      LoadReal(&Quad[ObjCnt[Quadratic]].c4);
      LoadReal(&Quad[ObjCnt[Quadratic]].c5);
      LoadReal(&Quad[ObjCnt[Quadratic]].xmin);
      LoadReal(&Quad[ObjCnt[Quadratic]].xmax);
      LoadReal(&Quad[ObjCnt[Quadratic]].ymin);
      LoadReal(&Quad[ObjCnt[Quadratic]].ymax);
      LoadReal(&Quad[ObjCnt[Quadratic]].zmin);
      LoadReal(&Quad[ObjCnt[Quadratic]].zmax);
      LoadText(MtlName);
      GetMatlNum(MtlName, &Quad[ObjCnt[Quadratic]].MtlNum);
      GetTexNum(Matl[Quad[ObjCnt[Quadratic]].MtlNum].Textur, &Quad[ObjCnt[Quadratic]].TexNum);
    }
    if(!(strcmp(Buf1, "CONE")))
    {
      ObjCnt[Cone]+=1;
      LoadTDA(Con[ObjCnt[Cone]].Loc);
      LoadTDA(Con[ObjCnt[Cone]].Direct);
      OrientQuadratic(&Con[ObjCnt[Cone]]);
      LoadReal(&Rad);
      LoadReal(&Hgt);
      Con[ObjCnt[Cone]].xmin=-Rad;
      Con[ObjCnt[Cone]].zmin=-Rad;
      Con[ObjCnt[Cone]].ymin=-Hgt;
      Con[ObjCnt[Cone]].xmax=Rad;
      Con[ObjCnt[Cone]].zmax=Rad;
      Con[ObjCnt[Cone]].ymax=0.0;
      Con[ObjCnt[Cone]].c1=SqrFP(Rad);
      Con[ObjCnt[Cone]].c2=-SqrFP(Con[ObjCnt[Cone]].c1);
      Con[ObjCnt[Cone]].c3=SqrFP(Hgt);
      Con[ObjCnt[Cone]].c1=Con[ObjCnt[Cone]].c1*Con[ObjCnt[Cone]].c3;
      Con[ObjCnt[Cone]].c3=Con[ObjCnt[Cone]].c1;
      Con[ObjCnt[Cone]].c4=0.0;
      Con[ObjCnt[Cone]].c5=0.0;
      LoadText(MtlName);
      GetMatlNum(MtlName, &Con[ObjCnt[Cone]].MtlNum);
      GetTexNum(Matl[Con[ObjCnt[Cone]].MtlNum].Textur, &Con[ObjCnt[Cone]].TexNum);
    }
    if(!(strcmp(Buf1, "CYLINDER")))
    {
      ObjCnt[Cylinder]+=1;
      LoadTDA(Cyl[ObjCnt[Cylinder]].Loc);
      LoadTDA(Cyl[ObjCnt[Cylinder]].Direct);
      OrientQuadratic(&Cyl[ObjCnt[Cylinder]]);
      LoadReal(&Rad);
      LoadReal(&Hgt);
      Cyl[ObjCnt[Cylinder]].xmin=-Rad;
      Cyl[ObjCnt[Cylinder]].zmin=-Rad;
      Cyl[ObjCnt[Cylinder]].ymin=0.0;
      Cyl[ObjCnt[Cylinder]].xmax=Rad;
      Cyl[ObjCnt[Cylinder]].zmax=Rad;
      Cyl[ObjCnt[Cylinder]].ymax=Hgt;
      Cyl[ObjCnt[Cylinder]].c1=1.0;
      Cyl[ObjCnt[Cylinder]].c2=0.0;
      Cyl[ObjCnt[Cylinder]].c3=1.0;
      Cyl[ObjCnt[Cylinder]].c4=SqrFP(Rad);
      Cyl[ObjCnt[Cylinder]].c5=0.0;
      LoadText(MtlName);
      GetMatlNum(MtlName, &Cyl[ObjCnt[Cylinder]].MtlNum);
      GetTexNum(Matl[Cyl[ObjCnt[Cylinder]].MtlNum].Textur, &Cyl[ObjCnt[Cylinder]].TexNum);
    }
    if(!(strcmp(Buf1, "BOX")))
    {
      LoadTDA(ShapeLoc);
      LoadTDA(vec1);
      LoadTDA(vec2);
      LoadTDA(vec3);
      LoadText(MtlName);
      GetMatlNum(MtlName, &MtlNumber);
      GetTexNum(Matl[MtlNumber].Textur, &TexNumber);
      ObjCnt[Parallelogram]+=1;
      VecCopy(ShapeLoc, Para[ObjCnt[Parallelogram]].Loc);
      VecCopy(vec1, Para[ObjCnt[Parallelogram]].v1);
      VecCopy(vec3, Para[ObjCnt[Parallelogram]].v2);
      VecCross(Para[ObjCnt[Parallelogram]].v1, Para[ObjCnt[Parallelogram]].v2, Para[ObjCnt[Parallelogram]].Norm);
      VecNormalize(Para[ObjCnt[Parallelogram]].Norm);
      Para[ObjCnt[Parallelogram]].NdotLoc=VecDot(Para[ObjCnt[Parallelogram]].Norm, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].MtlNum=MtlNumber;
      Para[ObjCnt[Parallelogram]].TexNum=TexNumber;
      ObjCnt[Parallelogram]+=1;
      VecCopy(ShapeLoc, Para[ObjCnt[Parallelogram]].Loc);
      VecCopy(vec3, Para[ObjCnt[Parallelogram]].v1);
      VecCopy(vec1, Para[ObjCnt[Parallelogram]].v2);
      VecCross(Para[ObjCnt[Parallelogram]].v1, Para[ObjCnt[Parallelogram]].v2, Para[ObjCnt[Parallelogram]].Norm);
      VecNormalize(Para[ObjCnt[Parallelogram]].Norm);
      Vec(0.0, vec2[1], 0.0, TempLoc);
      VecAdd(TempLoc, Para[ObjCnt[Parallelogram]].Loc, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].NdotLoc=VecDot(Para[ObjCnt[Parallelogram]].Norm, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].MtlNum=MtlNumber;
      Para[ObjCnt[Parallelogram]].TexNum=TexNumber;
      ObjCnt[Parallelogram]+=1;
      VecCopy(ShapeLoc, Para[ObjCnt[Parallelogram]].Loc);
      VecCopy(vec3, Para[ObjCnt[Parallelogram]].v1);
      VecCopy(vec2, Para[ObjCnt[Parallelogram]].v2);
      VecCross(Para[ObjCnt[Parallelogram]].v1, Para[ObjCnt[Parallelogram]].v2, Para[ObjCnt[Parallelogram]].Norm);
      VecNormalize(Para[ObjCnt[Parallelogram]].Norm);
      Para[ObjCnt[Parallelogram]].NdotLoc=VecDot(Para[ObjCnt[Parallelogram]].Norm, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].MtlNum=MtlNumber;
      Para[ObjCnt[Parallelogram]].TexNum=TexNumber;
      ObjCnt[Parallelogram]+=1;
      VecCopy(ShapeLoc, Para[ObjCnt[Parallelogram]].Loc);
      VecCopy(vec2, Para[ObjCnt[Parallelogram]].v1);
      VecCopy(vec3, Para[ObjCnt[Parallelogram]].v2);
      VecCross(Para[ObjCnt[Parallelogram]].v1, Para[ObjCnt[Parallelogram]].v2, Para[ObjCnt[Parallelogram]].Norm);
      VecNormalize(Para[ObjCnt[Parallelogram]].Norm);
      Vec(vec1[0], 0.0, 0.0, TempLoc);
      VecAdd(TempLoc, Para[ObjCnt[Parallelogram]].Loc, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].NdotLoc=VecDot(Para[ObjCnt[Parallelogram]].Norm, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].MtlNum=MtlNumber;
      Para[ObjCnt[Parallelogram]].TexNum=TexNumber;
      ObjCnt[Parallelogram]+=1;
      VecCopy(ShapeLoc, Para[ObjCnt[Parallelogram]].Loc);
      VecCopy(vec2, Para[ObjCnt[Parallelogram]].v1);
      VecCopy(vec1, Para[ObjCnt[Parallelogram]].v2);
      VecCross(Para[ObjCnt[Parallelogram]].v1, Para[ObjCnt[Parallelogram]].v2, Para[ObjCnt[Parallelogram]].Norm);
      VecNormalize(Para[ObjCnt[Parallelogram]].Norm);
      Para[ObjCnt[Parallelogram]].NdotLoc=VecDot(Para[ObjCnt[Parallelogram]].Norm, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].MtlNum=MtlNumber;
      Para[ObjCnt[Parallelogram]].TexNum=TexNumber;
      ObjCnt[Parallelogram]+=1;
      VecCopy(ShapeLoc, Para[ObjCnt[Parallelogram]].Loc);
      VecCopy(vec1, Para[ObjCnt[Parallelogram]].v1);
      VecCopy(vec2, Para[ObjCnt[Parallelogram]].v2);
      VecCross(Para[ObjCnt[Parallelogram]].v1, Para[ObjCnt[Parallelogram]].v2, Para[ObjCnt[Parallelogram]].Norm);
      VecNormalize(Para[ObjCnt[Parallelogram]].Norm);
      Vec(0.0, 0.0, vec3[2], TempLoc);
      VecAdd(TempLoc, Para[ObjCnt[Parallelogram]].Loc, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].NdotLoc=VecDot(Para[ObjCnt[Parallelogram]].Norm, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].MtlNum=MtlNumber;
      Para[ObjCnt[Parallelogram]].TexNum=TexNumber;
    }
    if(!(strcmp(Buf1, "PYRAMID")))
    {
      LoadTDA(ShapeLoc);
      LoadTDA(vec1);
      LoadTDA(vec2);
      LoadReal(&Hgt);
      LoadText(MtlName);
      GetMatlNum(MtlName, &MtlNumber);
      GetTexNum(Matl[MtlNumber].Textur, &TexNumber);
      ObjCnt[Parallelogram]+=1;
      VecCopy(ShapeLoc, Para[ObjCnt[Parallelogram]].Loc);
      VecCopy(vec2, Para[ObjCnt[Parallelogram]].v1);
      VecCopy(vec1, Para[ObjCnt[Parallelogram]].v2);
      VecCross(Para[ObjCnt[Parallelogram]].v1, Para[ObjCnt[Parallelogram]].v2, Para[ObjCnt[Parallelogram]].Norm);
      VecNormalize(Para[ObjCnt[Parallelogram]].Norm);
      Para[ObjCnt[Parallelogram]].NdotLoc=VecDot(Para[ObjCnt[Parallelogram]].Norm, Para[ObjCnt[Parallelogram]].Loc);
      Para[ObjCnt[Parallelogram]].MtlNum=MtlNumber;
      Para[ObjCnt[Parallelogram]].TexNum=TexNumber;
      ObjCnt[Triangle]+=1;
      VecCopy(ShapeLoc, Tri[ObjCnt[Triangle]].Loc);
      VecCopy(vec1, Tri[ObjCnt[Triangle]].v1);
      Vec(0.5*vec1[0], 0.5*vec2[1], Hgt, Tri[ObjCnt[Triangle]].v2);
      VecCross(Tri[ObjCnt[Triangle]].v1, Tri[ObjCnt[Triangle]].v2, Tri[ObjCnt[Triangle]].Norm);
      VecNormalize(Tri[ObjCnt[Triangle]].Norm);
      Tri[ObjCnt[Triangle]].NdotLoc=VecDot(Tri[ObjCnt[Triangle]].Norm, Tri[ObjCnt[Triangle]].Loc);
      Tri[ObjCnt[Triangle]].MtlNum=MtlNumber;
      Tri[ObjCnt[Triangle]].TexNum=TexNumber;
      ObjCnt[Triangle]+=1;
      Tri[ObjCnt[Triangle]].Loc[0]=ShapeLoc[0]+vec1[0];
      Tri[ObjCnt[Triangle]].Loc[1]=ShapeLoc[1]+vec2[1];
      Tri[ObjCnt[Triangle]].Loc[2]=ShapeLoc[2];
      VecScalMult(-1.0, vec1, Tri[ObjCnt[Triangle]].v1);
      Vec(-0.5*vec1[0], -0.5*vec2[1], Hgt, Tri[ObjCnt[Triangle]].v2);
      VecCross(Tri[ObjCnt[Triangle]].v1, Tri[ObjCnt[Triangle]].v2, Tri[ObjCnt[Triangle]].Norm);
      VecNormalize(Tri[ObjCnt[Triangle]].Norm);
      Tri[ObjCnt[Triangle]].NdotLoc=VecDot(Tri[ObjCnt[Triangle]].Norm, Tri[ObjCnt[Triangle]].Loc);
      Tri[ObjCnt[Triangle]].MtlNum=MtlNumber;
      Tri[ObjCnt[Triangle]].TexNum=TexNumber;
      ObjCnt[Triangle]+=1;
      Tri[ObjCnt[Triangle]].Loc[0]=ShapeLoc[0]+vec1[0];
      Tri[ObjCnt[Triangle]].Loc[1]=ShapeLoc[1]+vec2[1];
      Tri[ObjCnt[Triangle]].Loc[2]=ShapeLoc[2];
      Vec(-0.5*vec1[0], -0.5*vec2[1], Hgt, Tri[ObjCnt[Triangle]].v1);
      VecScalMult(-1.0, vec2, Tri[ObjCnt[Triangle]].v2);
      VecCross(Tri[ObjCnt[Triangle]].v1, Tri[ObjCnt[Triangle]].v2, Tri[ObjCnt[Triangle]].Norm);
      VecNormalize(Tri[ObjCnt[Triangle]].Norm);
      Tri[ObjCnt[Triangle]].NdotLoc=VecDot(Tri[ObjCnt[Triangle]].Norm, Tri[ObjCnt[Triangle]].Loc);
      Tri[ObjCnt[Triangle]].MtlNum=MtlNumber;
      Tri[ObjCnt[Triangle]].TexNum=TexNumber;
      ObjCnt[Triangle]+=1;
      VecCopy(ShapeLoc, Tri[ObjCnt[Triangle]].Loc);
      Vec(0.5*vec1[0], 0.5*vec2[1], Hgt, Tri[ObjCnt[Triangle]].v1);
      VecCopy(vec2, Tri[ObjCnt[Triangle]].v2);
      VecCross(Tri[ObjCnt[Triangle]].v1, Tri[ObjCnt[Triangle]].v2, Tri[ObjCnt[Triangle]].Norm);
      VecNormalize(Tri[ObjCnt[Triangle]].Norm);
      Tri[ObjCnt[Triangle]].NdotLoc=VecDot(Tri[ObjCnt[Triangle]].Norm, Tri[ObjCnt[Triangle]].Loc);
      Tri[ObjCnt[Triangle]].MtlNum=MtlNumber;
      Tri[ObjCnt[Triangle]].TexNum=TexNumber;
    }
  }
  while((Buf1[0]!=0) && (Buf1[0]!=0));
  fclose(TextDiskFile);
}

/* ***********************************************************************
   *									 *
   *		   Calculate Directions of Reflected Rays		 *
   *									 *
   ***********************************************************************

   CalcDirofReflRay - calculate the direction of a reflected ray

*/

void CalcDirofReflRay(TDA Dir, TDA SrfNrm, TDA ReflRay)
{
  float Tmp;

  Tmp=-2.0*VecDot(Dir, SrfNrm);
  VecAddScalMult(Tmp, SrfNrm, Dir, ReflRay);
}

/* ***********************************************************************
   *									 *
   *		      Intersection of Ray with Objects			 *
   *									 *
   ***********************************************************************

   GetIntrPt - find the intersection point given a point, direction and dist
   GetSrfNrm - find the surface normal given the point of intersection
   Intersect - determine if an object has been hit by a ray
   ShootRay  - check ray intersect against all objects and return closest

*/

void GetIntrPt(TDA Pt, TDA Dir, float Dist, TDA IntrPt)
{
  VecAddScalMult(Dist, Dir, Pt, IntrPt);
}

void Rotate(TDA vect,
	    float cos1, float sin1,
	    float cos2, float sin2,
	    TDA result)
{
  float temp;

  result[0]=vect[0]*cos1-vect[2]*sin1;
  temp=vect[0]*sin1+vect[2]*cos1;
  result[1]=-vect[1]*cos2+temp*sin2;
  result[2]=-vect[1]*sin2-temp*cos2;
}

void RevRotate(TDA vect,
	       float cos1, float sin1,
	       float cos2, float sin2,
	       TDA result)
{
  float temp;

  result[1]=vect[1]*cos2-vect[2]*sin2;
  temp=vect[1]*sin2+vect[2]*cos2;
  result[0]=-vect[0]*cos1+temp*sin1;
  result[2]=-vect[0]*sin1-temp*cos1;
}

void QuadraticSrfNrm(TDA IntrPt, TDA SrfNrm, QuadraticList *List)
{
  TDA NewPos, NewPos2, NewDir;
  float temp;

  VecSub(IntrPt, List->Loc, NewPos);
  VecCopy(NewPos, NewPos2);
  if((List->Direct[0]==0.0) && (List->Direct[1]==1.0) && (List->Direct[2]==0.0))
  {
    SrfNrm[0]=List->c1*NewPos[0];
    SrfNrm[1]=List->c2*NewPos[1];
    SrfNrm[2]=List->c3*NewPos[2];
  }
  else
  {
    Rotate(NewPos2, List->cos1, List->sin1, -List->cos2, List->sin2, NewPos);
    NewDir[0]=List->c1*NewPos[0];
    NewDir[1]=List->c2*NewPos[1];
    NewDir[2]=List->c3*NewPos[2];
    RevRotate(NewDir, -List->cos1, List->sin1, -List->cos2, List->sin2, SrfNrm);
  }
  VecNormalize(SrfNrm);
}

void GetSrfNrm(int Shp, int Obj, TDA IntrPt, TDA SrfNrm)
{
  switch(Shp)
  {
    case Ground        : Vec(0.0, 0.0, 1.0, SrfNrm);
			 break;
    case Lamp          : VecSub(IntrPt, Lmp[Obj].Loc, SrfNrm);
			 VecNormalize(SrfNrm);
			 break;
    case Triangle      : VecCopy(Tri[Obj].Norm, SrfNrm);
			 break;
    case Parallelogram : VecCopy(Para[Obj].Norm, SrfNrm);
			 break;
    case Circles       : VecCopy(Cir[Obj].Norm, SrfNrm);
			 break;
    case Ring	       : VecCopy(Rng[Obj].Norm, SrfNrm);
			 break;
    case Sphere	       : VecSub(IntrPt, Sphr[Obj].Loc, SrfNrm);
			 VecNormalize(SrfNrm);
			 break;
    case Quadratic     : QuadraticSrfNrm(IntrPt, SrfNrm, &Quad[Obj]);
			 break;
    case Cone	       : QuadraticSrfNrm(IntrPt, SrfNrm, &Con[Obj]);
			 break;
    case Cylinder      : QuadraticSrfNrm(IntrPt, SrfNrm, &Cyl[Obj]);
			 break;
  }
}

#define Small 1E-03

TDA I_IntrPoint;
TDA I_delta;
float I_dot;
float I_pos1, I_pos2;
FDA I_gu, I_gv;
TDA I_Temp;
float I_b, I_c, I_t;
float I_disc, I_sroot;
float I_rad;
float I_t1, I_t2;

void SetUpTriangle(Byte p1, Byte p2, int Obj)
{
  I_gu[0]=-I_delta[p1];
  I_gv[0]=-I_delta[p2];
  I_gu[1]=Tri[Obj].v1[p1]-I_delta[p1];
  I_gv[1]=Tri[Obj].v1[p2]-I_delta[p2];
  I_gu[2]=Tri[Obj].v2[p1]-I_delta[p1];
  I_gv[2]=Tri[Obj].v2[p2]-I_delta[p2];
}

void SetUpParallelogram(Byte p1, Byte p2, int Obj)
{
  I_gu[0]=-I_delta[p1];
  I_gv[0]=-I_delta[p2];
  I_gu[1]=Para[Obj].v1[p1]-I_delta[p1];
  I_gv[1]=Para[Obj].v1[p2]-I_delta[p2];
  I_gu[2]=Para[Obj].v2[p1]+Para[Obj].v1[p1]-I_delta[p1];
  I_gv[2]=Para[Obj].v2[p2]+Para[Obj].v1[p2]-I_delta[p2];
  I_gu[3]=Para[Obj].v2[p1]-I_delta[p1];
  I_gv[3]=Para[Obj].v2[p2]-I_delta[p2];
}

Boolean EvenCrossings(int Sides)
{
  Byte i, j;
  Word crossings;

  crossings=0;
  for(i=0; i<Sides; i++)
  {
    j=(i+1) % Sides;
    if(((I_gv[i]<0) && (I_gv[j]>=0)) || ((I_gv[j]<0) && (I_gv[i]>=0)))
    {
      if((I_gu[i]>=0) && (I_gu[j]>=0))
	++crossings;
      else
      {
	if((I_gu[i]>=0) || (I_gu[j]>=0))
	{
	  if((I_gu[i]-I_gv[i]*(I_gu[j]-I_gu[i])/(I_gv[j]-I_gv[i]))>0)
	    ++crossings;
	}
      }
    }
  }
  if((crossings%2)==0)
    return(t);
  else
    return(f);
}

float QuadraticIntersectionCheck()
{
  float intersection;

  if((!(I_t1>Small)) && (!(I_t2>Small)))
    intersection=-1.0;
  else
  {
    if(I_t1>I_t2)
    {
      if(I_t2<Small)
	I_t2=I_t1;
    }
    else
    {
      if(I_t1>Small)
	I_t2=I_t1;
    }
    intersection=I_t2;
  }
return(intersection);
}

Boolean PointOutofBounds(TDA Point, QuadraticList *List)
{
  if((Point[0]<List->xmin) || (Point[0]>List->xmax) ||
     (Point[1]<List->ymin) || (Point[1]>List->ymax) ||
     (Point[2]<List->zmin) || (Point[2]>List->zmax))
    return(t);
  else
    return(f);
}

float QuadraticShapes(QuadraticList *List, TDA Pt, TDA Dir)
{
  int i;
  TDA NewLoc, NewLoc2, NewDir;
  float a, disc;
  TDA loc2, loc1;
  float intersection, temp;

  VecSub(Pt, List->Loc, NewLoc);
  VecCopy(NewLoc, NewLoc2);
  if((List->Direct[0]==0.0) && (List->Direct[1]==1.0) && (List->Direct[2]==0.0))
    VecCopy(Dir, NewDir);
  else
  {
    Rotate(Dir, List->cos1, List->sin1, List->cos2, List->sin2, NewDir);
    Rotate(NewLoc2, List->cos1, List->sin1, List->cos2, List->sin2, NewLoc);
  }
  if(List->c5==0.0)
  {
    I_c=-List->c4+List->c1*SqrFP(NewLoc[0])+
		  List->c2*SqrFP(NewLoc[1])+
		  List->c3*SqrFP(NewLoc[2]);
    I_b=2.0*(List->c1*NewLoc[0]*NewDir[0]+
	     List->c2*NewLoc[1]*NewDir[1]+
	     List->c3*NewLoc[2]*NewDir[2]);
    a=List->c1*SqrFP(NewDir[0])+
      List->c2*SqrFP(NewDir[1])+
      List->c3*SqrFP(NewDir[2]);
  }
  else
  {
    I_c=-List->c4+List->c1*SqrFP(NewLoc[0])-
		  List->c5*NewLoc[1]+
		  List->c3*SqrFP(NewLoc[2]);
    I_b=2.0*(List->c1*NewLoc[0]*NewDir[0]-
	     List->c5*NewDir[1]+
	     List->c3*NewLoc[2]*NewDir[2]);
    a=List->c1*SqrFP(NewDir[0])+
      List->c3*SqrFP(NewDir[2]);
  }
  disc=SqrFP(I_b)-4.0*a*I_c;
  if(disc<0.0)
    intersection=-1.0;
  else
  {
    I_sroot=sqrt(disc);
    I_t2=(-I_b+I_sroot)/(a+a);
    GetIntrPt(NewLoc, NewDir, I_t2, loc2);
    if(PointOutofBounds(loc2, List))
      I_t2=-1.0;
    I_t1=(-I_b-I_sroot)/(a+a);
    GetIntrPt(NewLoc, NewDir, I_t1, loc1);
    if(PointOutofBounds(loc1, List))
      I_t1=-1.0;
    intersection=QuadraticIntersectionCheck();
  }
  return(intersection);
}

float Intersect(TDA Pt, TDA Dir, int Shp, int Obj)
{
  float intersection;

  switch(Shp)
  {
    case Ground :        if(Dir[2]==0.0)
			   intersection=-1.0;
			 else
			 {
			   I_t=-Pt[2]/Dir[2];
			   if(I_t>Small)
			     intersection=I_t;
			   else
			     intersection=-1.0;
			 }
			 break;
    case Lamp : 	 VecSub(Lmp[Obj].Loc, Pt, I_Temp);
			 I_b=VecDot(Dir, I_Temp)*-2.0;
			 I_c=VecDot(I_Temp, I_Temp)-Lmp[Obj].RadSqr;
			 I_disc=SqrFP(I_b)-4.0*I_c;
			 if(!(I_disc>0.0))
			   intersection=-1.0;
			 else
			 {
			   I_sroot=sqrt(I_disc);
			   I_t1=(-I_b-I_sroot)*0.5;
			   I_t2=(-I_b+I_sroot)*0.5;
			   intersection=QuadraticIntersectionCheck();
			 }
			 break;
    case Triangle : 	 I_dot=VecDot(Tri[Obj].Norm, Dir);
			 if(fabs(I_dot)<Small)
			   intersection=-1.0;
			 else
			 {
			   I_pos1=Tri[Obj].NdotLoc;
			   I_pos2=VecDot(Tri[Obj].Norm, Pt);
			   I_t=(I_pos1-I_pos2)/I_dot;
			   GetIntrPt(Pt, Dir, I_t, I_IntrPoint);
			   VecSub(I_IntrPoint, Tri[Obj].Loc, I_delta);
			   if((fabs(Tri[Obj].Norm[0])>fabs(Tri[Obj].Norm[1])) &&
			      (fabs(Tri[Obj].Norm[0])>fabs(Tri[Obj].Norm[2])))
			     SetUpTriangle(1, 2, Obj);
			   else
			   {
			     if(fabs(Tri[Obj].Norm[1])>=fabs(Tri[Obj].Norm[2]))
			       SetUpTriangle(0, 2, Obj);
			     else
			       SetUpTriangle(0, 1, Obj);
			   }
			 }
			 if(EvenCrossings(3))
			   intersection=-1.0;
			 else
			   intersection=I_t;
			 break;
    case Parallelogram : I_dot=VecDot(Para[Obj].Norm, Dir);
			 if(fabs(I_dot)<Small)
			   intersection=-1.0;
			 else
			 {
			   I_pos1=Para[Obj].NdotLoc;
			   I_pos2=VecDot(Para[Obj].Norm, Pt);
			   I_t=(I_pos1-I_pos2)/I_dot;
			   GetIntrPt(Pt, Dir, I_t, I_IntrPoint);
			   VecSub(I_IntrPoint, Para[Obj].Loc, I_delta);
			   if((fabs(Para[Obj].Norm[0])>fabs(Para[Obj].Norm[1])) &&
			      (fabs(Para[Obj].Norm[0])>fabs(Para[Obj].Norm[2])))
			     SetUpParallelogram(1, 2, Obj);
			   else
			   {
			     if(fabs(Para[Obj].Norm[1])>=fabs(Para[Obj].Norm[2]))
			       SetUpParallelogram(0, 2, Obj);
			     else
			       SetUpParallelogram(0, 1, Obj);
			   }
			 }
			 if(EvenCrossings(4))
			   intersection=-1.0;
			 else
			   intersection=I_t;
			 break;
    case Circles :	 I_dot=VecDot(Cir[Obj].Norm, Dir);
			 if(fabs(I_dot)<Small)
			   intersection=-1.0;
			 else
			 {
			   I_pos1=Cir[Obj].NdotLoc;
			   I_pos2=VecDot(Cir[Obj].Norm, Pt);
			   I_t=(I_pos1-I_pos2)/I_dot;
			   GetIntrPt(Pt, Dir, I_t, I_IntrPoint);
			   VecSub(I_IntrPoint, Cir[Obj].Loc, I_delta);
			   I_rad=sqrt(VecDot(I_delta, I_delta));
			   if(I_rad>Cir[Obj].Radius)
			     intersection=-1.0;
			   else
			     intersection=I_t;
			 }
			 break;
    case Ring :          I_dot=VecDot(Rng[Obj].Norm, Dir);
			 if(fabs(I_dot)<Small)
			   intersection=-1.0;
			 else
			 {
			   I_pos1=Rng[Obj].NdotLoc;
			   I_pos2=VecDot(Rng[Obj].Norm, Pt);
			   I_t=(I_pos1-I_pos2)/I_dot;
			   GetIntrPt(Pt, Dir, I_t, I_IntrPoint);
			   VecSub(I_IntrPoint, Rng[Obj].Loc, I_delta);
			   I_rad=sqrt(VecDot(I_delta, I_delta));
			   if((I_rad<Rng[Obj].Rad1) || (I_rad>Rng[Obj].Rad2))
			     intersection=-1.0;
			   else
			     intersection=I_t;
			 }
			 break;
    case Sphere : 	 VecSub(Sphr[Obj].Loc, Pt, I_Temp);
			 I_b=VecDot(Dir, I_Temp)*-2.0;
			 I_c=VecDot(I_Temp, I_Temp)-Sphr[Obj].RadSqr;
			 I_disc=SqrFP(I_b)-4.0*I_c;
			 if(!(I_disc>0.0))
			   intersection=-1.0;
			 else
			 {
			   I_sroot=sqrt(I_disc);
			   I_t1=(-I_b-I_sroot)*0.5;
			   I_t2=(-I_b+I_sroot)*0.5;
			   intersection=QuadraticIntersectionCheck();
			 }
			 break;
    case Quadratic :	 intersection=QuadraticShapes(&Quad[Obj], Pt, Dir);
			 break;
    case Cone :          intersection=QuadraticShapes(&Con[Obj], Pt, Dir);
			 break;
    case Cylinder :      intersection=QuadraticShapes(&Cyl[Obj], Pt, Dir);
			 break;
  }
  return(intersection);
}

Boolean ShootRay(TDA Start, TDA Dir, int *Shp, int *Obj, float *Dist)
{
  int ShapeNum;
  int ObjectNum;
  float NewDist;
  Boolean ObjHit;

  *Shp=-1;
  *Obj=-1;
  *Dist=-1.0;
  ObjHit=f;
  for(ShapeNum=0; ShapeNum<=MaxShapeType; ShapeNum++)
  {
    for(ObjectNum=1; ObjectNum<=ObjCnt[ShapeNum]; ObjectNum++)
    {
      if((ShapeNum==0) && (!(Gnd.MtlNum==0)))
	NewDist=Intersect(Start, Dir, Ground, 0);
      else
	NewDist=Intersect(Start, Dir, ShapeNum, ObjectNum);
      if(NewDist>Small)
      {
	if(*Dist==-1.0)
	{
	  ObjHit=t;
	  *Dist=NewDist;
	  *Shp=ShapeNum;
	  *Obj=ObjectNum;
	}
	else
	{
	  if(NewDist<*Dist)
	  {
	    *Dist=NewDist;
	    *Shp=ShapeNum;
	    *Obj=ObjectNum;
	  }
	}
      }
    }
  }
  return(ObjHit);
}

/* ***********************************************************************
   *									 *
   *  Calculate Contribution of Local Color Model at Intersection Point  *
   *									 *
   ***********************************************************************

   GetLoclCol - calculate ambient, diffuse, reflection and specular
		reflection

*/

#define  MaxNoise 28

Word NoiseMatrix[MaxNoise][MaxNoise][MaxNoise];

void InitNoise()
{
  Byte x, y, z;
  Byte i, j, k;

//  randomize();
  for(x=0; x<=MaxNoise-1; x++)
  {
    for(y=0; y<=MaxNoise-1; y++)
    {
      for(z=0; z<=MaxNoise-1; z++)
      {
	NoiseMatrix[x][y][z] = rand() % 12000;
	if(x==MaxNoise-1)
	  i=0;
	else
	  i=x;
	if(y==MaxNoise-1)
	  j=0;
	else
	  j=y;
	if(z==MaxNoise-1)
	  k=0;
	else
	  k=z;
	NoiseMatrix[x][y][z]=NoiseMatrix[i][j][k];
      }
    }
  }
}

int Noise(float x, float y, float z)
{

/* harmonic and random functions combined to create a noise function
   based on Perlin's (1985) noise function - ideas found in Alan Watt's -
   Fundamentals of Three-Dimensional Computer Graphics */

  Byte ix, iy, iz;
  float ox, oy, oz;
  int p000, p001;
  int p010, p011;
  int p100, p101;
  int p110, p111;
  int p00, p01;
  int p10, p11;
  int p0, p1;
  int d00, d01;
  int d10, d11;
  int d0, d1;
  int d;

  x=fabs(x);
  y=fabs(y);
  z=fabs(z);
  ix=Trunc(x)%MaxNoise;
  iy=Trunc(y)%MaxNoise;
  iz=Trunc(z)%MaxNoise;
  ox=Frac(x);
  oy=Frac(y);
  oz=Frac(z);
  p000=NoiseMatrix[ix][iy][iz];
  p001=NoiseMatrix[ix][iy][iz+1];
  p010=NoiseMatrix[ix][iy+1][iz];
  p011=NoiseMatrix[ix][iy+1][iz+1];
  p100=NoiseMatrix[ix+1][iy][iz];
  p101=NoiseMatrix[ix+1][iy][iz+1];
  p110=NoiseMatrix[ix+1][iy+1][iz];
  p111=NoiseMatrix[ix+1][iy+1][iz+1];
  d00=p100-p000;
  d01=p101-p001;
  d10=p110-p010;
  d11=p111-p011;
  p00=Trunc(d00*ox)+p000;
  p01=Trunc(d01*ox)+p001;
  p10=Trunc(d10*ox)+p010;
  p11=Trunc(d11*ox)+p011;
  d0=p10-p00;
  d1=p11-p01;
  p0=Trunc(d0*oy)+p00;
  p1=Trunc(d1*oy)+p01;
  d=p1-p0;
  return((Trunc(d*oz)+p0));
}

void MarbleTex(TDA Pt, TDA RGB)
{
  float i, d;
  float x, y, z;

  UnVec(Pt, &x, &y, &z);
  x*=0.2;
  d=x+0.0006*Noise(x, y*0.1, z*0.1);
  d*=(Trunc(d)%25);
  i=0.5+0.05*fabs(d-10.0-20.0*Trunc(d*0.05));
  if (i > 1.0)
    i = 1.0;
  Vec(i, i, i, RGB);
}

void WoodTex(TDA Pt, TDA RGB)
{
  float i, d;
  float x, y, z;

  UnVec(Pt, &x, &y, &z);
  x*=0.2;
  d=x+0.0002*Noise(x, y*0.1, z*0.1);
  d*=(Trunc(d)%25);
  i=0.7+0.05*fabs(d-10.0-20.0*Trunc(d*0.05));
  if (i > 1.0)
    i = 1.0;
  Vec(i, i, i, RGB);
}

int LC_Mtl, LC_Src;
int LC_ShadShp;
int LC_ShadObj;
float LC_ShadDist;
Boolean LC_ObjHit; // RFV
TDA LC_LmpDir;
TDA LC_Addition;
TDA LC_Total;
float SS_Lamb;
TDA SS_Diff, SS_Spec;

void Texture(int Tex, TDA Texturing, TDA IntrPt)
{
  int x, y, z, rt;
  float lev, r;

  switch(Tex)
  {
    case Checker       : x=Round(fabs(IntrPt[0])*Tile)%10000;
			 y=Round(fabs(IntrPt[1])*Tile)%10000;
			 z=Round(fabs(IntrPt[2])*Tile)%10000;
			 if(((x+y+z)%2)==1)
			   VecCopy(Tile1, Texturing);
			 else
			   VecCopy(Tile2, Texturing);
			 break;
    case Grit          : rt=rand() % 32767;
			 r=(float) rt/32768.0;
			 lev=r*0.2+0.8;
			 Vec(lev, lev, lev, Texturing);
			 break;
    case Marble	       : MarbleTex(IntrPt, Texturing);
			 break;
    case Wood	       : WoodTex(IntrPt, Texturing);
			 break;
    case Sheetrock     : rt=rand() % 32767;
			 r=(float) rt/32768.0;
			 lev=r*0.1+0.9;
			 Vec(lev, lev, lev, Texturing);
			 break;
  }
}

void Ambient()
{
  VecElemMult(1.0, Matl[LC_Mtl].AmbRfl, AmbIntens, LC_Total);
}

void Diffuse()
{
  VecElemMult(SS_Lamb, Matl[LC_Mtl].DifRfl, Lmp[LC_Src].Intens, SS_Diff);
}

void Specular(TDA Dir, TDA SrfNrm)
{
  TDA Temp;
  float cone, Glint;

  VecSub(LC_LmpDir, Dir, Temp);
  VecScalMult(0.5, Temp, Temp);
  VecNormalize(Temp);
  cone=VecDot(SrfNrm, Temp);
  if(cone>0.6)
  {
    Glint=exp(Matl[LC_Mtl].Gloss*Log(cone));
    VecElemMult(Glint, Matl[LC_Mtl].SpcRfl, Lmp[LC_Src].Intens, SS_Spec);
  }
  else
    VecNull(SS_Spec);
}

void UnShadowedSurface(TDA Dir, TDA SrfNrm)
{
  SS_Lamb=VecDot(SrfNrm, LC_LmpDir);
  if(SS_Lamb<=0.0)
  {
    VecNull(SS_Diff);
    VecNull(SS_Spec);
  }
  else
  {
    Diffuse();
    Specular(Dir, SrfNrm);
  }
  VecAdd(SS_Diff, SS_Spec, LC_Addition);
}

void ShadowFeeler(TDA IntrPt)
{
  LC_ObjHit=ShootRay(IntrPt, LC_LmpDir, &LC_ShadShp, &LC_ShadObj, &LC_ShadDist);
}

void GetLoclCol(int Shp, int Obj,
		   TDA Dir, TDA IntrPt, TDA SrfNrm,
		   float Dist, TDA LoclCol)
{
  int Tex;
  TDA Amb;
  TDA Temp;
  TDA ColorTexture;
  float IntensFactor;
  Boolean HitItself;

  if(Shp==Lamp)
  {
    IntensFactor=(1.0-DistEffect)+DistEffect*(-VecDot(SrfNrm, Dir)/sqrt(Dist));
    VecScalMult(IntensFactor, Lmp[Obj].Intens, LoclCol);
  }
  else
  {
    switch(Shp)
    {
      case Ground        : LC_Mtl=Gnd.MtlNum;
			   Tex=Gnd.TexNum;
			   break;
      case Triangle	 : LC_Mtl=Tri[Obj].MtlNum;
			   Tex=Tri[Obj].TexNum;
			   break;
      case Parallelogram : LC_Mtl=Para[Obj].MtlNum;
			   Tex=Para[Obj].TexNum;
			   break;
      case Circles 	 : LC_Mtl=Cir[Obj].MtlNum;
			   Tex=Cir[Obj].TexNum;
			   break;
      case Ring          : LC_Mtl=Rng[Obj].MtlNum;
			   Tex=Rng[Obj].TexNum;
			   break;
      case Sphere        : LC_Mtl=Sphr[Obj].MtlNum;
			   Tex=Sphr[Obj].TexNum;
			   break;
      case Quadratic     : LC_Mtl=Quad[Obj].MtlNum;
			   Tex=Quad[Obj].TexNum;
			   break;
      case Cone          : LC_Mtl=Con[Obj].MtlNum;
			   Tex=Con[Obj].TexNum;
			   break;
      case Cylinder      : LC_Mtl=Cyl[Obj].MtlNum;
			   Tex=Cyl[Obj].TexNum;
			   break;
    }
    Ambient();
    for(LC_Src=1; LC_Src<=ObjCnt[Lamp]; LC_Src++)
    {
      VecSub(Lmp[LC_Src].Loc, IntrPt, LC_LmpDir);
      VecNormalize(LC_LmpDir);
      ShadowFeeler(IntrPt);
//	  bool LCSS = LC_ShadShp==Lamp;
//	  bool LCSO = LC_ShadObj==LC_Src;
//	  bool SCOH = LC_ObjHit==1;
//      HitItself = (Boolean)(SCOH && LCSS && LCSO);
      HitItself=(Boolean)(LC_ObjHit&&(LC_ShadShp==Lamp)&&(LC_ShadObj==LC_Src));
      if((!(LC_ObjHit)) || HitItself)
      {
	UnShadowedSurface(Dir, SrfNrm);
	VecAdd(LC_Total, LC_Addition, LC_Total);
      }
    }
    if(Tex==Smooth)
      VecCopy(LC_Total, LoclCol);
    else
    {
      Texture(Tex, ColorTexture, IntrPt);
      VecElemMult(1.0, LC_Total, ColorTexture, LoclCol);
    }
  }
}

/* ***********************************************************************
   *									 *
   *			       Calculate Sky				 *
   *									 *
   ***********************************************************************

   Sky - blend a sky color from the horizon to the zenith

*/

void Sky(TDA Dir, TDA Col)
{
  float small=1E-03;
  float sin2, cos2;
  float x2, y2, z2;

  x2=SqrFP(Dir[0]);
  y2=SqrFP(Dir[1]);
  z2=SqrFP(Dir[2]);
  if(z2==0)
    z2=small;
  sin2=z2/(x2+y2+z2);
  cos2=1.0-sin2;
  VecLinComb(cos2, HorCol, sin2, ZenCol, Col);
}

/* ***********************************************************************
   *									 *
   *			    Recursive Ray Tracer			 *
   *									 *
   ***********************************************************************

   TraceRay - perform recursive ray tracing

*/

void Comb(TDA A, TDA B, TDA C, TDA D, TDA Col)
{
  TDA T1, T2;

  VecElemMult(1.0, A, B, T1);
  VecElemMult(1.0, C, D, T2);
  VecAdd(T1, T2, Col);
}

Boolean WgtMin(TDA TotWgt)
{
  if((TotWgt[0]<=MinWgt[0])&&(TotWgt[1]<=MinWgt[1])&&(TotWgt[2]<=MinWgt[2]))
    return(t);
  else
    return(f);
}

int TR_Shp, TR_Obj;

Boolean MaterialSpecular()
{
  int Mtl;

  switch(TR_Shp)
  {
    case Ground        : Mtl=Gnd.MtlNum;
			 break;
    case Triangle      : Mtl=Tri[TR_Obj].MtlNum;
			 break;
    case Parallelogram : Mtl=Para[TR_Obj].MtlNum;
			 break;
    case Circles       : Mtl=Cir[TR_Obj].MtlNum;
			 break;
    case Ring          : Mtl=Rng[TR_Obj].MtlNum;
			 break;
    case Sphere        : Mtl=Sphr[TR_Obj].MtlNum;
			 break;
    case Quadratic     : Mtl=Quad[TR_Obj].MtlNum;
			 break;
    case Cone          : Mtl=Con[TR_Obj].MtlNum;
			 break;
    case Cylinder      : Mtl=Cyl[TR_Obj].MtlNum;
			 break;
  }
  if(!(TR_Shp==Lamp))
  {
    if((Matl[Mtl].SpcRfl[0]==0)&&(Matl[Mtl].SpcRfl[1]==0)&&(Matl[Mtl].SpcRfl[2]==0))
      return(f);
    else
      return(t);
  }
  else
    return(t);
}

void TraceRay(TDA Start, TDA Dir, TDA TotWgt, int Depth, TDA Col)
{
  TDA LoclCol, ReflCol;
  TDA ReflDir, Wgt;
  TDA IntrPt, SrfNrm;
  float Dist;
  Boolean ObjHit;

  ObjHit=ShootRay(Start, Dir, &TR_Shp, &TR_Obj, &Dist);
  if(ObjHit)
  {
    GetIntrPt(Start, Dir, Dist, IntrPt);
    GetSrfNrm(TR_Shp, TR_Obj, IntrPt, SrfNrm);
    GetLoclCol(TR_Shp, TR_Obj, Dir, IntrPt, SrfNrm, Dist, LoclCol);
    if((Depth==MaxDepth) || (WgtMin(TotWgt)))
      VecElemMult(1.0, LoclCol, LoclWgt, Col);
    else
    {
      if(!(TR_Shp==Lamp) || (TR_Shp==Lamp)&ReflectiveLamps)
      {
	if(MaterialSpecular())
	{
	  CalcDirofReflRay(Dir, SrfNrm, ReflDir);
	  VecElemMult(1.0, TotWgt, ReflWgt, Wgt);
	  TraceRay(IntrPt, ReflDir, Wgt, Depth+1, ReflCol);
	}
	else
	  VecNull(ReflCol);
      }
      else
	VecNull(ReflCol);
      Comb(LoclCol, LoclWgt, ReflCol, ReflWgt, Col);
    }
  }
  else
    Sky(Dir, Col);
}

/* ***********************************************************************
   *									 *
   *			    Scan Pixel Display				 *
   *									 *
   ***********************************************************************

   GetEye - calculate the direction of a ray from the eye through a screen
	    pixel and into the scene
   Scan   - scan the pixel display with eye-rays

*/

TDA ViewVec;
int CenterX;
int CenterY;
int ScanXRes = XRes;
int ScanYRes = YRes;
float XPitchDivFocLen;
float YPitchDivFocLen;


void PreCalculation()
{
  float Scale;

  XPitchDivFocLen=XPitch/FocalLength;
  YPitchDivFocLen=YPitch/FocalLength;
  CenterX=ScanXRes>>1;
  CenterY=ScanYRes>>1;
  Scale=CenterX;
  VecScalMult(Scale, ViewDir, ViewVec);
}

void GetInitialDir(int i, int j, TDA Dir)
{
  float x, y;
  TDA EyeToPixVec;

  x=(i-CenterX)*XPitchDivFocLen;
  y=(CenterY-j)*YPitchDivFocLen;
  VecLinComb(x, U, y, V, EyeToPixVec);
  VecAdd(ViewVec, EyeToPixVec, Dir);
  VecNormalize(Dir);
}

void Scan(Name FileName)
{
  TDA InitialDir;
  TDA Col;
  TDIA Colr;
  int Xp, Yp;

  ScanXRes=XRes; // these were in main() to set up graphics
  ScanYRes=YRes; // XRes and YRes are loaded from GetData().
  PreCalculation();
  TextDiskFile=fopen(FileName, "w+b");
//  ScanXRes = 750; ScanYRes = 550; // RFV
  putw(ScanXRes, TextDiskFile);
  putw(ScanYRes, TextDiskFile);
  for(Yp=30; Yp<=ScanYRes-1; Yp++) // RFV
  {
    for(Xp=30; Xp<=ScanXRes-1; Xp++)
    {
      GetInitialDir(Xp, Yp, InitialDir);
      TraceRay(ObsPos, InitialDir, MaxWgt, 1, Col);
      VecScalMultInt(63.0, Col, Colr);
      Plot(Xp, Yp, (Colr[0]+Colr[1]+Colr[2])/3);
      fprintf(TextDiskFile, "%c%c%c", Colr[0], Colr[1], Colr[2]);
    }
  }
  fclose(TextDiskFile);
}
/*
Name FileName;

void GetFileName()
{
  Byte x, y;

  puts("");
  printf("Enter File Name -> ");
  x=wherex();
  y=wherey();
  gets(FileName);
  if(!(strcmp(FileName, "")))
  {
    strcpy(FileName, "Test");
    gotoxy(x, y);
    printf("%s", FileName);
  }
  strcat(FileName, ".RT");
  puts("");
  puts("");
}
*/
/* ***********************************************************************
   *									 *
   *				Main Program				 *
   *									 *
   ***********************************************************************
*/
/*
PaletteRegister PalArray;

void main()
{
}
*/