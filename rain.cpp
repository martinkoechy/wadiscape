#include "rain.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <valarray>

/**
 Determine the mean intensity and duration of rain.
 */

 float RAINSERIES::getMeanDuration_hours(unsigned short day)  {
	 // Derived from measurements at four Israeli field sites 2001-2004.
	 double z = Zzo.rand_halfclosed01();
	 float h = 0.0;
	 if(rainVolume[day] < 10) {
		if(z < 0.5)
			h = Zzo.uniform(1.0, 2.0);
		else if (z < 0.75)
			h = Zzo.uniform(2,7);
		 else if (z < 0.90)
			 h = Zzo.uniform(7,23);
		 else
			 h = 24;
	 }
	 else if (rainVolume[day] < 20) {
		 if (z < 0.10)
			 h = Zzo.uniform(1,5);
		else if (z < 0.25)
			h = Zzo.uniform(5,10);
	 else if (z < 0.5)
		 h = Zzo.uniform(10, 17);
	 else if (z < 0.75) 
		 h = Zzo.uniform(17, 21);
	 else
		 h = Zzo.uniform(21, 24);
	 } else {
		 if (z < 0.1)
			 h = Zzo.uniform(1,10);
		 else if (z < 0.25)
			 h = Zzo.uniform(10, 14);
		 else if (z < 0.5)
			 h = Zzo.uniform(14, 19);
		 else 
			 h = Zzo.uniform(19, 24);
	 }
	 return h;
}

/**
Determine the mean intensity and duration of rain. Formula according to Striem (1981), confirmed for Israel in 
 Goldreich (1995). BUT: TOO HIGH
 */
/*float RAINSERIES::getMeanDuration_hours(unsigned short day) const {
	return pow(rainVolume[day]/0.15, 0.75); // parameter values found in Goldreich 1995
}
*/
float RAINSERIES::getRain(unsigned short day) const {
	return rainVolume[day];
}

float RAINSERIES::getAnnualRain(void) const {
	return theAnnualRain;
}

float RAINSERIES::getPeriodRain(unsigned short start, unsigned short end) const {
	float sum = 0.0;
	for (unsigned short day = start; day <= end; day++)
		sum += rainVolume[day];
	return sum;
}

/**
 Returns the accumulated rain for each month of the year.
 The days per month deviate from that of calendar months in that they are more
 equally distributed.
 */
float RAINSERIES::getPeriodRain(month_names month) const { //from:  0  31  61  92 122 152 183 213 244 274 305 335
	//  to: 30  60  91 121 151 182 212 243 273 304 334 364
	float mrain = 0.0;
	if (days_in_year == 360)
		mrain = getPeriodRain(int(month)*30,int(month)*30+29);
	else {
		switch (month) {
		case Aug:
			mrain = getPeriodRain(0, 30);
			break; // 31
		case Sep:
			mrain = getPeriodRain(31, 60);
			break; // 30
		case Oct:
			mrain = getPeriodRain(61, 91);
			break; // 31
		case Nov:
			mrain = getPeriodRain(92, 121);
			break; // 30
		case Dec:
			mrain = getPeriodRain(122, 151);
			break; // 30
		case Jan:
			mrain = getPeriodRain(152, 182);
			break; // 31
		case Feb:
			mrain = getPeriodRain(183, 212);
			break; // 30
		case Mar:
			mrain = getPeriodRain(213, 243);
			break; // 31
		case Apr:
			mrain = getPeriodRain(244, 273);
			break; // 30
		case May:
			mrain = getPeriodRain(274, 304);
			break; // 31
		case Jun:
			mrain = getPeriodRain(305, 334);
			break; // 30
		case Jul:
			mrain = getPeriodRain(335, 364);
			break; // 30
		default:
			std::cerr << "wrong month in year.cpp, line 24ff" << std::endl;
		}
	}
	return mrain;
}

/**
 Read a text file with years × daily precipitation data into a vector of valarrays. 
 Assumes Day 1 = August 1 !!!!!
 */
RGCM::RGCM(climate_names climate, scenario_names scenario,
		unsigned short use_window = 1) :
	RGCM_window(use_window)

{
	std::ifstream rain_file;
	std::stringstream rf_name;

	rf_name << "../rainfiles/transient_daily_" << climate << "_" << scenario << ".txt";

	try {
		rain_file.open(rf_name.str().c_str());
		if (!rain_file) //Fehlerabfrage
		{
			throw FileNotFound();
		}
	} catch (FileNotFound) {
		std::cerr << "rain file '" << rf_name.str() << "' not found!" << std::endl;
	};

	std::valarray <float> singleYearData(0.0, days_in_year);

	for (unsigned short y = 0; !rain_file.eof(); y++) {
		for (unsigned short d = 0; d < days_in_year; d++)
			rain_file >> singleYearData[d];
		
		theRainDB.push_back(singleYearData);
	} // end for y
	rainVolume.resize(days_in_year);
}

void RGCM::drawYear(void) {
	unsigned short year_ID = (unsigned short) (Zzo.uniform(0.0,
			theRainDB.size() - 1));

	rainVolume = theRainDB[year_ID];
	theAnnualRain = rainVolume.sum();
}

void RGCM::drawYear(unsigned short year_ID) {
	if (year_ID < years_0)
		year_ID = (unsigned short) (Zzo.uniform(0.0, theRainDB.size() - 1));

	rainVolume = theRainDB[year_ID];
	theAnnualRain = rainVolume.sum();
}

void RGCM::drawYear(unsigned short yy, int windowSize) {
	if (yy - years_0 < windowSize/2) // burn-in period, not yet in rain data base
		drawYear(Zzo.rand_int()%windowSize);
	else if (yy - years_0 >= theRainDB.size() - windowSize/2) // beyond content of rain data base
		drawYear(theRainDB.size() - windowSize + Zzo.rand_int()%windowSize);
	else
		drawYear(yy - years_0  - (windowSize / 2)  + Zzo.rand_int()%windowSize); // within data base
}

