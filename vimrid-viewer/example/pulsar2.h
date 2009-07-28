#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <GLUT/glut.h>
#include <sys/resource.h>
#include "paulslib.h"
#include "bitmaplib.h"
#include "opengllib.h"

typedef struct {
   XYZ vp;                /* View position           */
   XYZ vd;                /* View direction vector   */
   XYZ vu;                /* View up direction       */
   XYZ pr;                /* Point to rotate about   */
   double focallength;    /* Focal Length along vd   */
   double aperture;       /* Camera aperture         */
   double eyesep;         /* Eye separation          */
	double near,far;       /* Cutting plane distances */
	int stereo;				  /* Are we in stereo mode   */
   int screenwidth;       /* Screen dimensions       */
   int screenheight;      /*                         */
	double cursordepth;    /* 3D cursor depth         */
} CAMERA;
typedef struct {
   double rotatespeed;       /* How fast the pulsar rotates             */
	int spinspeed;         /* How fast the user mouse moves           */
	int record,windowdump; /* Image recording modes                   */
	int fullscreen;        /* Game mode or not                        */
} OPTIONS;
typedef struct {
   double rotateangle;    /* Current angle of pulsar */
	int showconstruct;     /* Draw extra features     */
} PULSAR;

#define NOSTEREO 0
#define ACTIVESTEREO 1
#define DUALSTEREO 2

void HandleDisplay(void);
void MakeMenus(void);
void CreateEnvironment(void);
void MakeGeometry(int);
void CreatePulsar(void);
void MakeEnvironment(void);
void HandleKeyboard(unsigned char,int,int);
void HandleSpecialKeyboard(int,int,int);
void HandleMouse(int,int,int,int);
void HandleMainMenu(int);
void HandleSpeedMenu(int);
void HandleSpinMenu(int);
void HandleReshape(int,int);
void HandleMouseMotion(int,int);
void HandlePassiveMotion(int,int);
void HandlePassiveMotion(int,int);
void HandleVisibility(int);
void HandleIdle(void);
void UpdateTime(void);
void GiveUsage(char *);
void RotateCamera(int,int,int);
void TranslateCamera(int,int);
void CameraHome(void);
void CleanExit(void);

