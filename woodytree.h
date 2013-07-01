/*
 *  woody.h
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include <list>
#include <valarray>
#include "vegetation.h"
#include "water.h"

#ifndef WOODY_H
#define WOODY_H

extern RNG* zZzo;

class WOODY: public VEGETATION
{
public:
	WOODY (void) {months_since_fire = 0;};
	WOODY (CELL*);
	virtual ~WOODY(){};
	//  virtual void grow_year(float w_avail, float mean_w_avail)=0;
protected:
	float height_m;
    float leaf_mass_g;
	int months_since_fire;
};


class TREE: public WOODY
{
public:
	TREE(void) {};
	TREE(CELL*);
	~TREE(){};
	float radius_base_m;
    float radius_top_m;
    int age;
	int individuals;
    float top_shoot_mass_g;
    float base_shoot_mass_g;
	float edible_mass_g;
    float leaf_layer_depth;
	int yearsAfterSevereBurn; 

	unsigned long int acorns[2]; ///< the acorns produced for 2 years
	std::valarray<float> browse_history; ///< the annual browsing history of an individual for 4 years
	
	virtual species_names getTreeType(void) = 0;
    void set_height_m(float height);
    virtual float get_height_m(void);
    void set_age(int newAge);
    int get_age(void);
	
	/** Trees grow in monthly time steps, therefore this function is empty. */
	virtual void grow_year(WATER* W) {};
	
	// virtual TREE* initialize (climate_names c, int iz, int is)=0;
    virtual float get_feed_mass_g(void) = 0;
    virtual float removeMassByForaging(month_names month, float demand) = 0;
    void add_seeds_to_seedbank(int acorns);
    virtual bool mortality(float herb_mass_gm2, int n, int m)=0;
    virtual void annual_update_function(void);

	virtual bool burnTree(float herb_mass_gm2)=0;
	virtual bool cutTree(float height_m) = 0;
    float get_browse_layer_volume_m3 (void);
    virtual float get_browse_cylinder_volume_m3(float radius_base_m, float height_m);
    virtual float get_leaf_area_m2 (month_names month) = 0;
	virtual float get_transpiration_lm2mo (month_names month) = 0;
	virtual void transpiration (WATER* wc, month_names month, float amount, float bedrock_portion) = 0;
	float get_shoot_mass_kg(void);
	};
#endif
