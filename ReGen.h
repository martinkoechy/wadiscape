/*
 *  Rain.h
 *  WADISCAPE
 *
 *  Created by Martin Köchy on Thu Feb 06 2003.
 *
 */

#include <valarray>
#include <vector>
#include <string>
#include <math.h>
#include "rng.h"
#include "rain.h"
#include "main.h"
//--------------------------------------------------------------------
#ifndef RainReGenH
#define RainReGenH
//--------------------------------------------------------------------


/**
\brief RAINPARAMETERS stores the required parameters
 
 It determines the parameters from mean annual precipitation if they have not been specified by the user and stores the parameters.
 */

class RAINPARAMETERS
{
public:
	RAINPARAMETERS (void) {};
	RAINPARAMETERS (int, float dDMR=0);
	RAINPARAMETERS (float A, float L, float W, float a, float l, float w, float da=0);
	~RAINPARAMETERS (){};
	
	///@name parameters for Gauss peak shape function to calculate probability that a day of year is a rainy day 
	float lA; ///< amplitude
	float lL; ///< location
	float lW; ///< width
			  //@}
	
	///@name parameters for Gauss peak shape function to calculate mean daily rain for a day of year
	//@{
	float vA; ///< amplitde
	float vL; ///< location
	float vW; ///< width
			  //@}
	
	float d; ///< relative change to aV [-0.3,0.3]
	
	void calcParameters (float MAP, float deltaDMR); ///< calculate the parameters with only mean annual precipitation and a change to daily mean rain volume given
};


/** \brief ReGen determines the stochastic time series based on the specified parameters
*/
class ReGen: public RAINSERIES
{
public: 
	ReGen (RAINPARAMETERS* p_RainP);
	~ReGen(){};
	void drawYear(void); ///< creates a new one-year time series
	void drawYear(unsigned short yearID);  ///< not meaningful here, uses simple drawYear internally
	void drawYear(unsigned short yearID, int windowSize); ///< not meaningful here, uses simple drawYear internally
	
protected:
	 
	 RAINPARAMETERS* pP; ///< pointer to the parameters
	std::valarray <float> theDays; ///< array of days of year, i.e. 0 ... 364
	std::valarray <float> rainProb; ///< array of daily rain probability
	std::valarray <float> meanRainVolume; ///< aray of mean daily rain amount
	std::valarray <float> rainyDay; ///< array of zeros and ones for rainy and dry days
	std::valarray <float> calcGauss (float amplitude, float location, float width, int shape = 2); ///< function for a peak Gauss curve \f$ A*exp(-\frac{(mo-L)^S}{2*W^2}) \f$
	
	void calcRain(void); ///< does the calculations
};

#endif
