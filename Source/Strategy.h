#pragma once

#include <BWAPI.h>
#include "Common.h"
#include "PlanningUnit.h"

//class Rush : public Strategy; build up quickly, attack en masse
//class Steady : public Strategy; balance between moderate force and tech
//class LongGame public Strategy ; defensive and resource heavy, high tech 

namespace StrategyTypes
{
    enum { Default, Rush, Steady, Long};
}

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

        size_t _type;
        const size_t & type() const{ return _type; };

        void initialize(std::string s);
        std::string getStrategy(){
            return myStrategy;
        };
        void setStrategy(std::string s){ myStrategy = s; };
        PlanningUnit * getNewPlan();

        std::string myStrategy = "";
        PlanningUnit * currentPlan;

        std::list<PlanningUnit> planningList;
		
	};
        

}
