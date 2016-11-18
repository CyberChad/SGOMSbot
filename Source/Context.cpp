#pragma once

#include "Common.h"
#include "Context.h"
#include <iostream>
#include "Goal.h"

using namespace SGOMS;
using namespace BWAPI;

Context::Context()
{
    initialize();
	//do nothing for now
}

void Context::initialize()
{
    supplyProviderType = Broodwar->self()->getRace().getSupplyProvider();
    //Init unit groups
    workersUnion.clear();
    workersGas.clear();
    workersMinerals.clear();

    //Finding home base
    for (auto &u : Broodwar->self()->getUnits())
    {
        if (u->getType().isResourceDepot())
        {
            commandCentre = u;
        }

    }//end iterate to find command centre


}//end Context::initialize()

void Context::update()
{
    //================ MAYBE DECLARATIVE MEMORY  ===================
    // Iterate through all the units that we own, create proper groupings
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

        // If the unit is a worker unit
        if (u->getType().isWorker())
        {
            numWorkers++; //increase number of workers we are aware of
            workersUnion.insert(u);
            continue;
        }//end if isWorker

        // filter out units which are buildings under construction
        if (u->getType().isBuilding() && u->isBeingConstructed())
        {
            numUnderConstruction++;
        }

        if (u->getType() == BWAPI::UnitTypes::Terran_Marine && !u->isBeingConstructed())
        {
            numMarines++; //increase the number of marines we are aware of
            marinesUnion.insert(u);
        }
    }//end for all units loop
}//end Context::update()

void Context::updateSupplyInfo()
{
    reservedMineralsAll = reservedMineralsSupply + reservedMineralsBase + reservedMineralsUnits;
    reservedGasAll = reservedGasBase + reservedGasUnits;
    rawSupplyUsed = BWAPI::Broodwar->self()->supplyUsed();
    rawSupplyTotal = BWAPI::Broodwar->self()->supplyTotal();

    numBarracks = 0; //reset until Building Groups used
    numMarines = 0; //reset until Unit Groups used
    numWorkers = 0; //reset until Unit Groups used
    numUnderConstruction = 0;




    if (rawSupplyUsed >= rawSupplyTotal - 6) //we want a buffer of 3 units
    {
        //Broodwar << "SUPPLY CHECK: blocked." << std::endl;
        supplyBlocked = true;

    }//end if insufficient supply error
    else
    {
        //Broodwar << "SUPPLY CHECK: free." << std::endl;
        supplyBlocked = false;

    }//end else have enough supply
}