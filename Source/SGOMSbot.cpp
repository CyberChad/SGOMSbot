#pragma once

#include "Common.h"
#include "SGOMSbot.h"

#ifdef _DEBUG
    #undef _DEBUG
    #include <Python.h>
#define _DEBUG
#else
    #include <Python.h>
#endif


using namespace BWAPI;
using namespace Filter;
using namespace SGOMS;

//BWTA globals
bool analyzed;
bool analysis_just_finished;

std::string CallPythonPlugin(const std::string s)
{
    //Import the module "plugin"
    //PyObject* moduleName = Pystring_Fromstring("plugin");
    std::string blah = "test";
    return blah;
}

/*
==== Function: onStart() ====
Inputs: none
Outputs: none
Description: called when the SGOMSbot dll is loaded into game client
*/

void SGOMSbot::onStart()
{

  BWAPI::Broodwar->sendText("SGOMSbot initializing...");

  myself = (BWAPI::Player*)Broodwar->self(); //hook into game player
  
  awarenessModule.initialize(); //internal game state and understanding
  contextModule.initialize(); //internal game state and understanding
  strategyModule.initialize("default");

  
  
  //Py_Initialize();
  //PyRun_SimpleString("import ccm");
  //Py_Finalize();

  

  // Print the map name.
  Broodwar << "The map is [" << Broodwar->mapName() << "]" << std::endl;

  // Control bot and type messages
  Broodwar->enableFlag(Flag::UserInput);
  Broodwar << "UserInput flag is [enabled]" << std::endl;
  
  Broodwar->enableFlag(Flag::CompleteMapInformation); /*** uncomment to disable FOG OF WAR***/

  

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



	// Initialize Terrain Analyzer
	BWTA::readMap();
	analyzed = false;
	analysis_just_finished = false;

	Broodwar << "Analyzing map... this may take a minute" << std::endl;;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);


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

/************************************************************/
/********************   ON FRAME CYCLE  *********************/
/************************************************************/

void SGOMSbot::onFrame()
{
  // Called once every game frame

    /* The following code is included with the SSCAI and BWAPI ExampleAI module, and recommended for performance reasons. */
    // Return if the game is a replay or is paused
    if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
        return;

    // Prevent spamming by only running our onFrame once every number of latency frames.
    // Latency frames are the number of frames before commands are processed.
    if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
        //return;

    contextModule.updateSupplyInfo();
    drawHUDinfo();

    //gamePlan = strategyModule.getNextPlan();
    //if ( gamePlan != nullptr )

    

    //if (gamePlan->isDone() )
    //    gamePlan = strategyModule.getNextPlan();
    //if (gameTask->isDone())
    //    gameTask = gamePlan->getNextTask();
    //if (gameOp->isDone())
    //    gameOp = gameTask->getNextOp();



    /*=============================================*/
    /*==========  SGOMS LOOP  =====================*/
    /*=============================================*/

    {//while still have unit tasks

        //retrieve next Operator / Method
        {
            //Execute operator(s)
        }

        //Update situation knowledge (from Parallel external monitoring)
        

        //Evaluate situation (determine arousal / emergency)

        //Is everything okay?
        {
            //If not, consider problem solving (depends on risk/arousal)

            //Problem solve?
            {
                //doProblemSolve()
            }
        }

        if ( awarenessModule.emergency) //if emergency, break
        {
            Broodwar << "EMERGENCY: placeholder" << std::endl;;
            //break
        }

    }//while still have unit tasks

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

        /**********************************************/
        /*************** RESOURCE DEPOTS (Nexus/Command Centre/Hive) **************/
        /**********************************************/

        //================ PRODCUTION: UNIT TASK  ===================    

        if (u->getType().isResourceDepot() && contextModule.numWorkers < 20) // A resource depot is a Command Center, Nexus, or Hatchery
        {
            int scvCost = BWAPI::UnitTypes::Terran_SCV.mineralPrice();
            // Order the depot to construct more workers! But only when it is idle.
            if (!contextModule.supplyBlocked && u->isIdle() && Broodwar->self()->minerals() >= scvCost + contextModule.reservedMineralsAll)
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

        if (u->getType() == BWAPI::UnitTypes::Terran_Barracks)
        {
            if (!u->isBeingConstructed())
            {
                contextModule.numBarracks += 1; //increment number of barracks we are aware of

                if (!u->isTraining() && !contextModule.supplyBlocked && (Broodwar->self()->minerals() >= BWAPI::UnitTypes::Terran_Marine.mineralPrice() + contextModule.reservedMineralsAll))
                {
                    
                    u->build(BWAPI::UnitTypes::Terran_Marine);
                    
                    Broodwar << "BARRACKS: Training Marine." << std::endl;;
                }

            }
        }



    } //for loop closure: unit iterator


    /**********************************************/
    /*************** SUPPLY CONSTRUCTION ************/
    /**********************************************/

    /*======================= Build a Supply Depot=================*/
        if (contextModule.supplyBlocked && !contextModule.makingDepot && contextModule.supplyMaker_p && !contextModule.supplyMaker_p->isConstructing() && Broodwar->self()->minerals() >= (BWAPI::UnitTypes::Terran_Supply_Depot.mineralPrice() + contextModule.reservedMineralsAll))
    {
        Broodwar << "SUPPLY: Supply blocked, need a depo." << std::endl;

        // Retrieve a unit that is capable of constructing the supply needed
        //Unit supplyBuilder = u->getClosestUnit(GetType == supplyProviderType.whatBuilds().first && (IsIdle || IsGatheringMinerals) && IsOwned);
        //TilePosition buildPosition = Broodwar->getBuildLocation(BWAPI::BWAPI::UnitTypes::Terran_Barracks, constructionWorker_p->getTilePosition());
        //constructionWorker_p->build(BWAPI::UnitTypes::Terran_Barracks, buildPosition);

        UnitType supplyProviderType = contextModule.supplyMaker_p->getType().getRace().getSupplyProvider();

        if (supplyProviderType.isBuilding()) //we get unit supplies from a depot/pylon
        {
            Broodwar << "BUILDING: Tasking construction: Depot." << std::endl;
            TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, contextModule.supplyMaker_p->getTilePosition());
            if (targetBuildLocation.isValid())
            {
                // Register an event that draws the target build location
                Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*){Broodwar->drawBoxMap(Position(targetBuildLocation), Position(targetBuildLocation + supplyProviderType.tileSize()), Colors::Green); }, nullptr, supplyProviderType.buildTime() + 100);

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

    }// end if supply blocked


    /**********************************************/
    /*************** BARRACKS CONSTRUCTION ************/
    /**********************************************/

    int barracksCost = BWAPI::UnitTypes::Terran_Barracks.mineralPrice();

    if (!contextModule.supplyBlocked && !contextModule.makingBarracks  && contextModule.constructionWorker_p && !contextModule.constructionWorker_p->isConstructing() && Broodwar->self()->minerals() >= (barracksCost + contextModule.reservedMineralsAll))
    {


        //find a location and construct it
        BWAPI::TilePosition targetBuildLocation = Broodwar->getBuildLocation(BWAPI::UnitTypes::Terran_Barracks, contextModule.constructionWorker_p->getTilePosition());

        BWAPI::UnitType AttackProviderType = BWAPI::UnitTypes::Terran_Barracks;
        // Register an event that draws the target build location
        Broodwar->registerEvent([targetBuildLocation, AttackProviderType](BWAPI::Game*){Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation), BWAPI::Position(targetBuildLocation + AttackProviderType.tileSize()), BWAPI::Colors::Red); }, nullptr, AttackProviderType.buildTime() + 100);
        contextModule.constructionWorker_p->build(AttackProviderType, targetBuildLocation);

        Broodwar << "BUILDING: Tasking construction: Barracks" << std::endl;
        contextModule.reservedMineralsBase += barracksCost;
        //isConstruction = true;
        contextModule.makingBarracks = true;
        //state = 1; //now building barracks 1			
    }



    /*******************************************************/
    /*************    CONTROL ARMY   *******************/
    /*******************************************************/

    if (contextModule.numMarines >= 10)
    {
        if (!contextModule.attackingBase)
        {
            Broodwar << "COMBAT: Squad assembled, attacking nearest unit." << std::endl;
            contextModule.attackingBase = true;
        }

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

    if (contextModule.attackingBase && contextModule.numMarines < 10) //was attacking but we got our asses kicked
    {
        Broodwar << "COMBAT: Getting hurt, run away!!" << std::endl;
        contextModule.marinesUnion.move(contextModule.commandCentre->getPosition(), false);
        contextModule.attackingBase = false;
    }



    /*******************************************************/
    /*************    CLEAN UP           *******************/
    /*******************************************************/

    if (contextModule.constructionWorker_p && contextModule.makingBarracks)
    {
        if (!contextModule.constructionWorker_p->isConstructing() && !contextModule.constructionWorker_p->isMoving())
        {
            Broodwar << "BUILDING: barracks finished, release worker and reserved resources." << std::endl;
            contextModule.makingBarracks = false;
            contextModule.reservedMineralsBase = 0;
        }
        else if (contextModule.constructionWorker_p->isConstructing() && contextModule.reservedMineralsBase > 0)//still under construction
        {
            contextModule.reservedMineralsBase = 0; //very simple, should tracks multiple buildings and resource types
        }
        else
        {
            //worker is en route to construction site
        }
    }
    if (contextModule.supplyMaker_p && contextModule.makingDepot)
    {
        if (!contextModule.supplyMaker_p->isConstructing() && !contextModule.constructionWorker_p->isMoving())
        {
            Broodwar << "BUILDING: supply finished, release worker and reserved resources." << std::endl;
            contextModule.makingDepot = false;
            contextModule.reservedMineralsSupply = 0;
        }
        else if (contextModule.supplyMaker_p->isConstructing() && contextModule.reservedMineralsSupply > 0)//still under construction
        {
            contextModule.reservedMineralsSupply = 0; //very simple, should tracks multiple buildings and resource types
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

    for (auto &u : contextModule.workersUnion)
    {
        if (contextModule.constructionWorker_p == nullptr)
        {
            Broodwar << "WORKER: Assigning construction worker." << std::endl;
            contextModule.constructionWorker_p = u;
        }
        else if (contextModule.supplyMaker_p == nullptr)
        {
            Broodwar << "WORKER: Assigning supply maker." << std::endl;
            contextModule.supplyMaker_p = u;
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
                    //if (!u->gather(u->getClosestUnit( isMinera) ) )
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



void SGOMSbot::drawHUDinfo()
{
    // Display the game frame rate as text in the upper left area of the screen
    Broodwar->drawTextScreen(300, 0, "FPS: %d", Broodwar->getFPS());
    Broodwar->drawTextScreen(300, 10, "Average FPS: %f", Broodwar->getAverageFPS());
    Broodwar->drawTextScreen(10, 00, "Timer: %d", Broodwar->getFrameCount());

    Broodwar->drawTextScreen(10, 20, "Supply Blocked: %s", contextModule.isSupplyBlocked() ? "true" : "false");
    Broodwar->drawTextScreen(10, 50, "Barracks Construction: %s", contextModule.makingBarracks == false ? "false" : "true");
    Broodwar->drawTextScreen(10, 70, "Supply Construction: %s", contextModule.makingDepot == false ? "false" : "true");

    /* Update the HUD with debug stats*/
    Broodwar->drawTextScreen(150, 0, "Total Minerals: %d", Broodwar->self()->minerals());
    Broodwar->drawTextScreen(150, 10, "Reserved Minerals: %d", contextModule.reservedMineralsAll);
    Broodwar->drawTextScreen(150, 20, "Total Gas: %d", Broodwar->self()->gas());
    Broodwar->drawTextScreen(150, 30, "Reserved Gas: %d", contextModule.reservedGasAll);
    Broodwar->drawTextScreen(150, 40, "Raw Supply Total: %d", contextModule.rawSupplyTotal);
    Broodwar->drawTextScreen(150, 50, "Raw Supply Used: %d", contextModule.rawSupplyUsed);

    Broodwar->drawTextScreen(150, 70, "Barracks: %d", contextModule.numBarracks);
    Broodwar->drawTextScreen(150, 80, "Marines: %d", contextModule.numMarines);
    Broodwar->drawTextScreen(150, 90, "Workers: %d", contextModule.numWorkers);



    //BWTA draw if finished analyzing the map
    if (analysis_just_finished)
    {
        Broodwar << "Finished analyzing map." << std::endl;;
        analysis_just_finished = false;
    }

    if (analyzed)
        drawTerrainData();
}

void SGOMSbot::cycle()
{


}//end SGOMSbot::update()