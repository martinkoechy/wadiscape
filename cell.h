/*
 *  cell.h
 *  Landschaft
 *
 *  Created by Forschung on Tue Jun 20 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */

#include <list>
#include "main.h"
#include "water.h"
#include "vegetation.h"
#include "woodytree.h"
#include "calliprinos.h"
#include "ithaburensis.h"
//#include "shrubtree.h"
#include "dwarfshrub.h"
#include "herbs.h"

#ifndef CELLINGRID_H
#define CELLINGRID_H

extern RNG* zZzo;


struct CELL
{
	CALLIPRINOS* pCalliprinos;
	ITHABURENSIS* pIthaburensis;
	std::list<TREE*> treeIndividual;
	//SHRUB shrubs;
	DWARFSHRUB dwarfshrubs;
	HERB herbs;
	
	CELL(void);
	~CELL();
	unsigned long int calliprinosSeedbank;
	unsigned long int ithaburensisSeedbank;
	float totalTreeCover;
	float calliprinos_germination_fraction;	///< fraction [0,1] of germinated seeds
	float ithaburensis_germination_fraction;
	
	bool fireHistory[5];

	void updateTimeSinceFire(void);
	
	friend void WATER::evapotranspiration(CELL*, month_names month);
		
	/// calculate the total relative tree cover of a cell [0,1]
	void calc_tree_coverM_rel (month_names month);
	
	/// Returns the relative tree cover
	float get_tree_coverM_rel(month_names month);
	float get_calliprinos_coverM_rel(month_names month);
	float get_ithaburensis_coverM_rel(month_names month);
	
	/// Returns the weighted tree height (m)
	float get_tree_heightM_m(month_names month);

	/// Returns the tree leaf mass (kg) below the browsable height, this is the mass available to goats
	float get_tree_feedM_kg(month_names month);

	/// Returns the leaf area index of all trees of all life stages in the cell
	float get_tree_LAIM_rel(month_names month);

	/// Returns the cover of trees, shrubs, and dwarf shrubs in [0,1]
	float get_woody_coverM_rel(month_names month);
	
	/// Returns the leaf mass below the browsable height, this is the mass available to goats.
	float get_feedM_kg(month_names month);
	
  	/// Returns the cover of herbaceous plants in [0,1].
	float get_herb_coverM_rel(month_names month);
  
	/// Returns the total cover of vegetation in [0,1].
	float get_total_coverM_rel(month_names month);
	
	/// Returns the Strickler number of the dominant vegetation type by cover.
	float get_kStr(month_names month);
  
	/// Returns the average height of the vegetation.
	float get_heightM_m(month_names month);
  
	/// Returns the total LAI of the cell.
	float get_LAIM_rel(month_names month);
	
	/// Returns the total consumed mass of the cell.
	float get_consumed_massM_kgcell (month_names month);
  
	/// Set the initial vegetation composition of each cell.
	void initialize_composition (climate_names c);
	
	/// Remove biomass from this cell by grazing or browsing.
	float forage (float grazing_intensity, month_names month);
		
	// no longer used as of 6.4.1
	/*
	CALLIPRINOS *  initialize_new_calliprinos(int n);
	CALLIPRINOS *  initialize_new_calliprinos(TREE*);
	ITHABURENSIS *  initialize_new_ithaburensis(int n);
	ITHABURENSIS *  initialize_new_ithaburensis(TREE*);
	*/
	
	int germinateCalliprinos(WATER* W, month_names month);
	int germinateIthaburensis(WATER* W, month_names month);
	
	unsigned long int get_total_calliprinos_acorns_in_cell(void);
	unsigned long int get_total_ithaburensis_acorns_in_cell(void);
	
	void seedbank_mortality(int numberOfAdultTrees, float relativeShrubCover);
	
	float interception(float rain_mm, month_names month);
	
	float get_infiltration_rate_mmh(month_names month);
};

#endif
