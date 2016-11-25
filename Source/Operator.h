#pragma once

#include <BWAPI.h>
#include "Common.h"


namespace OperatorTypes
{
    enum OpCode
    {
        BuildBuilding,
        BuildUnit,

        SelectUnit,
        SelectBuilding,

        Train,

        Default
    };
}

class Operator
{
    public:
        Operator();        
        Operator(OperatorTypes::OpCode);
        ~Operator();
        
        OperatorTypes::OpCode myOp;

        void setUnit(BWAPI::Unit u);
        void setCommand(BWAPI::UnitCommand c);

        void move(BWAPI::UnitCommand c){};
        void build(BWAPI::UnitCommand c){};
        void stop(BWAPI::UnitCommand c){};
        void train(BWAPI::Unit u){ u->train(); };

        void build(){};
        void repair(){};
        void harvest(){};

        void execute();

        BWAPI::Unit subject;
        BWAPI::UnitType subjectType;
        
        BWAPI::Unit object;
        BWAPI::UnitType objectType;
        
        BWAPI::Unit theUnit;
        BWAPI::UnitType theUnitType;
        BWAPI::UnitCommand theUnitCommand;
        BWAPI::UnitCommandType theUnitCommandType;        

        BWAPI::Unit* theUnit_ptr;
        BWAPI::UnitCommand* theCommand_ptr;

    private:
               

};

