#pragma once

#include "Common.h"
#include "PlanningUnit.h"
#include "Context.h"


namespace GoalTypes
{
    enum {Attack, Defend, Scout, Expand, Default};
}

namespace SGOMS
{
    class Goal
    {

        size_t  _type;

    public:
        Goal();
        ~Goal();

        Goal(const std::string & name);

        bool isPossible();
        //PlanningUnit getNextPlan();

    protected:

        const size_t & type() const;
        bool checkPossible;
        std::string myName;
        double utility;

        //std::vector<PlanningUnit, size_t > PlanVector;
        //std::list<PlanningUnit> PlanList;


        std::string getName(){ return myName; }



    };

    class GoalAttack : public Goal
    {
    public:
        //not sure yet
    };

    /*
    Generic Goal class, to be implemented by subgoal type. Contains Planning Units

    enum Goal Types:
    Attack
    Defend
    Expand
    Scout


    */

}


