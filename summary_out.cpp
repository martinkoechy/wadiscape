/*
 *  summary_out.cpp
 *  MKLandscape
 *
 *  Created by Forschung on Sun Jun 25 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */
#include <math.h>
#include "summary_out.h"
#include "main.h"

extern long N_cells, S_cells, W_cells, P_cells;
extern int N_edge, S_edge, W_edge, P_edge;
extern CELL* C;

void reset_monthly_output_sums(void) {
	for (int m = 0; m <= out_month_max - out_month_min; m++) {
		VEGETATION::S_monthly_cover_herblayer[m] = 0.0;
		VEGETATION::S_monthly_mass_herblayer[m] = 0.0;
		VEGETATION::S_monthly_height_herblayer[m] = 0.0;
		VEGETATION::S_monthly_LAI_herblayer[m] = 0.0;
		VEGETATION::S_monthly_cover_shrublayer[m] = 0.0;
		VEGETATION::S_monthly_mass_shrublayer[m] = 0.0;
		VEGETATION::S_monthly_height_shrublayer[m] = 0.0;
		VEGETATION::S_monthly_LAI_shrublayer[m] = 0.0;
		VEGETATION::S_monthly_cover_treelayer[m] = 0.0;
		VEGETATION::S_monthly_cover_QItreelayer[m] = 0.0;
		VEGETATION::S_monthly_cover_QCtreelayer[m] = 0.0;
		VEGETATION::S_monthly_mass_treelayer[m] = 0.0;
		VEGETATION::S_monthly_height_treelayer[m] = 0.0;
		VEGETATION::S_monthly_LAI_treelayer[m] = 0.0;
		VEGETATION::S_monthly_cover_alllayers[m] = 0.0;
		VEGETATION::S_monthly_mass_alllayers[m] = 0.0;
		VEGETATION::S_monthly_height_alllayers[m] = 0.0;
		VEGETATION::S_monthly_LAI_alllayers[m] = 0.0;
		WATER::S_netSoilMovement[m]=0.0;
		WATER::S_netWaterMovement[m]=0.0;

		VEGETATION::N_monthly_cover_herblayer[m] = 0.0;
		VEGETATION::N_monthly_mass_herblayer[m] = 0.0;
		VEGETATION::N_monthly_height_herblayer[m] = 0.0;
		VEGETATION::N_monthly_LAI_herblayer[m] = 0.0;
		VEGETATION::N_monthly_cover_shrublayer[m] = 0.0;
		VEGETATION::N_monthly_mass_shrublayer[m] = 0.0;
		VEGETATION::N_monthly_height_shrublayer[m] = 0.0;
		VEGETATION::N_monthly_LAI_shrublayer[m] = 0.0;
		VEGETATION::N_monthly_cover_treelayer[m] = 0.0;
		VEGETATION::N_monthly_cover_QItreelayer[m] = 0.0;
		VEGETATION::N_monthly_cover_QCtreelayer[m] = 0.0;
		VEGETATION::N_monthly_mass_treelayer[m] = 0.0;
		VEGETATION::N_monthly_height_treelayer[m] = 0.0;
		VEGETATION::N_monthly_LAI_treelayer[m] = 0.0;
		VEGETATION::N_monthly_cover_alllayers[m] = 0.0;
		VEGETATION::N_monthly_mass_alllayers[m] = 0.0;
		VEGETATION::N_monthly_height_alllayers[m] = 0.0;
		VEGETATION::N_monthly_LAI_alllayers[m] = 0.0;
		WATER::N_netSoilMovement[m]=0.0;
		WATER::N_netWaterMovement[m]=0.0;

		VEGETATION::W_monthly_cover_herblayer[m] = 0.0;
		VEGETATION::W_monthly_mass_herblayer[m] = 0.0;
		VEGETATION::W_monthly_height_herblayer[m] = 0.0;
		VEGETATION::W_monthly_LAI_herblayer[m] = 0.0;
		VEGETATION::W_monthly_cover_shrublayer[m] = 0.0;
		VEGETATION::W_monthly_mass_shrublayer[m] = 0.0;
		VEGETATION::W_monthly_height_shrublayer[m] = 0.0;
		VEGETATION::W_monthly_LAI_shrublayer[m] = 0.0;
		VEGETATION::W_monthly_cover_treelayer[m] = 0.0;
		VEGETATION::W_monthly_cover_QItreelayer[m] = 0.0;
		VEGETATION::W_monthly_cover_QCtreelayer[m] = 0.0;
		VEGETATION::W_monthly_mass_treelayer[m] = 0.0;
		VEGETATION::W_monthly_height_treelayer[m] = 0.0;
		VEGETATION::W_monthly_LAI_treelayer[m] = 0.0;
		VEGETATION::W_monthly_cover_alllayers[m] = 0.0;
		VEGETATION::W_monthly_mass_alllayers[m] = 0.0;
		VEGETATION::W_monthly_height_alllayers[m] = 0.0;
		VEGETATION::W_monthly_LAI_alllayers[m] = 0.0;
		WATER::W_netSoilMovement[m]=0.0;
		WATER::W_netWaterMovement[m]=0.0;

		VEGETATION::P_monthly_cover_herblayer[m] = 0.0;
		VEGETATION::P_monthly_mass_herblayer[m] = 0.0;
		VEGETATION::P_monthly_height_herblayer[m] = 0.0;
		VEGETATION::P_monthly_LAI_herblayer[m] = 0.0;
		VEGETATION::P_monthly_cover_shrublayer[m] = 0.0;
		VEGETATION::P_monthly_mass_shrublayer[m] = 0.0;
		VEGETATION::P_monthly_height_shrublayer[m] = 0.0;
		VEGETATION::P_monthly_LAI_shrublayer[m] = 0.0;
		VEGETATION::P_monthly_cover_treelayer[m] = 0.0;
		VEGETATION::P_monthly_cover_QItreelayer[m] = 0.0;
		VEGETATION::P_monthly_cover_QCtreelayer[m] = 0.0;
		VEGETATION::P_monthly_mass_treelayer[m] = 0.0;
		VEGETATION::P_monthly_height_treelayer[m] = 0.0;
		VEGETATION::P_monthly_LAI_treelayer[m] = 0.0;
		VEGETATION::P_monthly_cover_alllayers[m] = 0.0;
		VEGETATION::P_monthly_mass_alllayers[m] = 0.0;
		VEGETATION::P_monthly_height_alllayers[m] = 0.0;
		VEGETATION::P_monthly_LAI_alllayers[m] = 0.0;
		WATER::P_netSoilMovement[m]=0.0;
		WATER::P_netWaterMovement[m]=0.0;
	}
	
	VEGETATION::P_annual_sum_mass_consumed_kg = 0.0;
	VEGETATION::N_annual_sum_mass_consumed_kg = 0.0;
	VEGETATION::W_annual_sum_mass_consumed_kg = 0.0;
	VEGETATION::S_annual_sum_mass_consumed_kg = 0.0;
}

/**
 The statistics values represent the whole cell area, i.e., the values must be
 converted to m² if required.
 */
void calculate_monthly_statistics(CELL* C, WATER* W) {
	month_names mn;
	int c = 0;
	for (int iz = 0; iz < NZ; iz++) {
		int is = 0, m = 0;

		// plateau
		for (is = 0; is < NS; is == P_edge ? (is = S_edge + 1) : is++) {
			for (m = 0; m <= out_month_max-out_month_min; m++) {
				mn = month_names(out_month_min + m);
				c = iz * NZ + is;

				VEGETATION::P_monthly_cover_herblayer[m]
						+= C[c].herbs.get_coverM_rel(mn);
				VEGETATION::P_monthly_mass_herblayer[m]
						+= C[c].herbs.get_feedM_kg(mn);
				VEGETATION::P_monthly_height_herblayer[m]
						+= C[c].herbs.get_heightM_m(mn);
				VEGETATION::P_monthly_LAI_herblayer[m]
						+= C[c].herbs.get_LAIM_rel(mn);
				VEGETATION::P_monthly_cover_shrublayer[m]
						+= C[c].dwarfshrubs.get_coverM_rel(mn);
				VEGETATION::P_monthly_mass_shrublayer[m]
						+= C[c].dwarfshrubs.get_feedM_kg(mn);
				VEGETATION::P_monthly_height_shrublayer[m]
						+= C[c].dwarfshrubs.get_heightM_m(mn);
				VEGETATION::P_monthly_LAI_shrublayer[m]
						+= C[c].dwarfshrubs.get_LAIM_rel(mn);
				VEGETATION::P_monthly_cover_treelayer[m]
						+= C[c].get_tree_coverM_rel(mn);
				VEGETATION::P_monthly_cover_QItreelayer[m]
						+= C[c].get_ithaburensis_coverM_rel(mn);
				VEGETATION::P_monthly_cover_QCtreelayer[m]
						+= C[c].get_calliprinos_coverM_rel(mn);
				VEGETATION::P_monthly_mass_treelayer[m]
						+= C[c].get_tree_feedM_kg(mn);
				VEGETATION::P_monthly_height_treelayer[m]
						+= C[c].get_tree_heightM_m(mn);
				VEGETATION::P_monthly_LAI_treelayer[m]
						+= C[c].get_tree_LAIM_rel(mn);
				VEGETATION::P_monthly_cover_alllayers[m]
						+= C[c].get_total_coverM_rel(mn);
				VEGETATION::P_monthly_mass_alllayers[m]
						+= C[c].get_feedM_kg(mn);
				VEGETATION::P_monthly_height_alllayers[m]
						+= C[c].get_heightM_m(mn);
				VEGETATION::P_monthly_LAI_alllayers[m]
						+= C[c].get_LAIM_rel(mn);
				WATER::P_netSoilMovement[m] 
						+= abs(W[c].netSoilMovement[mn]);
				WATER::P_netWaterMovement[m]
						+=  abs(W[c].netWaterMovement[mn]);

			} // for each month
		}

		// North-facing slope
		for (is = P_edge + 1; is <= N_edge; is++) {
			for (m = 0; m <= out_month_max-out_month_min; m++) {
				mn = month_names(out_month_min + m);
				c = iz * NZ + is;

				VEGETATION::N_monthly_cover_herblayer[m]
						+= C[c].herbs.get_coverM_rel(mn);
				VEGETATION::N_monthly_mass_herblayer[m]
						+= C[c].herbs.get_feedM_kg(mn);
				VEGETATION::N_monthly_height_herblayer[m]
						+= C[c].herbs.get_heightM_m(mn);
				VEGETATION::N_monthly_LAI_herblayer[m]
						+= C[c].herbs.get_LAIM_rel(mn);
				VEGETATION::N_monthly_cover_shrublayer[m]
						+= C[c].dwarfshrubs.get_coverM_rel(mn);
				VEGETATION::N_monthly_mass_shrublayer[m]
						+= C[c].dwarfshrubs.get_feedM_kg(mn);
				VEGETATION::N_monthly_height_shrublayer[m]
						+= C[c].dwarfshrubs.get_heightM_m(mn);
				VEGETATION::N_monthly_LAI_shrublayer[m]
						+= C[c].dwarfshrubs.get_LAIM_rel(mn);
				VEGETATION::N_monthly_cover_treelayer[m]
						+= C[c].get_tree_coverM_rel(mn);
				VEGETATION::N_monthly_cover_QItreelayer[m]
						+= C[c].get_ithaburensis_coverM_rel(mn);
				VEGETATION::N_monthly_cover_QCtreelayer[m]
						+= C[c].get_calliprinos_coverM_rel(mn);
				VEGETATION::N_monthly_mass_treelayer[m]
						+= C[c].get_tree_feedM_kg(mn);
				VEGETATION::N_monthly_height_treelayer[m]
						+= C[c].get_tree_heightM_m(mn);
				VEGETATION::N_monthly_LAI_treelayer[m]
						+= C[c].get_tree_LAIM_rel(mn);
				VEGETATION::N_monthly_cover_alllayers[m]
						+= C[c].get_total_coverM_rel(mn);
				VEGETATION::N_monthly_mass_alllayers[m]
						+= C[c].get_feedM_kg(mn);
				VEGETATION::N_monthly_height_alllayers[m]
						+= C[c].get_heightM_m(mn);
				VEGETATION::N_monthly_LAI_alllayers[m]
						+= C[c].get_LAIM_rel(mn);
				WATER::N_netSoilMovement[m] 
						+= abs(W[c].netSoilMovement[mn]);
				WATER::N_netWaterMovement[m]
						+= abs(W[c].netWaterMovement[mn]);
			} // for each month
		}

		// Wadi
		for (is = N_edge + 1; is <= W_edge; is++) {
			for (m = 0; m <= out_month_max-out_month_min; m++) {
				mn = month_names(out_month_min + m);
				c = iz * NZ + is;
				VEGETATION::W_monthly_cover_herblayer[m]
						+= C[c].herbs.get_coverM_rel(mn);
				VEGETATION::W_monthly_mass_herblayer[m]
						+= C[c].herbs.get_feedM_kg(mn);
				VEGETATION::W_monthly_height_herblayer[m]
						+= C[c].herbs.get_heightM_m(mn);
				VEGETATION::W_monthly_LAI_herblayer[m]
						+= C[c].herbs.get_LAIM_rel(mn);
				VEGETATION::W_monthly_cover_shrublayer[m]
						+= C[c].dwarfshrubs.get_coverM_rel(mn);
				VEGETATION::W_monthly_mass_shrublayer[m]
						+= C[c].dwarfshrubs.get_feedM_kg(mn);
				VEGETATION::W_monthly_height_shrublayer[m]
						+= C[c].dwarfshrubs.get_heightM_m(mn);
				VEGETATION::W_monthly_LAI_shrublayer[m]
						+= C[c].dwarfshrubs.get_LAIM_rel(mn);
				VEGETATION::W_monthly_cover_treelayer[m]
						+= C[c].get_tree_coverM_rel(mn);
				VEGETATION::W_monthly_cover_QItreelayer[m]
						+= C[c].get_ithaburensis_coverM_rel(mn);
				VEGETATION::W_monthly_cover_QCtreelayer[m]
						+= C[c].get_calliprinos_coverM_rel(mn);
				VEGETATION::W_monthly_mass_treelayer[m]
						+= C[c].get_tree_feedM_kg(mn);
				VEGETATION::W_monthly_height_treelayer[m]
						+= C[c].get_tree_heightM_m(mn);
				VEGETATION::W_monthly_LAI_treelayer[m]
						+= C[c].get_tree_LAIM_rel(mn);
				VEGETATION::W_monthly_cover_alllayers[m]
						+= C[c].get_total_coverM_rel(mn);
				VEGETATION::W_monthly_mass_alllayers[m]
						+= C[c].get_feedM_kg(mn);
				VEGETATION::W_monthly_height_alllayers[m]
						+= C[c].get_heightM_m(mn);
				VEGETATION::W_monthly_LAI_alllayers[m]
						+= C[c].get_LAIM_rel(mn);
				WATER::W_netSoilMovement[m] 
						+= abs(W[c].netSoilMovement[mn]);
				WATER::W_netWaterMovement[m]
						+=  abs(W[c].netWaterMovement[mn]);
			} // for each month
		}

		// statistics South-facing slope
		for (is = W_edge + 1; is <= S_edge; is++) {
			for (m = 0; m <= out_month_max-out_month_min; m++) {
				c = iz * NZ + is;
				mn = month_names(out_month_min + m);

				VEGETATION::S_monthly_cover_herblayer[m]
						+= C[c].herbs.get_coverM_rel(mn);
				VEGETATION::S_monthly_mass_herblayer[m]
						+= C[c].herbs.get_feedM_kg(mn);
				VEGETATION::S_monthly_height_herblayer[m]
						+= C[c].herbs.get_heightM_m(mn);
				VEGETATION::S_monthly_LAI_herblayer[m]
						+= C[c].herbs.get_LAIM_rel(mn);
				VEGETATION::S_monthly_cover_shrublayer[m]
						+= C[c].dwarfshrubs.get_coverM_rel(mn);
				VEGETATION::S_monthly_mass_shrublayer[m]
						+= C[c].dwarfshrubs.get_feedM_kg(mn);
				VEGETATION::S_monthly_height_shrublayer[m]
						+= C[c].dwarfshrubs.get_heightM_m(mn);
				VEGETATION::S_monthly_LAI_shrublayer[m]
						+= C[c].dwarfshrubs.get_LAIM_rel(mn);
				VEGETATION::S_monthly_cover_treelayer[m]
						+= C[c].get_tree_coverM_rel(mn);
				VEGETATION::S_monthly_cover_QItreelayer[m]
						+= C[c].get_ithaburensis_coverM_rel(mn);
				VEGETATION::S_monthly_cover_QCtreelayer[m]
						+= C[c].get_calliprinos_coverM_rel(mn);
				VEGETATION::S_monthly_mass_treelayer[m]
						+= C[c].get_tree_feedM_kg(mn);
				VEGETATION::S_monthly_height_treelayer[m]
						+= C[c].get_tree_heightM_m(mn);
				VEGETATION::S_monthly_LAI_treelayer[m]
						+= C[c].get_tree_LAIM_rel(mn);
				VEGETATION::S_monthly_cover_alllayers[m]
						+= C[c].get_total_coverM_rel(mn);
				VEGETATION::S_monthly_mass_alllayers[m]
						+= C[c].get_feedM_kg(mn);
				VEGETATION::S_monthly_height_alllayers[m]
						+= C[c].get_heightM_m(mn);
				VEGETATION::S_monthly_LAI_alllayers[m]
						+= C[c].get_LAIM_rel(mn);
				WATER::S_netSoilMovement[m] 
						+= abs(W[c].netSoilMovement[mn]);
				WATER::S_netWaterMovement[m]
						+= abs(W[c].netWaterMovement[mn]);
				// transpired water !!!
			} // for each month
		}
	} // for each column

	// calculate annual values for consumed mass (kg/cell)
	for (mn = Aug; mn <= Jul; ++mn) {
		VEGETATION::P_annual_sum_mass_consumed_kg += VEGETATION::P_monthly_massconsumed_kg_alllayers[mn];
		VEGETATION::N_annual_sum_mass_consumed_kg += VEGETATION::N_monthly_massconsumed_kg_alllayers[mn];
		VEGETATION::S_annual_sum_mass_consumed_kg += VEGETATION::S_monthly_massconsumed_kg_alllayers[mn];
		VEGETATION::W_annual_sum_mass_consumed_kg += VEGETATION::W_monthly_massconsumed_kg_alllayers[mn];
	}


	const float qScells = 1.0 / float(S_cells);
	const float qNcells = 1.0 / float(N_cells);
	const float qWcells = 1.0 / float(W_cells);
	const float qPcells = 1.0 / float(P_cells);

	// Convert sums to means
	for (int m = 0; m <= out_month_max-out_month_min; m++) {
		VEGETATION::S_monthly_cover_herblayer[m] *= qScells;
		VEGETATION::S_monthly_mass_herblayer[m] *= qScells;
		VEGETATION::S_monthly_height_herblayer[m] *= qScells;
		VEGETATION::S_monthly_LAI_herblayer[m] *= qScells;
		VEGETATION::S_monthly_cover_shrublayer[m] *= qScells;
		VEGETATION::S_monthly_mass_shrublayer[m] *= qScells;
		VEGETATION::S_monthly_height_shrublayer[m] *= qScells;
		VEGETATION::S_monthly_LAI_shrublayer[m] *= qScells;
		VEGETATION::S_monthly_cover_treelayer[m] *= qScells;
		VEGETATION::S_monthly_cover_QItreelayer[m] *= qScells;
		VEGETATION::S_monthly_cover_QCtreelayer[m] *= qScells;
		VEGETATION::S_monthly_mass_treelayer[m] *= qScells;
		VEGETATION::S_monthly_height_treelayer[m] *= qScells;
		VEGETATION::S_monthly_LAI_treelayer[m] *= qScells;
		VEGETATION::S_monthly_cover_alllayers[m] *= qScells;
		VEGETATION::S_monthly_mass_alllayers[m] *= qScells;
		VEGETATION::S_monthly_height_alllayers[m] *= qScells;
		VEGETATION::S_monthly_LAI_alllayers[m] *= qScells;
		VEGETATION::S_monthly_massconsumed_kg_alllayers[out_month_min + m] *= qScells;
		WATER::S_netSoilMovement[m] *= qScells;
		WATER::S_netWaterMovement[m] *= qScells;
		
		VEGETATION::N_monthly_cover_herblayer[m] *= qNcells;
		VEGETATION::N_monthly_mass_herblayer[m] *= qNcells;
		VEGETATION::N_monthly_height_herblayer[m] *= qNcells;
		VEGETATION::N_monthly_LAI_herblayer[m] *= qNcells;
		VEGETATION::N_monthly_cover_shrublayer[m] *= qNcells;
		VEGETATION::N_monthly_mass_shrublayer[m] *= qNcells;
		VEGETATION::N_monthly_height_shrublayer[m] *= qNcells;
		VEGETATION::N_monthly_LAI_shrublayer[m] *= qNcells;
		VEGETATION::N_monthly_cover_treelayer[m] *= qNcells;
		VEGETATION::N_monthly_cover_QItreelayer[m] *= qNcells;
		VEGETATION::N_monthly_cover_QCtreelayer[m] *= qNcells;
		VEGETATION::N_monthly_mass_treelayer[m] *= qNcells;
		VEGETATION::N_monthly_height_treelayer[m] *= qNcells;
		VEGETATION::N_monthly_LAI_treelayer[m] *= qNcells;
		VEGETATION::N_monthly_cover_alllayers[m] *= qNcells;
		VEGETATION::N_monthly_mass_alllayers[m] *= qNcells;
		VEGETATION::N_monthly_height_alllayers[m] *= qNcells;
		VEGETATION::N_monthly_LAI_alllayers[m] *= qNcells;
		VEGETATION::N_monthly_massconsumed_kg_alllayers[out_month_min + m] *= qNcells;
		WATER::N_netSoilMovement[m] *= qNcells;
		WATER::N_netWaterMovement[m] *= qNcells;

		VEGETATION::W_monthly_cover_herblayer[m] *= qWcells;
		VEGETATION::W_monthly_mass_herblayer[m] *= qWcells;
		VEGETATION::W_monthly_height_herblayer[m] *= qWcells;
		VEGETATION::W_monthly_LAI_herblayer[m] *= qWcells;
		VEGETATION::W_monthly_cover_shrublayer[m] *= qWcells;
		VEGETATION::W_monthly_mass_shrublayer[m] *= qWcells;
		VEGETATION::W_monthly_height_shrublayer[m] *= qWcells;
		VEGETATION::W_monthly_LAI_shrublayer[m] *= qWcells;
		VEGETATION::W_monthly_cover_treelayer[m] *= qWcells;
		VEGETATION::W_monthly_cover_QItreelayer[m] *= qWcells;
		VEGETATION::W_monthly_cover_QCtreelayer[m] *= qWcells;
		VEGETATION::W_monthly_mass_treelayer[m] *= qWcells;
		VEGETATION::W_monthly_height_treelayer[m] *= qWcells;
		VEGETATION::W_monthly_LAI_treelayer[m] *= qWcells;
		VEGETATION::W_monthly_cover_alllayers[m] *= qWcells;
		VEGETATION::W_monthly_mass_alllayers[m] *= qWcells;
		VEGETATION::W_monthly_height_alllayers[m] *= qWcells;
		VEGETATION::W_monthly_LAI_alllayers[m] *= qWcells;
		VEGETATION::W_monthly_massconsumed_kg_alllayers[out_month_min + m] *= qWcells;
		WATER::W_netSoilMovement[m] *= qWcells;
		WATER::W_netWaterMovement[m] *= qWcells;

		VEGETATION::P_monthly_cover_herblayer[m] *= qPcells;
		VEGETATION::P_monthly_mass_herblayer[m] *= qPcells;
		VEGETATION::P_monthly_height_herblayer[m] *= qPcells;
		VEGETATION::P_monthly_LAI_herblayer[m] *= qPcells;
		VEGETATION::P_monthly_cover_shrublayer[m] *= qPcells;
		VEGETATION::P_monthly_mass_shrublayer[m] *= qPcells;
		VEGETATION::P_monthly_height_shrublayer[m] *= qPcells;
		VEGETATION::P_monthly_LAI_shrublayer[m] *= qPcells;
		VEGETATION::P_monthly_cover_treelayer[m] *= qPcells;
		VEGETATION::P_monthly_cover_QItreelayer[m] *= qPcells;
		VEGETATION::P_monthly_cover_QCtreelayer[m] *= qPcells;
		VEGETATION::P_monthly_mass_treelayer[m] *= qPcells;
		VEGETATION::P_monthly_height_treelayer[m] *= qPcells;
		VEGETATION::P_monthly_LAI_treelayer[m] *= qPcells;
		VEGETATION::P_monthly_cover_alllayers[m] *= qPcells;
		VEGETATION::P_monthly_mass_alllayers[m] *= qPcells;
		VEGETATION::P_monthly_height_alllayers[m] *= qPcells;
		VEGETATION::P_monthly_LAI_alllayers[m] *= qPcells;
		VEGETATION::P_monthly_massconsumed_kg_alllayers[out_month_min + m] *= qPcells;
		WATER::P_netSoilMovement[m] *= qPcells;
		WATER::P_netWaterMovement[m] *= qPcells;
	} // for each month

	VEGETATION::S_annual_sum_mass_consumed_kg *= qScells;
	VEGETATION::N_annual_sum_mass_consumed_kg *= qNcells;
	VEGETATION::W_annual_sum_mass_consumed_kg *= qWcells;
	VEGETATION::P_annual_sum_mass_consumed_kg *= qPcells;

}

/**
 Output of landscape statistics for each year, month, and habitat into a file.
 */
void monthly_summary_output(int ll, climate_names cc, int yy, float LSU_km2, float annual_rain,
		float* mrain, scenario_names scenario, const char* file_name, WATER* W,
		CELL* C, float fireArea_m2, month_names fireMonth) {

	reset_monthly_output_sums();

	calculate_monthly_statistics(C, W);

	std::ofstream ann_stats(file_name, std::ios_base::app);

	//------ summary statistics

	for (int m = 0; m <= out_month_max-out_month_min; ++m) {
		//std::cout.precision(3);
		ann_stats << ll << "\t" << scenario << "\t" << cc << "\t"
				<< float(LSU_km2 * 0.01) << "\t" << 1961 - years_0 + yy << "\t" << m
		<<"\t"<< mrain[out_month_min+m] << "\t" << annual_rain << "\t" << (int(fireMonth)==m?fireArea_m2:0.0) << "\t";

		ann_stats << "|P|\t" 
				<< VEGETATION::P_monthly_cover_herblayer[m] << "\t" 
				<< VEGETATION::P_monthly_mass_herblayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::P_monthly_height_herblayer[m] << "\t"
				<< VEGETATION::P_monthly_LAI_herblayer[m] << "\t"
				<< VEGETATION::P_monthly_cover_shrublayer[m] << "\t"
				<< VEGETATION::P_monthly_mass_shrublayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::P_monthly_height_shrublayer[m] << "\t"
				<< VEGETATION::P_monthly_LAI_shrublayer[m] << "\t"
				<< VEGETATION::P_monthly_cover_treelayer[m] << "\t"
				<< VEGETATION::P_monthly_cover_QItreelayer[m] << "\t"
				<< VEGETATION::P_monthly_cover_QCtreelayer[m] << "\t"
				<< VEGETATION::P_monthly_mass_treelayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::P_monthly_height_treelayer[m] << "\t"
				<< VEGETATION::P_monthly_LAI_treelayer[m] << "\t"
				<< VEGETATION::P_monthly_cover_alllayers[m] << "\t"
				<< VEGETATION::P_monthly_mass_alllayers[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::P_monthly_height_alllayers[m] << "\t"
				<< VEGETATION::P_monthly_LAI_alllayers[m] << "\t"
				<< VEGETATION::P_monthly_massconsumed_kg_alllayers[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::P_annual_sum_mass_consumed_kg * q1_cell_area_m2 << "\t"
				<< WATER::P_netSoilMovement[m] * q1_cell_area_m2 * 0.5 << "\t"
				<< WATER::P_netWaterMovement[m] * q1_cell_area_m2 * 0.5 << "\t";


		ann_stats << "|N|\t" 
				<< VEGETATION::N_monthly_cover_herblayer[m] << "\t" 
				<< VEGETATION::N_monthly_mass_herblayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::N_monthly_height_herblayer[m] << "\t"
				<< VEGETATION::N_monthly_LAI_herblayer[m] << "\t"
				<< VEGETATION::N_monthly_cover_shrublayer[m] << "\t"
				<< VEGETATION::N_monthly_mass_shrublayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::N_monthly_height_shrublayer[m] << "\t"
				<< VEGETATION::N_monthly_LAI_shrublayer[m] << "\t"
				<< VEGETATION::N_monthly_cover_treelayer[m] << "\t"
				<< VEGETATION::N_monthly_cover_QItreelayer[m] << "\t"
				<< VEGETATION::N_monthly_cover_QCtreelayer[m] << "\t"
				<< VEGETATION::N_monthly_mass_treelayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::N_monthly_height_treelayer[m] << "\t"
				<< VEGETATION::N_monthly_LAI_treelayer[m] << "\t"
				<< VEGETATION::N_monthly_cover_alllayers[m] << "\t"
				<< VEGETATION::N_monthly_mass_alllayers[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::N_monthly_height_alllayers[m] << "\t"
				<< VEGETATION::N_monthly_LAI_alllayers[m] << "\t"
				<< VEGETATION::N_monthly_massconsumed_kg_alllayers[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::N_annual_sum_mass_consumed_kg * q1_cell_area_m2 << "\t"
				<< WATER::N_netSoilMovement[m] * q1_cell_area_m2 * 0.5 << "\t"
				<< WATER::N_netWaterMovement[m] * q1_cell_area_m2 * 0.5 << "\t";

		ann_stats << "|W|\t" 
				<< VEGETATION::W_monthly_cover_herblayer[m] << "\t" 
				<< VEGETATION::W_monthly_mass_herblayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::W_monthly_height_herblayer[m] << "\t"
				<< VEGETATION::W_monthly_LAI_herblayer[m] << "\t"
				<< VEGETATION::W_monthly_cover_shrublayer[m] << "\t"
				<< VEGETATION::W_monthly_mass_shrublayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::W_monthly_height_shrublayer[m] << "\t"
				<< VEGETATION::W_monthly_LAI_shrublayer[m] << "\t"
				<< VEGETATION::W_monthly_cover_treelayer[m] << "\t"
				<< VEGETATION::W_monthly_cover_QItreelayer[m] << "\t"
				<< VEGETATION::W_monthly_cover_QCtreelayer[m] << "\t"
				<< VEGETATION::W_monthly_mass_treelayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::W_monthly_height_treelayer[m] << "\t"
				<< VEGETATION::W_monthly_LAI_treelayer[m] << "\t"
				<< VEGETATION::W_monthly_cover_alllayers[m] << "\t"
				<< VEGETATION::W_monthly_mass_alllayers[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::W_monthly_height_alllayers[m] << "\t"
				<< VEGETATION::W_monthly_LAI_alllayers[m] << "\t"
				<< VEGETATION::W_monthly_massconsumed_kg_alllayers[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::W_annual_sum_mass_consumed_kg * q1_cell_area_m2 << "\t"
				<< WATER::W_netSoilMovement[m] * q1_cell_area_m2 * 0.5 << "\t"
				<< WATER::W_netWaterMovement[m] * q1_cell_area_m2 * 0.5 << "\t";
		
		ann_stats << "|S|\t" 
				<< VEGETATION::S_monthly_cover_herblayer[m] << "\t"
				<< VEGETATION::S_monthly_mass_herblayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::S_monthly_height_herblayer[m] << "\t"
				<< VEGETATION::S_monthly_LAI_herblayer[m] << "\t"
				<< VEGETATION::S_monthly_cover_shrublayer[m] << "\t"
				<< VEGETATION::S_monthly_mass_shrublayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::S_monthly_height_shrublayer[m] << "\t"
				<< VEGETATION::S_monthly_LAI_shrublayer[m] << "\t"
				<< VEGETATION::S_monthly_cover_treelayer[m] << "\t"
				<< VEGETATION::S_monthly_cover_QItreelayer[m] << "\t"
				<< VEGETATION::S_monthly_cover_QCtreelayer[m] << "\t"
				<< VEGETATION::S_monthly_mass_treelayer[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::S_monthly_height_treelayer[m] << "\t"
				<< VEGETATION::S_monthly_LAI_treelayer[m] << "\t"
				<< VEGETATION::S_monthly_cover_alllayers[m] << "\t"
				<< VEGETATION::S_monthly_mass_alllayers[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::S_monthly_height_alllayers[m] << "\t"
				<< VEGETATION::S_monthly_LAI_alllayers[m] << "\t"
				<< VEGETATION::S_monthly_massconsumed_kg_alllayers[m] * q1_cell_area_m2 << "\t"
				<< VEGETATION::S_annual_sum_mass_consumed_kg * q1_cell_area_m2 << "\t"
				<< WATER::S_netSoilMovement[m] * q1_cell_area_m2 * 0.5 << "\t"
				<< WATER::S_netWaterMovement[m] * q1_cell_area_m2 * 0.5;

		ann_stats << std::endl;
	} // for each month
	ann_stats.close();
}


void tree_summary_output(const int maxYears, float year_rain, CELL* C, int yy,
		const char* fileName) {
	std::ofstream test_output(fileName, std::ios_base::app);
	month_names reporting_month = Jul;

	for (int iz = 0; iz < NZ; iz++) {
		for (int is = 0; is < NS; is++) {

			int row = is;
			int column = iz;
			species_names species = none;
			int totalNumberOfTrees = 0;
			int numberCalli = 0, numberCalliSeed = 0, numberCalliSap = 0, numberCalliAdu = 0;
			int numberItha = 0, numberIthaSeed = 0, numberIthaSap = 0, numberIthaAdu = 0;
			float meanAHeightCalli = 0.0, totalMassCalli = 0.0, meanAAgeCalli = 0.0, meanAHeightItha = 0.0, totalMassItha = 0.0, meanAAgeItha = 0.0;

			std::list<TREE*>::iterator treeIterator;
			
			if (C[iz * NZ + is].treeIndividual.size()) { // if there are tree individuals:
				for (treeIterator = C[iz * NZ + is].treeIndividual.begin(); treeIterator
						!= C[iz * NZ + is].treeIndividual.end(); treeIterator++) {
					species = (*treeIterator)->getTreeType(); // of what type is the tree?
					if (species == Q_calliprinos) {
						numberCalli++; // increment the number of calliprinos trees
						if ((*treeIterator)->get_heightM_m(reporting_month) < 0.3)
							numberCalliSeed++;
						else if ((*treeIterator)->get_heightM_m(reporting_month) < 1.5)
							numberCalliSap++;
						else {
							numberCalliAdu++;
							meanAHeightCalli += (*treeIterator)->get_heightM_m(reporting_month);
							meanAAgeCalli += (*treeIterator)->get_age();
						}
						totalMassCalli += (*treeIterator)->get_shoot_mass_kg();
					}
					if (species == Q_ithaburensis) {
						numberItha++; // increment the number of ithaburensis trees
						if ((*treeIterator)->get_heightM_m(reporting_month) < 0.3)
							numberIthaSeed++;
						else if ((*treeIterator)->get_heightM_m(reporting_month) < 1.5)
							numberIthaSap++;
						else {
							numberIthaAdu++;
							meanAHeightItha += (*treeIterator)->get_heightM_m(reporting_month);
							meanAAgeItha += (*treeIterator)->get_age();
						}
						totalMassItha += (*treeIterator)->get_shoot_mass_kg();
					}

				} // end for each tree in cell
				totalNumberOfTrees = numberCalli + numberItha;

				if (numberCalliAdu) { // Vermeidung von Division durch Null!
					meanAHeightCalli /= float(numberCalliAdu);
					meanAAgeCalli /= float(numberCalliAdu);
				}
				if (numberIthaAdu) {
					meanAAgeItha /= float(numberIthaAdu);
					meanAHeightItha /= float(numberIthaAdu);
				}
			}
			test_output << 1961+yy-years_0 << "\t" << row << "\t" << column << "\t" << totalNumberOfTrees << "\t";
			test_output << numberCalli << "\t" << numberCalliSeed << "\t" << numberCalliSap << "\t" << numberCalliAdu << "\t";
			test_output << meanAHeightCalli << "\t" << meanAAgeCalli << "\t" << totalMassCalli << "\t";
			test_output << C[iz * NZ + is].get_calliprinos_coverM_rel(reporting_month) << "\t";
			test_output	<< numberItha << "\t" << numberIthaSeed << "\t" << numberIthaSap << "\t" << numberIthaAdu << "\t";
			test_output	<< meanAHeightItha << "\t" << meanAAgeItha << "\t" << totalMassItha << "\t";
			test_output << C[iz * NZ + is].get_ithaburensis_coverM_rel(reporting_month) << "\t";
			test_output << short(bool(numberCalliAdu))*Q_calliprinos + short(bool(numberIthaAdu))*Q_ithaburensis << std::endl;
			// 0 = empty, 1 = Q. call., 2 = Q. ith., 3 = both

		} // end for each column
	} // end for each row
	test_output.close();
}

void monthly_output_header(const char* file_name)
{
	std::ofstream ann_stats_header(file_name, std::ios_base::app);
	ann_stats_header
	<< "LL.ID\tscenario\tclimate\tLSU\tyear\tmonth\tm.rain\tyr.rain\tfireA_m2\tPPP\t";
	
	ann_stats_header
	<< "P.cover.herb\tP.Fmass.kg.m2.herb\tP.height.herb\tP.LAI.herb\t";
	ann_stats_header
	<< "P.cover.shrub\tP.Fmass.kg.m2.shrub\tP.height.shrub\tP.LAI.shrub\t";
	ann_stats_header
	<< "P.cover.tree\tP.cover.ith\tP.cover.call\tP.Fmass.kg.m2.tree\tP.height.tree\tP.LAI.tree\t";
	ann_stats_header
	<< "P.cover.all\tP.Fmass.kg.m2.all\tP.height.all\tP.LAI.all\t";
	ann_stats_header << "P.consumed.kg.m2.all\tP.consumed.kg.m2.yr.all\tP.acc.soil.mvmt.kg.m2\tP.acc.water.mvmt.l.m2\tNNN\t";
	
	ann_stats_header
	<< "N.cover.herb\tN.Fmass.kg.m2.herb\tN.height.herb\tN.LAI.herb\t";
	ann_stats_header
	<< "N.cover.shrub\tN.Fmass.kg.m2.shrub\tN.height.shrub\tN.LAI.shrub\t";
	ann_stats_header
	<< "N.cover.tree\tN.cover.ith\tN.cover.call\tN.Fmass.kg.m2.tree\tN.height.tree\tN.LAI.tree\t";
	ann_stats_header
	<< "N.cover.all\tN.Fmass.kg.m2.all\tN.height.all\tN.LAI.all\t";
	ann_stats_header << "N.consumed.kg.m2.all\tN.consumed.kg.m2.yr.all\tN.acc.soil.mvmt.kg.m2\tN.acc.water.mvmt.l.m2\tWWW\t";
	
	ann_stats_header
	<< "W.cover.herb\tW.Fmass.kg.m2.herb\tW.height.herb\tW.LAI.herb\t";
	ann_stats_header
	<< "W.cover.shrub\tW.Fmass.kg.m2.shrub\tW.height.shrub\tW.LAI.shrub\t";
	ann_stats_header
	<< "W.cover.tree\tW.cover.ith\tW.cover.call\tW.Fmass.kg.m2.tree\tW.height.tree\tW.LAI.tree\t";
	ann_stats_header
	<< "W.cover.all\tW.Fmass.kg.m2.all\tW.height.all\tW.LAI.all\t";
	ann_stats_header << "W.consumed.kg.m2.all\tW.consumed.kg.m2.yr.all\tW.acc.soil.mvmt.kg.m2\tW.acc.water.mvmt.l.m2\tSSS\t";
	
	ann_stats_header
	<< "S.cover.herb\tS.Fmass.kg.m2.herb\tS.height.herb\tS.LAI.herb\t";
	ann_stats_header
	<< "S.cover.shrub\tS.Fmass.kg.m2.shrub\tS.height.shrub\tS.LAI.shrub\t";
	ann_stats_header
	<< "S.cover.tree\tS.cover.ith\tS.cover.call\tS.Fmass.tree\tS.height.tree\tS.LAI.tree\t";
	ann_stats_header
	<< "S.cover.all\tS.Fmass.kg.m2.all\tS.height.all\tS.LAI.all\t";
	ann_stats_header << "S.consumed.kg.m2.all\tS.consumed.kg.m2.yr.all\tS.acc.soil.mvmt.kg.m2\tS.acc.water.mvmt.l.m2";
	ann_stats_header << std::endl;
	
}

void annual_spatial_output_header(const char* file_name) // new v. 3.2.1
{
	std::ofstream row_stats(file_name, std::ios_base::app);
	row_stats << "year	col	row	habitat	rain.mm	moisture.mm	herb.cover	herb.mass.kg.m2	shrub.cover	ith.cover	cal.cover	tot.cover	deposition.kg.m2	runon.mm\n";	
}

void annual_tree_output_header(const char* file_name)
{
	std::ofstream tree_details_per_cell(file_name, std::ios_base::ate); // "ate" bewirkt Fortsetzung am Ende des schon bestehenden Dokuments
	tree_details_per_cell
			<< "year\trow\tcol\tTrees\tCalli\tCSeedl\tCSapl\tCAdul\tCAHeight\tCAAge\tCTMass\tCTCover\tItha\tISeedl\tISapl\tIAdul\tIAHeight\tIAAge\tITMass\tITCover\ttreeID\n";
}

void annual_spatial_output_data(int yy, const char* file_name, WATER* W, CELL* C) // new v. 3.2.1
{
	month_names mn = Apr;

	std::ofstream row_stats(file_name, std::ios_base::app);

	for (int iz = 0; iz < NZ; iz++) {
		for (int is = 0; is < NS; is++) {
			row_stats
			<< 1961 - years_0 + yy << "\t"
			<< iz << "\t"
			<< is << "\t"
			<< W[iz	* NZ + is].habitat << "\t"
			<< W[iz * NZ + is].ann_rain_mm << "\t"
			<< W[iz * NZ + is].avail_abs[0] << "\t"
			<< C[iz * NZ + is].herbs.get_coverMax_rel() << "\t"
			<< C[iz * NZ + is].herbs.get_feedM_kg(mn)*q1_cell_area_m2 << "\t"
			<< C[iz * NZ + is].dwarfshrubs.get_coverM_rel(mn) << "\t"
			<< C[iz * NZ + is].get_ithaburensis_coverM_rel(mn) << "\t"
			<< C[iz * NZ + is].get_calliprinos_coverM_rel(mn) << "\t"
			<< C[iz * NZ + is].get_total_coverM_rel(mn) << "\t"
			<< W[iz	* NZ + is].moved_soil_kg*q1_cell_area_m2 << "\t"
			<< W[iz	* NZ + is].accumulated_runon_mm << std::endl;
		} // end for iz
	} // end for is
}

