#include "SGOMSbot.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

//bool barracks = false;
BWAPI::Player *myself; 

//BWTA globals
bool analyzed;
bool analysis_just_finished;


//Building Management globals
int lastChecked = 0; // last game frame we checked if supply blocked

UnitType supplyProviderType = NULL;
int reservedMineralsAll = 0;
int reservedMineralsSupply = 0;
int reservedMineralsBase = 0;
int reservedMineralsUnits = 0;

int reservedGasAll = 0;
int reservedGasBase = 0;
int reservedGasUnits = 0;

int rawSupplyUsed = 0;
int rawSupplyTotal = 0;
int unitSupplyUsed = 0;
int unitSupplyTotal = 0;

BWAPI::Unit commandCentre;

BWAPI::Unitset unitSetHarvestMinerals;
BWAPI::Unitset unitSetHarvestGas;
BWAPI::Unitset unitSetConstruction;

int numBarracks = 0;
int nextBuilding = 0;
int numUnderConstruction = 0;
/*
0 = supply
1 = barracks
*/


//State transition globals
bool makingDepot = false;
bool makingBarracks = false;
bool isConstruction = false;
bool supplyBlocked = false;
/*
state 0 = just starting
state 1 = barracks 1
state 2 = barracks 2
state 3 = building squad
state 4 = attacking
*/

//Unit Management globals
BWAPI::Unitset infantry;
BWAPI::Unitset workersUnion;
BWAPI::Unitset workersMinerals;
BWAPI::Unitset workersGas;
BWAPI::Unitset availableWorkers;

BWAPI::Unit constructionWorker_p = nullptr;
BWAPI::Unit supplyMaker_p = nullptr;

BWAPI::Unitset marinesUnion;

//Combat globals
bool attackingBase = false;


int numWorkers = 0;
int numMarines = 0;
int nextUnit = 0;

/*
0 = worker
1 = marine
*/



/*
==== Function: onStart() ====
Inputs: none
Outputs: none
Description: called when the SGOMSbot dll is loaded into game client
*/

void SGOMSbot::onStart()
{

  Broodwar->sendText("SGOMSbot initializing...");



  /* DEBUG INITS */
  myself = (BWAPI::Player*)Broodwar->self();

  /* MY CUSTOM INITS */

  // Print the map name.
  Broodwar << "The map is [" << Broodwar->mapName() << "]" << std::endl;

  // Control bot and type messages
  Broodwar->enableFlag(Flag::UserInput);
  Broodwar << "UserInput flag is [enabled]" << std::endl;
  
  Broodwar->enableFlag(Flag::CompleteMapInformation); /*** uncomment to disable FOG OF WAR***/


  supplyProviderType = Broodwar->self()->getRace().getSupplyProvider();

  // Set the command optimization level so that common commands can be grouped
  // and reduce the bot's APM (Actions Per Minute). 
  //CP: similar to chaining lower level Operations Tasks
  Broodwar->setCommandOptimizationLevel(2);

  /* REPLAY CODE
  // Check if this is a replay
  if ( Broodwar->isReplay() )
  {
    // Announce the players in the replay
    Broodwar << "The following players are in this replay:" << std::endl;
    
    // Iterate all the players in the game using a std:: iterator
    Playerset players = Broodwar->getPlayers();
    for(auto p : players)
    {
      // Only print the player if they are not an observer
      if ( !p->isObserver() )
        Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
    }

  }//end if replay check
  */

	  // Retrieve you and your enemy's races. enemy() will just return the first enemy.
	// If you wish to deal with multiple enemies then you must use enemies().
	if ( Broodwar->enemy() ) // First make sure there is an enemy
		Broodwar << "SGOMSbot set to race: " << Broodwar->self()->getRace() << " vs enemy race: " << Broodwar->enemy()->getRace() << std::endl;

	//Init unit groups
	workersUnion.clear();
	workersGas.clear();
	workersMinerals.clear();


	// Initialize Terrain Analyzer
	BWTA::readMap();
	analyzed = false;
	analysis_just_finished = false;

	Broodwar << "Analyzing map... this may take a minute" << std::endl;;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);

	//Finding home base
	for (auto &u : Broodwar->self()->getUnits())
	{
		if (u->getType().isResourceDepot())
		{
			commandCentre = u;

		}
		
	}//end iterate to find command centre
}//end onStart

/*
	Called when the game ends
*/

void SGOMSbot::onEnd(bool isWinner)
{
  
  if ( isWinner )
  {
    // Record someplace to compile stats on win/loss
  }
}

/*
	Called on every game frame. 
	Default is 24 FPS.
*/

void SGOMSbot::onFrame()
{
  // Called once every game frame

	
	//BWTA draw if finished analyzing the map
	if (analysis_just_finished)
	{
		Broodwar << "Finished analyzing map." << std::endl;;
		analysis_just_finished = false;
	}

	if (analyzed)
		drawTerrainData();

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	// if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
	//	return;

	/* The following code is included with the SSCAI and BWAPI ExampleAI module, and recommended for performance reasons. */


    reservedMineralsAll = reservedMineralsSupply + reservedMineralsBase + reservedMineralsUnits;
    reservedGasAll = reservedGasBase + reservedGasUnits;

    rawSupplyUsed = BWAPI::Broodwar->self()->supplyUsed();
    //unitSupplyUsed = rawSupplyUsed;
    rawSupplyTotal = BWAPI::Broodwar->self()->supplyTotal();
    //unitSupplyUsed = rawSupplyTotal / 2;

    lastChecked = Broodwar->getFrameCount();

    Broodwar->drawTextScreen(10, 00, "Timer: %d", lastChecked);
    Broodwar->drawTextScreen(10, 20, "Supply Blocked: %s", supplyBlocked ? "true" : "false");
    //Broodwar->drawTextScreen(10, 30, "Next Building: %s", nextBuilding == 0 ? "Supply" : "Barracks");
    //Broodwar->drawTextScreen(10, 40, "Next Unit: %s", nextUnit == 0 ? "SCV" : "Marine");
    Broodwar->drawTextScreen(10, 50, "Barracks Construction: %s", makingBarracks == false? "false" : "true");
    Broodwar->drawTextScreen(10, 70, "Supply Construction: %s", makingDepot == false ? "false" : "true");
    //Broodwar->drawTextScreen(10, 60, "Construction: %d", numUnderConstruction);
  
    /* Update the HUD with debug stats*/
    Broodwar->drawTextScreen(150, 0, "Total Minerals: %d", Broodwar->self()->minerals());
    Broodwar->drawTextScreen(150, 10, "Reserved Minerals: %d", reservedMineralsAll);
    Broodwar->drawTextScreen(150, 20, "Total Gas: %d", Broodwar->self()->gas());
    Broodwar->drawTextScreen(150, 30, "Reserved Gas: %d", reservedGasAll);
    Broodwar->drawTextScreen(150, 40, "Raw Supply Total: %d", rawSupplyTotal);
    Broodwar->drawTextScreen(150, 50, "Raw Supply Used: %d", rawSupplyUsed);
  
    Broodwar->drawTextScreen(150, 70, "Barracks: %d", numBarracks);
    Broodwar->drawTextScreen(150, 80, "Marines: %d", numMarines);
    Broodwar->drawTextScreen(150, 90, "Workers: %d", numWorkers);
  
    // Display the game frame rate as text in the upper left area of the screen
    Broodwar->drawTextScreen(300, 0, "FPS: %d", Broodwar->getFPS());
    Broodwar->drawTextScreen(300, 10, "Average FPS: %f", Broodwar->getAverageFPS());
 
    /*
    state 0 = just starting
    state 1 = barracks 1
    state 2 = barracks 2
    state 3 = building squad
    state 4 = attacking
    */

    numBarracks = 0; //reset until Building Groups used
    numMarines = 0; //reset until Unit Groups used
    numWorkers = 0; //reset until Unit Groups used
    numUnderConstruction = 0;


	// Return if the game is a replay or is paused
  if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
    return;

  // Prevent spamming by only running our onFrame once every number of latency frames.
  // Latency frames are the number of frames before commands are processed.
//  if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
  //  return;

   
    if ( rawSupplyUsed >= rawSupplyTotal -6 ) //we want a buffer of 3 units
    {
        //Broodwar << "SUPPLY CHECK: blocked." << std::endl;
		supplyBlocked = true;
        
	}//end if insufficient supply error
    else 
	{
        //Broodwar << "SUPPLY CHECK: free." << std::endl;
        supplyBlocked = false;
        
	}//end else have enough supply

	

  
  /*
  ==================================================
  ====================  MAIN LOOP  =================
  ==================================================
  */
  
	

  // Iterate through all the units that we own, create proper groupings
  for (auto &u : Broodwar->self()->getUnits())
  {	  
    // Ignore the unit if it no longer exists
    // Make sure to include this block when handling any Unit pointer!
    if ( !u->exists() )
      continue;

    // Ignore the unit if it has one of the following status ailments
    if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() )
      continue;

    // Ignore the unit if it is in one of the following states
    if ( u->isLoaded() || !u->isPowered() || u->isStuck() )
      continue;

    // Ignore the unit if it is incomplete
    if ( !u->isCompleted() )
      continue;
    
    // Tally unit if it is contructing
	if (u->isConstructing())
	{
        numWorkers++; //increase number of workers we are aware of
        workersUnion.insert(u);
        continue;
	}

	// filter out units which aren't buildings under construction
	if (u->getType().isBuilding() && u->isBeingConstructed() )
	{
        numUnderConstruction++;
	}

    /**********************************************/
    /*************** WORKER UNIT TALLY *******/
    /**********************************************/

    // If the unit is a worker unit
    if (u->getType().isWorker())
    {
        numWorkers++; //increase number of workers we are aware of
        workersUnion.insert(u);
     }//end if isWorker

    /**********************************************/
    /*************** ATTACK UNIT TALLY *****************/
    /**********************************************/

    if ( u->getType() == UnitTypes::Terran_Marine && !u->isBeingConstructed() )
    {
        numMarines++; //increase the number of marines we are aware of
		marinesUnion.insert(u);        
    }

    /**********************************************/
    /*************** RESOURCE DEPOTS (Nexus/Command Centre/Hive) **************/
    /**********************************************/

    

    if (u->getType().isResourceDepot() && numWorkers < 20 ) // A resource depot is a Command Center, Nexus, or Hatchery
    {
        int scvCost = UnitTypes::Terran_SCV.mineralPrice();
        // Order the depot to construct more workers! But only when it is idle.
        if (!supplyBlocked && u->isIdle() && Broodwar->self()->minerals() >= scvCost + reservedMineralsAll )
        {
            Broodwar << "UNITS: Building SCV." << std::endl;
            u->train(u->getType().getRace().getWorker());
            // If that fails, draw the error at the location so that you can visibly see what went wrong!
            // However, drawing the error once will only appear for a single frame
            // so create an event that keeps it on the screen for some frames
        }

    }//if unit is a resource depot

 




	/**********************************************/
	/*************** BARRACKS *****************/
	/**********************************************/
	
	if (u->getType() == UnitTypes::Terran_Barracks)
	{
		if (!u->isBeingConstructed())
		{
			numBarracks += 1; //increment number of barracks we are aware of

            if (!u->isTraining() && (Broodwar->self()->minerals() >= UnitTypes::Terran_Marine.mineralPrice() + reservedMineralsAll) )
			{
				u->build(UnitTypes::Terran_Marine);
				Broodwar << "BARRACKS: Training Marine." << std::endl;;
			}
			
		}		
	}

   

  } //for loop closure: unit iterator
    

  /**********************************************/
  /*************** SUPPLY CONSTRUCTION ************/
  /**********************************************/

  /*======================= Build a Supply Depot=================*/
  if (supplyBlocked && !makingDepot && supplyMaker_p && !supplyMaker_p->isConstructing() && Broodwar->self()->minerals() >= ( UnitTypes::Terran_Supply_Depot.mineralPrice() + reservedMineralsAll ) )
  {
	  Broodwar << "SUPPLY: Supply blocked, need a depo." << std::endl;
	  
	  // Retrieve a unit that is capable of constructing the supply needed
	  //Unit supplyBuilder = u->getClosestUnit(GetType == supplyProviderType.whatBuilds().first && (IsIdle || IsGatheringMinerals) && IsOwned);
	  //TilePosition buildPosition = Broodwar->getBuildLocation(BWAPI::UnitTypes::Terran_Barracks, constructionWorker_p->getTilePosition());
	  //constructionWorker_p->build(UnitTypes::Terran_Barracks, buildPosition);

	  UnitType supplyProviderType = supplyMaker_p->getType().getRace().getSupplyProvider();

	  if (supplyProviderType.isBuilding()) //we get unit supplies from a depot/pylon
	  {
		  Broodwar << "BUILDING: Tasking construction: Depot." << std::endl;
		  TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyMaker_p->getTilePosition());
		  if (targetBuildLocation.isValid())
		  {
			  // Register an event that draws the target build location
			  Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*){Broodwar->drawBoxMap(Position(targetBuildLocation), Position(targetBuildLocation + supplyProviderType.tileSize()), Colors::Green); }, nullptr, supplyProviderType.buildTime() + 100);

			  // Order the builder to construct the supply structure
			  supplyMaker_p->build(supplyProviderType, targetBuildLocation);

			  //reserve minerals for the depot, set state flags
			  reservedMineralsSupply += UnitTypes::Terran_Supply_Depot.mineralPrice();

			  //isConstruction = true;
			  makingDepot = true;

		  }// end if target building location
	  }// end if supply is from a building
	  else //supply is from a unit (overlord)
	  {
		  Broodwar << "WARNING: Training Overlord (Zerg only). Should not be here." << std::endl;
		  // Train the supply provider (Overlord) if the provider is not a structure
		  supplyMaker_p->train(supplyProviderType);
	  }//end else supply is from a unit

  }// end if supply blocked


  /**********************************************/
  /*************** BARRACKS CONSTRUCTION ************/
  /**********************************************/

  int barracksCost = UnitTypes::Terran_Barracks.mineralPrice();

  if (!supplyBlocked && !makingBarracks  && constructionWorker_p && !constructionWorker_p->isConstructing() && Broodwar->self()->minerals() >= (barracksCost + reservedMineralsAll))
  {
	  

	  //find a location and construct it
	  TilePosition targetBuildLocation = Broodwar->getBuildLocation(BWAPI::UnitTypes::Terran_Barracks, constructionWorker_p->getTilePosition());

	  UnitType AttackProviderType = BWAPI::UnitTypes::Terran_Barracks;
	  // Register an event that draws the target build location
	  Broodwar->registerEvent([targetBuildLocation, AttackProviderType](Game*){Broodwar->drawBoxMap(Position(targetBuildLocation), Position(targetBuildLocation + AttackProviderType.tileSize()), Colors::Red); }, nullptr, AttackProviderType.buildTime() + 100);
	  constructionWorker_p->build(AttackProviderType, targetBuildLocation);
	  
	  Broodwar << "BUILDING: Tasking construction: Barracks" << std::endl;
	  reservedMineralsBase += barracksCost;
	  //isConstruction = true;
	  makingBarracks = true;
	  //state = 1; //now building barracks 1			
  }

  /*******************************************************/
  /*************    CONTROL ARMY   *******************/
  /*******************************************************/

  if (numMarines >= 10)
	{
		if (!attackingBase)
		{
			Broodwar << "COMBAT: Squad assembled, attacking nearest unit." << std::endl;
			attackingBase = true;
		}

		for (auto &u : Broodwar->self()->getUnits())
		{
			if (u->getType() == UnitTypes::Terran_Marine && !u->isBeingConstructed() && !u->isAttacking() && !u->isMoving())
			{
				Unit closestEnemy = NULL;

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
	}	  
	  

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

  if (attackingBase && numMarines < 10) //was attacking but we got our asses kicked
  {
	  Broodwar << "COMBAT: Getting hurt, run away!!" << std::endl;
	  marinesUnion.move(commandCentre->getPosition(), false);
	  attackingBase = false;
  }
  

  
  /*******************************************************/
  /*************    CLEAN UP           *******************/
  /*******************************************************/

  if (constructionWorker_p && makingBarracks )      
  {
      if (!constructionWorker_p->isConstructing() && !constructionWorker_p->isMoving() )
      {
          Broodwar << "BUILDING: barracks finished, release worker and reserved resources." << std::endl;
          makingBarracks = false;
          reservedMineralsBase= 0;
      }
      else if (constructionWorker_p->isConstructing() && reservedMineralsBase > 0)//still under construction
      {
          reservedMineralsBase = 0; //very simple, should tracks multiple buildings and resource types
      }
      else
      {
          //worker is en route to construction site
      }      
  }
  if (supplyMaker_p && makingDepot)
  {
      if ( !supplyMaker_p->isConstructing() && !constructionWorker_p->isMoving() )
      {
          Broodwar << "BUILDING: supply finished, release worker and reserved resources." << std::endl;
          makingDepot = false;          
          reservedMineralsSupply = 0;
      }
      else if (supplyMaker_p->isConstructing() && reservedMineralsSupply > 0)//still under construction
      {
          reservedMineralsSupply = 0; //very simple, should tracks multiple buildings and resource types
      }
      else
      {
          //worker is en route to construction site
      }
  }


  /*******************************************************/
  /*************    Assign Workers           *******************/
  /*******************************************************/

  //check to see if we have an assigned construction worker

  for (auto &u : workersUnion)
  {
      if (constructionWorker_p == nullptr)
      {
          Broodwar << "WORKER: Assigning construction worker." << std::endl;
          constructionWorker_p = u;
      }
      else if (supplyMaker_p == nullptr)
      {
          Broodwar << "WORKER: Assigning supply maker." << std::endl;
          supplyMaker_p = u;
      }

      if (u->isIdle()) // if our worker is idle, make them so domething useful
      {
          // Order workers carrying a resource to return them to the center, otherwise find a mineral patch to harvest.
          if (u->isCarryingGas() || u->isCarryingMinerals())
          {
              u->returnCargo();
          }
          else if (!u->getPowerUp())  // The worker cannot harvest anything if it is carrying a powerup such as a flag (CTF only)
          {
              // Harvest from the nearest mineral patch or gas refinery
              if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery)))
              {
                  // If the call fails, then print the last error message
                  Broodwar << Broodwar->getLastError() << std::endl;
              }

          } // closure: has no powerup
      } // closure: if idle
      //do something
  }//end for workers union
  
}//Function onFrame()




void SGOMSbot::onSendText(std::string text)
{

  /* BWTA analyze command*/

  if (text == "/analyze") {

  }
  else {
	  // Send the text to the game if it is not being processed.
	  // Make sure to use %s and pass the text as a parameter,
	  // otherwise you may run into problems when you use the %(percent) character!
	  Broodwar->sendText("%s", text.c_str());
  }
}

void SGOMSbot::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void SGOMSbot::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void SGOMSbot::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if ( target )
  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else 
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void SGOMSbot::onUnitDiscover(BWAPI::Unit unit)
{
	//CP TODO: Create Units data structures for Intelligence gathering.
}

void SGOMSbot::onUnitEvade(BWAPI::Unit unit)
{
}

void SGOMSbot::onUnitShow(BWAPI::Unit unit)
{
}

void SGOMSbot::onUnitHide(BWAPI::Unit unit)
{
}

void SGOMSbot::onUnitCreate(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void SGOMSbot::onUnitDestroy(BWAPI::Unit unit)
{
}

void SGOMSbot::onUnitMorph(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void SGOMSbot::onUnitRenegade(BWAPI::Unit unit)
{
}

void SGOMSbot::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void SGOMSbot::onUnitComplete(BWAPI::Unit unit)
{
}


/* spawn a seperate thread to analyze the terrain detail.*/
DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();
	analyzed = true;
	analysis_just_finished = true;
	return 0;
}

/* Example taken from BWTA2 wiki to draw terrain features overlay*/

void SGOMSbot::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for (const auto& baseLocation : BWTA::getBaseLocations()) {
		TilePosition p = baseLocation->getTilePosition();

		//draw outline of center location
		Position leftTop(p.x * TILE_SIZE, p.y * TILE_SIZE);
		Position rightBottom(leftTop.x + 4 * TILE_SIZE, leftTop.y + 3 * TILE_SIZE);
		Broodwar->drawBoxMap(leftTop, rightBottom, Colors::Blue);

		//draw a circle at each mineral patch
		for (const auto& mineral : baseLocation->getStaticMinerals()) {
			Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, Colors::Cyan);
		}

		//draw the outlines of Vespene geysers
		for (const auto& geyser : baseLocation->getGeysers()) {
			TilePosition p1 = geyser->getInitialTilePosition();
			Position leftTop1(p1.x * TILE_SIZE, p1.y * TILE_SIZE);
			Position rightBottom1(leftTop1.x + 4 * TILE_SIZE, leftTop1.y + 2 * TILE_SIZE);
			Broodwar->drawBoxMap(leftTop1, rightBottom1, Colors::Orange);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if (baseLocation->isIsland()) {
			Broodwar->drawCircleMap(baseLocation->getPosition(), 80, Colors::Yellow);
		}
	}

	//we will iterate through all the regions and ...
	for (const auto& region : BWTA::getRegions()) {
		// draw the polygon outline of it in green
		BWTA::Polygon p = region->getPolygon();
		for (size_t j = 0; j < p.size(); ++j) {
			Position point1 = p[j];
			Position point2 = p[(j + 1) % p.size()];
			Broodwar->drawLineMap(point1, point2, Colors::Green);
		}
		// visualize the chokepoints with red lines
		for (auto const& chokepoint : region->getChokepoints()) {
			Position point1 = chokepoint->getSides().first;
			Position point2 = chokepoint->getSides().second;
			Broodwar->drawLineMap(point1, point2, Colors::Red);
		}
	}
}