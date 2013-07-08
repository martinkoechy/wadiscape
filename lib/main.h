/*
 *  main.h
 *  Landschaft
 *
 *  Created by Forschung on Tue Jun 20 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <valarray>
#include <limits>
#include "rng.h"

#ifndef MARTIN_MAIN_H
#define MARTIN_MAIN_H

// ===> do not change these constants and variables

typedef unsigned int uint32; //	typedef unsigned long int uint32; // use the latter if 'int' is too short
typedef unsigned short ushort; 

enum habitat_names {South_f, North_f, Plateau, Wadi}; // 1 ... 4 laut Definition in LandscapeCreator
enum climate_names {arid, semiarid, drymed, typmed, mesicmed, allC};
enum month_names {Aug, Sep, Oct, Nov, Dec, Jan, Feb, Mar, Apr, May, Jun, Jul, nvr};
enum species_names {none, Q_calliprinos, Q_ithaburensis};
enum scenario_names {CT, A2, B1, B2, A1B};


const short NZ = 300;      //rows, West-East
const short NS = 300;      //columns, North-South

const float cell_length_m = 5.0; ///< length (m) of the edge of one cell


// physical constants
const unsigned short days_in_year = 365; 
const float _g = 9.81; // Erdbeschleunigung, 9.81 m/s²
const float pi = 3.1416; // π

// constants for run-time optimization
const uint32 all_cells = NZ*NS;
const float cell_area_m2 = cell_length_m * cell_length_m;
	// length of cell edge if the cell is thought as an octogon with the same area (Handbuch der Mathematik).
const float cell_octogon_edge_m = sqrtf(cell_area_m2/(2.0*sqrtf(2.0)))*sqrtf(2.0-sqrtf(2.0));
const float pi2 = 2*pi;
const float q1_6 = 1.0/6.0;
const float q1_3 = 1.0/3.0;
const float q2_3 = 2.0/3.0;
const float q1_3600= 1.0/3600.0;
const float q1_365 = 1.0/float(days_in_year);
const float q1_360 = 1.0/360;
const float q1_cell_area_m2 = 1.0/cell_area_m2;
const float perCell = 1.0/float(all_cells);
const float gROOT = sqrtf(_g);
const float qpi2_360 = pi2/360.0;
const float q1_pi = 1.0/pi;
const float qpi_3 = pi/3.0;

extern RNG* zZzo;

template<class T> const T mm_to_m(const T& distance_mm) {return distance_mm * 0.001;}
template<class T> const T g_to_kg(const T& mass_g) {return mass_g * 0.001;}
template<class T> const T kg_to_g(const T& mass_kg) {return mass_kg * 1000.0;}
template<class T> const T cell_to_m2(const T& amount) {return T(amount * q1_cell_area_m2);}


/// replacement for the max function, 
template<class T> const T& max(const T& a, const T&b) {
	return (a<b) ? b : a;
}

/// replacement for the min function, 
template<class T> const T& min(const T& a, const T&b) {
	return (a<b) ? a : b;
}

/// keep number within range [l,u]
template<class T> const T in(const T& l, T&n, const T&u) {
	return (n<l) ? l : ((n>u) ? u : n);
}


/// square function, 
template<class T> const T square(const T& a) {
	return a*a;
}

/// sign function
inline short signum(float i) {
	return (i>0) ? 1 : ((i<0) ? -1 : 0);
}


//=======> templates for creating a sorting index
//======= http://www4.ncsu.edu/~kaltofen/courses/Languages/C++Examples/indir_sort.cpp E. Kaltofen, 2008
// must overload comparison on index array indir_comp I[]:
//                I[i] < I[j] if A[I[i]] < A[I[j]]
template <class ElType> class indir_comp
{
	ElType* Aptr;
public:
	// overload function call on indir_comp object
	//     (needed in third argument to STL sort function)
	indir_comp(ElType* p) : Aptr(p) {}
	bool operator() (const uint32& i, const uint32& j) { return Aptr[i] > Aptr[j]; }
};


template <class ElType> void indir_sort(ElType A[], uint32 n, uint32 I[])
// set the array I[0],...,I[n-1] such that
// A[I[0]] <= A[I[1]] <= ... <= A[I[n-1]]
// note: one must have ElType& ElType::operator<(ElType&)
{
	for(uint32 i = 0; i < n; i++)
		I[i] = i;
	indir_comp<ElType> comp_fun(A);
	std::stable_sort(I, I+n, comp_fun );
	//sort(I, I+n, comp_fun );
}
// <======= templates for creating a sorting index

/* // used for creating a more elegant but not working sorting index
 
 class gt {
 WATER* myW;
 public:
 gt( WATER* inW ) : myW(inW) {}
 bool comp( uint32& j, uint32& k ) const { return myW[j].elevation > myW[k].elevation; }
 };
 
 struct c_unique {
 int current;
 c_unique() {current=0;}
 int operator()() {return ++current;}
 } UniqueNumber;
 */


inline short days_in_month(const month_names & m)
{	
	short d = 0;
	if(days_in_year==360)
		d = 30;
	else if (days_in_year==365)
	{
		switch (m) {
			case Aug: d = 31; break;
			case Sep: d = 30; break;
			case Oct: d = 31; break;
			case Nov: d = 30; break;
			case Dec: d = 30; break;
			case Jan: d = 31; break;
			case Feb: d = 30; break;
			case Mar: d = 31; break;
			case Apr: d = 30; break;
			case May: d = 31; break;
			case Jun: d = 30; break;
			case Jul: d = 30; break;
		}
	}
	else 
		std::cerr << "days per month not defined for this number of days per year!\n" << std::endl;

	return d;	
}

inline climate_names & operator++(climate_names & c) {
	return c = climate_names(int(c)+1);
}

inline month_names & operator++(month_names & m) {
	return m = month_names(int(m)+1);
}

inline month_names operator+ (month_names& m, int i) {
	return month_names((int(m) + i) % 12);
}

inline month_names operator- (month_names& m, int i) {
	return month_names((int(m) + 12 - i%12) % 12);
}

inline std::ostream & operator<<(std::ostream & s, const month_names & m) {
	switch (m) {
		case Aug: s << "Aug"; break;
		case Sep: s << "Sep"; break;
		case Oct: s << "Oct"; break;
		case Nov: s << "Nov"; break;
		case Dec: s << "Dec"; break;
		case Jan: s << "Jan"; break;
		case Feb: s << "Feb"; break;
		case Mar: s << "Mar"; break;
		case Apr: s << "Apr"; break;
		case May: s << "May"; break;
		case Jun: s << "Jun"; break;
		case Jul: s << "Jul"; break;
	}
	return s;
}

inline std::ostream & operator<<(std::ostream & s, const climate_names & c) {
	switch (c) {
	case arid:		s << "arid"; break;
	case semiarid:	s << "semiarid"; break;
	case drymed:	s << "drymed"; break;
	case typmed:	s << "typmed"; break;
	case mesicmed:	s << "mesicmed"; break;
	case allC:		s << "all"; break;
	}
	return s;
}

inline const climate_names to_climate_names(char c) {
	climate_names cn;
	 switch(c) {
		case 'A': cn = arid; break;
		case 'S': cn = semiarid; break;
		case 'D': cn = drymed; break;
		case 'T': cn = typmed; break;
		case 'M': cn = mesicmed; break;
		default: cn = allC;
	}
	return cn;
}

inline scenario_names & operator++(scenario_names & s) {
	return s = scenario_names(int(s)+1);
}

inline std::ostream & operator<<(std::ostream & s, const scenario_names & sn) {
	switch (sn) {
	case CT:
		s << "CT";
		break;
	case A2:
		s << "A2";
		break;
	case B1:
		s << "B1";
		break;
	case B2:
		s << "B2";
		break;
	case A1B:
		s << "A1B";
		break;
	}
	return s;
}

inline std::ostream & operator<<(std::ostream & s, const habitat_names & hn) {
	switch (hn) {
		case Plateau: s << "P"; break;
		case North_f: s << "N"; break;
		case South_f: s << "S"; break;
		case Wadi:    s << "W"; break;
	}
	return s;
}


inline double z01(void) {
//	return (rand()%10)*0.1;
	return zZzo->rand_halfclosed01();
}

/** curently not used:
Convert a uniform random number uz into a triangular random number,
http://www.brighton-webs.co.uk/distributions/triangular.asp
*/
/*float triangular(float uz, float my_min, float my_mode, float my_max) {
	float tz = 0.0;
	if (uz <= (my_mode-my_min)/(my_max-my_min)) {
		tz=my_min+sqrtf(uz*(my_max-my_min)*(my_mode-my_min));
	}
	else {
		tz=my_max-sqrtf((1-uz)*(my_max-my_min)*(my_max-my_mode));
	}
	return tz;
}
*/

/** 
Convert a uniform random number uz into a Weibull random number;
http://www.brighton-webs.co.uk/distributions/weibull2.asp
*/
inline float Weibull(float uz, float scale, float shape) {
	return scale*pow(-log(uz), shape);
}

// ####################################################################################
// ===> global constants and variables that may be changed

// set time periods
const short years_0 = 60; ///< burn-in period, 40 years, TODO: reset to 40
const short years_CT = 30; ///< historic years, 1961-1990 = 30
const short years_trans = 70;///< transition years,  1990-2070 = 80
const short years_scen = 0; ///< future scenario years, 2071-2100 = 30

// type of rain input; set false for stochastic, true for time series from GCM (requires input files)
const bool use_RGCM_time_series = true;
const int windowSize = use_RGCM_time_series?30:1;

// set the grazing period; the repective month is included in the period
const month_names start_month = Mar; // variable used in year.cpp
const month_names end_month = Nov; // variable used in year.cpp

// set the detail of output to file
const bool spatial_output = true; // BIG file
const bool tree_detail_output = false;

// set temporal output detail
const int out_month_min = int(Aug); // 9 = April
const int out_month_max = int(Jul); // 9 = April
const int out_month_length = out_month_max - out_month_min + 1;

// maximum biomass of annuals
const float max_ann_mass_kgm2 = 0.6; // limit to growth function

// height of tree cutting
const float cut_height_m = 0.3;

// <=== (above)  global constants and variables that may be changed

#endif
#define DOTEST 1
