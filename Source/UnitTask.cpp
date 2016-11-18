#pragma once

#include "UnitTask.h"
#include "Common.h"
#include <BWAPI.h>

using namespace BWAPI;

UnitTask::UnitTask()
{
	//do nothing for now
}

UnitTask::UnitTask(UnitTaskTypes::UnitType t)
{
    myType = t;
    bool scanunits = false;

    Broodwar << "UNITTASK:CREATE_NEW: " << t << std::endl;

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

