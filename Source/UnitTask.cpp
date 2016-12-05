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
    //iter = this->opList.begin();
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

    //this->init();
};

bool UnitTask::isFinished()
{
    return opQueue.empty();    
}

void UnitTask::execute()
{    


}

void UnitTask::init(Context & contextModule)
{
    Unit subject = NULL;
    BWAPI::UnitType subjectType = NULL;
    
    Unit object = NULL;
    BWAPI::UnitType objectType = NULL;

    Operator * op = new Operator();

    switch (myType) 
    {
        case UnitTaskTypes::BuildWorker :

            myName = "Build SCV";
            //Broodwar << "UNITTASK:CREATE_NEW: " << "BuildWorker" << std::endl;
            
            op->myOpType = OperatorTypes::Train; //we know we are training a unit

    
            contextModule.declarative = "<property> command <category> train";
            contextModule.procedural = "<property> command <category> building";
            contextModule.production = "<if> SCV <then> click";

            subject = contextModule.commandCentre; //set subject of op            
            op->subject = subject;

            objectType = subject->getType().getRace().getWorker();
            //objectType = object->getType();
                        
            op->objectType = objectType;

            
            
            op->myName = "TRAIN UNIT at COMMAND";

            //this->opList.push_back(op);
            opQueue.push(op);                        
            break; 

        case UnitTaskTypes::BuildingSupply :
                myName = "Building: Supply";
                buildSupply(contextModule);
            break;
        case UnitTaskTypes::BuildingBarracks:
            myName = "Building: Barracks";
            buildBarracks(contextModule);            
            break;        
        case UnitTaskTypes::BuildMarine :
            //create ops for marine build
            myName = "Build Unit: Marine";
            buildMarine(contextModule);
            break;             
        case UnitTaskTypes::MoveUnits:
            //create ops for marine build
            myName = "Move Units: Loc x,y";
            sectionMove(contextModule);
            break;
        case UnitTaskTypes::AttackMoveUnits:
            //create ops for marine build
            myName = "Attack: Closest Enemy";
            sectionAttack(contextModule);
            break;
        default: //Optional
            myName = "Waiting";
            op->myOpType = OperatorTypes::None;
            opQueue.push(op);
            
            ;//do nothing for now
    }
    
    //iter = opList.begin();

}

Operator * UnitTask::getNextOp()
{    
    if (!opQueue.empty())
    {
        nextOp = opQueue.front();

        opQueue.pop();
    }
    return nextOp;
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

void UnitTask::buildMarine(Context & context)
{
    context.declarative = "<property> barracks <category> building";
    context.procedural = "<if> marine <then> unit";

    Unit barrack = context.unitFocus;    
    context.currUnitTask = "Create a Marine";
    context.currGameOp = "Select Barracks";    
    barrack->build(BWAPI::UnitTypes::Terran_Marine);

    //BWAPI::Broodwar->setScreenPosition(barrack->getPosition() - BWAPI::Position(320, 180));


    context.currGameOp = "Train Marine";
    Broodwar << "BARRACKS: Training Marine." << std::endl;;
}

void UnitTask::buildSupply(Context & contextModule)
{

    Unit subject = NULL;
    BWAPI::UnitType subjectType;

    Unit object = NULL;
    BWAPI::UnitType objectType;

    TilePosition position;

    contextModule.declarative = "<property> tild <category> location";
    contextModule.procedural = "<if> marine <then> buildmarine";
    contextModule.production = "<if> unit <then> click";

    BWAPI::UnitType supplyProviderType = Broodwar->self()->getRace().getSupplyProvider();
    Operator* op = new Operator();

    if (supplyProviderType.isBuilding()) //we get unit supplies from a depot/pylon
    {


    
        TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, contextModule.supplyMaker_p->getTilePosition());

        if (targetBuildLocation.isValid())
        {
            // Register an event that draws the target build location

            Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*){Broodwar->drawBoxMap(Position(targetBuildLocation), Position(targetBuildLocation + supplyProviderType.tileSize()), Colors::Green); }, nullptr, supplyProviderType.buildTime() + 100);
            
            position = targetBuildLocation;

            //BWAPI::Broodwar->setScreenPosition(subject->getPosition() - BWAPI::Position(320, 180));

            op->myOpType = OperatorTypes::Default; //Just do nothing for now

            subject = contextModule.supplyMaker_p; //set subject of op            
            op->subject = subject;
            op->subjectType = subject->getType();
            op->position = position;
            op->objectType = supplyProviderType;
            op->myName = "BUILD Supply Depot";

            //this->opList.push_back(op);
            opQueue.push(op);

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
}

void UnitTask::buildBarracks(Context & contextModule)
{
    int barracksCost = BWAPI::UnitTypes::Terran_Barracks.mineralPrice();

    //if (!contextModule.supplyBlocked && !contextModule.makingBarracks  && contextModule.constructionWorker_p && !contextModule.constructionWorker_p->isConstructing() && Broodwar->self()->minerals() >= (barracksCost + contextModule.reservedMineralsAll))
    //{
        contextModule.currGameOp = "Select SCV";

        contextModule.declarative = "<property> unit <category> select";
        contextModule.procedural = "<if> SCV <then> select";

        //find a location and construct it
        BWAPI::TilePosition targetBuildLocation = Broodwar->getBuildLocation(BWAPI::UnitTypes::Terran_Barracks, contextModule.constructionWorker_p->getTilePosition());

        contextModule.currGameOp = "Select Location";

        contextModule.declarative = "<property> unit <category> location";
        contextModule.procedural = "<if> valid  <then> select";
        contextModule.production = "<if> location <then> click";

        BWAPI::UnitType AttackProviderType = BWAPI::UnitTypes::Terran_Barracks;
        // Register an event that draws the target build location
        Broodwar->registerEvent([targetBuildLocation, AttackProviderType](BWAPI::Game*){Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation), BWAPI::Position(targetBuildLocation + AttackProviderType.tileSize()), BWAPI::Colors::Red); }, nullptr, AttackProviderType.buildTime() + 100);
        
        contextModule.constructionWorker_p->build(AttackProviderType, targetBuildLocation);

        //BWAPI::Broodwar->setScreenPosition(contextModule.constructionWorker_p->getPosition() - BWAPI::Position(320, 180));

        contextModule.currGameOp = "Click Build Barracks";

        //contextModule.currUnitTask = "Build a Barracks";
        Broodwar << "BUILDING: Tasking construction: Barracks" << std::endl;
        contextModule.reservedMineralsBase += barracksCost;
        //isConstruction = true;
        contextModule.makingBarracks = true;
        //state = 1; //now building barracks 1			
    //}
}

//combat tasks
void UnitTask::sectionAttack(Context & contextModule)
{
    contextModule.declarative = "<property> marines <category> unitgroup";
    contextModule.procedural = "<if> marinesgroup <then> select";
    contextModule.production = "<if> marinesgroup <then> click";

    contextModule.currUnitTask = "Attack Closest Enemy";
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

            contextModule.currGameOp = "Section ATTACK-MOVE";
            u->attack(closestEnemy->getPosition(), false);

        }//end if marine
    }//end for all units

    //contextModule.numMarines = 0;

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

void UnitTask::sectionMove(Context & contextModule)
{
    contextModule.currUnitTask = "Section Patrol";
    Unitset units = contextModule.marinesUnion;
    Position homePos = contextModule.commandCentre->getPosition();    
    Broodwar << "UNITS: Section MOVE to LOC x,y" << std::endl;
    units.patrol(homePos,false);

    contextModule.declarative = "<property> unit <category> location";
    contextModule.procedural = "<if> valid  <then> select";
    contextModule.production = "<if> location <then> click";


    contextModule.currGameOp = "Section PATROL to LOC x,y";
}