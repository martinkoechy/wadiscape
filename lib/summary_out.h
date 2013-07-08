/*
 *  summary_out.h
 *  MKLandscape
 *
 *  Created by Forschung on Sun Jun 25 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */

#include "cell.h"

#ifndef ANNUAL_SUMMARY_H
#define ANNUAL_SUMMARY_H

/// Reset the variables used to sum the monthly values.
void reset_monthly_output_sums(void);

/// Sum the monthly values of each cell and store them in an array for output
void calculate_monthly_statistics(CELL* C, WATER* W);

/// Output the monthly mean values in a file.
void monthly_summary_output(int ll, climate_names cc, int yy, float LSU_km2, float annual_rain,
		float* monthly_rain, scenario_names sc, const char* file_name, WATER*,
		CELL*, float fireArea_m2, month_names fireMonth);

void annual_tree_output_header(const char* file_name);

/// Detailed output for trees, used only for testing purposes. Normally, this function is disabled
void tree_summary_output(const int maxYears, float year_rain, CELL* C, int yy,
		const char* fileName);

void monthly_output_header(const char* file_name);

void annual_spatial_output_header(const char* file_name);

void annual_spatial_output_data(int year, const char* file_name, WATER* W, CELL* C);

#endif

