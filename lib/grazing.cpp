/*
 *  grazing.cpp
 *  MKLandscape
 *
 *  Created by Forschung on Fri Jun 23 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */

#include <math.h>
#include "grazing.h"

extern long N_cells, S_cells, W_cells, P_cells;
extern int N_edge, S_edge, W_edge, P_edge;

void foraging(CELL* C, float lsu, WATER* W, month_names month) {

	std::cout << "g";
	VEGETATION::S_sum_veg_mass_kg = 0.0;
	VEGETATION::N_sum_veg_mass_kg = 0.0;
	VEGETATION::W_sum_veg_mass_kg = 0.0;
	VEGETATION::P_sum_veg_mass_kg = 0.0;
	
	VEGETATION::S_monthly_massconsumed_kg_alllayers[month] = 0.0;
	VEGETATION::N_monthly_massconsumed_kg_alllayers[month] = 0.0;
	VEGETATION::W_monthly_massconsumed_kg_alllayers[month] = 0.0;
	VEGETATION::P_monthly_massconsumed_kg_alllayers[month] = 0.0;
	

	for (int iz = 0; iz<NZ; iz++) {
		for (int is = 0; is < NS; is == P_edge ? (is = S_edge+1) : is++)
			// plateau
			VEGETATION::P_sum_veg_mass_kg += C[iz*NZ+is].get_feedM_kg(month);
		for (int is = P_edge+1; is <= N_edge; is++)
			// N slope
			VEGETATION::N_sum_veg_mass_kg += C[iz*NZ+is].get_feedM_kg(month);
		for (int is = N_edge+1; is <= W_edge; is++)
			// wadi
			VEGETATION::W_sum_veg_mass_kg += C[iz*NZ+is].get_feedM_kg(month);
		for (int is = W_edge+1; is <= S_edge; is++)
			// S slope
			VEGETATION::S_sum_veg_mass_kg += C[iz*NZ+is].get_feedM_kg(month);
	}

	// calculation of total demand per month assuming 10 kg/(ha * lsu * day)
	const float conversion_factor = 365.0/12.0 *  10  *cell_area_m2 * 1e-4; // to kg/(m2 month)
	float S_total_monthly_demand_kg = lsu * S_cells * conversion_factor;
	float N_total_monthly_demand_kg = lsu * N_cells * conversion_factor;
	float W_total_monthly_demand_kg = lsu * W_cells * conversion_factor;
	float P_total_monthly_demand_kg = lsu * P_cells * conversion_factor;

	// calculate the relative monthly demand per cell assuming constant grazing to meet demand,
	// the value can range between +∞ to 0
	float S_mean_monthly_demand_kg_per_cell = 
		VEGETATION::S_sum_veg_mass_kg ? S_total_monthly_demand_kg/VEGETATION::S_sum_veg_mass_kg : 0.0;
	float N_mean_monthly_demand_kg_per_cell = 
		VEGETATION::N_sum_veg_mass_kg ? N_total_monthly_demand_kg/VEGETATION::N_sum_veg_mass_kg : 0.0;
	float W_mean_monthly_demand_kg_per_cell = 
		VEGETATION::W_sum_veg_mass_kg ? W_total_monthly_demand_kg/VEGETATION::W_sum_veg_mass_kg : 0.0;
	float P_mean_monthly_demand_kg_per_cell = 
		VEGETATION::P_sum_veg_mass_kg ? P_total_monthly_demand_kg/VEGETATION::P_sum_veg_mass_kg : 0.0;

	float actual_monthly_demand_kg_per_cell = 0.0;
	float consumption = 0.0;
	const float search_effort = 1.0; // number of times that the animals search the area
	int iz = -1, is = -1;

	// ******************** Foraging on South slope *************************

	long graze_attempts = 0;

	while (VEGETATION::S_monthly_massconsumed_kg_alllayers[month] < S_total_monthly_demand_kg &&
		   graze_attempts < search_effort * S_cells &&
		   VEGETATION::S_monthly_massconsumed_kg_alllayers[month] < VEGETATION::S_sum_veg_mass_kg) // 2006-06-15: Laufbedingung von ODER zu UND geaendert
	{
		// find a random cell within habitat area
		iz = int(z01()*NZ);
		is = W_edge + 1 + int(z01() * (S_edge-W_edge)); // nur Suedhang betrachten

		//grazing intensity at patch level, the value can range between one and almost zero
		// mean(2*U[0,1)) = 1.0
		actual_monthly_demand_kg_per_cell = min(1.0, 2*z01()*S_mean_monthly_demand_kg_per_cell);

		consumption = C[iz*NZ+is].forage(actual_monthly_demand_kg_per_cell, month);
		VEGETATION::S_monthly_massconsumed_kg_alllayers[month] += consumption;
		graze_attempts++;

	} // end while South
	// ******************** Foraging on North slope *************************

	graze_attempts = 0;

	while (VEGETATION::N_monthly_massconsumed_kg_alllayers[month] < N_total_monthly_demand_kg &&
		   graze_attempts < search_effort * N_cells &&
		   VEGETATION::N_monthly_massconsumed_kg_alllayers[month] < VEGETATION::N_sum_veg_mass_kg) // 2006-06-15: Laufbedingung von ODER zu UND gendert
	{
		// find a random cell within N facing area
		iz = int(z01()*NZ);
		is = P_edge + 1 + int(z01() * (N_edge-P_edge)); // nur Nordhang betrachten

		//grazing intensity at patch level, the value can range between one and almost zero
		actual_monthly_demand_kg_per_cell = min(1.0, 2*z01()*N_mean_monthly_demand_kg_per_cell);

		consumption = C[iz*NZ+is].forage(actual_monthly_demand_kg_per_cell, month);
			VEGETATION::N_monthly_massconsumed_kg_alllayers[month] += consumption;
		graze_attempts++;

	} // end while North

	// ******************** Foraging on Wadi *************************
	graze_attempts = 0;
	while (VEGETATION::W_monthly_massconsumed_kg_alllayers[month] < W_total_monthly_demand_kg &&
		   graze_attempts < search_effort * W_cells &&
		   VEGETATION::W_monthly_massconsumed_kg_alllayers[month] < VEGETATION::W_sum_veg_mass_kg) // 2006-06-15: Laufbedingung von ODER zu UND geaendert
	{
		// find a random cell within wadi area
		iz = int(z01()*NZ);
		is = N_edge + 1 + int(z01() * (W_edge-N_edge));

		//grazing intensity at patch level, the value can range between one and almost zero
		// mean(2*U[0,1)) = 1.0
		actual_monthly_demand_kg_per_cell = min(1.0, 2*z01()*W_mean_monthly_demand_kg_per_cell);

		consumption = C[iz*NZ+is].forage(actual_monthly_demand_kg_per_cell, month);
			VEGETATION::W_monthly_massconsumed_kg_alllayers[month] += consumption;
		graze_attempts++;

	} // end while Wadi

	// ******************** Foraging on Plateau *************************
	graze_attempts = 0;
	while (VEGETATION::P_monthly_massconsumed_kg_alllayers[month] < P_total_monthly_demand_kg &&
		   graze_attempts < search_effort * P_cells &&
		   VEGETATION::P_monthly_massconsumed_kg_alllayers[month] < VEGETATION::P_sum_veg_mass_kg) // 2006-06-15: Laufbedingung von ODER zu UND gendert
	{
		iz = int(z01()*NZ);
		if (z01()<0.50)
			is = int(z01() * (P_edge+1)); // nur Plateaus betrachten
		else
			is = S_edge + 1 + int(z01() * (NS-S_edge-1)); // nur Plateaus betrachten

		//grazing intensity at patch level, the value can range between one and almost zero
		actual_monthly_demand_kg_per_cell = min(1.0, 2*z01()*P_mean_monthly_demand_kg_per_cell);

		consumption = C[iz*NZ+is].forage(actual_monthly_demand_kg_per_cell, month);
		VEGETATION::P_monthly_massconsumed_kg_alllayers[month] += consumption;
		graze_attempts++;

	} // end while plateau
}

