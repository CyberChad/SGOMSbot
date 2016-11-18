#pragma once

#include <BWAPI.h>
#include "Common.h"
#include "PlanningUnit.h"

namespace SGOMS
{
	//virtual function to inherit attributes common to all strategies
	class Strategy
	{
		//int Utility Value : some kind of Utility value

		//enum PhaseValue : what stage of the game
	public:
		Strategy();
		~Strategy();

        void initialize(std::string s);
        std::string getStrategy();
        void setStrategy(std::string s){ myStrategy = s; };
        PlanningUnit * getNextPlan();

        std::string myStrategy;
        PlanningUnit * currentPlan;
        PlanningUnit * buildPlan;
        PlanningUnit * attackPlan;
        PlanningUnit * defendPlan;

        std::list<PlanningUnit*> planningList;
		
	};
        
	//class Rush : public Strategy; build up quickly, attack en masse

	//class Steady : public Strategy; balance between moderate force and tech

	//class LongGame public Strategy ; defensive and resource heavy, high tech 
}
