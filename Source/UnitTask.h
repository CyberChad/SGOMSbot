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
        
        BuildingSupply, 
        BuildingBarracks,
        BuildWorker, 
        BuildMarine,
        
        SelectUnit, 
        SelectBuilding,

        MoveUnits,
        AttackMoveUnits,

        Waiting,
                
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
        Operator * getNextOp();

        std::string myName;

        bool isFinished();

        void execute();

        bool finished = false;

        //unit tasks
        void buildWorker(){};
        void buildWorker(BWAPI::Unit nexus);
        void buildMarine(Context & cm);
        //void buildMarine(BWAPI::Unit barrack);

        //building tasks
        void buildSupply(Context & cm);
        void buildBarracks(Context & cm);

        //combat tasks
        void sectionAttack(Context & contextModule);
        void sectionMove(Context & contextModule);
        
        UnitTaskTypes::UnitType myType;
        Operator* nextOp;
        std::list<Operator*> opList;
        std::list<Operator*>::iterator iter;
        std::queue<Operator*> opQueue;

        BWAPI::Unit unit;
        BWAPI::UnitCommand command;
};

//class MakeUnit : public UnitTask
//{
//    public:
//        MakeUnit(){};
//        ~MakeUnit(){};
//}



