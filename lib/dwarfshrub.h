/*
 *  dwarfshrub.h
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "woodytree.h"
#include "water.h"
#include "main.h"

#ifndef DWARFSHRUB_H
#define DWARFSHRUB_H

class DWARFSHRUB: public WOODY
{  
public:
	DWARFSHRUB (void);
	DWARFSHRUB (CELL*);
	~DWARFSHRUB(){};
	
	float cover; ///< the cover in and after May
	float delta_cover; ///< the cover produced in May
	float colonized_cov;
	
	DWARFSHRUB* initialize (climate_names c, CELL*);
	
	void grow_year(WATER* W);
	bool grow_month (WATER *W, month_names month);
	void colonize (short column, short row, float water_availability, CELL* C);
	
	float convert_cover_to_leaf_mass_gCell(float cover);
	float convert_leaf_mass_gCell_to_cover(float leaf_mass_g);
	float calc_height_m (float cover);
	float calc_LAI_rel(month_names month);

	/// Calculates the leaf mass (dry mass, g/m<sup>2</sup>) per square metre of ground
	//  float calc_leafmass_g_per_m2(month_names m, float cover);
	
	/// Calculate leaf area index (m<sup>2</sup>/m<sup>2</sup>)
	float get_LAI_rel(month_names);
};


#endif
