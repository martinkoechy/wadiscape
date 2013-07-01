/*
 *  vegetation.cpp
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "vegetation.h"

float VEGETATION::S_sum_veg_mass_kg = 0.0;
float VEGETATION::N_sum_veg_mass_kg = 0.0;
float VEGETATION::W_sum_veg_mass_kg = 0.0;
float VEGETATION::P_sum_veg_mass_kg = 0.0;

float VEGETATION::S_monthly_massconsumed_kg_alllayers[12];
float VEGETATION::N_monthly_massconsumed_kg_alllayers[12];
float VEGETATION::W_monthly_massconsumed_kg_alllayers[12];
float VEGETATION::P_monthly_massconsumed_kg_alllayers[12];

float VEGETATION::S_annual_sum_mass_consumed_kg = 0.0;
float VEGETATION::N_annual_sum_mass_consumed_kg = 0.0;
float VEGETATION::W_annual_sum_mass_consumed_kg = 0.0;
float VEGETATION::P_annual_sum_mass_consumed_kg = 0.0;

float VEGETATION::S_monthly_cover_herblayer[out_month_length]; 
float VEGETATION::S_monthly_mass_herblayer[out_month_length]; 
float VEGETATION::S_monthly_height_herblayer[out_month_length];
float VEGETATION::S_monthly_LAI_herblayer[out_month_length];
float VEGETATION::S_monthly_cover_shrublayer[out_month_length];
float VEGETATION::S_monthly_mass_shrublayer[out_month_length];
float VEGETATION::S_monthly_height_shrublayer[out_month_length];
float VEGETATION::S_monthly_LAI_shrublayer[out_month_length];
float VEGETATION::S_monthly_cover_treelayer[out_month_length];
float VEGETATION::S_monthly_cover_QItreelayer[out_month_length];
float VEGETATION::S_monthly_cover_QCtreelayer[out_month_length];
float VEGETATION::S_monthly_mass_treelayer[out_month_length];
float VEGETATION::S_monthly_height_treelayer[out_month_length];
float VEGETATION::S_monthly_LAI_treelayer[out_month_length];
float VEGETATION::S_monthly_cover_alllayers[out_month_length];
float VEGETATION::S_monthly_mass_alllayers[out_month_length];
float VEGETATION::S_monthly_height_alllayers[out_month_length];
float VEGETATION::S_monthly_LAI_alllayers[out_month_length];

float VEGETATION::N_monthly_cover_herblayer[out_month_length];
float VEGETATION::N_monthly_mass_herblayer[out_month_length];
float VEGETATION::N_monthly_height_herblayer[out_month_length];
float VEGETATION::N_monthly_LAI_herblayer[out_month_length];
float VEGETATION::N_monthly_cover_shrublayer[out_month_length];
float VEGETATION::N_monthly_mass_shrublayer[out_month_length];
float VEGETATION::N_monthly_height_shrublayer[out_month_length];
float VEGETATION::N_monthly_LAI_shrublayer[out_month_length];
float VEGETATION::N_monthly_cover_treelayer[out_month_length];
float VEGETATION::N_monthly_cover_QItreelayer[out_month_length];
float VEGETATION::N_monthly_cover_QCtreelayer[out_month_length];
float VEGETATION::N_monthly_mass_treelayer[out_month_length];
float VEGETATION::N_monthly_height_treelayer[out_month_length];
float VEGETATION::N_monthly_LAI_treelayer[out_month_length];
float VEGETATION::N_monthly_cover_alllayers[out_month_length];
float VEGETATION::N_monthly_mass_alllayers[out_month_length];
float VEGETATION::N_monthly_height_alllayers[out_month_length];
float VEGETATION::N_monthly_LAI_alllayers[out_month_length];

float VEGETATION::W_monthly_cover_herblayer[out_month_length];
float VEGETATION::W_monthly_mass_herblayer[out_month_length];
float VEGETATION::W_monthly_height_herblayer[out_month_length];
float VEGETATION::W_monthly_LAI_herblayer[out_month_length];
float VEGETATION::W_monthly_cover_shrublayer[out_month_length];
float VEGETATION::W_monthly_mass_shrublayer[out_month_length];
float VEGETATION::W_monthly_height_shrublayer[out_month_length];
float VEGETATION::W_monthly_LAI_shrublayer[out_month_length];
float VEGETATION::W_monthly_cover_treelayer[out_month_length];
float VEGETATION::W_monthly_cover_QItreelayer[out_month_length];
float VEGETATION::W_monthly_cover_QCtreelayer[out_month_length];
float VEGETATION::W_monthly_mass_treelayer[out_month_length];
float VEGETATION::W_monthly_height_treelayer[out_month_length];
float VEGETATION::W_monthly_LAI_treelayer[out_month_length];
float VEGETATION::W_monthly_cover_alllayers[out_month_length];
float VEGETATION::W_monthly_mass_alllayers[out_month_length];
float VEGETATION::W_monthly_height_alllayers[out_month_length];
float VEGETATION::W_monthly_LAI_alllayers[out_month_length];

float VEGETATION::P_monthly_cover_herblayer[out_month_length];
float VEGETATION::P_monthly_mass_herblayer[out_month_length];
float VEGETATION::P_monthly_height_herblayer[out_month_length];
float VEGETATION::P_monthly_LAI_herblayer[out_month_length];
float VEGETATION::P_monthly_cover_shrublayer[out_month_length];
float VEGETATION::P_monthly_mass_shrublayer[out_month_length];
float VEGETATION::P_monthly_height_shrublayer[out_month_length];
float VEGETATION::P_monthly_LAI_shrublayer[out_month_length];
float VEGETATION::P_monthly_cover_treelayer[out_month_length];
float VEGETATION::P_monthly_cover_QItreelayer[out_month_length];
float VEGETATION::P_monthly_cover_QCtreelayer[out_month_length];
float VEGETATION::P_monthly_mass_treelayer[out_month_length];
float VEGETATION::P_monthly_height_treelayer[out_month_length];
float VEGETATION::P_monthly_LAI_treelayer[out_month_length];
float VEGETATION::P_monthly_cover_alllayers[out_month_length];
float VEGETATION::P_monthly_mass_alllayers[out_month_length];
float VEGETATION::P_monthly_height_alllayers[out_month_length];
float VEGETATION::P_monthly_LAI_alllayers[out_month_length];

VEGETATION::VEGETATION(void) {
	palatability = 1.0;
	cumulated_forage_loss_kgcell = 0.0;
	for (short m = 0; m < 12; m++) {
		LAIM_rel[m] = 0.01;
		feedM_kgcell[m] = 0.0;
		coverM_rel[m] = 0.0;
		heightM_m[m] = 0.0;
	}	
}

VEGETATION::VEGETATION (CELL* pContaining_cell): the_pCell(pContaining_cell)
{
	palatability = 1.0;
	cumulated_forage_loss_kgcell = 0.0;
	for (short m = 0; m < 12; m++) {
		LAIM_rel[m] = 0.01;
		feedM_kgcell[m] = 0.0;
		coverM_rel[m] = 0.0;
		heightM_m[m] = 0.0;
	}
}

/** @name AccessFunctionsV
*	These functions are inherited by each vegetation type and return the 
monthly value
*/
//@{
/** Get the relative cover [0;1].*/
float VEGETATION::get_coverM_rel(month_names m) {
	return coverM_rel[int(m)];
}

/** Get the the height (m).*/
float VEGETATION::get_heightM_m(month_names m) {
	return heightM_m[int(m)];
}

/** Get the mass of leaves and herbaceous material at &lt; 1.5 m height */
float VEGETATION::get_feedM_kg(month_names month) {
	return feedM_kgcell[int(month)];
}

/** Get the leaf area index (m<sup>2</sup>/m<sup>2</sup>) */
float VEGETATION::get_LAIM_rel(month_names month) {
	return LAIM_rel[int(month)];
}

float VEGETATION::get_coverMax_rel(void) {
	float maxCover = 0.0;
	for(short m = 0; m<12; m++)
		maxCover = coverM_rel[m]>maxCover?coverM_rel[m]:maxCover;
	return maxCover;
}

void VEGETATION::removeMassByForaging(month_names m, float demand_mass) {
	if (feedM_kgcell[int(m)] < demand_mass * palatability)
		demand_mass = feedM_kgcell[int(m)] * palatability;

	feedM_kgcell[int(m)] -= demand_mass;
	cumulated_forage_loss_kgcell += demand_mass;
}

