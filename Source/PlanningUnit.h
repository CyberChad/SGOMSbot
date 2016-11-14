#pragma once


#include "Common.h"
#include "Goal.h"
#include "UnitTask.h"
#include "Operator.h"

//typedef std::pair<UnitTask, size_t> UnitTaskPair;
//typedef std::vector<UnitTaskPair> UnitTaskVector;
//typedef std::list<UnitTask> UnitTaskList;

namespace PlanningUnitTypes
{
    enum { Attack, Defend, Scout, Expand, Default };
}

class PlanningUnit
{

    size_t  _type;

public:
	PlanningUnit();
    PlanningUnit(const std::string & name);
	~PlanningUnit();

    bool isOne(){ return true; }
    bool isTwo(){ return true; }
    bool isThree(){ return true; }

    const size_t & type() const;

    std::string getName() const;
	/*
	Different types of Planning Units
	A list or plan of unit tasks to perform. Can be modified/updated.
    */

protected:
       
    std::string myName;

    std::vector<UnitTask> UnitTaskVector;
    std::list<UnitTask> UnitTaskList;

};

