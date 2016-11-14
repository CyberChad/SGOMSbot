#pragma once
#include "UnitTask.h"

class SelectUnits
{
public:
	SelectUnits();
	~SelectUnits(){};

	bool addToGroup(BWAPI::Unitset unitset);

	/*
	group units by proximity (area of 	screen)
	Add units to a squad (group #)
	remove units from a squad? (group #)
	Select inidivudal unit by type*/
};

