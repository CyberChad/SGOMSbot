#include "Goal.h"
#include "PlanningUnit.h"

using namespace SGOMS;

Goal::Goal() : _type(GoalTypes::Default){}

Goal::Goal(const std::string & name) : _type(GoalTypes::Default)
{
    myName = name;
}

const size_t & Goal::type() const
{
    return _type;
}

Goal::~Goal()
{
}

//PlanningUnit Goal::getNextPlan()
//{
//    PlanningUnit currentPlan;
//    return currentPlan;
//}
