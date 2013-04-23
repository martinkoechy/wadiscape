#include "main.h"
#include <fstream>
#include <vector>
#include <valarray>

#ifndef RAIN_H
#define RAIN_H

class RAINSERIES {
protected:
	RNG Zzo;
	float theAnnualRain;
	float thedailyRain;

	std::valarray <float> rainVolume; ///< array of daily rain volume amount

public:
	RAINSERIES(void) {

	}
	;
	virtual ~RAINSERIES() {
	}

	virtual void drawYear(void)=0; ///< creates a new one-year time series
	virtual void drawYear(unsigned short yearID)=0; ///< selects a certain one-year time series
	virtual void drawYear(unsigned short yearID, int windowSize)=0; ///< selects a certain one-year time series within the indicated period of years (windowSize)
	virtual float getRain(unsigned short day) const; ///< returns the rain for a specific day of year (Aug 1 = day 0)
	virtual float getAnnualRain(void) const; ///< returns the annual rain amount
	virtual float getPeriodRain(unsigned short start_day,
			unsigned short end_day) const; ///< returns the rain amount for a specific period, end day included
	virtual float getPeriodRain(month_names month) const; ///< returns the amount of rain for a specific month (30 or 31 days)
//	virtual float getMeanDuration_hours(float rain_volume_mm) const; ///< returns the mean duration of rain during a day
	virtual float getMeanDuration_hours(unsigned short day); ///< returns the mean duration of rain during a day
};

/// Time series of daily precipitation are read from supplied files.
class RGCM: public RAINSERIES {
public:
	RGCM(void) {};
	
	RGCM(climate_names climate, scenario_names scenario,
			unsigned short use_window);
	~RGCM() {};

	void drawYear(void);
	void drawYear(unsigned short yearID);
	void drawYear(unsigned short yearID, int windowSize); // method to get a rain year within a certain period of years (windowSize)
	float getAnnualRain(int){return RAINSERIES::getAnnualRain();}; // for compatibility with versions <3.2.4
	
protected:
	unsigned int RGCM_window;

	///< a vector containing all time series for a certain climate-scenario combination
	std::vector <std::valarray <float> > theRainDB;
	class FileNotFound {
	}; // for error handling
};

#endif
