/*
 *  year.h
 *  MKLandscape
 *
 *  Created by Forschung on Fri Jun 23 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */
#include "rain.h"
#include "ReGen.h"
#include "grazing.h"
#include "fire.h"
#include "temperature.h"

#ifndef YEAR_GLOWA_H
#define YEAR_GLOWA_H

float year (int yy, WATER* W, CELL* C, FIRE_PARAMS* pFireParams, float lsu, RAINSERIES* pTS, 
							float* mrain, uint32* sID, CLIMATE& grid_climate, const char*, 
							const bool fire, const bool cut, month_names& which_month, bool treesim);

unsigned long randInt(float f);

#endif
