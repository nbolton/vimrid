/* ============================================================================
**
** Demonstration of spinning cube
** Copyright (C) 2005  Julien Guertault
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
** ========================================================================= */

#include	<GL/glut.h>
#include	<math.h>
#include	<stdlib.h>
#include	<stdio.h>

#ifdef USE_TRACKD
#include <trackdAPI.h> 
#endif

double mouseX, mouseY;

void *tracker;
void *controller;

/*
** Function called to update rendering
*/
void		DisplayFunc (void)
{
  static float alpha = 0;

#ifdef USE_TRACKD
  TrackerSensor *s = getSensor(tracker,0);
#endif

  glDrawBuffer(GL_BACK_LEFT);

  /* Clear the buffer, clear the matrix */
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();


#ifdef USE_TRACKD
  /* A step backward, then spin the cube */
  glTranslatef (0+s->x, -1.5-(s->z/2), -14+(s->y/2));
#else
  glTranslatef(mouseX, -1.5-(mouseY/2), -14);
#endif
//  printf("%f %f %f\n",s->x,-3.5-(s->z/2),-s->y);
//  glTranslatef (0, 0, -10.0);
  glRotatef (30, 1, 0, 0);
  glRotatef (alpha, 0, 1, 0);

  /* We tell we want to draw quads */
  glBegin (GL_QUADS);

  /* Every four calls to glVertex, a quad is drawn */
  glColor3f (0, 0, 0); glVertex3f (-1, -1, -1);
  glColor3f (0, 0, 1); glVertex3f (-1, -1,  1);
  glColor3f (0, 1, 1); glVertex3f (-1,  1,  1);
  glColor3f (0, 1, 0); glVertex3f (-1,  1, -1);

  glColor3f (1, 0, 0); glVertex3f ( 1, -1, -1);
  glColor3f (1, 0, 1); glVertex3f ( 1, -1,  1);
  glColor3f (1, 1, 1); glVertex3f ( 1,  1,  1);
  glColor3f (1, 1, 0); glVertex3f ( 1,  1, -1);

  glColor3f (0, 0, 0); glVertex3f (-1, -1, -1);
  glColor3f (0, 0, 1); glVertex3f (-1, -1,  1);
  glColor3f (1, 0, 1); glVertex3f ( 1, -1,  1);
  glColor3f (1, 0, 0); glVertex3f ( 1, -1, -1);

  glColor3f (0, 1, 0); glVertex3f (-1,  1, -1);
  glColor3f (0, 1, 1); glVertex3f (-1,  1,  1);
  glColor3f (1, 1, 1); glVertex3f ( 1,  1,  1);
  glColor3f (1, 1, 0); glVertex3f ( 1,  1, -1);

  glColor3f (0, 0, 0); glVertex3f (-1, -1, -1);
  glColor3f (0, 1, 0); glVertex3f (-1,  1, -1);
  glColor3f (1, 1, 0); glVertex3f ( 1,  1, -1);
  glColor3f (1, 0, 0); glVertex3f ( 1, -1, -1);

  glColor3f (0, 0, 1); glVertex3f (-1, -1,  1);
  glColor3f (0, 1, 1); glVertex3f (-1,  1,  1);
  glColor3f (1, 1, 1); glVertex3f ( 1,  1,  1);
  glColor3f (1, 0, 1); glVertex3f ( 1, -1,  1);

  /* No more quads */
  glEnd ();

  /* End */
  glFlush ();

  glDrawBuffer(GL_BACK_RIGHT);

  /* Clear the buffer, clear the matrix */
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();


  /* A step backward, then spin the cube */
#ifdef USE_TRACKD
  /* A step backward, then spin the cube */
  glTranslatef (0.1+s->x, -1.5-(s->z/2), -14+(s->y/2));
#else
  glTranslatef(0.1+mouseX, -1.5-(mouseY/2), -14);
#endif
  
//  printf("%f %f %f\n",s->x,-3.5-(s->z/2),s->y);
//  glTranslatef (0, 0, -10.0);
  glRotatef (30, 1, 0, 0);
  glRotatef (alpha, 0, 1, 0);

  /* We tell we want to draw quads */
  glBegin (GL_QUADS);

  /* Every four calls to glVertex, a quad is drawn */
  glColor3f (0, 0, 0); glVertex3f (-1, -1, -1);
  glColor3f (0, 0, 1); glVertex3f (-1, -1,  1);
  glColor3f (0, 1, 1); glVertex3f (-1,  1,  1);
  glColor3f (0, 1, 0); glVertex3f (-1,  1, -1);

  glColor3f (1, 0, 0); glVertex3f ( 1, -1, -1);
  glColor3f (1, 0, 1); glVertex3f ( 1, -1,  1);
  glColor3f (1, 1, 1); glVertex3f ( 1,  1,  1);
  glColor3f (1, 1, 0); glVertex3f ( 1,  1, -1);

  glColor3f (0, 0, 0); glVertex3f (-1, -1, -1);
  glColor3f (0, 0, 1); glVertex3f (-1, -1,  1);
  glColor3f (1, 0, 1); glVertex3f ( 1, -1,  1);
  glColor3f (1, 0, 0); glVertex3f ( 1, -1, -1);

  glColor3f (0, 1, 0); glVertex3f (-1,  1, -1);
  glColor3f (0, 1, 1); glVertex3f (-1,  1,  1);
  glColor3f (1, 1, 1); glVertex3f ( 1,  1,  1);
  glColor3f (1, 1, 0); glVertex3f ( 1,  1, -1);

  glColor3f (0, 0, 0); glVertex3f (-1, -1, -1);
  glColor3f (0, 1, 0); glVertex3f (-1,  1, -1);
  glColor3f (1, 1, 0); glVertex3f ( 1,  1, -1);
  glColor3f (1, 0, 0); glVertex3f ( 1, -1, -1);

  glColor3f (0, 0, 1); glVertex3f (-1, -1,  1);
  glColor3f (0, 1, 1); glVertex3f (-1,  1,  1);
  glColor3f (1, 1, 1); glVertex3f ( 1,  1,  1);
  glColor3f (1, 0, 1); glVertex3f ( 1, -1,  1);

  /* No more quads */
  glEnd ();

  /* Rotate a bit more */
  alpha = alpha + 0.1;

  /* End */
  glFlush ();


  glutSwapBuffers ();

  /* Update again and again */
  glutPostRedisplay();
}

/*
** Function called when the window is created or resized
*/
void		ReshapeFunc (int width, int height)
{
  glMatrixMode(GL_PROJECTION);

  glLoadIdentity ();
  gluPerspective (20, width / (float) height, 5, 15);
  glViewport (0, 0, width, height);

  glMatrixMode(GL_MODELVIEW);
  glutPostRedisplay();
}

/*
** Function called when a key is hit
*/
void		KeyboardFunc (unsigned char key, int x, int y)
{
  int foo;

  foo = x + y; /* Has no effect: just to avoid a warning */
  if ('q' == key || 'Q' == key || 27 == key)
      exit (0);

}

#ifndef USE_TRACKD
void PassiveMotionFunc(int x, int y)
{
	mouseX = (250 - x) * .005;
	mouseY = (250 - y) * .005;
}
#endif

int		main_spinning_cube (int argc, char **argv)
{
  /* Creation of the window */
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STEREO);
  glutInitWindowSize (500, 500);
  glutCreateWindow ("Spinning cube");

  /* OpenGL settings */
  glClearColor (0, 0, 0, 0);
  glEnable (GL_DEPTH_TEST);
  glClearColor (0, 0, 0, 0);

  /* Declaration of the callbacks */
  glutDisplayFunc (&DisplayFunc);
  glutReshapeFunc (&ReshapeFunc);
  glutKeyboardFunc (&KeyboardFunc);
  
#ifdef USE_TRACKD
  /* Open the tracker */
  tracker = initTracker(4126);
#else
	glutPassiveMotionFunc(&PassiveMotionFunc);
#endif

  /* Loop */
  glutMainLoop ();

  /* Never reached */
  return 0;
}
