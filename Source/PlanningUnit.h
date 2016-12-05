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
        Wait,

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
        PlanningUnit(PlanningUnitTypes::PlanType type );
	    ~PlanningUnit();

        int numUnitTasks;
        
        bool isFinished();

        bool isEmpty();

        bool finished = false;

        bool empty = false;

        UnitTask * getNextTask(Context & cm);

        const size_t & type() const;

        PlanningUnitTypes::PlanType myType;

        std::string getName(){ return myName; };
	

        void initialize(Context & cm);
        
        //Miscallenous
        void watchAndWait(Context & cm);

        //Expand Base        
        void trainResourceUnits(Context & cm);
        void createResourceBuildings(Context & cm);

        //Create Army
        void createBarracks(Context & cm);
        void trainMarines(Context & cm);
        

        //Defend Base
        void defendMyBase(Context & cm);

        //Attack Enemy
        void moveUnitsToLocation(Context & cm);


        size_t  _type;

        UnitTask* nextTask = nullptr;

        std::string myName;
        //std::vector<UnitTask> UnitTaskVector;
        std::list<UnitTask*> unitTaskList;
        std::queue<UnitTask*> unitTaskQueue;
        std::list<UnitTask*>::iterator iter;
        
       

};

