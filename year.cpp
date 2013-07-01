/*
 *  year.cpp
 *  MKLandscape
 *
 *  Created by Forschung on Fri Jun 23 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */
#include "year.h"
#include "ReGen.h"
#include "fire.h"
#include "summary_out.h"

extern int N_edge, S_edge, W_edge, P_edge;
extern bool simtrees;

unsigned long randInt(float f) {
	unsigned long int_out = 0;
	if (f>1.0)
		int_out = (unsigned long)(f);
	else
		if(f<z01())
			int_out = 1;
	return int_out;
}

float year (int yy, WATER* W, CELL* C, FIRE_PARAMS* pFireParams, float lsu, RAINSERIES* pTS, float* mrain, 
						  uint32* sID, CLIMATE& rGrid_climate, const char* file_name, const bool runFire, const bool cut, 
						   month_names& whichMonth, bool simtrees)
{
	
	HERB::sum_seeds_prod_grid = 0;
	pTS->getAnnualRain();
	float did_burn = false;
	
	// add annual and monthly rain
	for (uint32 c = 0; c < all_cells; c++) {
		C[c].dwarfshrubs.grow_year(W + c); // MK 2011-03-15: leave this line before W[c].update()
		W[c].update(); // available_water (t-1) <- available water (t), reset cummulative monthly water
		C[c].updateTimeSinceFire(); //MM, Memory wann Feuer war, wird weiter gereicht - altert!
		
		// calculate potential annual biomass production of each growth form
		// growth of annuals calculated first because annuals start growing before shrubs
		// effect of shrubs on growth of annuals is the effect of last year's shrub cover!
		C[c].herbs.mortality_seedbank();
		C[c].herbs.grow_year(); // a few maintenance processes
	}
	
	unsigned short vd = 0; ///< days after July 31 (Aug 1 = 1)
	unsigned short jd = 0; ///< day of calendar year (Jan 1 = 1)
	
	unsigned long int NumberOfCalliprinosAcornsInTheLandscape = 0;
	unsigned long int NumberOfIthaburensisAcornsInTheLandscape = 0;
	unsigned long int NumberOfTreesInTheLandscape = 0;
	unsigned long int NumberOfCalliprinosTreesInTheLandscape = 0;
	unsigned long int NumberOfIthaburensisTreesInTheLandscape = 0;
	unsigned long int NumberOfAdultTreeIndividualsInLandscape = 0;
	unsigned long int NumberOfAdultCalliprinosTreesInTheLandscape = 0;
	unsigned long int NumberOfAdultIthaburensisTreesInTheLandscape = 0;
	unsigned long int total_calliprinos_acorns_in_cell = 0;
	unsigned long int total_ithaburensis_acorns_in_cell = 0;
	unsigned long int number_germinated_calliprinos = 0;
	unsigned long int number_germinated_ithaburensis = 0;
	int NumberOfAdultTreeIndividualsInCell = 0;
	int NumberOfAdultIthaburensisIndividualsInCell = 0;
	int NumberOfAdultCalliprinosIndividualsInCell = 0;
	int NumberofSaplingsInCell = 0;
	int NumberOfTreeIndividualsInCell = 0;			
	int NumberOfAllCalliprinosIndividualsInCell = 0;
	int NumberOfAllIthaburensisIndividualsInCell = 0;
	
	std::list<TREE*>::iterator treeIterator;
	std::list<TREE*>* newSaplings = new std::list<TREE*>;
	
	for (month_names month = Aug; month <= Jul; ++month) {
		std::cout << "\n\t\t\t - " << month;
		
		// ======== set environment ==================
		jd = rGrid_climate.set_day(vd+15);
		WATER::p_my_earth->set_daily_parameters(jd);
		float TdegC = rGrid_climate.get_temperature();
		
		// ======= hydrological parameters for each cell
		for (uint32 cell = 0; cell < all_cells; cell++)	{
			W[cell].kStr = C[cell].get_kStr(month);	 // set Strickler number
			W[cell].infiltration_rate_mmh = C[cell].get_infiltration_rate_mmh(month);
			W[cell].reset_mWater();
			float dr = WATER::p_my_earth->get_daily_radiation(W[cell].cell_slope_rad, W[cell].aspect_rad, jd);
			W[cell].pET_mm = W[cell].get_pot_evapo_mm(dr, TdegC); // 0-5, falls notwendig fuers Testen
		}
		
		// ======= rain and water processes ============= daily time step ==========================
		for (ushort d = 0; d < days_in_month(month); d++) {
			float daily_rain_mm = pTS->getRain(vd++); // increment vd after extracting rain amount
			mrain[int(month)] += daily_rain_mm;
			float rain_duration_h = daily_rain_mm > 0.0? floor(pTS->getMeanDuration_hours(vd)+0.5): 0.0;

			if(rain_duration_h > 0.0) {
				for (uint32 c = 0; c < all_cells; c++) {
					//					float ortho_rain_mm = W[c].ortho(daily_rain_mm); // slope reduces rain amount
					float ortho_rain_mm = daily_rain_mm; // slope does not reduce rain amount; simplification
					W[c].add_daily_rain(ortho_rain_mm); // add daily rain to mRain_mm
					W[c].rain_mmh = ortho_rain_mm / rain_duration_h; // TODO: use a different distribution, e.g. Gumbel
				}
				for (ushort h = 0; h < rain_duration_h; h++) {
					for (uint32 c = 0; c < all_cells; c++) {
						uint32 sc = sID[c]; // cells sorted by height
						W[sc].aboveground_water_mm += C[sc].interception(W[sc].rain_mmh, month);
						W[sc].infiltration(1.0); // infiltration() also reduces aboveground_water_mm!
						if(W[sc].aboveground_water_mm > 0.0) {
							float local_runoff = W[sc].distribute_runoff(sc); // calculate in order of decreasing elevation
							W[sc].aboveground_water_mm -= local_runoff;
						}
						if (h == rain_duration_h - 1)
							W[sc].rain_mmh = 0.0; // reset
					}
				} // for each hour of rain
			
			for (ushort h = ushort(rain_duration_h); h < 24; h++) { // as long as there is runon water and it is before midnight
					for (uint32 c = 0; c < all_cells; c++) {
						uint32 sc = sID[c];
						W[sc].infiltration();  // infiltration() also reduces aboveground_water_mm!
						if(W[sc].aboveground_water_mm > 0.0) {
							float local_runoff = W[sc].distribute_runoff(sc); // calculate in order of decreasing elevation
							W[sc].aboveground_water_mm -= local_runoff;
						}
					}
				} // end for each hour after end of rain

			} // end if rain duration

			for (uint32 c = 0; c < all_cells; c++){
				W[c].evapotranspiration(&C[c], month); // only pET, herbs, and shrubs
			}
		} // end for days in month
		
		std::cout << "|v"; 
		// ======= vegetation processes =============================================
		for (uint32 c = 0; c < all_cells; c++) {
			W[c].accumulate_monthly_water(month);
			W[c].netSoilMovement[month]=W[c].moved_soil_kg;
			W[c].netWaterMovement[month]=W[c].accumulated_runon_mm;

			
			C[c].herbs.grow_month(W + c, month); // update cover, height, LAI, etc.
			C[c].dwarfshrubs.grow_month(W + c, month); // update cover, height, LAI, etc.
			
			NumberOfAdultTreeIndividualsInCell = 0;
			NumberOfAdultIthaburensisIndividualsInCell = 0;
			NumberOfAdultCalliprinosIndividualsInCell = 0;
			NumberofSaplingsInCell = 0;
			NumberOfTreeIndividualsInCell = C[c].treeIndividual.size();			
			NumberOfAllCalliprinosIndividualsInCell = 0;
			NumberOfAllIthaburensisIndividualsInCell = 0;
			C[c].ithaburensis_germination_fraction = 0.0;
			C[c].calliprinos_germination_fraction = 0.0;
			number_germinated_calliprinos = 0;
			number_germinated_ithaburensis = 0;
					
			if (simtrees) {
				if (NumberOfTreeIndividualsInCell) { // if there are tree individuals:
					
					float herb_mass_gm2 = kg_to_g(C[c].herbs.get_feedM_kg(month) * q1_cell_area_m2); 
					
					for (treeIterator = C[c].treeIndividual.begin();
						 treeIterator !=C[c].treeIndividual.end();
						 treeIterator++) {
						
						// update the individual in height, age, etc.
						if (month == Aug)
							(*treeIterator)->annual_update_function();
						
						// monthly growth including reproduction in Feb or May and mortality from desiccation
						bool desiccation=false;
						desiccation = ((*treeIterator)->grow_month(W + c, month));
						if (desiccation) {
							delete *treeIterator;
							treeIterator = C[c].treeIndividual.erase(treeIterator);
							std::cout << "†";
							continue; // next step in for-loop
						}
						
						if ((*treeIterator)->getTreeType() == Q_calliprinos) {
							NumberOfAllCalliprinosIndividualsInCell++;
							((*treeIterator)->yearsAfterSevereBurn)++;
							if ((*treeIterator)->get_height_m() > 1.5) { // and if it is an adult
								NumberOfAdultTreeIndividualsInCell++;
								NumberOfCalliprinosTreesInTheLandscape++;
								if (month == Dec) {
									// in DECEMBER: disperse seeds
									// first: count the total number of acorns in landscape and the total number of both tree species
									// 75% of seeds are dispersed by birds, 25% fall under tree and remain in the cell
									NumberOfAdultCalliprinosIndividualsInCell++; // add one tree to the tree number variable
									NumberOfAdultCalliprinosTreesInTheLandscape++;
									total_calliprinos_acorns_in_cell = C[c].get_total_calliprinos_acorns_in_cell();
									NumberOfCalliprinosAcornsInTheLandscape += ((long unsigned int) (total_calliprinos_acorns_in_cell*0.75)); // get the number of acorns for dispersal
									C[c].calliprinosSeedbank = (long unsigned int) (total_calliprinos_acorns_in_cell*0.25);
								}
							} // if adult tree
							else if ((*treeIterator)->get_height_m() > 0.3) // and if it is a sapling
								NumberofSaplingsInCell++;
						}						
						else if ((*treeIterator)->getTreeType() == Q_ithaburensis) {
							NumberOfAllIthaburensisIndividualsInCell++;
							if ((*treeIterator)->get_height_m() > 1.5) {
								NumberOfAdultTreeIndividualsInCell++;
								NumberOfIthaburensisTreesInTheLandscape++;
								if(month == Dec) {
									// in DECEMBER: disperse seeds
									// first: count the total number of acorns in landscape and the total number of both tree species
									// 75% of seeds are dispersed by birds, 25% fall under tree and remain in the cell
									NumberOfAdultIthaburensisIndividualsInCell++;
									NumberOfAdultIthaburensisTreesInTheLandscape++;
									total_ithaburensis_acorns_in_cell = C[c].get_total_ithaburensis_acorns_in_cell();
									NumberOfIthaburensisAcornsInTheLandscape += ((long unsigned int) (total_ithaburensis_acorns_in_cell*0.75));
									C[c].ithaburensisSeedbank = (long unsigned int) (total_ithaburensis_acorns_in_cell*0.25);
								}
							} // if adult tree
							else if ((*treeIterator)->get_height_m() > 0.3) // and if it is a sapling
								NumberofSaplingsInCell++;
						} // end ithaburensis
						
						// mortality per month and individual
						if ((*treeIterator)->mortality(herb_mass_gm2, NumberOfAdultTreeIndividualsInCell, NumberofSaplingsInCell) == true) {
							delete *treeIterator;
							treeIterator = C[c].treeIndividual.erase(treeIterator);
						}
						
					} // end for each tree
					
				} // end if NumberOfTreeIndividualsInCell
				
				C[c].calc_tree_coverM_rel(month);

				// promote tall seedling groups to individual saplings
				 for (treeIterator = C[c].treeIndividual.begin();
					treeIterator !=C[c].treeIndividual.end();
					treeIterator++) {
					if ((*treeIterator)->individuals > 1 && (*treeIterator)->get_height_m() > 0.3) { // the seedlings are now > 0.3 m
						if((*treeIterator)->getTreeType() == Q_ithaburensis) 
						{ // it's an ithaburensis
							for(int i = 1; i < (*treeIterator)->individuals; i++) {
								try{
								ITHABURENSIS* pIthaburensis= NULL;
								pIthaburensis = new ITHABURENSIS((*treeIterator)->get_height_m(), 
																 (*treeIterator)->radius_base_m, 0, 
																 (*treeIterator)->get_age(), C+c, 1);
								newSaplings->push_back(pIthaburensis); // n-1 new saplings
								}
								catch(...){
									std::cerr<<"Not enough memory for a new Q. ith. sapling, year " << 1961 - years_0 + yy << ".\n";
								}
							}
							(*treeIterator)->individuals = 1; // reset seedling group to become a single sapling
						}else 
						{// it's a calliprinos
							for(int i = 1; i < (*treeIterator)->individuals; i++) {
								try{
									CALLIPRINOS* pCalliprinos= NULL;
									pCalliprinos = new CALLIPRINOS((*treeIterator)->get_height_m(), 
																   (*treeIterator)->radius_base_m, 0.0, 
																   (*treeIterator)->get_age(), C+c, 1);
									newSaplings->push_back(pCalliprinos); // n-1 new saplings
								}
								catch(...){
									std::cerr<<"Not enough memory for a new Q. cal. sapling, year " << 1961 - years_0 + yy << ".\n";
								}
								
							}
							(*treeIterator)->individuals = 1; // reset seedling group to become a single sapling
						}
					}
				}
				 C[c].treeIndividual.merge(*newSaplings);
							
				// for testing
	/*			for (treeIterator = C[c].treeIndividual.begin();
					 treeIterator !=C[c].treeIndividual.end();
					 treeIterator++) {
				}
	*/			// end testing
			} // end if simtrees
		} // end for c
		
		
		NumberOfTreesInTheLandscape = NumberOfIthaburensisTreesInTheLandscape + NumberOfCalliprinosTreesInTheLandscape;


	if(month == Dec && simtrees) {
			// birds disperse acorns preferably to other trees, therefore
			// count total number of cells with (adult) trees in landscape
			// (does not account for cells with individuals of both calliprinos and ithaburensis)

		NumberOfAdultTreeIndividualsInLandscape = NumberOfAdultCalliprinosTreesInTheLandscape
			+ NumberOfAdultIthaburensisTreesInTheLandscape;
			
			// calculate the correct amount of acorns that will be dispersed into the cells
			double NumberOfDispersedCalliprinosAcornsIntoCellsWithoutAdultTrees = 
		              (NumberOfCalliprinosAcornsInTheLandscape*0.15)/float(all_cells
							-NumberOfAdultTreeIndividualsInLandscape);
			double NumberOfDispersedIthaburensisAcornsIntoCellsWithoutAdultTrees = 
			          (NumberOfIthaburensisAcornsInTheLandscape*0.15)/float(all_cells
							-NumberOfAdultTreeIndividualsInLandscape);

			double NumberOfDispersedCalliprinosAcornsIntoCellsWithAdultTrees = 0;
			double NumberOfDispersedIthaburensisAcornsIntoCellsWithAdultTrees = 0;
			if(NumberOfAdultTreeIndividualsInLandscape) {
				NumberOfDispersedCalliprinosAcornsIntoCellsWithAdultTrees = NumberOfCalliprinosAcornsInTheLandscape*0.85
					/float(NumberOfAdultTreeIndividualsInLandscape);
				NumberOfDispersedIthaburensisAcornsIntoCellsWithAdultTrees = NumberOfIthaburensisAcornsInTheLandscape*0.85
					/float(NumberOfAdultTreeIndividualsInLandscape);}

			
			// add seeds to the seedbank of all the cells, but different amounts to cells with and without adult trees
			for (uint32 c = 0; c < all_cells; c++) {
				float relativeShrubCover = (C[c].dwarfshrubs.get_coverM_rel(month));
				float zuf1 = z01()*2.0; // get random number between 0 and 2 for the final dispersal of calliprinos acorns into cells with any adult tree
				float zuf2 = z01()*2.0; // get random number between 0 and 2 for the final dispersal of ithaburensis acorns into cells with any adult tree
				if (C[c].treeIndividual.empty()) { // cell without any tree
					C[c].calliprinosSeedbank += randInt(NumberOfDispersedCalliprinosAcornsIntoCellsWithoutAdultTrees * zuf1);
					C[c].ithaburensisSeedbank += randInt(NumberOfDispersedIthaburensisAcornsIntoCellsWithoutAdultTrees * zuf2);
				} else { // cell with at least one tree
					for (treeIterator = C[c].treeIndividual.begin(); treeIterator != C[c].treeIndividual.end(); treeIterator++) {
						if ((*treeIterator)->get_height_m() > 1.5) {
							C[c].calliprinosSeedbank += randInt (NumberOfDispersedCalliprinosAcornsIntoCellsWithAdultTrees * zuf1);
							C[c].ithaburensisSeedbank += randInt (NumberOfDispersedIthaburensisAcornsIntoCellsWithAdultTrees * zuf2);
							break; // exit for-loop
						} // end if adult tree
						else { // cell with only young trees
							C[c].calliprinosSeedbank += randInt (NumberOfDispersedCalliprinosAcornsIntoCellsWithoutAdultTrees * zuf1);
							C[c].ithaburensisSeedbank += randInt (NumberOfDispersedIthaburensisAcornsIntoCellsWithoutAdultTrees * zuf2);
						} // end else only young trees
					} // end for-loop
				} // end if !treeIndividual.empty
				
//				std::cout << "call sb: " << C[c].calliprinosSeedbank << ", ith sb: " << C[c].ithaburensisSeedbank;
				// seed predation by animals
				C[c].seedbank_mortality(NumberOfAdultTreeIndividualsInCell, relativeShrubCover); //tree seed bank
//				std::cout << ", call sb2: " << C[c].calliprinosSeedbank << ", ith sb2: " << C[c].ithaburensisSeedbank << std::endl;
				
			} // end for c
			
		/* // output to console for debugging
		std::cout << "TotalNumberOfTreesInTheLandscape:  " << NumberOfTreesInTheLandscape << std::endl;                                                \
		std::cout << "TotalNumberOfAdultTreeIndividuals: " << NumberOfAdultTreeIndividualsInLandscape << std::endl;                                   \
		std::cout << "TotalNumberOfCalliprinosAcornsInLandscape (before dispersal):  " << NumberOfCalliprinosAcornsInTheLandscape << std::endl;       \
		std::cout << "\t# dispersed (with adult):    " << NumberOfDispersedCalliprinosAcornsIntoCellsWithAdultTrees                                   \
				<< "\t# dispersed (without adult): " << NumberOfDispersedCalliprinosAcornsIntoCellsWithoutAdultTrees << std::endl;                          \
		std::cout << "TotalNumberOfIthaburensisAcornsInLandscape (before dispersal): " << NumberOfIthaburensisAcornsInTheLandscape << std::endl;      \
		std::cout <<  "\t# dispersed (with adult):    " << NumberOfDispersedIthaburensisAcornsIntoCellsWithAdultTrees                                 \
				<< "\t# dispersed (without adult): " << NumberOfDispersedIthaburensisAcornsIntoCellsWithoutAdultTrees << std::endl;
*/		} // end month == Dec
		

		
		if (simtrees)
			for (uint32 c = 0; c < all_cells; c++) {
			int newCalliprinosIndividuals  = C[c].germinateCalliprinos (W+c, month); // determine number of germinated acorns
			int newIthaburensisIndividuals = C[c].germinateIthaburensis(W+c, month); // same for ithaburensis seedlings
			
			number_germinated_calliprinos += newCalliprinosIndividuals; // TODO: remove
			number_germinated_ithaburensis += newIthaburensisIndividuals; // TODO: remove
			try{
				float z = z01();
				if(z>0.5 && newCalliprinosIndividuals) {
					CALLIPRINOS* pCalliprinos = NULL;
					pCalliprinos = new CALLIPRINOS(0.05, 0.02, 0.0, 0, C+c, newCalliprinosIndividuals);
					C[c].treeIndividual.push_back(pCalliprinos);
				}
				if(newIthaburensisIndividuals) {
					ITHABURENSIS* pIthaburensis = NULL;
					pIthaburensis = new ITHABURENSIS(0.05, 0.07, 0.0, 0, C+c, newIthaburensisIndividuals);
					C[c].treeIndividual.push_back(pIthaburensis);
				}
				if(z<=0.5 && newCalliprinosIndividuals) {
					CALLIPRINOS* pCalliprinos = NULL;
					pCalliprinos = new CALLIPRINOS(0.05, 0.02, 0.0, 0, C+c, newCalliprinosIndividuals);
					C[c].treeIndividual.push_back(pCalliprinos);
				}
			} // try
			catch (...) {
				std::cerr<<"Not enough memory for " << newCalliprinosIndividuals << " Q. cal. seedlings and "
				<< newIthaburensisIndividuals << " Q. ith. seedlings, year " << 1961 - years_0 + yy << ".\n";
			}
		} // for each cell
		
		if(simtrees && (number_germinated_calliprinos || number_germinated_ithaburensis)) std::cout << "\tgerminated call./ith.: " << number_germinated_calliprinos << "/" << number_germinated_ithaburensis;

		//---------------------------------
		if (lsu && !(month > end_month && month < start_month)) foraging(C, lsu, W, month);
		//---------------------------------
		
		//--------------------------------- fires occur either in May and June:
		if (runFire && ((month == May && z01()<0.5) || (month == Jun && did_burn == 0.0))) {
			did_burn = FireDisturbance(W, C, pFireParams, month);
			whichMonth = month;
		}
		//---------------------------------
		
		//---------------------------------
		if (cut && month == May && simtrees)
			for (uint32 c = 0; c < all_cells; c++)
				for (treeIterator = C[c].treeIndividual.begin(); treeIterator != C[c].treeIndividual.end(); treeIterator++)
					(*treeIterator)->cutTree(cut_height_m);
		//---------------------------------
		
			
		if (spatial_output && month == Jul && 
				(yy == 0 || (yy-years_0 == 0) || (yy-years_0 == 1990-1961) || (yy-years_0 == 2025-1961) || (yy-years_0 == 2050-1961)))
			annual_spatial_output_data(yy, file_name, W, C);

		float brw = 0.0;

		for (uint32 c = 0; c < all_cells; c++) {
			brw=(W[c].bedrock_water_mm * pow(0.5,W[c].bedrock_water_mm/500.0));  // max 3 m * 50% capacity * 30 % soil = 3000*0.5*0.3 = 

			W[c].bedrock_water_mm = brw;
		}

	
	} // end months	
	
	for (uint32 c = 0; c < all_cells; c++) {
		HERB::sum_seeds_prod_grid += C[c].herbs.prod_seeds_cell();
	} // for c
	float LDD_seeds = HERB::sum_seeds_prod_grid/float(all_cells);
	
	// long distance dispersal for annuals, colonization for shrubs
	// in 50% der leeren Zellen -> 0.1% der mittleren Samenzahl // woher Daten?
	for (int iz = 0; iz<NZ; iz++) {
		for (int is = 0; is<NS; is++) {			
			C[iz*NZ+is].dwarfshrubs.colonize(iz, is, W[iz*NZ+is].avail_abs[0], C);
			C[iz*NZ+is].herbs.reduce_seedbank();
			if (C[iz*NZ+is].herbs.get_seeds_produced_m2() == 0 && z01()<0.5)
				C[iz*NZ+is].herbs.add_to_seedbank(long(0.001*LDD_seeds));
		} // for is
	} // for iz

	delete newSaplings;
	
	return did_burn;
}
