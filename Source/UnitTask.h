#pragma once

#include <BWAPI.h>
#include "Common.h"
#include "Operator.h"
#include "Context.h"

namespace UnitTaskTypes
{
    enum UnitType
    { 
        ScanUnits,
        BuildBuilding, 
        BuildWorker, 
        BuildMarine,
        
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
        
        void init(Context & cm);
        Operator * nextOp();

        //unit tasks
        void buildWorker(){};
        void buildWorker(BWAPI::Unit nexus);
        void buildMarine(){};
        void buildMarine(BWAPI::Unit barrack);

        //building tasks
        void buildSupply(Context & cm);
        void buildBarracks(Context & cm);

        //combat tasks
        void sectionAttack( );
        void sectionMove( BWAPI::Unitset units, BWAPI::Position pos );
        
        UnitTaskTypes::UnitType myType;
        Operator myOperator;
        std::list<Operator*> opList;
        std::list<Operator*>::iterator iter;

        BWAPI::Unit unit;
        BWAPI::UnitCommand command;
};

//class MakeUnit : public UnitTask
//{
//    public:
//        MakeUnit(){};
//        ~MakeUnit(){};
//}



