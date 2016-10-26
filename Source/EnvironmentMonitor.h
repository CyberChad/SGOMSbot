#pragma once
class EnvironmentMonitor
{
public:
	EnvironmentMonitor();
	~EnvironmentMonitor();

	/*
	Could feed off BWAPI virtual functions (screen alerts)
	onUnitCreate (mine)
	onUnitShow (enemy observed on screen or mini map)
	onUnitDestry (mine or enemy, depending on context/target)
	nuclearLaunchDetected
	*/
};

