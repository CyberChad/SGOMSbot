#include "Awareness.h"

using namespace SGOMS;

Awareness::Awareness()
{
}

void Awareness::initialize()
{
    emergency = false;
    arousal = 0;
    flag = 0; //create enum
}

Awareness::~Awareness()
{
}
