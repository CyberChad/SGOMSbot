#pragma once

#include <BWAPI.h>
#include "Common.h"
#include "Operator.h"

namespace UnitTaskTypes
{
    enum UnitType
    { 
        ScanUnits,
        BuildBuilding, 
        BuildUnit, 
        
        SelectUnit, 
        SelectBuilding,
                
        Default 
    };
}

class UnitTask
{
    
    public:
        UnitTask();
        UnitTask(UnitTaskTypes::UnitType t);
	    ~UnitTask(){};
        void execute();
        
        UnitTaskTypes::UnitType myType;
        Operator myOperator;
        std::list<Operator*> opList;
};



