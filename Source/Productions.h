#pragma once

#include <utility>
#include "Common.h"
#include "Chunk.h"


using namespace std;

//typedef std::pair <int, int> ProductionPair;

class Productions
{
public:
	Productions();
	~Productions();

	//update() called each SGOMSbot::onFrame();

	/* Basal Ganglia, instantiated in SGOMSbot. Contains:
		Matching
		Selection
		Execution
		Visual Buffer
		Manual Buffer
		Goal Buffer
		Retrieval Buffer
	*/
};

