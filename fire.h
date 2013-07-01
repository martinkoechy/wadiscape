/*
 *  fire.h
 *  MKLandscape
 *
 *  Created by Forschung on Fri Jun 23 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */
#include "main.h"
#include "grazing.h"
#include "cell.h"
#include <list>
#include <valarray>

extern RNG* zZzo;

#ifndef FIRE_H_
#define FIRE_H_

struct FIRE_PARAMS {
	float Vegetationsdichte;// Vegetationsparameter
	float Wassersaettigung;// Klimaparameter
	float log_alpha;// Vegetationsparameter fuer die Gleichung nach Malamud
	float beta; // Klimaparameter fuer die Gleichung nach Malamud
	float calc_burn_area_m2(void); // Feuergroesse bestimmen
	const float total_grid_area_m2;
	const float minimum_detection_area_m2;
	
	FIRE_PARAMS (void) : total_grid_area_m2(all_cells * cell_area_m2), 
		minimum_detection_area_m2 (0.004 * 1000000) // Malamud et al. 2005
	{}
};

// Feuerfunktion die die Vegetation bei Feuerausbruch vernichtet
void Feuerbrennt(CELL* C, int x, int y, int widthOfSubareaToBurn, int heightOfSubareaToBurn);

float FireDisturbance(WATER* W, CELL* C, FIRE_PARAMS* pFireParams, month_names month);

float burn_subarea(CELL* C, int x, int y, int widthOfSubareaToBurn, int heightOfSubareaToBurn);

#endif /*FIRE_H_*/
