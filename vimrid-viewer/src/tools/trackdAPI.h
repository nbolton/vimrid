/** trackd API Header file for programmer reference.
 *
 * NOTE: This header file is purely for programmer reference, and should
 * not be deleted, even though it isn't referred to by the application.
 */

/******************************************************************************
 * trackdAPI.h                                                                *
 *                                                                            *
 *   Interface to the VRCO trackd daemon                                      *
 *                                                                            *
 * (C) See3D Visualisation                                                    *
 ******************************************************************************/

#include <sys/types.h>

typedef struct {
  float x,y,z;
  float azim, elev, roll;
  u_int32_t timestamp[2];
  int32_t cal;
  int32_t frame;
} TrackerSensor;

void * initTracker(int key);
void releaseTracker(void *tracker);
int getNumSensors(void *tracker);
TrackerSensor * getSensor(void *tracker, int sensor);

void * initController(int key);
void releaseController(void *controller);
int getNumValuators(void *controller);
int getNumButtons(void *controller);
int getButton(void *controller, int button);
float getValuator(void *controller, int valnum);
