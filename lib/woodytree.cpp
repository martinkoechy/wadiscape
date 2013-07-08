/*
 *  woody.cpp
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "woodytree.h"
#include "water.h"
#include "cell.h"


WOODY::WOODY(CELL* pContaining_cell) : VEGETATION(pContaining_cell)
{ leaf_mass_g = 0.0;
	months_since_fire = 0;
}
// ********************* TREE ************************

TREE::TREE(CELL* pContaining_cell) : WOODY(pContaining_cell)
{
	browse_history.resize(4);
	edible_mass_g = 0.0;
}

void TREE::set_height_m(float height) {
	height_m = height;
}

float TREE::get_height_m(void) {
	return height_m;
}

void TREE::set_age(int newAge) {
	age = newAge;
}

int TREE::get_age(void) {
	return age;
}

void TREE::annual_update_function(void) {
	// update the browse history: remove last year
	std::valarray<float>temp_browse_history = browse_history.shift(-1); // shifts all entries to the right, puts 0.0 at index 0
	browse_history = temp_browse_history;
	
	// update the acorn array
	acorns[1] = acorns[0];
	acorns[0] = 0;
	
	// aging
	age++;
	yearsAfterSevereBurn++; 

}

float TREE::get_browse_layer_volume_m3(void) {
	float l = min(leaf_layer_depth, radius_base_m);
	float h = 0.0;
	
	if (height_m > 1.5)
		h = 1.5;
	else
		h = height_m; //min(1.5, height_m);
	
	float outer_volume = get_browse_cylinder_volume_m3(radius_base_m, h);
	float inner_volume = get_browse_cylinder_volume_m3(radius_base_m - l, h==1.5 ? h : h-l);
	
	return outer_volume - inner_volume;
}

float TREE::get_browse_cylinder_volume_m3(float radius_base_m, float height_m) {
	return 3.1416 * radius_base_m * radius_base_m * height_m;
}


float TREE::get_shoot_mass_kg(void) {
	return g_to_kg((top_shoot_mass_g + base_shoot_mass_g) * individuals);
}

/*
 TREE* TREE::initialize_new_sapling(CELL* pContaining_cell, TREE* pTREE) {
	TREE* tree= NULL;
	return tree;
}
*/

// ************** WOODY **********************

// no functions for WOODY
