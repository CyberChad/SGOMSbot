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

        Default
    };
}

class Operator
{
    public:
        Operator();        
        ~Operator();
        
        OperatorTypes::OpCode myOp;

        void move(BWAPI::UnitCommand c){};
        void build(BWAPI::UnitCommand c){};
        void stop(BWAPI::UnitCommand c){};
        void train(BWAPI::Unit u){ u->train(); };

        void build(){};
        void repair(){};
        void harvest(){};

        BWAPI::Unit* theUnit;
        BWAPI::UnitCommand* theCommand;

    private:
               

};

