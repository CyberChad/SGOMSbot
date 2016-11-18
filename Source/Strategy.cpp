#include <BWAPI.h>
#include "Strategy.h"
#include "Common.h"


using namespace SGOMS;
using namespace BWAPI;

Strategy::Strategy()
{
    Broodwar << "Strategy: Creating" << std::endl;
}


Strategy::~Strategy()
{
}

void Strategy::initialize(std::string s)
{
    myStrategy = s;
    //currentPlan = selectPlan();
    //based on current strategy

    if (s == "Default")
    {
        currentPlan = new PlanningUnit("Default");
    }
    else if (s == "Rush")
    {
        currentPlan = new PlanningUnit("Attack");

    }
    else if (s == "Tech")
    {
        currentPlan = new PlanningUnit("Attack");
        
    }
    else if (s == "Turtle")
    {
        currentPlan = new PlanningUnit("Defend"); 

    }
    else // Default
    {
        currentPlan = new PlanningUnit("Default");
    }
    
    //planningList.push_back(buildPlan);
    
    PlanningUnit * crap = getNextPlan();
}

std::string Strategy::getStrategy()
{
    return myStrategy;
}

PlanningUnit * Strategy::getNextPlan()
{
    //based on context and awareness, choose the most appropriate plan.
        
    PlanningUnit* pu;
    std::string tmpname;

    Broodwar << "getNextPlan: Enter" << std::endl;

    for (std::list<PlanningUnit*>::iterator list_iter = planningList.begin(); list_iter != planningList.end(); list_iter++)
    {
        pu = *list_iter;
        tmpname = (std::string)pu->getName();
        Broodwar << "Next Plan: " << tmpname << std::endl;
    }

    return buildPlan;
    
}//Strategy::getNextPlan()
