#pragma once

#include "Context.h"
#include "UnitTask.h"
#include "Operator.h"
#include "Common.h"
#include <BWAPI.h>

using namespace BWAPI;
using namespace UnitTaskTypes;

UnitTask::UnitTask()
{
	//do nothing for now
    iter = this->opList.begin();
}

UnitTask::UnitTask(UnitTaskTypes::UnitType t)
{
    myType = t;
    bool scanunits = false;



    switch (myType)
    {
        case UnitTaskTypes::ScanUnits:
            scanunits = true;
        default:
            break;
    }

    if (scanunits)
    {
        for (auto &u : Broodwar->self()->getUnits())
        {

            // Ignore the unit if it no longer exists
            // Make sure to include this block when handling any Unit pointer!
            if (!u->exists())
                continue;

            // Ignore the unit if it has one of the following status ailments
            if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
                continue;

            // Ignore the unit if it is in one of the following states
            if (u->isLoaded() || !u->isPowered() || u->isStuck())
                continue;

            // Ignore the unit if it is incomplete
            if (!u->isCompleted())
                continue;
        }//end for unit iterator
    }
};

void UnitTask::init(Context & cm)
{
    Unit subject;
    BWAPI::UnitType subjectType;
    
    Unit object;
    BWAPI::UnitType objectType;    

    Operator * op = new Operator();

    switch (myType) 
    {
        case UnitTaskTypes::BuildWorker :

            //Broodwar << "UNITTASK:CREATE_NEW: " << "BuildWorker" << std::endl;
            subject = cm.commandCentre;            
            op->subject = subject;
            objectType = subject->getType().getRace().getWorker();
            op->myOp = OperatorTypes::Train;
            this->opList.push_back(op);
                        
            break; 
        case UnitTaskTypes::BuildMarine :
            //create ops for marine build

            break;             
        default: //Optional
            ;//do nothing for now
    }
}

Operator * UnitTask::nextOp()
{
    Operator * op = nullptr;
 
    if (iter != this->opList.end())
    {
        op = (*iter);
        iter++;
    }

    return op;
}

void UnitTask::buildWorker(Unit base)
{
    base->train(base->getType().getRace().getWorker());
    //Broodwar << "BUILDING: " << std::endl;
    Broodwar << "UNITS: Building SCV." << std::endl;
    //Operator gameOp;
    //gameOp.theUnit = base;
    //UnitCommand command;
    //command.train( base,  );
    //gameOp->theUnitCommand = UnitCommand()

    // If that fails, draw the error at the location so that you can visibly see what went wrong!
    // However, drawing the error once will only appear for a single frame
    // so create an event that keeps it on the screen for some frames

}//end UnitTask::buildWorker

void UnitTask::buildMarine(Unit barrack)
{
                barrack->build(BWAPI::UnitTypes::Terran_Marine);
                Broodwar << "BARRACKS: Training Marine." << std::endl;;
}

void UnitTask::buildSupply(Context & contextModule)
{
    /*======================= Build a Supply Depot=================*/
    if (contextModule.supplyBlocked && !contextModule.makingDepot && contextModule.supplyMaker_p && !contextModule.supplyMaker_p->isConstructing() && Broodwar->self()->minerals() >= (BWAPI::UnitTypes::Terran_Supply_Depot.mineralPrice() + contextModule.reservedMineralsAll))
    {
        Broodwar << "SUPPLY: Supply blocked, need a depo." << std::endl;


        //UnitType supplyProviderType = contextModule.supplyProviderType;
        // Retrieve a unit that is capable of constructing the supply needed
        //Unit supplyBuilder = contextModule.mainBase->getClosestUnit(GetType == supplyProviderType.whatBuilds().first && (IsIdle || IsGatheringMinerals) && IsOwned);
        //TilePosition buildPosition = Broodwar->getBuildLocation(BWAPI::BWAPI::UnitTypes::Terran_Barracks, constructionWorker_p->getTilePosition());
        //constructionWorker_p->build(BWAPI::UnitTypes::Terran_Barracks, buildPosition);

        BWAPI::UnitType supplyProviderType = Broodwar->self()->getRace().getSupplyProvider();

        if (supplyProviderType.isBuilding()) //we get unit supplies from a depot/pylon
        {
            contextModule.currUnitTask = "Build a Supply Depot";
            Broodwar << "BUILDING: Tasking construction: Depot." << std::endl;
            TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, contextModule.supplyMaker_p->getTilePosition());
            if (targetBuildLocation.isValid())
            {
                // Register an event that draws the target build location
                Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*){Broodwar->drawBoxMap(Position(targetBuildLocation), Position(targetBuildLocation + supplyProviderType.tileSize()), Colors::Green); }, nullptr, supplyProviderType.buildTime() + 100);

                // Order the builder to construct the supply structure
                contextModule.supplyMaker_p->build(supplyProviderType, targetBuildLocation);

                //reserve minerals for the depot, set state flags
                contextModule.reservedMineralsSupply += BWAPI::UnitTypes::Terran_Supply_Depot.mineralPrice();

                //isConstruction = true;
                contextModule.makingDepot = true;

            }// end if target building location
        }// end if supply is from a building
        else //supply is from a unit (overlord)
        {
            Broodwar << "WARNING: Training Overlord (Zerg only). Should not be here." << std::endl;
            // Train the supply provider (Overlord) if the provider is not a structure
            contextModule.supplyMaker_p->train(supplyProviderType);
        }//end else supply is from a unit

    }// end if supply blocked
}

void UnitTask::buildBarracks(Context & contextModule)
{
    int barracksCost = BWAPI::UnitTypes::Terran_Barracks.mineralPrice();

    if (!contextModule.supplyBlocked && !contextModule.makingBarracks  && contextModule.constructionWorker_p && !contextModule.constructionWorker_p->isConstructing() && Broodwar->self()->minerals() >= (barracksCost + contextModule.reservedMineralsAll))
    {


        //find a location and construct it
        BWAPI::TilePosition targetBuildLocation = Broodwar->getBuildLocation(BWAPI::UnitTypes::Terran_Barracks, contextModule.constructionWorker_p->getTilePosition());

        BWAPI::UnitType AttackProviderType = BWAPI::UnitTypes::Terran_Barracks;
        // Register an event that draws the target build location
        Broodwar->registerEvent([targetBuildLocation, AttackProviderType](BWAPI::Game*){Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation), BWAPI::Position(targetBuildLocation + AttackProviderType.tileSize()), BWAPI::Colors::Red); }, nullptr, AttackProviderType.buildTime() + 100);
        contextModule.constructionWorker_p->build(AttackProviderType, targetBuildLocation);

        contextModule.currUnitTask = "Build a Barracks";
        Broodwar << "BUILDING: Tasking construction: Barracks" << std::endl;
        contextModule.reservedMineralsBase += barracksCost;
        //isConstruction = true;
        contextModule.makingBarracks = true;
        //state = 1; //now building barracks 1			
    }
}

//combat tasks
void UnitTask::sectionAttack()
{

    for (auto &u : Broodwar->self()->getUnits())
    {
        if (u->getType() == BWAPI::UnitTypes::Terran_Marine && !u->isBeingConstructed() && !u->isAttacking() && !u->isMoving())
        {
            BWAPI::Unit closestEnemy = NULL;

            for (auto &e : Broodwar->enemy()->getUnits())
            {
                if ((closestEnemy == NULL) || (e->getDistance(u->getPosition()) < closestEnemy->getDistance(u->getPosition())))
                {
                    closestEnemy = e;
                }
            }
            u->attack(closestEnemy->getPosition(), false);

        }//end if marine
    }//end for all units

    //Unit closestEnemy = NULL;

    //for (auto &e : Broodwar->enemy()->getUnits())
    //{
    // if ((closestEnemy == NULL) || (e->getDistance( marinesUnion.getPosition() ) < closestEnemy->getDistance( marinesUnion.getPosition() )))
    // {
    //  closestEnemy = e;
    // }
    //}
    //marinesUnion.attack(closestEnemy->getPosition(), false);

    //u->attack(closestEnemy->getPosition(), false);


    //command marine group to attack closest enemy/structure
}

void UnitTask::sectionMove(BWAPI::Unitset units, BWAPI::Position pos)
{

    Broodwar << "COMBAT: Getting hurt, run away!!" << std::endl;
    units.move(pos, false);  
}