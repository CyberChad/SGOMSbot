#include <BWAPI.h>
#include "Strategy.h"
#include "Context.h"
#include "Common.h"


using namespace SGOMS;
using namespace BWAPI;

bool debug = false;


Strategy::Strategy()
{
    Broodwar << "CREATE: Strategy Module" << std::endl;
    myStrategy = "Default";        
}


Strategy::~Strategy()
{
}

void Strategy::initialize(std::string s, Context & cm)
{
    myStrategy = s;
    Broodwar << "STRATEGY: Initialized! Using: " << myStrategy << std::endl;   
    createNewPlan(cm);
}

PlanningUnit* Strategy::getNextPlan(Context & cm)
{
    if (currentPlan != nullptr)
    {
        if (currentPlan->isFinished() )
        {
            planningStack.pop();
        }
    }

    if ( !planningStack.empty() )
    {
        currentPlan = (PlanningUnit*)planningStack.top();        
    }
    else
    {
        currentPlan = createNewPlan(cm);
    }    
    
    return currentPlan;    
}

PlanningUnit * Strategy::createNewPlan(Context & cm)
{
    //based on context and awareness, choose the most appropriate plan.

    if (debug)
        Broodwar << "Strategy: ENTER Get New Plan: Enter" << std::endl;

    //***************** CHECK CONTEXT *********************

    //for now we just go with Default, and build the base....
    if (myStrategy == "Default")
    {
        currentPlan = getDefaultPlan(cm);
    }
    else if (myStrategy == "Rush")
    {
        //currentPlan = new PlanningUnit("Attack");
    }
    else if (myStrategy == "Tech")
    {
        //currentPlan = new PlanningUnit("Expand");

    }
    else if (myStrategy == "Turtle")
    {
        //currentPlan = new PlanningUnit("Defend");
    }
    else // Default
    {
        //currentPlan = new PlanningUnit("Default");
    }

    currentPlan->initialize( cm );

    planningStack.push(currentPlan);
    
    //planningList.insert(currentPlan);
    
    return currentPlan;
    


    //for (std::list<PlanningUnit>::iterator list_iter = planningList.begin(); list_iter != planningList.end(); list_iter++)
    //{
    //    pu = *list_iter;
    //    tmpname = (std::string)pu.getName();

    //    if (debug)
    //        Broodwar << "Next Plan: " << tmpname << std::endl;
    //}

    //return currentPlan;
    
}//Strategy::getNextPlan()

PlanningUnit * Strategy::getDefaultPlan(Context & cm)
{
    //check if under attack
    
    if (cm.numMarines > 10)
    {
        return new PlanningUnit(PlanningUnitTypes::Attack);
    }
    else
    {
        return new PlanningUnit(PlanningUnitTypes::Expand);
    }
    
    
    //check if need repair
    

    //build base




    //build army



    
    //attack enemy


    
}