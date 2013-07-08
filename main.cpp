/*
 Version 3.1
 Adaptation for B2 climate change scenario based on data provided by Andreas
 Heckl, Kassel. The original data had a resolution of 18 square kilometers.

 Version 3.2
 Uses climate change scenarios CT, A2, B2 based on data provided by Alpert et al.

 Version 3.2.1
 Spatial output: summaries for each row of cells parallel to the wadi

 Version 3.2.2
 [Improved function for calculation of seed production of annuals in cell.cpp:prod_seeds().]
 New parameters retained but new function removed - biomass was less than half!!!
 Rearranged output order of columns. SD of production is output only when requested.

 20070519: cosmetic changes to output. The statistics are now a function that
 can be calculated before or after grazing (in year.cpp). The appropriate column
 title can be changed by redefining " << when << "

 Version 3.2.3
 Version for transient climate change scenarios (A2, 50 km, Garmisch)
 20070702: changed food demand from 10.0 to 13.5 kg/LSU/d; simulations run only to year 2050; rain is read from a file
 20070705: wadiscape ID and slope can now be passed as parameters to the programme at startup;
 20070706: output is separated into one file with column headers and a 2nd file with data; rain series are now drawn 
           with each new LSU parameter to increase variability

 Version 3.2.4
 Rain series read from daily data, a random year is chosen from the series within a moving window of x years.
 New transition probabilites for herb growth and seed production.
 Food demand back to 10.0 kg/LSU/d, it's the value used by FAO, ICARDA and more
 
 Version 4.0 (never used for simulations)
 More plant types: evergreen and deciduous trees (Code for trees by Anja Prüfert)
 Monthly time steps

 Version 5.0
 Improvement of tree module (Anja Prüfert, Martin Köchy)
 Included the fire module of Martin Mathaj (Anja Prüfert, Martin Köchy)
 Rearrangement of classes in cpp and header files
 Food demand back to 10.0 kg/LSU/d, it's the value used by FAO and ICARDA and it is more
 representative of rural breeds than the 1.35 kg/goat in Perevolotsky et al. 1998.
 Foraging can be restricted to part of the year: common practice in the Israel (Gutman et al. 2000)
 Included parts of the soil erosion and runoff module of Martin Mathaj.
 
 Version 6.0
 Clean up of Version 5, debugging

 Version 6.0.1
 Tree seedlings represented now by 1 instance of the tree class and a multiplier in cell. This should reduce simulation time
 
 Version 6.0.2
 LSU steps changed to 5/km2 and going down from max to min.
 external files moved to two folders at ../topographies and ../rainfiles
 
 Version 6.0.3
 The program still runs out of memory. Therefore tree saplings are now also represented by a single instance of the tree class
 and a multiplier in a cell.
 
 Version 6.0.4
 Reverted program to individual tree seedlings and saplings because the memory problem was not solved. Reduced the 
 static arrays for output to the months needed. Made the spatial output and detailed tree output optional.
 
 Version 6.0.5
 Changed algorithm for counting food searches (graze_attempts). Now each search is counted. Before, only successful 
 searches were counted. Up to five input parameters at start up now possible.
 
 Version 6.1.0 2010-01-14
 Memory problems remain with runs for the mesic Mediterranean. New attempt to group seedlings as in v6.0.2
 Simulation years end 2055 (2010-01-22).
 
 Version 6.1.1 2010-02-05
 Made seed desiccation for Q. calliprinos more probable because too many seedlings germinated in T and M.
 
 Version 6.1.2 2010-02-16
 Factor for calculation of herb LAI was off by one order of magnitude. Reparameterized shrub LAI. Moved palatability
 factor. Changed Tree output format. 2010-03-02: Annual output for each factor combination is written to a separate file.
 
 Version 6.1.3 2010-03-17
 Switch for fire module added to list of start-up parameters 
 
 Version 6.2.0
 Spatial output of herb cover and total cover reflects maximum herbaceous cover in a vegetation year (Aug - Jul). Fixed 
 output of runoff and erosion. No validation check yet. Output of runoff is in mm (litres/m2), output of erosion is kg/m2. 
 Column headers of spatial output changed accordingly.
 
 Version 6.2.1 2010-04-13
 Time series of rain now extend beyond 2050. Therefore, simulation time now ends 2060, climate window is now 30 years, 
 spatial output is for 1961, 1990, 2020, and 2045.
 
 Version 6.3.0 2010-05-07
 For simplicity and consistency within the model, rain onto inclined surfaces is no longer reduced by cos(slope).
 Fixed error in sediment algorithm.
 Added an else-clause in getMeanDuration_hours (rain.cpp)
 Reduced the buffer for moving the rain window at start and end of rain data base to half the window size (2010-05-28).
 Name of tree output file indicates clearly the absence of fire.
 Fixed runoff algorithm. flow_direction_id is now global, its default value is -1.
 Spatial output is for 1961, 1990, 2025, and 2045.
 'Erosion' in spatial output file renamed 'Deposition' to match sign of number.
 
 Version 6.3.1 2010-06-13
 Changed the definition of length of months and q1_365 so that changes to days_in_year (e.g. to 360) are reflected for 
 time series of rain with short years. 
 No changes in the calculation of the monthly feed requirements of livestock. Calculations of daily evapotranspiration
 and temperature are also affected by days_in_year but I did not correct for the length of year here. I believe this does
 not make any significant difference.
 
 Version 6.4.0 2010-06-21
 Corrected the calculation of DWARFSHRUB::calc_LAI_rel(month_names m).
 Made the initial value of shrub cover dependent on climate region.
 
 Version 6.4.1 2010-07-10
 Corrected initialization of oaks in standard constructor.
 Corrected deleting of objects (trees) on the free store in the CELL deconstructor.
 Moved the construction of tree objects for seedlings and saplings from cell.cpp to year.cpp.
 Turned the initial tree objects in the CELL constructor into a pointer. 
 TODO: CHECK THAT THIS DID NOT AFFECT THE OUTPUT!
 Changed output from 2045 to 2050.
 
 Version 6.5.0 2010-08-29, 2010-09-08
 Increased infiltration rates (CELL::get_infiltration_rate_mmh) so that available soil moisture matches that in 
 Wadiscape 3.
 Corrected algorithm of calculating shrub leaf mass. (Leaf mass conversion was applied only to annual increment of 
 cover not to existing cover.
 Changed name of column "N.kg.m2.Fmass.all" in annual_stats to "N.Fmass.kg.m2.all".
 Changed allometric equations for Q. ith leaf mass and shoot mass.
 
 Version 6.5.1 2010-09-17
 Corrected  double subtraction of infiltrating water from aboveground water. This error was effective only under Windows.
	2010-10-01, 2010-10-07
 Changed allometric equation for Q. call and Q. ith leaf mass.
 
 Version 6.5.2 2010-11-05 [KG]
 Decreased values of ithaburensis_germination_fraction in CELL::germinateIthaburensis (0.35 to 0.25 and 0.2 to 0.1)
 to slow down C.ithaburensis density growth 

 Version 6.5.3 2010-11-22 - 2010-12-12
 Changes to tree parameterization to reduce long-term cover in Mediterranean climates. Further changes to germination
 fractions and mortality of both tree species. Limited tree radius of ithaburensis.
 Added cover to detailed tree output.
 Changed calculation of total and tree cover.
 Changed calculation of tree LAI. LAI is calculated as total leaf area divided by cell area (before, it was LA/crown area)
 Fixed an error that caused trees to access bedrock water only from cell [0,0].

 Version 6.5.4 2011-02-21
 Fixed errors in the fire area calculation, in the function for deleting burned trees, in the function to set the fire 
 area coordinates.
 
 Version 6.5.5
 Annual shrub growth was not based on the amount of water of the whole year. Now it's based on the previous year's total water.
 
 Version 6.6.0 2011-03-21
 Increased the effect of adult trees on mortality of saplings and other adult trees.
 Changed the fire input parameter from "f" to the interval between fire years.
 Removed the function to calculate number of fires per year because this is set at startup and we assume now that there 
 is a 50% chance of having a fire in either May or June.
 Changes in year() and FireDisturbance() for recording whether there was a fire.
 In annuals_stats there is a new column for fire area (m2).
 Changed the seedling mortality factors of calliprinos and ithaburensis so that Q calliprinos dominates (Liphschitz & Biger 1990)

 Version 6.6.1 2011-04-29, 2011-05-04, 2011-05-16
 Changes to fire-mortality parameters of Q. ith. and Q. call.
 Changes to grazing effect on seedlings in ITHABURENSIS/CALIPRINOS::removeMassByForaging.
 Included fire frequency in output file names.
 
 Version 6.6.2 2011-05-19
 Added the effect of sapling competition to mortality, competitive effect differs between ith and cal.
 Changed transpiration of ith. Water is now removed from upper soil layer.
 
 2011-05-30
 Accelerated growth for calliprinos (more water, greater height, greater radius; see notes in grow function. This indicates 
 gaps of knowledge of species ecology. Changed initial probabilities of distribution of species. 
 Increased risk of death by fire for both oak species to get an effect of fire. Shows more gaps in knowledge.
 Corrected a formula in the calculation of biomass of Q. caliprinos.
 Stronger grazing effect on tree seedlings.
 
 Version 6.7.0 2011-06-17/20
 Introduced oak mortality due to desiccation (in grow_month) with different susceptibilities for each species.
 Increased susceptibility of seedlings to sapling competition.
 Increased transpiration.
 Moved germination routine to after seedbank mortality.
 Bedrock water is limited to 450 mm (accounting for drainage and storage capacity).
 Changed ithaburensis' germination fractions (faster germination).
 Increased maximum number of ithaburensis seeds produced per cell.
 Made sure that integer seed numbers multiplied by float values < 1 do not always become zero (seed bank mortality
 and germination).
 Reduced exponents in oak mortality (-> higher mortality rate).
 Replaced max by min function in HERB::grow to limit number of seeds per cell.
 In several places replaced code of the form v=f(v) by t=f(v); v=t; just in case the function is evaluated after the assignment.
 In germination... changed code of the form "(long int) u * v" where u is within [0,1] and v is a long int to "(long int) (u * v)".
 Output of how many individuals have died from desiccation, how many seeds have germinated to console.
 Corrected an error that caused germination to access always the water status of the first cell only.
 
 Version 6.8.0 2011-06-21
 Added a function to simulate tree cutting. At calling the program on the console, the cutting frequency can be indicate
 as the seventh parameter. The cutting is done in regular intervals to the height specified in year(), currently 0.5 m.
 
 Version 6.8.1 2011-07-08
 Made the order among trees random for initialization and germination.
 
 Version 6.8.2 2011-09-06
 Calibrated shrub cover in arid climate. Made Q. cal. sensitive to repeated burning. Restructured growth of dwarfshrubs.
 Added monthly output for runoff and erosion. (Increased burn-in period, set cut height from 0.5 to 0.3 m.)
*/
#include <math.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "rain.h"
#include "annrain.h"
#include "ReGen.h"
#include "water.h"
#include "init.h"
#include "year.h"
#include "summary_out.h"
#include "read_landscape.h"
#include "radiation.h"

/*
 The program expects that some variables are supplied when it is called.
 The first variable is the slope of the wadiscape, in integer degrees, 
 The second variable is the landscape ID. 
	If the landscape ID is not supplied the program will iterate over IDs 0 through 5.
 The third variable is the climatic region.
	If the region ID is not supplied the program will iterate over all regions (arid to mesic Mediterranean).
 The fourth and fifth variable is the grazing intensity (from - to) in FAO livestock units per km^2.
	If the number of LSUs is not supplied, the program will iterate from 0 to 80.
 The sixth variable is the fire frequency (years).
 The seventh variable is the cutting frequency (years).

 ===>> Global variables for simulation scenarios can be set in main.h.
 */
int slope;
RNG* zZzo;
long N_cells = 0, S_cells = 0, W_cells = 0, P_cells = 0;
int N_edge = 0, S_edge = 0, W_edge = 0, P_edge = 0;

// ===========================================================================
// ========== THE PROGRAM STARTS HERE ========================================
// ===========================================================================
int main(int argc, const char * argv[]) {

	// write an error message and stop program if the input was not as expected
	if (argc < 2 || argc > 9) {
		std::cout
				<< "usage is one of the following: \n"
					"   'wadiscape slope landscapeID climateID LSUmin LSUmax fireFreq cutFreq (e.g. '15 3 A 7 80 80 5')\n"
					"   'wadiscape slope landscapeID climateID' (e.g. '15 3 A') to iterate from 0 to 80 LSU/km²\n"
					"or 'wadiscape slope landscapeID' (e.g. '15 3') to iterate over all climates and LSUs\n"
					"or 'wadiscape slope' (e.g. '15') to iterate over all landscapeIDs, climates, and LSUs)n";
		return 1;
	}

	//========== set scenario variables =====================================

	// set the simulation time, see main.h for definitions
	const int maxYears = years_0 + years_CT + years_trans + years_scen; // till 2050
	
	// put slope of wadiscape into a local variable
	const char* Slope = argv[1];

	// convert from text to number
	slope = atoi(Slope); // degrees

	// set a character indicating that all wadiscape slopes have been used
	// this character is appended to the output file
	const char a = 'a';

	// if a landscape ID was supplied at startup (two arguments supplied by user)
	// the ID is in position 2, put ID into a local variable
	const char* LID = argc > 2 ? argv[2] : &a;

	// if a landscape ID was supplied iterate only over that landscape
	// if not, iterate over landscapes 0 through 5
	const int minLandscape = argc > 2 ? atoi(LID) : 0; // convert char argument to integer
	const int maxLandscape = argc > 2 ? atoi(LID) : 5;// convert char argument to integer
	
	// set a character indicating that all wadiscape slopes have been used
	// this character is appended to the output file
	const climate_names c = allC;

	// if a climate ID was supplied at startup (three arguments supplied by user)
	// the ID is in position 3, put ID into a local variable
	const climate_names CID = argc > 3 ? to_climate_names(*(argv[3])) : c;
	
	// if a climate ID was supplied iterate only over that climate
	// if not, iterate over all climates from arid to mesicmed
	const climate_names CIDmin = argc > 3 ? CID : arid;
	const climate_names CIDmax = argc > 3 ? CID : mesicmed;

	// set the bounds for iteration over lifestock units
	const int minLSU_km2 = argc > 4 ? atoi(argv[4]) : 0;  // 1 LSU/ha = 100 LSU/km2, 20 LSU/km2 is intense grazing
	const int maxLSU_km2 = argc > 5 ? atoi(argv[5]) : (argc < 5 ? 80 :minLSU_km2); // if no LSU is provided, step from 0 to 80; if one LSU is provided, do only that; if two are provided, step from first to second
	
	// turn the fire module on or off
	const int fire_interval = argc > 6 ? atoi(argv[6]) : 0;

	// turn tree cutting on or off
	int cf = argc > 7 ? atoi(argv[7]) : maxYears+1;
	const int cutFreq = cf ? cf : maxYears+1;

	// turn trees on or off
	const bool simtrees = argc > 8 ? (*(argv[8])=='T'?true:false) : false;
	
	
		zZzo = new RNG; ///< pointer to a random-number-generator object for repeated use during the simulation
	SOLAR_RADIATION* p_earth = new SOLAR_RADIATION(32.25); // 32°15' center of lower Jordan River valley

	// a pointer to the rain database
	RAINSERIES* pTS = NULL;
	/*temp added*/ //ReGen* pTS;
	RAINPARAMETERS* pP = NULL; // v.4.0.0, used only by ReGen class

	uint32* sID = new uint32[all_cells]; ///< array for indexing
	uint32* my_elevation = new uint32[all_cells]; ///< array storing the cell elevation, used for sorting

	// additional grids used by MMathaj2007
	WATER::soilP = new SOIL_PARAMS(0.02); //if grain fractions are to differ among cells, this must be a grid
	WATER::p_my_earth = new SOLAR_RADIATION(32.5); // create an object, 32.25° ≈ latitude of central lower Jordan valley
	
	for (int ll = minLandscape; ll <= maxLandscape; ll++) {
		float* contributing_cells = new float[all_cells];

		// create a grid containing water information
		WATER* W = new WATER[all_cells];

		read_landscape(W, ll, contributing_cells, slope, my_elevation);
		initialize(W, contributing_cells);
		delete[] contributing_cells;

		// sort W by elevation. This is needed for the runoff algorithm.
		//sort(sID.begin(), sID.end(), gt(W)); // elegant, --but does not work
		indir_sort<uint32> (my_elevation, all_cells, sID);

		for (scenario_names scenario = A1B; scenario <= A1B; ++scenario)
		{
			for (climate_names climate = CIDmin; climate <= CIDmax; ++climate) {
				CLIMATE grid_climate(20.0, 7, 32.5);

				VEGETATION::our_climate = climate;

				if(use_RGCM_time_series) {
					pTS = new RGCM(climate, scenario, use_RGCM_time_series);
				}
				
				int step = 2;
				for (int LSU_km2 = minLSU_km2; LSU_km2 <= maxLSU_km2; LSU_km2
						+= step) {
					try {
						std::cout << "landscape: " << ll << "\t-\tscenario: "
								<< scenario << "\t-\tclimate: " << climate
								<< std::endl;
						std::cout << "\tLSU/km2: " << LSU_km2 << "  -  fire interval: " << fire_interval;
						std::cout << "  -  cut interval: " << (cutFreq>maxYears?0:cutFreq) << std::endl;

							
						// create output files
						std::stringstream outfile_name;
							outfile_name << "annual_stats_" << Slope << "_" << ll << "_" << climate << "_"  << scenario
								<< "_" << LSU_km2 << "_" << fire_interval << "_c" << (cutFreq>maxYears?0:cutFreq) 
								<< (simtrees?"_T":"N") << ".txt";

						std::stringstream spatial_outfile_name;
							spatial_outfile_name << "spatial_" << Slope << "_" << ll << "_" << climate << "_" << scenario
								<< "_" << LSU_km2 << "_" << fire_interval << "_c" << (cutFreq>maxYears?0:cutFreq) 
								<< (simtrees?"_T":"N") << ".txt";
							
						std::stringstream tree_outfile_name;
							tree_outfile_name << "tree_details_" << Slope << "_" << ll << "_" << climate << "_" << scenario
								<< "_" << LSU_km2 << "_" << fire_interval << "_c" << (cutFreq>maxYears?0:cutFreq) 
								<< (simtrees?"_T":"_N") << ".txt";
							
						monthly_output_header(outfile_name.str().c_str());

						if (spatial_output) {
							annual_spatial_output_header(spatial_outfile_name.str().c_str());
							annual_tree_output_header(tree_outfile_name.str().c_str());
						}

							
						if (LSU_km2 == 100)
							step = 50;
						if (LSU_km2 == 80)
							step = 20;
						if (LSU_km2 == 60)
							step = 10;
						if (LSU_km2 == 40)
							step = 5;

						CELL* C = new CELL[all_cells];
						FIRE_PARAMS fire_params;

						for (int iz = 0; iz < NZ; iz++)
							for (int is = 0; is < NS; is++)
								C[iz * NZ + is].initialize_composition(climate);

						for (int yy = 0; yy < maxYears; yy++) {
						//	if ((yy - years_0) % 10 == 0)
							std::cout << "\n\t\tyear: " << 1961 - years_0 + yy; // << std::endl;

							// determine a daily rain series from the time series generator 
							if(!use_RGCM_time_series) {
								ANNRAIN rain(climate, scenario); // used for selecting an annual value from a gamma distribution
								pP = new RAINPARAMETERS (int(rain.get_annual_rain(yy)));
								pTS = new ReGen(pP);
							}
							if(use_RGCM_time_series)
								pTS->drawYear(yy, windowSize);
							else
								pTS->drawYear(yy);

							float year_rain = pTS->getAnnualRain();
							std::cout << " (rain: " << int(year_rain) << " mm)"; 
							
							float mrain[12] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
							// end: determine monthly rain
							
							bool fire = false;
							if(fire_interval>0)
								fire = zZzo->rand_halfopen01()<1.0/float(fire_interval)?true:false;

							month_names fireMonth = nvr;
							
							bool cutNow = !((yy+10)%cutFreq); // true when remainder equals zero, starts after 10 years
							
							float did_burn = year(yy, W, C, &fire_params, yy < years_0 / 2 ? 0.0 : LSU_km2 * 0.01, pTS, mrain, sID, 
								 grid_climate, spatial_outfile_name.str().c_str(), fire, cutNow, fireMonth, simtrees);
							// austauschbarer Header in year.h um das Problem des fehlenden Regen-Objekts zu umgehen.

							//----- detailed output to screen for testing
							/*for (int iz = 0; iz<NZ; iz++)
							 for (int is = 0; is<NS; is++)
							 std::cout<< iz << "\t" << is << "\t" << ll << "\t" << climate << "\t" << float(LSU_km2*0.01) << "\t" << yy<< "\t" <<
							 V[iz*NZ+is].ann_mass << "\t" << V[iz*NZ+is].shrub_cov << "\t" << W[iz*NZ+is].habitat << "\t" << W[iz*NZ+is].avail_rel << std::endl;
							 */
							
	/* STANDARD	*/					if (yy >= years_0)
	///* for debugging*/						if (yy >= 0)

							//		  if (yy >= years_0+years_CT+years_trans)
							{
								monthly_summary_output(ll, climate, yy, LSU_km2, year_rain,
										mrain, scenario,
										outfile_name.str().c_str(), W, C, did_burn, fireMonth);
							
//							if (yy == 0 || (yy-years_0 == 0) || (yy-years_0 == 1990-1961) || (yy-years_0 == 2025-1961) || (yy-years_0 == 2050-1961)) {
							if(yy%5 == 0){
							if(spatial_output && simtrees) 
									tree_summary_output(maxYears, year_rain, C, yy,
										tree_outfile_name.str().c_str());
							}
							} // end if year
						} // end for yy year
						delete[] C;

					} // end try
					catch (...) {
						std::cerr<<"skipped LSU = " << LSU_km2 <<" because of errors.\n";
					}
					
				} // end for lsu
				
				if(pTS != NULL) delete pTS; pTS = NULL;// v.4.0.0
				if(pP  != NULL) delete pP;  pP  = NULL;// v.4.0.0
				
			} // end for cc region, arid … mesic Mediterranean
		} // end for IPCC climate scenario

	delete[] W;
	}// end for ll

	delete[] sID;
	delete[] my_elevation;
	delete zZzo;
	delete p_earth;
	delete WATER::soilP;
	delete WATER::p_my_earth;
	std::cout << "\nDas Programm ist vollstaendig durchgelaufen!" << std::endl;
	return 0;
}
