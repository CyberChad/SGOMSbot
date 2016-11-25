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
}

PlanningUnit::PlanningUnit(const std::string & name) 
    : _type(PlanningUnitTypes::Default)
{
    myName = name;

    //contextModule = cm;

    //BWAPI::Broodwar << "PLAN:CREATE_NEW: " << myName << std::endl;

    if (myName == "Default")
    {
        //BWAPI::Broodwar << "PLAN:INIT: Default" << std::endl;

        //UnitTask tempTask = new UnitTask(UnitTaskTypes::SelectUnit);

        
    }
}

const size_t & PlanningUnit::type() const
{
    return _type;
}


PlanningUnit::~PlanningUnit()
{
    //BWAPI::Broodwar << "PLAN:DELETE: " << myName << std::endl;
}

bool PlanningUnit::isDone()
{
    //int size = (int)unitTaskList.size();    
    //Broodwar->sendText("Unit Task List Size: %.2d", size);
    return false;
}

UnitTask * PlanningUnit::nextTask()
{
    UnitTask * task = nullptr;

    if (iter != this->unitTaskList.end())
    {
        task = (*iter);
        iter++;
    }

    return task;
}

void PlanningUnit::initialize(Context & contextModule)
{

    UnitTask * ut = nullptr;

    if (myName == "Default") //assuming default for now
    {   
        /**********************************************/
        /*****CREATE UNIT TASK: RESOURCE ************/
        /**********************************************/

        contextModule.currPlanUnit = "Build Base";

        if (contextModule.numWorkers < 20)
        {
            
            

            int scvCost = BWAPI::UnitTypes::Terran_SCV.mineralPrice();
            // Order the depot to construct more workers! But only when it is idle.

            Unit base = contextModule.commandCentre;

            if (!contextModule.supplyBlocked && base->isIdle() && Broodwar->self()->minerals() >= scvCost + contextModule.reservedMineralsAll)
            {
                contextModule.currUnitTask = "Create a Worker";
                ut = new UnitTask(UnitTaskTypes::BuildWorker);
                this->unitTaskList.push_back(ut);
                numUnitTasks++;
                contextModule.currGameOp = "Select Nexus";
                contextModule.currGameOp = "Train Worker";
            }

        }//end if need workers


        /**** CREATE NEW UNIT TASKS ***********/


        /**********************************************/
        /*************** BARRACKS *****************/
        /**********************************************/
        /*
        
        
        
        for (auto &barrack : contextModule.barrackSet)
        {
            if (!barrack->isBeingConstructed())
            {
                if (!barrack->isTraining() && !contextModule.supplyBlocked && (Broodwar->self()->minerals() >= BWAPI::UnitTypes::Terran_Marine.mineralPrice() + contextModule.reservedMineralsAll))
                {
                    
                    contextModule.currUnitTask = "Create a Marine";
                    UnitTask marineTask;
                    contextModule.currGameOp = "Select Barracks";
                    marineTask.buildMarine(barrack);
                    contextModule.currGameOp = "Train Marine";
                }//if
            }//if
        }//for all barracks



        /**********************************************/
        /*************** SUPPLY CONSTRUCTION ************/
        /**********************************************/

        /*
        
        

        UnitTask supplyTask;
        supplyTask.buildSupply(contextModule);

        */


        /**********************************************/
        /*************** BARRACKS CONSTRUCTION ************/
        /**********************************************/

        /*

        UnitTask barracksTask;
        barracksTask.buildBarracks(contextModule);

        */

        /*******************************************************/
        /*************    CONTROL ARMY   *******************/
        /*******************************************************/

        /*


        Unitset units = contextModule.marinesUnion;

        if (contextModule.numMarines >= 10)
        {
            if (!contextModule.attackingBase)
            {
                contextModule.currUnitTask = "Attack the Enemy";
                Broodwar << "COMBAT: Squad assembled, attacking nearest unit." << std::endl;

                UnitTask attackTask;
                attackTask.sectionAttack();

                contextModule.attackingBase = true;
            }
        }
        else if (contextModule.attackingBase && contextModule.numMarines < 10) //was attacking but we got our asses kicked
        {
            Broodwar << "COMBAT: Getting hurt, run away!!" << std::endl;
            contextModule.currUnitTask = "Flee from Enemy";
            Position homePos = contextModule.commandCentre->getPosition();
            UnitTask moveTask;
            moveTask.sectionMove(units, homePos);

            contextModule.attackingBase = false;
        }

        */

    }//end if Default Plan


}//PlanningUnit::execute()

