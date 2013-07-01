/*
 *  fire.cpp
 *  
 *
 *
 */
#include "fire.h"
#include "woodytree.h"
#include "rng.h"
#include <math.h>
#include <cstdlib>
#include <iostream>

//------------------------------------------------------------------------------------------------------------------
// Martin Mathaj 2007; fire area-frequency distribution and return interval, Malamud et al. 2005

/// calculate the number of fires per year for the whole area 
/** 
The calculations are based on frequency-area density statistics presented in Malamud et al. 2005. To calculate the 
cumulative number of fires for the total wadiscape area, I determine the triangular area under the line defined by the  
density parameters alpha and beta and the lower bound (detection limit) and upper bound (wadiscape area). 
 
 To determine the area to be burned I use the triangular area under the line defined by the  
 density parameters alpha and beta (Malamud et al. 2005) and the lower bound (detection limit) 
 and upper bound (wadiscape area). From this I subtract the triangular area under the line for a  
 lower limit > detection limit. The remainder of this area divided by the total triangular area is the relative
 cumulative number (probability) for an area smaller than the lower limit. By solving the equation for
 the lower limit I can determine the area to be burned based on a randomly selected relative frequency:
*/
float FIRE_PARAMS::calc_burn_area_m2() // Aus Wahrscheinlichkeit der Feuergroesse tatsaechliche Feuergroesse berechnen
{	
	float beta = 1.30; // Malamud, Table 1, Mediterranean
	//float z = zZzo->rand_halfclosed01(); // nur zum Testen
    //if(z==0) std::cout << "z=0!";
	
	//float temp = exp(sqrt(beta * pow(log(minimum_detection_area_m2),2.0) - zZzo->rand_halfclosed01())); //MK
	//float temp = exp(sqrt(beta * pow(log(minimum_detection_area_m2),2.0)) - log(zZzo->rand_halfclosed01())); //MK+KG
	//float temp = exp(log(total_grid_area_m2)-sqrt(pow((log(total_grid_area_m2)-log(minimum_detection_area_m2)),2.0)-2.0*(log(zZzo->rand_halfclosed01()))/beta)); //KG
	float temp = 25*pow(zZzo->rand_open01(),1/(-beta+1));  //MK+KG 
	if(temp > total_grid_area_m2) temp = total_grid_area_m2; // MK: limit fire area to grid area
	return temp;
}

float burn_subarea(CELL* C, int x, int y, int widthOfSubareaToBurn, int heightOfSubareaToBurn) {
	long unburnedCells = 0;
	int I = 0, J = 0;
	long c = 0; // cell ID
	
	for (int i = x; i < x+widthOfSubareaToBurn; i++) {
		for (int j=y; j < y+heightOfSubareaToBurn; j++) {
			I = i<NZ? i : i%NZ;  // Hoehe von der anderen Seite nehmen, wenn am Rand!!!
			J = j<NS? j : j%NS;  // Hoehe von der anderen Seite nehmen, wenn am Rand!!!
			
			c = I * NZ + J;
			
			if (!C[c].fireHistory[0] && !C[c].fireHistory[1] && !C[c].fireHistory[2] && !C[c].fireHistory[3] && !C[c].fireHistory[4]) {
				C[c].dwarfshrubs.cover *= 0.5; //  50% of shrub cover regrows within one year, Perevolotsky et al. 2001
				C[c].herbs.cover = 0.0; //MM,  herbaceous cover is removed completely
				
				std::list<TREE*>::iterator treeIterator;
				for (treeIterator = C[c].treeIndividual.begin(); treeIterator !=C[c].treeIndividual.end(); treeIterator++) {
					if ((*treeIterator)->burnTree(C[c].herbs.mass_kgm2)) {// burn it and if it died, remember it
						delete *treeIterator;
						treeIterator = C[c].treeIndividual.erase(treeIterator);
					}
				}
				C[c].fireHistory[0] = true;
				if (widthOfSubareaToBurn * heightOfSubareaToBurn <= all_cells * 0.1)
					C[c].fireHistory[1] = true; //MM/MK, Feuer die kleiner 1/10 (MM: 1/9) der Simulationsflaeche sind, kommen in Feuerspeicher, damit dort nicht gleich wieder ein Feuer im selben Jahr ausbricht
			} // if
			else {
				unburnedCells++; //MM, Falls nicht die ganze Gitterflaeche Feuer gefangen hat, dann Zweitfeuer
			} // else
		} // for i
	} // for j
	return unburnedCells * cell_area_m2 ;
}

float FireDisturbance(WATER* W, CELL* C, FIRE_PARAMS* pFireParams, month_names month) {
	long thisCell = 0;
	float length = 0.0;
	int widthOfSubareaToBurn_m = 0, heightOfSubareaToBurn_m = 0;
	
/* currently not used: fire frequency distribution depending on soil moisture
	// set the parameters for Malamud et al (2005)'s equation
	// the variation with climate and vegetation is a wild guess proposed by MM
	pFireParams->Vegetationsdichte=0.0; // MM, Feuerinfos loeschen
	pFireParams->Wassersaettigung=0.0; // MM, Feuerinfos loeschen

	for (uint32 c = 0; c < all_cells; c++) {
			// use prvious year's values (still stored in the arrays)
			pFireParams->Vegetationsdichte += ((C[c].herbs.get_coverM_rel(month) * 0.5) * 100.0
			  +(C[c].dwarfshrubs.get_coverM_rel(month) * 1.5) * 100.0) * perCell; //weighted cumulative cover
			pFireParams->Wassersaettigung += W[c].get_wavg_water_mm() * perCell; //MM, weighted mean water availability across the whole landscape
	}
	
	pFireParams->log_alpha = log(2.906E-4 + 1.3905E-7 * pFireParams->Wassersaettigung
		- 3.81E-11 * pow(pFireParams->Wassersaettigung - 550.0, 2.0));
	pFireParams->beta = 1.0 * (1.2902914 - 4.541E-4 * pFireParams->Vegetationsdichte
						   + 2.8E-6 *pow(pFireParams->Vegetationsdichte - 130.0, 2.0)
						   - 2.8312E-8 *pow(pFireParams->Vegetationsdichte - 130.0, 3.0)); // MM, beta aus Biomasse (beeinflusst stark die Feuergroesse)
	
*/
	float fireArea_m2 = 0.0;
	float fireArea_m2_forOutput = 0.0;
	

	fireArea_m2 = pFireParams->calc_burn_area_m2(); // random fire area
	fireArea_m2_forOutput = fireArea_m2;
	int fireCounter = 0; // additional break condition, so we don't try for too long
	while (fireArea_m2 > cell_area_m2 && fireCounter<100) {
		fireCounter++;
		int xx, yy;
		do {
			xx = int(zZzo->uniform(0, NZ)); //MM, choose coordinates for fire to start where there was no fire in the previous five years
			yy = int(zZzo->uniform(0, NS));
			thisCell = xx*NZ+yy;
		} while (C[thisCell].fireHistory[0] && 
				 C[thisCell].fireHistory[1] && 
				 C[thisCell].fireHistory[2] && 
				 C[thisCell].fireHistory[3] &&
				 C[thisCell].fireHistory[4]);
		
		 //MM, determine length and number of cells to be burned; this algorithm allows rectangles or squares
		length = sqrt(fireArea_m2);
		widthOfSubareaToBurn_m = int((z01()+0.5) * length); // vary length between 0.5 and 1.5 of square edge
		if (widthOfSubareaToBurn_m < cell_length_m)
			widthOfSubareaToBurn_m = int(cell_length_m); //MM, minimal width = 25 m
		if (widthOfSubareaToBurn_m > cell_length_m * (NS-1))
			widthOfSubareaToBurn_m = int(length); //MM, maximal width, unless the whole wadiscape burns
		
		heightOfSubareaToBurn_m=int(floor(fireArea_m2 /(float)widthOfSubareaToBurn_m));
		if (heightOfSubareaToBurn_m < cell_length_m)
			heightOfSubareaToBurn_m = int(cell_length_m);
		if (heightOfSubareaToBurn_m > cell_length_m * (NZ-1))
			heightOfSubareaToBurn_m= int(length);
		
		fireArea_m2 = 
			burn_subarea(C, xx, yy, 
						 int(widthOfSubareaToBurn_m/cell_length_m), int(heightOfSubareaToBurn_m/cell_length_m)); // MM, Flaeche brennt ab
	}

	if(fireArea_m2_forOutput>100000) std::cout << " FIRE!!!!!!\n";
	else if(fireArea_m2_forOutput>10000) std::cout << " Fire!!!!\n";
	else if(fireArea_m2_forOutput>100) std::cout << " Fire!!\n";
	else if(fireArea_m2_forOutput>0) std::cout << " Fire\n";

	
	return fireArea_m2_forOutput;
}
