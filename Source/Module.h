#pragma once
class Module
{
public:
	Module();
	~Module();

	/*Declarative Memory knows about the game:
	isMyRace
	isEnemyRace
	myBaseLocation
	enemyBaseLocation
	myBuildingTypes
	enemyBuildingTypes
	myUnitTypes
	enemyUnitTypes
	myGoals
	myStrategies
	*/

	/*Procedural Memory knows how to get stuff done:
	buildSomething
	attackSomething
	createTeam
	moveTeam
	*/
};

