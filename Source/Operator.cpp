#include "Operator.h"
#include <BWAPI.h>
#include "Common.h"

using namespace OperatorTypes;
using namespace BWAPI;

Operator::Operator()
{

}

Operator::Operator(OperatorTypes::OpCode code)
{
    myOpType = code;
}

Operator::~Operator()
{
}

void Operator::execute()
{

    BWAPI::TilePosition targetBuildLocation;
    BWAPI::UnitType supplyProviderType;
    BWAPI::Unit unit;
    
    switch (myOpType)
    {
    case OperatorTypes::Train:
        subject->train(objectType);
        break;
    case OperatorTypes::BuildBuilding:

        //targetBuildLocation = position;
        //supplyProviderType = objectType;

        //Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*){Broodwar->drawBoxMap(Position(targetBuildLocation), Position(targetBuildLocation + supplyProviderType.tileSize()), Colors::Green); }, nullptr, supplyProviderType.buildTime() + 100);

        //subject->build(objectType,position);

        break;        
    default:
        ;
    }
}
