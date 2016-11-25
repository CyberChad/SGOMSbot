#pragma once

#include "Common.h"
#include <BWAPI.h>

namespace SGOMS
{
    class Awareness
    {
    public:
        Awareness();
        ~Awareness();

        void initialize();

        /*
        Could feed off BWAPI virtual functions (screen alerts)
        onUnitCreate (mine)
        onUnitShow (enemy observed on screen or mini map)
        onUnitDestry (mine or enemy, depending on context/target)
        nuclearLaunchDetected
        */

        bool emergency = false;
        int arousal;
        int flag;

    };
}



