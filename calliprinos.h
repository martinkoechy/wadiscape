/*
 *  calliprinos.h
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "woodytree.h"
#include "water.h"
#include "main.h"

#ifndef CALLIPRINOS_H
#define CALLIPRINOS_H

class CALLIPRINOS:public TREE {
public:
	CALLIPRINOS(void);
	CALLIPRINOS(CELL*);
	CALLIPRINOS(float height, float radius_base, float radius_top, int age, CELL*, int n);
	~CALLIPRINOS(){};
	
	CALLIPRINOS* initialize(climate_names c, CELL*);
	float get_LAI_rel(month_names);
	float get_leaf_area_m2 (month_names month);
	float get_aboveground_mass_g(void);
	bool grow_month(WATER *W, month_names month);
	void update_shoot_mass(void);
	void update_leaf_mass(float DBH);
	float get_transpiration_lm2mo (month_names m);
	void transpiration (WATER * Wc, month_names m, float amount, float portion);
	void acorn_production(float mrain);
	float get_feed_mass_g(void);
	float removeMassByForaging(month_names month, float demand);
	bool mortality(float herb_mass_gm2, int adult_trees, int saplings);
	
	// CALLIPRINOS* initialize_new_seedling(CELL*, int n); // no longer used as of 6.4.1
	// CALLIPRINOS* initialize_new_sapling(CELL*, TREE*); // no longer used as of 6.4.1
	bool burnTree(float herb_mass_gm2);
	bool cutTree(float height_m);
	species_names getTreeType(void);
	
	static float desiccation_sensitivity;
};


#endif

