#include "pulsar2.h"

/*
   Demonstration Stereographics OpenGL application
   Initially used to demonstrate correct off-axis frustum
   Create a model of a pulsar
   Updated to handle passive stereo
*/

OPTIONS options;
PULSAR pulsar;
CAMERA camera;

int uselist = TRUE;
int currentbutton = -1;
iPOINT mousepos = {-1,-1};
int framecounter = 0;
double targetfps     = 25; 
int debug = FALSE;

int main(int argc,char **argv)
{
   int i;

   /* The camera attributes are passed from server to client as necessary */
   CameraHome();
   camera.stereo       = NOSTEREO;
   camera.cursordepth  = camera.focallength;
   camera.screenwidth  = 400;
   camera.screenheight = 300;

   /* Options are relevent to the normal or server modes onle */
   options.rotatespeed  = 1;
   options.spinspeed    = 1;
   options.fullscreen   = FALSE;
   options.windowdump   = FALSE;
   options.record       = FALSE;

   /* Pulsar attributes are passed from server to client when they change */
   pulsar.rotateangle    = 0;
   pulsar.showconstruct  = FALSE;

   /* Parse the command line arguments */
   for (i=1;i<argc;i++) {
      if (strcmp(argv[i],"-h") == 0) 
         GiveUsage(argv[0]);
      if (strcmp(argv[i],"-f") == 0)
         options.fullscreen = !options.fullscreen;
      if (strcmp(argv[i],"-s") == 0)
         camera.stereo = ACTIVESTEREO;
      if (strcmp(argv[i],"-ss") == 0) {
         camera.stereo = DUALSTEREO;
         camera.screenwidth *= 2;
      }
      if (strcmp(argv[i],"-d") == 0)
         debug = !debug;
   }

   /* Set things up and go */
   glutInit(&argc,argv);
   if (camera.stereo == ACTIVESTEREO)
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
   else
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

   /* Create the window and handlers */
   glutCreateWindow("Pulsar model");
   glutReshapeWindow(camera.screenwidth,camera.screenheight);
   if (options.fullscreen)
      glutFullScreen();
   glutDisplayFunc(HandleDisplay);
   glutReshapeFunc(HandleReshape);

   /* 
      The client doesn't deal with user input
      Turn of the mouse since it is a killer in passive stereo
      to have a mouse on only one screen.
   */
   MakeMenus();
   glutKeyboardFunc(HandleKeyboard);
   glutSpecialFunc(HandleSpecialKeyboard);
   glutMouseFunc(HandleMouse);
   glutMotionFunc(HandleMouseMotion);
   glutPassiveMotionFunc(HandlePassiveMotion);
   glutVisibilityFunc(HandleVisibility);      
   glutSetCursor(GLUT_CURSOR_NONE);
   glutIdleFunc(HandleIdle);
   CreateEnvironment();

   /* Ready to go! */
   glutMainLoop();
   return(0);
}

/*
   Form the menu structure
*/
void MakeMenus(void)
{
   int mainmenu,speedmenu,spinmenu;

   /* Set up the speed menu */
   speedmenu = glutCreateMenu(HandleSpeedMenu);
   glutAddMenuEntry("Stop",1);
   glutAddMenuEntry("Slow",2);
   glutAddMenuEntry("Medium",3);
   glutAddMenuEntry("Fast",4);
   glutAddMenuEntry("Very fast",5);
   spinmenu = glutCreateMenu(HandleSpinMenu);
   glutAddMenuEntry("1 degree",1);
   glutAddMenuEntry("2 degrees",2);
   glutAddMenuEntry("3 degrees",3);
   glutAddMenuEntry("5 degrees",4);
   mainmenu = glutCreateMenu(HandleMainMenu);
   glutAddSubMenu("Rotation",speedmenu);
   glutAddSubMenu("Camera rotation steps",spinmenu);
   glutAddMenuEntry("Quit",9);
   glutAttachMenu(GLUT_RIGHT_BUTTON);

}

/*
   This is where global OpenGL/GLUT settings are made, 
   that is, things that will not change in time 
*/
void CreateEnvironment(void)
{
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_DITHER);
   glDisable(GL_LINE_SMOOTH);
   glDisable(GL_POINT_SMOOTH);
   glDisable(GL_POLYGON_SMOOTH); 
   glShadeModel(GL_SMOOTH);    
   glDisable(GL_CULL_FACE);
   glLineWidth(1.0);
   glPointSize(1.0);
   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   glFrontFace(GL_CW);
   glClearColor(0.0,0.0,0.0,0.0);
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   glPixelStorei(GL_UNPACK_ALIGNMENT,1);
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
}

/*
   This is the basic display callback routine
   It creates the geometry, lighting, and viewing position
*/
void HandleDisplay(void)
{
   XYZ r;
   double ratio,radians,wd2,ndfl;
   double left,right,top,bottom;

   /* Stuff for framerate calculations */
   static double tstart = -1,tstop;
   static double framerate = -1;

   /* Set the time the first time around */
   if (tstart < 0) 
      tstart = GetRunTime();

   /* Misc stuff needed for the frustum */
   ratio   = camera.screenwidth / (double)camera.screenheight;
   if (camera.stereo == DUALSTEREO)
      ratio /= 2;
   radians = DTOR * camera.aperture / 2;
   wd2     = camera.near * tan(radians);
   ndfl    = camera.near / camera.focallength;
   top     =   wd2;
   bottom  = - wd2;

   /* Determine the right eye vector */
   CROSSPROD(camera.vd,camera.vu,r);
   Normalise(&r);
   r.x *= camera.eyesep / 2.0;
   r.y *= camera.eyesep / 2.0;
   r.z *= camera.eyesep / 2.0;

   if (camera.stereo == ACTIVESTEREO || camera.stereo == DUALSTEREO) {

      if (camera.stereo == DUALSTEREO) {
         glDrawBuffer(GL_BACK);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      left  = - ratio * wd2 - 0.5 * camera.eyesep * ndfl;
      right =   ratio * wd2 - 0.5 * camera.eyesep * ndfl;
      glFrustum(left,right,bottom,top,camera.near,camera.far);
      if (camera.stereo == DUALSTEREO)
         glViewport(camera.screenwidth/2,0,camera.screenwidth/2,camera.screenheight);
      else
         glViewport(0,0,camera.screenwidth,camera.screenheight);
      glMatrixMode(GL_MODELVIEW);
      glDrawBuffer(GL_BACK_RIGHT);
      if (camera.stereo == ACTIVESTEREO)
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();
      gluLookAt(camera.vp.x + r.x,camera.vp.y + r.y,camera.vp.z + r.z,
                camera.vp.x + r.x + camera.vd.x,
                camera.vp.y + r.y + camera.vd.y,
                camera.vp.z + r.z + camera.vd.z,
                camera.vu.x,camera.vu.y,camera.vu.z);
      MakeEnvironment();
      MakeGeometry(pulsar.showconstruct);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      left  = - ratio * wd2 + 0.5 * camera.eyesep * ndfl;
      right =   ratio * wd2 + 0.5 * camera.eyesep * ndfl;
      glFrustum(left,right,bottom,top,camera.near,camera.far);
      if (camera.stereo == DUALSTEREO)
         glViewport(0,0,camera.screenwidth/2,camera.screenheight);
      else
         glViewport(0,0,camera.screenwidth,camera.screenheight);
      glMatrixMode(GL_MODELVIEW);
      glDrawBuffer(GL_BACK_LEFT);
      if (camera.stereo == ACTIVESTEREO)
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();
      gluLookAt(camera.vp.x - r.x,camera.vp.y - r.y,camera.vp.z - r.z,
                camera.vp.x - r.x + camera.vd.x,
                camera.vp.y - r.y + camera.vd.y,
                camera.vp.z - r.z + camera.vd.z,
                camera.vu.x,camera.vu.y,camera.vu.z);
      MakeEnvironment();
      MakeGeometry(pulsar.showconstruct);

   } else {

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glViewport(0,0,camera.screenwidth,camera.screenheight); 
      left  = - ratio * wd2;
      right =   ratio * wd2;
      glFrustum(left,right,bottom,top,camera.near,camera.far);
      glMatrixMode(GL_MODELVIEW);
      glDrawBuffer(GL_BACK_LEFT);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();
      gluLookAt(camera.vp.x,camera.vp.y,camera.vp.z,
                camera.vp.x + camera.vd.x,
                camera.vp.y + camera.vd.y,
                camera.vp.z + camera.vd.z,
                camera.vu.x,camera.vu.y,camera.vu.z);
      MakeEnvironment();
      MakeGeometry(pulsar.showconstruct);
   }

   /* Swap buffers */
   glutSwapBuffers();

   /*
      Handle frame rate dependent features
      Vary the resolution of spheres
   */
   if (framecounter > 0 && (framecounter % 200) == 0) {
      tstop = GetRunTime();
      framerate = 200.0 / (tstop - tstart);
      if (debug) {
         fprintf(stderr,"Frame:       %-6d  Framerate:  %.1f frames/second\n",
            framecounter,framerate);
      }
      tstart = tstop;
   }

   /* Dump the window in tif format */
   if (options.record || options.windowdump) {
      if (camera.stereo == ACTIVESTEREO)
         WindowDump("pulsar",camera.screenwidth,camera.screenheight,FALSE,5);
      else
         WindowDump("pulsar",camera.screenwidth,camera.screenheight,TRUE,5);
      options.windowdump = FALSE;
   }

   framecounter++;
}

/*
   Create the geometry for the pulsar.
   Draw contrustion lines if asked for.
   Draw the stars.
   Draw the cursor as a 3D object.
*/
void MakeGeometry(int construction)
{
   static int first = TRUE;

   /* Top level rotation  - spin */
   glPushMatrix();
   glRotatef(pulsar.rotateangle,0.0,1.0,0.0);

   /* Axis of rotation */
   if (construction) {
        glColor3f(1.0,1.0,1.0);
        glBegin(GL_LINES);
        glVertex3f(0.0,-60.0,0.0);
        glVertex3f(0.0,60.0,0.0);
        glEnd();
   }

   /* Rotation about spin axis */
   glPushMatrix();
   glRotatef(45.0,0.0,0.0,1.0);

   /* Magnetic axis */
   if (construction) {
      glColor3f(1.0,1.0,1.0);
      glBegin(GL_LINES);
      glVertex3f(0.0,-60.0,0.0);
      glVertex3f(0.0,60.0,0.0);
      glEnd();
   }

   if (uselist && first) {
      glNewList(1,GL_COMPILE);
      CreatePulsar();
      glEndList();
      first = FALSE;
   } else {
      glCallList(1);
   }
   glPopMatrix(); /* Pulsar axis rotation */
   glPopMatrix(); /* Pulsar spin */

   /* Draw flat things without lights */
   glDisable(GL_LIGHTING);
   glShadeModel(GL_FLAT);

   /* Place some stars in the background */
   CreateRandomStars(2000);

   /* Draw the cursor */
   glColor3f(0.5,1.0,0.5);
   glLineWidth(1.0);
   Create3DCursor(mousepos.h,mousepos.v,camera.screenwidth,camera.screenheight,
      camera.vp,camera.vd,camera.vu,camera.aperture,camera.cursordepth);

   if (debug) {
      glColor3f(1.0,0.0,0.0);
      DrawGLText(10,10,"Pulsar demo");
   }
}

/*
   Create the pulsar geometry 
*/
void CreatePulsar(void)
{
   int i,j,k;
   double cradius = 5.3;         /* Final radius of the cone */
   double clength = 30;          /* Cone length */
   double sradius = 10;          /* Final radius of sphere */
   double r1,r2;                 /* Min and Max radius of field lines */
   double x,y,z;
   XYZ p[4],n[4],origin={0,0,0};
   
   /* Light in center */
   glColor3f(1.0,1.0,1.0);
   glutSolidSphere(5.0,16,8);

   /* Spherical center */
   for (i=0;i<360;i+=5) {
      for (j=-80;j<80;j+=5) {

         p[0].x = sradius * cos(j*DTOR) * cos(i*DTOR);
         p[0].y = sradius * sin(j*DTOR);
         p[0].z = sradius * cos(j*DTOR) * sin(i*DTOR);
         n[0] = p[0];

         p[1].x = sradius * cos((j+5)*DTOR) * cos(i*DTOR);
         p[1].y = sradius * sin((j+5)*DTOR);
         p[1].z = sradius * cos((j+5)*DTOR) * sin(i*DTOR);
         n[1] = p[1];

         p[2].x = sradius * cos((j+5)*DTOR) * cos((i+5)*DTOR);
         p[2].y = sradius * sin((j+5)*DTOR);
         p[2].z = sradius * cos((j+5)*DTOR) * sin((i+5)*DTOR);
         n[2] = p[2];

         p[3].x = sradius * cos(j*DTOR) * cos((i+5)*DTOR);
         p[3].y = sradius * sin(j*DTOR);
         p[3].z = sradius * cos(j*DTOR) * sin((i+5)*DTOR);
         n[3] = p[3];

         glBegin(GL_POLYGON);
         if (i % 20 == 0)
            glColor3f(1.0,0.0,0.0);
         else
            glColor3f(0.5,0.0,0.0);
         for (k=0;k<4;k++) {
            glNormal3f(n[k].x,n[k].y,n[k].z);
            glVertex3f(p[k].x,p[k].y,p[k].z);
         }
         glEnd();
      }
   }

   /* Draw the cones */
   for (j=-1;j<=1;j+=2) {
      for (i=0;i<360;i+=10) {
         
         p[0]   = origin;
         n[0]   = p[0];
         n[0].y = -1;

         p[1].x = cradius * cos(i*DTOR);
         p[1].y = j*clength;
         p[1].z = cradius * sin(i*DTOR);
         n[1]   = p[1];
         n[1].y = 0;

         p[2].x = cradius * cos((i+10)*DTOR);
         p[2].y = j*clength;
         p[2].z = cradius * sin((i+10)*DTOR);
         n[2]   = p[2];
         n[2].y = 0;

         glBegin(GL_POLYGON);
         if (i % 30 == 0)
            glColor3f(0.0,0.2,0.0);
         else
            glColor3f(0.0,0.5,0.0);
         for (k=0;k<3;k++) {
            glNormal3f(n[k].x,n[k].y,n[k].z);
            glVertex3f(p[k].x,p[k].y,p[k].z);
         }
         glEnd();
      }
   }

   /* Draw the field lines */
   r1 = 12;
   r2 = 16;
   for (j=0;j<360;j+=20) {
      glPushMatrix();
      glRotatef((double)j,0.0,1.0,0.0);
      glBegin(GL_LINE_STRIP);
      glColor3f(0.75,0.75,0.75);
      for (i=-140;i<140;i++) {
         x = r1 + r1 * cos(i*DTOR);
         y = r2 * sin(i*DTOR);
         z = 0;
         glVertex3f(x,y,z);   
      }   
      glEnd();
      glPopMatrix();      
   }
}

/*
   Set up the global material and lighing environment
   This doesn't ever change so could be done only once
*/
void MakeEnvironment(void)
{
   GLfloat spec[4] = {1.0,1.0,1.0,1.0};
   GLfloat shin[1] = {5.0};
   GLfloat fullambient[4] = {1.0,1.0,1.0,1.0};
   GLfloat position[4] = {0.0,0.0,0.0,0.0};
   GLfloat ambient[4]  = {0.2,0.2,0.2,1.0};
   GLfloat diffuse[4]  = {1.0,1.0,1.0,1.0};
   GLfloat specular[4] = {0.0,0.0,0.0,1.0};

   /* Default meterials */
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shin);

   /* Turn off all the lights */
   glDisable(GL_LIGHT0);
   glDisable(GL_LIGHT1);
   glDisable(GL_LIGHT2);
   glDisable(GL_LIGHT3);
   glDisable(GL_LIGHT4);
   glDisable(GL_LIGHT5);
   glDisable(GL_LIGHT6);
   glDisable(GL_LIGHT7);

   /* Turn on the appropriate lights */
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT,fullambient);
   glLightfv(GL_LIGHT0,GL_POSITION,position);
   glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
   glEnable(GL_LIGHT0);
   glDisable(GL_LIGHTING);

   glShadeModel(GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);
}

/*
   Deal with plain key strokes
   Pass the key on to the client
   This is only handled by the server
*/
void HandleKeyboard(unsigned char key,int x, int y)
{
   /* Handle keyboard commands */
   switch (key) {
   case ESC:                            /* Quit */
   case 'Q':
   case 'q': 
      CleanExit();
      break;
   case 'h':                           /* Go home     */
   case 'H':
      CameraHome();
      break;
   case '[':                           /* Roll anti clockwise */
      RotateCamera(0,0,-1);
      break;
   case ']':                           /* Roll clockwise */
      RotateCamera(0,0,1);
      break;
   case 'i':                           /* Translate camera up */
   case 'I':
      TranslateCamera(0,1);
      break;
   case 'k':                           /* Translate camera down */
   case 'K':
      TranslateCamera(0,-1);
      break;
   case 'j':                           /* Translate camera left */
   case 'J':
      TranslateCamera(-1,0);
      break;
   case 'l':                           /* Translate camera right */
   case 'L':
      TranslateCamera(1,0);
      break;
   case 'w':                           /* Write the image to disk */
   case 'W':
      options.windowdump = !options.windowdump;
      break;
   case '<':                           /* Decrease cursor depth */
   case ',':
      camera.cursordepth *= 0.95;
      break;
   case '>':                           /* Increase 3D cursor depth */
   case '.':
      if (camera.cursordepth < camera.far/1.1)
         camera.cursordepth *= 1.05;
      break;
   case 'r':                           /* Frame recording */
   case 'R':
      options.record = !options.record;
      break;
   case '=':
   case '+':
      options.rotatespeed *= 1.5;
      break;
   case '_':
   case '-':
      options.rotatespeed /= 1.5; 
      break;
   case 'c':
   case 'C':
      pulsar.showconstruct = !pulsar.showconstruct;
      break;
   }
}

/*
   Deal with special key strokes
   All these are represented with normal keyboard commands
   and as such will be passed to the client.
*/
void HandleSpecialKeyboard(int key,int x, int y)
{
   switch (key) {
   case GLUT_KEY_LEFT:  RotateCamera(-1,0,0); break;
   case GLUT_KEY_RIGHT: RotateCamera(1,0,0);  break;
   case GLUT_KEY_UP:    RotateCamera(0,1,0);  break;
   case GLUT_KEY_DOWN:  RotateCamera(0,-1,0); break;
   }
}

/*
   Rotate (ix,iy) or roll (iz) the camera about the focal point
   ix,iy,iz are flags, 0 do nothing, +- 1 rotates in opposite directions
   Correctly updating all camera attributes
*/
void RotateCamera(int ix,int iy,int iz)
{
   XYZ vp,vu,vd;
   XYZ right;
   XYZ newvp,newr;
   double radius,dd,radians;
   double dx,dy,dz;

   vu = camera.vu;
   Normalise(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalise(&vd);
   CROSSPROD(vd,vu,right);
   Normalise(&right);
   radians = options.spinspeed * PI / 180.0;

   /* Handle the roll */
   if (iz != 0) {
      camera.vu.x += iz * right.x * radians;
      camera.vu.y += iz * right.y * radians;
      camera.vu.z += iz * right.z * radians;
      Normalise(&camera.vu);
      return;
   }

   /* Distance from the rotate point */
   dx = camera.vp.x - camera.pr.x;
   dy = camera.vp.y - camera.pr.y;
   dz = camera.vp.z - camera.pr.z;
   radius = sqrt(dx*dx + dy*dy + dz*dz);

   /* Determine the new view point */
   dd = radius * radians;
   newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
   newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
   newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
   Normalise(&newvp);
   camera.vp.x = camera.pr.x + radius * newvp.x;
   camera.vp.y = camera.pr.y + radius * newvp.y;
   camera.vp.z = camera.pr.z + radius * newvp.z;

   /* Determine the new right vector */
   newr.x = camera.vp.x + right.x - camera.pr.x;
   newr.y = camera.vp.y + right.y - camera.pr.y;
   newr.z = camera.vp.z + right.z - camera.pr.z;
   Normalise(&newr);
   newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
   newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
   newr.z = camera.pr.z + radius * newr.z - camera.vp.z;

   camera.vd.x = camera.pr.x - camera.vp.x;
   camera.vd.y = camera.pr.y - camera.vp.y;
   camera.vd.z = camera.pr.z - camera.vp.z;
   Normalise(&camera.vd);

   /* Determine the new up vector */
   CROSSPROD(newr,camera.vd,camera.vu);
   Normalise(&camera.vu);
}

/*
   Translate (pan) the camera view point
   In response to i,j,k,l keys
   Also move the camera rotate location in parallel
*/
void TranslateCamera(int ix,int iy)
{
   XYZ vp,vu,vd;
   XYZ right;
   double delta;

   vu = camera.vu;
   Normalise(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalise(&vd);
   CROSSPROD(vd,vu,right);
   Normalise(&right);
   delta = options.spinspeed * camera.focallength / 90.0;

   camera.vp.x += iy * vu.x * delta;
   camera.vp.y += iy * vu.y * delta;
   camera.vp.z += iy * vu.z * delta;
   camera.pr.x += iy * vu.x * delta;
   camera.pr.y += iy * vu.y * delta;
   camera.pr.z += iy * vu.z * delta;

   camera.vp.x += ix * right.x * delta;
   camera.vp.y += ix * right.y * delta;
   camera.vp.z += ix * right.z * delta;
   camera.pr.x += ix * right.x * delta;
   camera.pr.y += ix * right.y * delta;
   camera.pr.z += ix * right.z * delta;
}

/*
   Handle mouse events
   Only handled by the server 
*/
void HandleMouse(int button,int state,int x,int y)
{
   if (state == GLUT_DOWN) {
      if (button == GLUT_LEFT_BUTTON) {
         currentbutton = GLUT_LEFT_BUTTON;
      } else if (button == GLUT_MIDDLE_BUTTON) {
         currentbutton = GLUT_MIDDLE_BUTTON;
      } else {

      }
   }
   mousepos.h = x;
   mousepos.v = y;
}

/*
   Handle mouse motion
   Only handled by the server
*/
void HandleMouseMotion(int x,int y)
{
   int dx,dy;

   dx = x - mousepos.h;
   dy = y - mousepos.v;
   if (dx < 0)      dx = -1;
   else if (dx > 0) dx =  1;
   if (dy < 0)      dy = -1;
   else if (dy > 0) dy =  1;

   if (currentbutton == GLUT_LEFT_BUTTON)
      RotateCamera(-dx,dy,0);
   else if (currentbutton == GLUT_MIDDLE_BUTTON)
      RotateCamera(0,0,dx);

   mousepos.h = x;
   mousepos.v = y;
}

/*
   Collect mouse positions for 3D cursor
*/
void HandlePassiveMotion(int x,int y)
{
   mousepos.h = x;
   mousepos.v = y;
}


/*
   Handle the main menu
*/
void HandleMainMenu(int whichone)
{
   switch (whichone) {
   case 9: 
      CleanExit();
      break;
   }
}

/*
   Handle the speed menu
   The rotate speed is in degrees
   Send to clients
*/
void HandleSpeedMenu(int whichone)
{
   switch (whichone) {
   case 1: options.rotatespeed =  0; break;
   case 2: options.rotatespeed =  1; break;
   case 3: options.rotatespeed =  2; break;
   case 4: options.rotatespeed =  5; break;
   case 5: options.rotatespeed = 10; break;
   }
}

/*
   Handle the camera spin menu
   Only handle by the server
*/
void HandleSpinMenu(int whichone)
{
   switch (whichone) {
   case 1: options.spinspeed = 1; break;
   case 2: options.spinspeed = 2; break;
   case 3: options.spinspeed = 3; break;
   case 4: options.spinspeed = 5; break;
   }
}

/*
   Handle any time dependent state variables
   Not handled by the client
*/
void UpdateTime(void)
{
   struct timeval tp;
   long secs;

   gettimeofday(&tp,NULL);
   secs = ((1000000L * tp.tv_sec + tp.tv_usec) / 10000) % 36000;

   pulsar.rotateangle += options.rotatespeed;
}

/*
   How to handle visibility
   It is tricky to know how covered windows should be handled.
   At the moment, by normal and server, not client.
*/
void HandleVisibility(int visible)
{
   if (visible == GLUT_VISIBLE)
      glutIdleFunc(HandleIdle);
   else
      glutIdleFunc(NULL);
}

/*
   What to do on an idle event
   The server uses this to synchronise updates and swaps
   The clients use this to update states from the server
*/
void HandleIdle(void)
{
   static double tstart = -1;
   double tstop;

   if (tstart < 0)
      tstart = GetRunTime();
   tstop = GetRunTime();
   if (tstop - tstart > 1.0/targetfps) {
      UpdateTime();
      glutPostRedisplay();
      tstart = tstop;
   }
}

/* 
   Exit Cleanly
*/
void CleanExit(void)
{
   exit(0);
}

/*
   Handle a window reshape/resize
*/
void HandleReshape(int w,int h)
{
   camera.screenwidth = w;
   camera.screenheight = h;
}

/*
   Display the program usage information
*/
void GiveUsage(char *cmd)
{
   fprintf(stderr,"Usage: %s\n",cmd);
   fprintf(stderr,"Command line arguments\n");
   fprintf(stderr,"                -h   this text\n");
   fprintf(stderr,"                -f   full screen\n");
   fprintf(stderr,"                -s   active stereo\n");
   fprintf(stderr,"                -ss  dual screen stereo\n");
   fprintf(stderr,"Key Strokes\n");
   fprintf(stderr,"        arrow keys   rotate left/right/up/down\n");
   fprintf(stderr,"        left mouse   rotate\n");
   fprintf(stderr,"      middle mouse   roll\n");
   fprintf(stderr,"                 i   translate up\n");
   fprintf(stderr,"                 k   translate down\n");
   fprintf(stderr,"                 j   translate left\n");
   fprintf(stderr,"                 l   translate right\n");
   fprintf(stderr,"                 [   roll clockwise\n");
   fprintf(stderr,"                 ]   roll anti clockwise\n");
   fprintf(stderr,"           -server   act as a server, expect a client\n");
   fprintf(stderr,"-client servername   act as a client\n");
   fprintf(stderr,"                 q   quit\n");
   exit(-1);
}

/*
   Move the camera to the home position 
*/
void CameraHome(void)
{
   XYZ origin = {0,0,0};

   camera.aperture = 60;
   camera.focallength = 100;
   camera.eyesep = 4;
   camera.near = camera.focallength / 10;
   camera.far = 2000;

   camera.vp.x = 70;
   camera.vp.y = 0; 
   camera.vp.z = 0;
   camera.vd.x = -1;
   camera.vd.y = 0;
   camera.vd.z = 0;
   camera.vu.x = 0;
   camera.vu.y = 1;
   camera.vu.z = 0;
   camera.pr = origin;

   Normalise(&camera.vd);
   Normalise(&camera.vu);
}







