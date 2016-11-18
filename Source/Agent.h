#pragma once

#include <BWAPI.h>
#include <BWTA.h>

#include "Common.h"
#include "Awareness.h"
#include "Context.h"
#include "Goal.h"

// Remember not to use "Broodwar" in any global class constructor!

namespace SGOMS
{
    class Agent
    {
    public:
        Agent();
        ~Agent(){};

        void initialize();

        void update();

    
    };
}




