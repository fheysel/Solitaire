/*
 * fortyThievesAttempt1.h
 *
 *  Created on: Nov 10, 2018
 *      Author: Poe
 */

#pragma once

#include <vector>
#include <iostream>
#include <stack>

using namespace std;

struct card{
	string suit;
	int value;
	bool intermediate;
};

class table{
public:
	table();
	void reset();
	//public as stack data type provides enough protection for my purposes
	stack<card> unsortedPiles[10];//the 10 piles of 4 laid out at beginning of game
	stack<card> sortedPiles[8];//the 8 piles to build onto
};





