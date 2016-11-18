#include "PlanningUnit.h"
#include "UnitTask.h"
#include <BWAPI.h>

PlanningUnit::PlanningUnit()
{
}

PlanningUnit::PlanningUnit(const std::string & name) 
    : _type(PlanningUnitTypes::Default)
{
    myName = name;
    BWAPI::Broodwar << "PLAN:CREATE_NEW: " << myName << std::endl;

    if (myName == "Default")
    {
        BWAPI::Broodwar << "PLAN:INIT: Default" << std::endl;

        //create new unit tasks
        UnitTask* tempTask = new UnitTask(UnitTaskTypes::SelectUnit);

        UnitTaskList.push_back(tempTask);
    }
}

const size_t & PlanningUnit::type() const
{
    return _type;
}


PlanningUnit::~PlanningUnit()
{
    BWAPI::Broodwar << "PLAN:DELETE: " << myName << std::endl;
}

