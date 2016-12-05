#include "PlanningUnit.h"
#include "UnitTask.h"
#include "Context.h"
#include <BWAPI.h>

using namespace BWAPI;
using namespace UnitTaskTypes;
using namespace PlanningUnitTypes;

PlanningUnit::PlanningUnit()
{
    numUnitTasks = 0;
    myName = "Default";
    //myType = type;
}

PlanningUnit::PlanningUnit(PlanningUnitTypes::PlanType type)
    : _type(PlanningUnitTypes::Default)
{
    myName = "Default";
    myType = type;
    numUnitTasks = 0;
    //contextModule = cm;

    //BWAPI::Broodwar << "PLAN:CREATE_NEW: " << myName << std::endl;

    
    //    BWAPI::Broodwar << "PLAN:INIT: " << myName << std::endl;
    //UnitTask tempTask = new UnitTask(UnitTaskTypes::SelectUnit);                
    
}

const size_t & PlanningUnit::type() const
{
    return _type;
}


PlanningUnit::~PlanningUnit()
{
    //BWAPI::Broodwar << "PLAN:DELETE: " << myName << std::endl;
}

bool PlanningUnit::isEmpty()
{    
    if (unitTaskQueue.empty())
    {        
        finished = true;
    }

    return finished;
}

bool PlanningUnit::isFinished()
{
    if (unitTaskQueue.empty())
    {
        finished = true;
    }

    return finished;
}


UnitTask * PlanningUnit::getNextTask(Context & contextModule)
{   

    /*
    Can use conditionals here to create/return the next Unit Task based on context....    
    */    

    if (nextTask != nullptr)
    {
        if (nextTask->isFinished() && !unitTaskQueue.empty() ) //no more operators
        {
            //iter++;            
            unitTaskQueue.pop();;
            numUnitTasks--;
        }
    }

    if (!unitTaskQueue.empty())
    {
        nextTask = unitTaskQueue.front();
    }
    else
    {
        //make some new Unit Tasks, or just start from fresh?
    }
    
    return nextTask;
}

void PlanningUnit::initialize(Context & contextModule)
{

    //UnitTask * ut = nullptr;
    


    switch (myType)
    {
    case PlanningUnitTypes::Expand:

        myName = "Expand Base";

        //contextModule.currUnitTask = "Watching Resources";
        //contextModule.currGameOp = "None";

        trainResourceUnits(contextModule);
        createResourceBuildings(contextModule);
        createBarracks(contextModule);
        trainMarines(contextModule);

        break;
    case PlanningUnitTypes::Defend:

        myName = "Defend Base";
        defendMyBase(contextModule);

       
    case PlanningUnitTypes::Attack:
        myName = "Attack Enemy";
        //Attack Enemy
        moveUnitsToLocation(contextModule);
        createBarracks(contextModule);
        trainMarines(contextModule);
        break;
    default:
        myName = "Waiting...";
        //watchAndWait(contextModule);
        break;
    }
 

}//PlanningUnit::initialize()

void watchAndWait(Context & cm)
{

}


/**********************************************/
/*************** EXPAND BASE PLANS ************/
/**********************************************/


void PlanningUnit::trainResourceUnits(Context & contextModule)
{
    UnitTask * ut = nullptr;
    int scvCost = BWAPI::UnitTypes::Terran_SCV.mineralPrice();
    // Order the depot to construct more workers! But only when it is idle.

    Unit base = contextModule.commandCentre;

    if (contextModule.numWorkers < 20)
    {        
        if (!contextModule.supplyBlocked && base->isIdle() && Broodwar->self()->minerals() >= scvCost + contextModule.reservedMineralsAll)
        {
            ut = new UnitTask(UnitTaskTypes::BuildWorker);
            ut->init(contextModule);

            unitTaskQueue.push(ut);
            numUnitTasks++;
        }
    }
    else
    {

        //ut = new UnitTask(UnitTaskTypes::Waiting);
        //ut->init(contextModule);

        //unitTaskQueue.push(ut);
        //numUnitTasks++;

    }

}//end if need workers

void PlanningUnit::createResourceBuildings(Context & contextModule)
{
    UnitTask* ut = nullptr;
    /*======================= Build a Supply Depot=================*/
    if (contextModule.supplyBlocked && !contextModule.makingDepot && contextModule.supplyMaker_p && !contextModule.supplyMaker_p->isConstructing() && Broodwar->self()->minerals() >= (BWAPI::UnitTypes::Terran_Supply_Depot.mineralPrice() + contextModule.reservedMineralsAll))
    {
        Broodwar << "SUPPLY: Supply blocked, need a depo." << std::endl;


        //UnitType supplyProviderType = contextModule.supplyProviderType;
        // Retrieve a unit that is capable of constructing the supply needed
        //Unit supplyBuilder = contextModule.mainBase->getClosestUnit(GetType == supplyProviderType.whatBuilds().first && (IsIdle || IsGatheringMinerals) && IsOwned);
        //TilePosition buildPosition = Broodwar->getBuildLocation(BWAPI::BWAPI::UnitTypes::Terran_Barracks, constructionWorker_p->getTilePosition());
        //constructionWorker_p->build(BWAPI::UnitTypes::Terran_Barracks, buildPosition);

        ut = new UnitTask(UnitTaskTypes::BuildingSupply);
        ut->init(contextModule);

        unitTaskQueue.push(ut);
        numUnitTasks++;

    }// end if supply blocked
}


/**********************************************/
/*************** EXPAND ARMY PLANS ************/
/**********************************************/
//Create Army
void PlanningUnit::createBarracks(Context & contextModule)
{
    int barracksCost = BWAPI::UnitTypes::Terran_Barracks.mineralPrice();

    if (!contextModule.supplyBlocked && !contextModule.makingBarracks  && contextModule.constructionWorker_p && !contextModule.constructionWorker_p->isConstructing() && Broodwar->self()->minerals() >= (barracksCost + contextModule.reservedMineralsAll))
    {
        //contextModule.currUnitTask = "BUILD: Barracks";
        UnitTask* ut = new UnitTask(UnitTaskTypes::BuildingBarracks);
        ut->init(contextModule);
        unitTaskQueue.push(ut);
        numUnitTasks++;
    }

}
void PlanningUnit::trainMarines(Context & contextModule)
{

    for (auto &barrack : contextModule.barrackSet)
    {
        if (!barrack->isBeingConstructed())
        {
            if (!barrack->isTraining() && !contextModule.supplyBlocked && (Broodwar->self()->minerals() >= BWAPI::UnitTypes::Terran_Marine.mineralPrice() + contextModule.reservedMineralsAll))
            {                
                contextModule.unitFocus = barrack;
                UnitTask* ut = new UnitTask(UnitTaskTypes::BuildMarine);
                ut->init(contextModule);
                unitTaskQueue.push(ut);
                numUnitTasks++;                
            }//if
        }//if
    }//for all barracks

}


/*******************************************************/
/*************    CONTROL ARMY PLANS  *******************/
/*******************************************************/

//Defend Base
void PlanningUnit::defendMyBase(Context & contextModule)
{
    //Unitset units = contextModule.marinesUnion;

    UnitTask * moveTask = new UnitTask(UnitTaskTypes::MoveUnits);
    moveTask->init(contextModule);
    unitTaskQueue.push(moveTask);
    //Broodwar << "COMBAT: Getting hurt, run away!!" << std::endl;
    
    //contextModule.attackingBase = false;
    
}

//Attack Enemy
void PlanningUnit::moveUnitsToLocation(Context & contextModule)
{
    Unitset units = contextModule.marinesUnion;

           contextModule.currUnitTask = "Attack the Enemy";
            //Broodwar << "COMBAT: Squad assembled, attacking nearest unit." << std::endl;

            UnitTask * attackTask = new UnitTask(UnitTaskTypes::AttackMoveUnits);
            attackTask->init(contextModule);
            unitTaskQueue.push(attackTask);
            numUnitTasks++;        

            contextModule.attackingBase = true;
        
    
}


