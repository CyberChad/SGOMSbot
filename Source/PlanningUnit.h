#pragma once

#include <BWAPI.h>
#include "Common.h"
#include "Context.h"
#include "Awareness.h"
#include "Goal.h"
#include "UnitTask.h"
#include "Operator.h"

namespace PlanningUnitTypes
{
    enum PlanType
    { 
        Attack, 
        Defend, 
        Scout, 
        Expand, 
        
        Default 
    };
}

/*
Different types of Planning Units
A list or plan of unit tasks to perform. Can be modified/updated.
*/

class PlanningUnit
{

    public:
	    PlanningUnit();
        PlanningUnit(const std::string & type );
	    ~PlanningUnit();

        int numUnitTasks;
        bool isDone();        

        UnitTask * nextTask();

        const size_t & type() const;
        PlanningUnitTypes::PlanType myType;

        std::string getName(){ return myName; };
	

        void initialize(Context & cm);

        size_t  _type;

        std::string myName;
        //std::vector<UnitTask> UnitTaskVector;
        std::list<UnitTask*> unitTaskList;
        std::list<UnitTask*>::iterator iter;
        //UnitTaskList myCrazyList;
       

};

