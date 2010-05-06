/***************************************************************************
 *   Copyright (C) 2006 by Timofey Balashov                                *
 *   Timofey.Balashov@pi.uka.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/

#ifndef SCALA_H
#define SCALA_H

#include <mychar.h>
#include <malloc.h>
#include <stdio.h>

#include <format.h>

/*TSCALAINFO*/
typedef struct {

  char date[20];
  char user[10];
  char comment[60];

  float field_X_size;
  float field_Y_size;

  int pixel_X_size;
  int pixel_Y_size;

  float inc_X;
  float inc_Y;

  float angle;

  float X_offset;
  float Y_offset;

  float gap_voltage;
  float setpoint;
  float loop_gain;
  float scan_speed;
  float X_drift;
  float Y_drift;
  float topotime;
  short X_sgrid;
  short Y_sgrid;
  short X_spoints;
  short Y_slines;

  short ctf;
  short ctb;
  short csf;
  short csb;

  short sign_x;
  short sign_y;
  short sign_z;

} TSCALAINFO, *PSCALAINFO;

/*TSCALATOPO*/ 
typedef struct {

  char channel[5];
  char direction[10];
  int raw_max;
  int raw_min;
  float phys_max;
  float phys_min;
  float resolution;
  char unit[5];
  char filename[15]; // w/o number

  short sign_x;
  short sign_y;
  short sign_z;

} TSCALATOPO, *PSCALATOPO;

/*TSCALASPEC*/
typedef struct {

  char channel[5];
  char parameter[5];
  char direction[10];
  int raw_max;
  int raw_min;
  float phys_max;
  float phys_min;
  float resolution;
  char unit[5];
  short points;
  float start_x;
  float end_x;
  float inc_x;
  float spectime;
  float specdelaytime;
  char feedback[4]; // On/Off
  char filename[15]; // w/o number

  short sign_x;
  short sign_y;
  short sign_z;

} TSCALASPEC, *PSCALASPEC;

PFILE_OBJ loadSCALAFILE (char *);
PFILE_OBJ loadSCALAFILE (FILE *);
bool saveSCALAFILE (PFILE_OBJ);
bool saveSCALAFILEas (PFILE_OBJ, filename);
//void freeRHKFILE (PFILE_OBJ);

char* getSCALApages(FILE*, bool*, bool*);


#endif