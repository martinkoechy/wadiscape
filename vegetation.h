/*
 *  vegetation.h
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#ifndef VEGETATATION_H
#define VEGETATATION_H

#include "main.h"
#include "water.h"

extern RNG* zZzo;

class CELL; // advance declaration

class VEGETATION
{
public:
	VEGETATION (void);
	VEGETATION (CELL* pContaining_cell);
	virtual ~VEGETATION (){};
	
	//@{
	/** All vegetation types must supply these functions. Therefore they are 
		'pure virtual functions'.*/
	virtual bool grow_month (WATER *W, month_names month)=0;
	virtual VEGETATION* initialize (climate_names c, CELL*)=0;
	//@}
	
	//@{
	/**  Access function for each vegetation type */
	virtual float get_coverM_rel (month_names month);
	virtual float get_heightM_m (month_names month);
	virtual float get_feedM_kg (month_names month);
	virtual float get_LAIM_rel (month_names month);
	virtual float get_coverMax_rel(void);
	//@}
	
	void removeMassByForaging (month_names month, float mass);
	
	static climate_names our_climate;
	
	static float S_sum_veg_mass_kg; 
	static float N_sum_veg_mass_kg; 
	static float W_sum_veg_mass_kg; 
	static float P_sum_veg_mass_kg;
	static float S_monthly_massconsumed_kg_alllayers[12]; 
	static float N_monthly_massconsumed_kg_alllayers[12]; 
	static float W_monthly_massconsumed_kg_alllayers[12]; 
	static float P_monthly_massconsumed_kg_alllayers[12];
	
	static float S_annual_sum_mass_consumed_kg;
	static float N_annual_sum_mass_consumed_kg;
	static float W_annual_sum_mass_consumed_kg;
	static float P_annual_sum_mass_consumed_kg;

	static float S_monthly_cover_herblayer[out_month_length]; 
	static float S_monthly_mass_herblayer[out_month_length]; 
	static float S_monthly_height_herblayer[out_month_length];
	static float S_monthly_LAI_herblayer[out_month_length];
	static float S_monthly_cover_shrublayer[out_month_length];
	static float S_monthly_mass_shrublayer[out_month_length];
	static float S_monthly_height_shrublayer[out_month_length];
	static float S_monthly_LAI_shrublayer[out_month_length];
	static float S_monthly_cover_treelayer[out_month_length];
	static float S_monthly_cover_QItreelayer[out_month_length];
	static float S_monthly_cover_QCtreelayer[out_month_length];
	static float S_monthly_mass_treelayer[out_month_length];
	static float S_monthly_height_treelayer[out_month_length];
	static float S_monthly_LAI_treelayer[out_month_length];
	static float S_monthly_cover_alllayers[out_month_length];
	static float S_monthly_mass_alllayers[out_month_length];
	static float S_monthly_height_alllayers[out_month_length];
	static float S_monthly_LAI_alllayers[out_month_length];
	
	static float N_monthly_cover_herblayer[out_month_length];
	static float N_monthly_mass_herblayer[out_month_length];
	static float N_monthly_height_herblayer[out_month_length];
	static float N_monthly_LAI_herblayer[out_month_length];
	static float N_monthly_cover_shrublayer[out_month_length];
	static float N_monthly_mass_shrublayer[out_month_length];
	static float N_monthly_height_shrublayer[out_month_length];
	static float N_monthly_LAI_shrublayer[out_month_length];
	static float N_monthly_cover_treelayer[out_month_length];
	static float N_monthly_cover_QItreelayer[out_month_length];
	static float N_monthly_cover_QCtreelayer[out_month_length];
	static float N_monthly_mass_treelayer[out_month_length];
	static float N_monthly_height_treelayer[out_month_length];
	static float N_monthly_LAI_treelayer[out_month_length];
	static float N_monthly_cover_alllayers[out_month_length];
	static float N_monthly_mass_alllayers[out_month_length];
	static float N_monthly_height_alllayers[out_month_length];
	static float N_monthly_LAI_alllayers[out_month_length];
	
	static float W_monthly_cover_herblayer[out_month_length];
	static float W_monthly_mass_herblayer[out_month_length];
	static float W_monthly_height_herblayer[out_month_length];
	static float W_monthly_LAI_herblayer[out_month_length];
	static float W_monthly_cover_shrublayer[out_month_length];
	static float W_monthly_mass_shrublayer[out_month_length];
	static float W_monthly_height_shrublayer[out_month_length];
	static float W_monthly_LAI_shrublayer[out_month_length];
	static float W_monthly_cover_treelayer[out_month_length];
	static float W_monthly_cover_QItreelayer[out_month_length];
	static float W_monthly_cover_QCtreelayer[out_month_length];
	static float W_monthly_mass_treelayer[out_month_length];
	static float W_monthly_height_treelayer[out_month_length];
	static float W_monthly_LAI_treelayer[out_month_length];
	static float W_monthly_cover_alllayers[out_month_length];
	static float W_monthly_mass_alllayers[out_month_length];
	static float W_monthly_height_alllayers[out_month_length];
	static float W_monthly_LAI_alllayers[out_month_length];
	
	static float P_monthly_cover_herblayer[out_month_length];
	static float P_monthly_mass_herblayer[out_month_length];
	static float P_monthly_height_herblayer[out_month_length];
	static float P_monthly_LAI_herblayer[out_month_length];
	static float P_monthly_cover_shrublayer[out_month_length];
	static float P_monthly_mass_shrublayer[out_month_length];
	static float P_monthly_height_shrublayer[out_month_length];
	static float P_monthly_LAI_shrublayer[out_month_length];
	static float P_monthly_cover_treelayer[out_month_length];
	static float P_monthly_cover_QItreelayer[out_month_length];
	static float P_monthly_cover_QCtreelayer[out_month_length];
	static float P_monthly_mass_treelayer[out_month_length];
	static float P_monthly_height_treelayer[out_month_length];
	static float P_monthly_LAI_treelayer[out_month_length];
	static float P_monthly_cover_alllayers[out_month_length];
	static float P_monthly_mass_alllayers[out_month_length];
	static float P_monthly_height_alllayers[out_month_length];
	static float P_monthly_LAI_alllayers[out_month_length];
	
protected:
	friend void monthly_summary_output(int ll, climate_names cc, int yy, float LSU_km2, float* monthly_rain, scenario_names sc, WATER*, CELL*, char* file_name);
	float feedM_kgcell[12]; ///< mass (kg) of leaves and herbaceous material below 1.5 m height available to goats and sheep
	float coverM_rel[12]; ///< relative cover
	float heightM_m[12]; ///<  height (m)
	float LAIM_rel[12]; ///< leaf area index
	float palatability; ///< a factor for the palatability of the vegetation type
	CELL* the_pCell; ///< a pointer to the containing cell
	float cumulated_forage_loss_kgcell;
};

#endif

