#include "PlanningUnit.h"

PlanningUnit::PlanningUnit()
{
}

PlanningUnit::PlanningUnit(const std::string & name) 
    : _type(PlanningUnitTypes::Default)
{
    myName = name;
}

const size_t & PlanningUnit::type() const
{
    return _type;
}


PlanningUnit::~PlanningUnit()
{
}

