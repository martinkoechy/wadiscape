/*
 *  cell.cpp
 *  Landschaft
 *
 *  Created by Forschung on Tue Jun 20 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */
#include "cell.h"
#include <math.h>
#include <cstdlib>
#include <iostream>

CELL::CELL(void)
{
	pCalliprinos = new CALLIPRINOS(this);
	pIthaburensis = new ITHABURENSIS(this) ;
	//shrubs = SHRUB(this);
	dwarfshrubs = DWARFSHRUB(this);
	herbs = HERB(this);
	totalTreeCover = 0.0;
	calliprinosSeedbank = 0;
	ithaburensisSeedbank = 0;
	calliprinos_germination_fraction = 0.0;
	ithaburensis_germination_fraction = 0.0;
	for(short y = 0; y < 5; fireHistory[y++]);
}

CELL::~CELL() {
	std::list<TREE*>::iterator treeIterator;
	for (treeIterator = treeIndividual.begin();
		 treeIterator != treeIndividual.end();
		 treeIterator++) {
		delete *treeIterator;
	}
	treeIndividual.clear();
	delete pCalliprinos;
	delete pIthaburensis;
}


/**
 I assume that only bush and tree canopies or shrub and tree canopies overlap. It is unlikely that bushes grow under shrubs.
 */

float CELL::get_feedM_kg(month_names month) {
	float total_tree_mass_kg = 0.0;
	int individualSize = treeIndividual.size();
	if (individualSize != 0)
		total_tree_mass_kg = get_tree_feedM_kg(month);
	float total_dwarfshrub_mass_kg = dwarfshrubs.get_feedM_kg(month);
	float total_herb_mass_kg = herbs.get_feedM_kg(month);
	float total_mass_kg = total_tree_mass_kg + total_dwarfshrub_mass_kg + total_herb_mass_kg;
	return total_mass_kg;
}

float CELL::get_tree_feedM_kg(month_names month) {
	float sum_mass_kg = 0.0;
	std::list<TREE*>::iterator treeIterator;
	for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) { // iterate over every individual in cell				
		sum_mass_kg += (*treeIterator)->get_feedM_kg(month);
	}
	return sum_mass_kg;
}

float CELL::get_tree_LAIM_rel(month_names month) {
	float total_LA = 0.0;
	std::list<TREE*>::iterator treeIterator;
	for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) { // iterate over every individual in cell				
		total_LA += (*treeIterator)->get_leaf_area_m2(month);
	}
	return total_LA*q1_cell_area_m2;
}

float CELL::get_herb_coverM_rel(month_names month) {
	return herbs.get_coverM_rel(month);
}

float CELL::get_woody_coverM_rel(month_names month) {
	float total_cover = 0.0;
	float tc = get_tree_coverM_rel(month);
	float bc = dwarfshrubs.get_coverM_rel(month);

	total_cover = tc + bc;
	if (total_cover > 1)
		total_cover = 1;

	return total_cover;
}

void CELL::calc_tree_coverM_rel(month_names month) {
	float coverOfSeedlings= 0.0;
	float coverOfSaplings = 0.0;
	float coverOfAdults   = 0.0;
	float coverOfAllTrees = 0.0;
	
	std::list<TREE*>::iterator treeIterator;
	if(!treeIndividual.empty()){
	for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) { // iterate over every individual in cell		
		if ((*treeIterator)->get_heightM_m(month) < 0.3)
			coverOfSeedlings+=(*treeIterator)->get_coverM_rel(month); // Bedeckung aller seedlings in der Zelle
		else if ((*treeIterator)->get_heightM_m(month) <1.5)
			coverOfSaplings+=(*treeIterator)->get_coverM_rel(month); // Bedeckung aller saplings in der Zelle
		else 
			coverOfAdults+=(*treeIterator)->get_coverM_rel(month); // Bedeckung aller adults in der Zelle
	}}
	
	float tempCover = 0.0;
	tempCover = min(1.0F, coverOfSeedlings); coverOfSeedlings = tempCover;
	tempCover = min(1.0F, coverOfSaplings);  coverOfSaplings = tempCover;
	tempCover = min(1.0F, coverOfAdults);    coverOfAdults = tempCover;

	coverOfAllTrees = coverOfSeedlings+coverOfSaplings+coverOfAdults; // 0.04, 015, 0,66 = Bedeckungsgrade der einzelnen Klassen

	totalTreeCover = 
		 coverOfAllTrees-
		 (coverOfSeedlings * coverOfSaplings) - (coverOfSeedlings * coverOfAdults) - (coverOfSaplings * coverOfAdults) +
		 (coverOfSeedlings * coverOfSaplings * coverOfAdults);
}

float CELL::get_tree_coverM_rel(month_names month) {
	
	return totalTreeCover;
}

float CELL::get_ithaburensis_coverM_rel(month_names month) {
	float coverOfSeedlings= 0.0;
	float coverOfSaplings = 0.0;
	float coverOfAdults   = 0.0;
	float coverOfAllTrees = 0.0;
	float totalIthCover = 0.0;
	
	std::list<TREE*>::iterator treeIterator;
	for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) { // iterate over every individual in cell		
		if ((*treeIterator)->getTreeType() == Q_ithaburensis) {
			if ((*treeIterator)->get_heightM_m(month) < 0.3)
				coverOfSeedlings+=(*treeIterator)->get_coverM_rel(month); // Bedeckung aller seedlings in der Zelle
			else if ((*treeIterator)->get_heightM_m(month) < 1.5)
				coverOfSaplings+=(*treeIterator)->get_coverM_rel(month); // Bedeckung aller saplings in der Zelle
			else
				coverOfAdults+=(*treeIterator)->get_coverM_rel(month); // Bedeckung aller adults in der Zelle
		}
	}
	
	coverOfSeedlings = min(1.0F, coverOfSeedlings);
	coverOfSaplings  = min(1.0F, coverOfSaplings);
	coverOfAdults    = min(1.0F, coverOfAdults);
	
	coverOfAllTrees = coverOfSeedlings+coverOfSaplings+coverOfAdults; // 0.04, 015, 0,66 = Bedeckungsgrade der einzelnen Klassen
	
	totalIthCover = 
		 coverOfAllTrees-
		 (coverOfSeedlings * coverOfSaplings) - (coverOfSeedlings * coverOfAdults) - (coverOfSaplings * coverOfAdults) +
		 (coverOfSeedlings * coverOfSaplings * coverOfAdults);
		 
		 return totalIthCover;
}



float CELL::get_calliprinos_coverM_rel(month_names month) {
	float coverOfSeedlings= 0.0;
	float coverOfSaplings = 0.0;
	float coverOfAdults   = 0.0;
	float coverOfAllTrees = 0.0;
	float totalCalCover = 0.0;
	
	std::list<TREE*>::iterator treeIterator;
	for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) { // iterate over every individual in cell		
		if ((*treeIterator)->getTreeType() == Q_calliprinos) {
			if ((*treeIterator)->get_heightM_m(month) < 0.3)
				coverOfSeedlings+=(*treeIterator)->get_coverM_rel(month); // Bedeckung aller seedlings in der Zelle
			else if ((*treeIterator)->get_heightM_m(month) < 1.5)
				coverOfSaplings+=(*treeIterator)->get_coverM_rel(month); // Bedeckung aller saplings in der Zelle
			else
				coverOfAdults+=(*treeIterator)->get_coverM_rel(month); // Bedeckung aller adults in der Zelle
		}
	}
	
	coverOfSeedlings = min(1.0F, coverOfSeedlings); // ensures that the cover is ≤ 1.0
	coverOfSaplings  = min(1.0F, coverOfSaplings); // ensures that the cover is ≤ 1.0
	coverOfAdults    = min(1.0F, coverOfAdults); // ensures that the cover is ≤ 1.0

	coverOfAllTrees = coverOfSeedlings+coverOfSaplings+coverOfAdults; // 0.04, 0.15, 0.66 = Bedeckungsgrade der einzelnen Klassen
	
	totalCalCover = 
		coverOfAllTrees-
		 (coverOfSeedlings * coverOfSaplings) - (coverOfSeedlings * coverOfAdults) - (coverOfSaplings * coverOfAdults) +
		 (coverOfSeedlings * coverOfSaplings * coverOfAdults);
		 
	return totalCalCover;
}



/**
	Tree height is weighted by the cover of each individual.
*/
float CELL::get_tree_heightM_m(month_names month) {
	float coversum = 0.0; ///< cumulative cover
	float hc = 0.0; ///< height * cover
	float cover = 0.0; ///< cover
	
	std::list<TREE*>::iterator treeIterator;
	 // iterate over every individual in cell, use only big trees
	if(!treeIndividual.empty()) {
		for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) {	
			cover = (*treeIterator)->get_coverM_rel(month);
			hc += (*treeIterator)->get_heightM_m(month) * cover;
			coversum += cover;
		}
	}	
	return coversum > 0? hc/coversum : 0.0;
}

float CELL::get_total_coverM_rel(month_names month) {
	float wc = get_woody_coverM_rel(month);
	float hc = herbs.get_coverMax_rel();

	return min(1.0f,wc+hc);
}

/**
This function produces higher values than those used by Mathaj 2007. It is based on the table by Ven-Te Chow (1959)
 reproduced in Mathaj 2007, page <i>v</i>. The function uses the (yet unchanged) vegetation cover of the current month,
 which equals that of the month before.
*/
float CELL::get_kStr(month_names month) {
	
	float ac = get_total_coverM_rel(month);
	float wc = get_woody_coverM_rel(month);
	float tc = get_tree_coverM_rel(month);
	float kStr = 20.0; // mixed vegetation;
	
	if (ac < 0.5) // mostly bare ground
		kStr = 40.0;
	else if (ac-wc > 0.5) // mostly herbs
		kStr = 30.0;
	else if (wc - tc > 0.5) // mostly shrubs
		kStr = 15.0;
	else if (tc > 0.5) // mostly trees
		kStr = 10.0;

	return kStr;
}

float CELL::get_infiltration_rate_mmh(month_names month) {
	float ac = get_total_coverM_rel(month);
	float wc = get_woody_coverM_rel(month);
	float tc = get_tree_coverM_rel(month);
	float infiltration_rate_mmh = 2.0; // mixed vegetation;
	
	if (ac < 0.5) // mostly bare ground
		infiltration_rate_mmh = 5.0;
	else if (ac-wc > 0.5) // mostly herbs
		infiltration_rate_mmh = 15.0;
	else if (wc - tc > 0.5) // mostly shrubs
		infiltration_rate_mmh = 25.0;
	else if (tc > 0.5) // mostly trees
		infiltration_rate_mmh = 50.0;
	infiltration_rate_mmh*=2.5; // [W 6.5.0] to adjust to median soil moisture used in Wadiscape 3 
	return zZzo->normal(infiltration_rate_mmh, infiltration_rate_mmh * 0.2);
}


float CELL::get_heightM_m(month_names month) {
	float th = get_tree_heightM_m(month);
	float bh = dwarfshrubs.get_heightM_m(month);
	float hh = herbs.get_heightM_m(month);
	float tc = get_tree_coverM_rel(month);
	float bc = dwarfshrubs.get_coverM_rel(month);
	float hc = herbs.get_coverM_rel(month);
	
	return tc+bc+hc > 0.0 ? (th*tc + bh*bc + hh*hc)/(tc+bc+hc) : 0.0;
}

float CELL::get_LAIM_rel(month_names month) {
	float tree_LA = 0.0;
	if(!treeIndividual.empty()) {
		std::list<TREE*>::iterator treeIterator;
		for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) { 
			// iterate over every individual in cell		
			tree_LA += (*treeIterator)->get_leaf_area_m2(month);
		}
	}
	return (tree_LA*q1_cell_area_m2 + dwarfshrubs.get_LAIM_rel(month) + herbs.get_LAIM_rel(month));
}


float CELL::get_consumed_massM_kgcell(month_names month)
{
		std::cerr << "undefined function: CELL::get_consumed_massM_kg\n" << std::endl;
	return 1.0;
}

/**
The mass removed from each growth form is proportional to its abundance. Returns 
 the total amount of eaten biomass.
 */
float CELL::forage (float actual_monthly_demand_kg_per_cell, month_names month) {
	float tree_mass_kg = get_tree_feedM_kg(month); // only leaves up to 1.5 m
	float shrub_mass_kg = dwarfshrubs.get_feedM_kg(month); // only leaves up to 1.5 m
	float herb_mass_kg = herbs.get_feedM_kg(month);
	float demand_kg = 0.0;
	float eaten_kg = 0.0;
	
	if(herb_mass_kg < 0) std::cerr << "herb mass = " << herb_mass_kg << " before grazing" << std::endl;
	if(shrub_mass_kg < 0) std::cerr << "shrub mass = " << shrub_mass_kg << " before grazing" << std::endl;
	if(tree_mass_kg < 0) std::cerr << "tree mass = " << tree_mass_kg << " before grazing" << std::endl;

	if (herb_mass_kg) {
		 //grazing in proportion to herbaceous mass
		demand_kg = actual_monthly_demand_kg_per_cell * herb_mass_kg/(herb_mass_kg + shrub_mass_kg + tree_mass_kg);
		demand_kg = min(ceil(demand_kg), double(herb_mass_kg)); // avoid too tiny values
		herbs.removeMassByForaging(month, demand_kg);
		eaten_kg += demand_kg;
	}
	if (shrub_mass_kg) {
		 //grazing in proportion to shrub mass
		demand_kg = actual_monthly_demand_kg_per_cell * shrub_mass_kg/(herb_mass_kg + shrub_mass_kg + tree_mass_kg);
		demand_kg = min(ceil(demand_kg), double(shrub_mass_kg)); // avoid too tiny values
		dwarfshrubs.removeMassByForaging(month, demand_kg);
		eaten_kg += demand_kg;
	}
	if (tree_mass_kg) {
		 //grazing in proportion to tree mass
		demand_kg = actual_monthly_demand_kg_per_cell * tree_mass_kg/(herb_mass_kg + shrub_mass_kg + tree_mass_kg);
		std::list<TREE*>::iterator treeIterator;
		float cell_tree_mass_eaten_kg = 0.0;
		for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) {
			float before_mass_kg = (*treeIterator)->get_feed_mass_g()*0.001;
			float ind_tree_mass_eaten_kg = (*treeIterator)->removeMassByForaging(month, demand_kg);
			cell_tree_mass_eaten_kg += ind_tree_mass_eaten_kg; 
			if((*treeIterator)->get_feed_mass_g()<0) std::cout<< before_mass_kg << " - " << cell_tree_mass_eaten_kg << "< 0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;		

			if (cell_tree_mass_eaten_kg >= demand_kg) break;
		}	// end for each tree	
		eaten_kg += demand_kg;
	}
	return eaten_kg;
}

void CELL::initialize_composition(climate_names climate) {
	
	dwarfshrubs.initialize(climate, this);
	herbs.initialize(climate, this);
	float z = z01();
	ITHABURENSIS* new_ithaburensis = pIthaburensis->initialize(climate, this);
	CALLIPRINOS* new_calliprinos = pCalliprinos->initialize(climate, this);
	if (z<0.5 && new_ithaburensis != NULL)
		treeIndividual.push_back(new_ithaburensis);
	if (new_calliprinos != NULL)
		treeIndividual.push_back(new_calliprinos);
	if (z>=0.5 && new_ithaburensis != NULL)
		treeIndividual.push_back(new_ithaburensis);
}


/**
 Germination fraction is between 75 and 100% (Oppenheimer 1933 in Oppenheimer 1940). Seeds quickly lose their
 viability when they dry out (www.pfaf.org, Waisel & Friedman 1960). Under dense tree canopy germination fraction 
 is 2% (Alon & Kadmon 1996). 
 Germination is higher under shrubs than in the open (Harif in Kaplan).
 Seeds need more than three months to germinate (Alon & Kadmon 1996)
 */
int CELL::germinateCalliprinos(WATER* W, month_names month) {
	/* How to _increase_ germination rates:
	 - increase fraction added to 'calliprinos_germination_fraction' when surface_water_mm > x
	 - decrease fraction subtracted from 'calliprinos_germination_fraction' when surface_water_mm < x
	 - decrease threshold 'x' in 'if (surface_water_mm < x)'
	 - decrease threshold 'x' in 'if (surface_water_mm > x)'
	 - adjust parameters in CELL::seedbank_mortality()
	 */
	unsigned long int germinated = 0;
	float surface_water_mm = W->get_mWater();
	float z2=z01();
	float gt = 100.0; // threshold for increasing germination fraction
	float lt = 60.0; // threshold for decreasing germination fraction
//	unsigned long int csb = calliprinosSeedbank;
	switch (month) {
		case Aug: break;
		case Sep: break;
		case Oct: break;
		case Nov: break;
		case Dec:
			if (surface_water_mm > gt)
				calliprinos_germination_fraction += 0.35;
			break;
		case Jan:
				if (surface_water_mm > gt)
					calliprinos_germination_fraction += 0.35;
				else if (surface_water_mm < lt && calliprinos_germination_fraction > 0.0) 
					calliprinos_germination_fraction -= 0.25;
			break;
		case Feb: 
			if (surface_water_mm > gt)
				calliprinos_germination_fraction += 0.35;
			else if (surface_water_mm < lt && calliprinos_germination_fraction > 0.0) 
				calliprinos_germination_fraction -= 0.25;
			
			calliprinos_germination_fraction = in (0.0f, calliprinos_germination_fraction, 1.0f);

			germinated = (long unsigned int) (calliprinos_germination_fraction * calliprinosSeedbank);
			if(z2 < calliprinos_germination_fraction * calliprinosSeedbank) germinated++;
			calliprinosSeedbank -= germinated;
			break;
		case Mar: 
			if (surface_water_mm > gt)
				calliprinos_germination_fraction += 0.35;
			else if (surface_water_mm < lt && calliprinos_germination_fraction > 0.0) 
				calliprinos_germination_fraction -= 0.25;
			
			calliprinos_germination_fraction = in (0.0f, calliprinos_germination_fraction, 1.0f);

			germinated = (long unsigned int) (calliprinos_germination_fraction * calliprinosSeedbank);
			if(z2 < calliprinos_germination_fraction * calliprinosSeedbank) germinated++;
			calliprinosSeedbank -= germinated;
			break;
		case Apr: 
			if (surface_water_mm > gt)
				calliprinos_germination_fraction += 0.35;
			else if (surface_water_mm < lt && calliprinos_germination_fraction > 0.0) 
				calliprinos_germination_fraction -= 0.25;
			
			calliprinos_germination_fraction = in (0.0f, calliprinos_germination_fraction, 1.0f);

			germinated = (long unsigned int) (calliprinos_germination_fraction * calliprinosSeedbank);
			if(z2 < calliprinos_germination_fraction * calliprinosSeedbank) germinated++;
			calliprinosSeedbank -= germinated;
			break;
		case May: calliprinosSeedbank = 0; // too late; Oppenheimer 1940
			break;
		case Jun: break;
		case Jul: break;
	}
//	std::cout << "water: " << surface_water_mm << ", gf: " << calliprinos_germination_fraction << ", csb: " << csb << std::endl;

	return germinated;
}

/**
 Germination fraction is 70-90% (Piotto & Di Noi 2003).
 */
int CELL::germinateIthaburensis(WATER* W, month_names month) {
	/* How to _increase_ germination rates:
	 - increase fraction added to 'ithaburensis_germination_fraction' when surface_water_mm > x
	 - decrease fraction subtracted from 'ithaburensis_germination_fraction' when surface_water_mm < x
	 - decrease threshold 'x' in 'if (surface_water_mm < x)'
	 - decrease threshold 'x' in 'if (surface_water_mm > x)'
	 - adjust parameters in CELL::seedbank_mortality()
	 */
	unsigned long int germinated = 0;
	float surface_water_mm = W->get_mWater();
	float z2=z01();
	float gt = 90.0; // threshold for increasing germination fraction
	float lt = 50.0; // threshold for decreasing germination fraction
	
	switch (month) {
		case Aug: break;
		case Sep: break;
		case Oct: break;
		case Nov: break;
		case Dec:
			if (surface_water_mm > gt)
				ithaburensis_germination_fraction += 0.35;
			break;
		case Jan:
				if (surface_water_mm > gt)
					ithaburensis_germination_fraction += 0.35;
				else if (surface_water_mm < lt && ithaburensis_germination_fraction > 0.0)
					ithaburensis_germination_fraction -= 0.25;
			break;
		case Feb: 
			if (surface_water_mm > gt)
				ithaburensis_germination_fraction += 0.35;
			else if (surface_water_mm < lt && ithaburensis_germination_fraction > 0.0)
				ithaburensis_germination_fraction -= 0.25;
			
			ithaburensis_germination_fraction = in(0.0f, ithaburensis_germination_fraction, 1.0f); // keep within [0,1]

			germinated = (long unsigned int) (ithaburensis_germination_fraction * ithaburensisSeedbank);
			if(z2 < ithaburensis_germination_fraction * ithaburensisSeedbank) germinated++;
			ithaburensisSeedbank -= germinated;
			break;
		case Mar: 
			if (surface_water_mm > gt)
				ithaburensis_germination_fraction += 0.35;
			else if (surface_water_mm < lt && ithaburensis_germination_fraction > 0.0)
				ithaburensis_germination_fraction -= 0.25;
			
			ithaburensis_germination_fraction = in(0.0f, ithaburensis_germination_fraction, 1.0f); // keep within [0,1]

			germinated = (long unsigned int) (ithaburensis_germination_fraction * ithaburensisSeedbank);
			if(z2 < ithaburensis_germination_fraction * ithaburensisSeedbank) germinated++;
			ithaburensisSeedbank -= germinated;
			break;
		case Apr: 
			if (surface_water_mm > gt)
				ithaburensis_germination_fraction += 0.35;
			else if (surface_water_mm < lt && ithaburensis_germination_fraction > 0.0)
				ithaburensis_germination_fraction -= 0.25;
			
			ithaburensis_germination_fraction = in(0.0f, ithaburensis_germination_fraction, 1.0f); // keep within [0,1]

			germinated = (long unsigned int) (ithaburensis_germination_fraction * ithaburensisSeedbank);
			if(z2 < ithaburensis_germination_fraction * ithaburensisSeedbank) germinated++;
			ithaburensisSeedbank -= germinated;
			break;
		case May: ithaburensisSeedbank = 0; // too late
			break;
		case Jun: break;
		case Jul: break;
	}
	
	return germinated;
}

unsigned long int CELL::get_total_calliprinos_acorns_in_cell(void) {
	unsigned long int TotalNumberOfCalliprinosAcornsProducedInThisCell = 0;
	std::list<TREE*>::iterator treeIterator;
	for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) { // iterate over every individual in cell
		if ((*treeIterator)->getTreeType() == Q_calliprinos) {
			TotalNumberOfCalliprinosAcornsProducedInThisCell += (*treeIterator)->acorns[1];
		}
	}
	if (TotalNumberOfCalliprinosAcornsProducedInThisCell > 6000) // max theoretical number is 6205 when  canopy radius is 4 m and height 15 m
		TotalNumberOfCalliprinosAcornsProducedInThisCell = 6000;

	return TotalNumberOfCalliprinosAcornsProducedInThisCell;
}

unsigned long int CELL::get_total_ithaburensis_acorns_in_cell(void) {
	unsigned long int TotalNumberOfIthaburensisAcornsProducedInThisCell = 0;
	std::list<TREE*>::iterator treeIterator;
	for (treeIterator = treeIndividual.begin(); treeIterator != treeIndividual.end(); treeIterator++) { // iterate over every individual in cell
		if ((*treeIterator)->getTreeType() == Q_ithaburensis) {
			TotalNumberOfIthaburensisAcornsProducedInThisCell +=  (*treeIterator)->acorns[1];
		}
	}
	if (TotalNumberOfIthaburensisAcornsProducedInThisCell > 3000)
		TotalNumberOfIthaburensisAcornsProducedInThisCell = 3000; // no further increase because of old age, estimated number -> Johnson 1995
		/* Kaplan 1984 (Tab. 7.1, p 106) found that ithaburensis trees can bear up to 7800 acorns per tree in a good year. */

	return TotalNumberOfIthaburensisAcornsProducedInThisCell;
}

/* Mortality due to seed predation, unfavourable seed bed, etc.
 */
void CELL::seedbank_mortality(int numberOfAdultTrees, float relativeShrubCover) {
	const float call_adjustment_factor = 0.025; // adjusts survival of seeds until germination; 
											  //  used for fitting to historical distribution, see Liphschitz & Biger, JVS, 1990
	const float ith_adjustment_factor = 0.5; // adjusts survival of seeds until germination
	
	if (numberOfAdultTrees > 0) {
		calliprinosSeedbank *= (0.01 * call_adjustment_factor); // estimated from Bayesian network
		if (z01() < 0.01 * call_adjustment_factor) calliprinosSeedbank++;		
		ithaburensisSeedbank *= (0.01 * ith_adjustment_factor);
		if (z01() < 0.01 * ith_adjustment_factor) ithaburensisSeedbank++;		
	} else if (relativeShrubCover > 0.5) { // >50% cover
		calliprinosSeedbank *= (0.9 * call_adjustment_factor); // estimated from Bayesian network
		if (z01() < 0.9 * call_adjustment_factor) calliprinosSeedbank++;		
		ithaburensisSeedbank *= (0.9 * ith_adjustment_factor);
		if (z01() < 0.9 * ith_adjustment_factor) ithaburensisSeedbank++;
	} else {
		calliprinosSeedbank *= (0.7 * call_adjustment_factor); // estimated from Bayesian network + penalty for herb competition
		if (z01() < 0.7 * call_adjustment_factor) calliprinosSeedbank++;		
		ithaburensisSeedbank *= (0.7 * ith_adjustment_factor);
		if (z01() < 0.7 * ith_adjustment_factor) ithaburensisSeedbank++;
	}
}

/**  This function updates the memory of the years when a fire occurred
*/
void CELL::updateTimeSinceFire(void)
{
	fireHistory[4] = fireHistory[3];
	fireHistory[3] = fireHistory[2];
	fireHistory[2] = fireHistory[0];
	fireHistory[0] = false; // larger fires, current year
	fireHistory[1] = false; // small fires, current year
}

/** The interception of rain by a canopy has been assessed by various methods. In general, the fraction of intercepted 
 rain decreases with rainfall amount (Haworth & McPherson 1995). We assume here that the leaves represent the greatest
 intercepting surface and that each leaf has a constant capacity of water adsorption independent of rain amount. From
 casual observation we assume that wetted leaves can retain about a 0.5 mm layer of water.
 
 Haworth K, McPherson GR (1995) Effects of Quercus emoryi trees on precipitation distribution and microclimate in a 
 semi-arid savanna. J. Arid Environ. 31:153-170.
 */
float CELL::interception(float rain_mm, month_names m)
{
	rain_mm -= 0.5 * get_LAIM_rel(m); // storage capacity of canopy, 0.5 mm per square meter, my assumption
	
	if (rain_mm < 0.0)
		rain_mm = 0.0;
	return rain_mm;
}
