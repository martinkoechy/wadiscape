/*
 *  herbs.h
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "vegetation.h"
#include "water.h"

#ifndef HERBS_H
#define HERBS_H

class HERB: public VEGETATION
{
public:
	HERB (void) {};
	HERB (CELL*);
	~HERB(){};

	static unsigned long int sum_seeds_prod_grid;
	static float q10Inds [13];
	static float q30Inds [13];
	static float q50Inds [13];
	static float q70Inds [13];
	static float q90Inds [13];

	float mass_kgm2;
	float cover;
	float height_m;
	float zq; ///< random number used for determining the quantile of mass production
	float zc; ///< random number used for determining the quantile of cover

	unsigned long seeds_produced_m2;
	unsigned long seedbank_m2;
	unsigned long max_individuals_m2;

	float leaf_mass[12];

	HERB* initialize (climate_names c, CELL*);

	void grow (float available_water_mm, float woody_cover);
	void grow_year (void);
	bool grow_month(WATER* W, month_names m);
	unsigned long int prod_seeds_cell (void);
	void mortality_seedbank (void);

	unsigned long int get_seeds_produced_m2 (void);
	unsigned long int get_seedbank_m2 (void);
	
	void add_to_seedbank (unsigned long int number_cell);

	/// Subtract the number of germinated seeds from the seed bank
	void reduce_seedbank (void); 

	unsigned long int calc_n_annual_individuals_m2 (void);

	/// Calculate leaf area index (m<sup>2</sup>/m<sup>2</sup>) in a monthly time step
	float calc_LAI_rel(WATER* W, month_names m);
	
	/// Calculate the cover ([0;1]) in a monthly time step
	float calc_cover_rel (void);
	
	/// Calculate the height (m) in a monthly time step
	inline float calc_height_m(void);
};

#endif

