#pragma once

#include "Common.h"
#include "SGOMSbot.h"
#include "Operator.h"
#include "UnitTask.h"
#include "PlanningUnit.h"

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
using namespace UnitTaskTypes;

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
  myID = Broodwar->self()->getID();
  
  awarenessModule.initialize(); //internal game state and understanding
  contextModule.initialize(); //internal game state and understanding  
  strategyModule.initialize("Default", contextModule );
  
  
  Py_Initialize();

  PyObject* myModuleImportString = PyString_FromString((char*)"import ccm");
  PyObject* myModule = PyImport_Import(myModuleImportString);  
  //PyObject* myFunction = PyObject_GetAttrString(myModule, (char*)"myabs");
  //PyObject* args = PyTuple_Pack(1, PyFloat_FromDouble(2.0));
  

  //PyObject* myResult = PyObject_CallObject(myFunction, args)
  //double result = PyFloat_AsDouble(myResult);
  
  //PyRun_SimpleString("import ccm");
  Py_Finalize();
   

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


/************************************************************/
/********************   ON FRAME CYCLE  *********************/
/************************************************************/

void SGOMSbot::onFrame()
{
  // Called once every game frame. Default is 24 FPS.
    
    /* The following code is included with the SSCAI and BWAPI ExampleAI module, and recommended for performance reasons. */
    // Return if the game is a replay or is paused
    if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
        return;

    // Prevent spamming by only running our onFrame once every number of latency frames.
    // Latency frames are the number of frames before commands are processed.
    if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
        //return;

    contextModule.update();
    drawHUDinfo();

    /*=============================================*/
    /*==========  SGOMS LOOP  =====================*/
    /*=============================================*/
    
 
    if ( !awarenessModule.flag ) //RHS: everything's okay
    {      

        if ( nextTask && !nextTask->isFinished() )
        {   
            nextOp = nextTask->getNextOp();            
            if (nextOp)
            {
                contextModule.currGameOp = nextOp->myName;
                nextOp->execute();
            }
        }
        else if ( planUnit && !planUnit->isFinished() )
        {   
            nextTask = planUnit->getNextTask(contextModule);
            contextModule.currUnitTask = nextTask->myName;
        }
        else
        {            
            planUnit = strategyModule.getNextPlan(contextModule);                 
            contextModule.currPlanUnit = planUnit->myName;
        }        
    }
    else //LHS: Possible Problem
    {
        //If not, consider problem solving (depends on risk/arousal)
        if (!awarenessModule.emergency)
        {
            //doProblemSolve(), may reduce arousal?
        }
        else //EMERGENCY!!!!!
        {
            //push a new plan onto the stack
            planUnit = strategyModule.createNewPlan(contextModule);
            nextTask = nullptr;
            nextOp = nullptr;            
        }
    }
    

    // UPDATE SITUATION KNOWLEDGE  
    {
        //Update situation knowledge (from Parallel external monitoring)
        contextModule.update();
    }

    // EVALUATE SITUATION 
    {
        //input from prior problem solving
        

    }

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
 
}

void SGOMSbot::onUnitEvade(BWAPI::Unit unit)
{
}

void SGOMSbot::onUnitShow(BWAPI::Unit unit)
{

    //CP TODO: Create Units data structures for Intelligence gathering.
    
    int unitId = unit->getPlayer()->getID();

    if (unitId == myID) //unit is mine
    {
        Broodwar << "AWARENESS: Unit Shown: " << unit->getType().c_str() << std::endl;

    }//end if my unit

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
    //CP TODO: Create Units data structures for Intelligence gathering.
    
    int unitId = unit->getPlayer()->getID();

    if (unitId == myID) //unit is mine
    {
        Broodwar << "AWARENESS: New Unit Complete: " << unit->getType().c_str() << std::endl;

        if (unit->getType().isWorker())
        {
            contextModule.workerSet.insert(unit);
            if (contextModule.constructionWorker_p == nullptr)
            {
                Broodwar << "WORKER: Assigning construction worker." << std::endl;
                contextModule.constructionWorker_p = unit;
            }
            else if (contextModule.supplyMaker_p == nullptr)
            {
                Broodwar << "WORKER: Assigning supply maker." << std::endl;
                contextModule.supplyMaker_p = unit;
            }

            // if our worker is idle, make them so domething useful (gather minerals for now)
            if (unit->isIdle())
            {
                // Order workers carrying a resource to return them to the center, otherwise find a mineral patch to harvest.
                if (unit->isCarryingGas() || unit->isCarryingMinerals())
                {
                    unit->returnCargo();
                }
                else if (!unit->getPowerUp())  // The worker cannot harvest anything if it is carrying a powerup such as a flag (CTF only)
                {
                    // Harvest from the nearest mineral patch or gas refinery
                    if (!unit->gather(unit->getClosestUnit(IsMineralField || IsRefinery)))
                        //if (!u->gather(u->getClosestUnit( isMinera) ) )
                    {
                        // If the call fails, then print the last error message
                        Broodwar << Broodwar->getLastError() << std::endl;
                    }

                } // closure: has no powerup

            } // closure: if idle

            //do something

        }//end if worker

        if (unit->getType() == BWAPI::UnitTypes::Terran_Marine )
        {
            contextModule.marineSet.insert(unit);
            contextModule.marinesUnion.insert(unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Barracks )
        {
            contextModule.barrackSet.insert(unit);           
            
        }

    }//end if my unit
    else
    {
        awarenessModule.emergency = true;
        awarenessModule.arousal = 10;
    }


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
    
    /********************/
    /****** Debug *******/
    /********************/

    Broodwar->drawTextScreen(10, 00, "Timer: %d", Broodwar->getFrameCount());

    Broodwar->drawTextScreen(10, 20, "Supply Blocked: %s", contextModule.isSupplyBlocked() ? "true" : "false");
    Broodwar->drawTextScreen(10, 30, "Barracks Construction: %s", contextModule.makingBarracks == false ? "false" : "true");
    Broodwar->drawTextScreen(10, 40, "Supply Construction: %s", contextModule.makingDepot == false ? "false" : "true");

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

    Broodwar->drawTextScreen(300, 0, "FPS: %d", Broodwar->getFPS());
    Broodwar->drawTextScreen(300, 10, "Average FPS: %f", Broodwar->getAverageFPS());
    
    /********************/
    /****** SGOMS *******/
    /********************/

    int sgoms = 350;
    
    Broodwar->drawTextScreen(sgoms, 30, "*** SGOMS : Macro ***");
    Broodwar->drawTextScreen(sgoms, 40, "STRATEGY: %s", strategyModule.getStrategy().c_str());
    Broodwar->drawTextScreen(sgoms, 50, "PLANUNIT: %s", contextModule.currPlanUnit.c_str());
    Broodwar->drawTextScreen(sgoms, 60, "UNITTASK: %s", contextModule.currUnitTask.c_str());
    Broodwar->drawTextScreen(sgoms, 70, "OPERATOR: %s", contextModule.currGameOp.c_str());
        BWAPI::Color sgomsBoxColor(255, 0, 255);
        Broodwar->drawBoxScreen(sgoms - 10, 30, sgoms+200, 90, sgomsBoxColor, false);

    /********************/
    /****** SGOMS *******/
    /********************/
    Broodwar->drawTextScreen(sgoms, 95, "*** ACT-R : Micro ***");
    Broodwar->drawTextScreen(sgoms, 105, "Declarative: %s", contextModule.declarative.c_str());
    Broodwar->drawTextScreen(sgoms, 115, "Procedural: %s", contextModule.procedural.c_str());
    Broodwar->drawTextScreen(sgoms, 125, "Production: %s", contextModule.production.c_str());
    Broodwar->drawTextScreen(sgoms, 135, "PyOper: contextModule.pyOper.c_str()" );

    BWAPI::Color actrBoxColor(150, 150, 150);
    Broodwar->drawBoxScreen(sgoms - 10, 91, sgoms + 300, 150, actrBoxColor, false);


    //BWTA draw if finished analyzing the map
    if (analysis_just_finished)
    {
        Broodwar << "Finished analyzing map." << std::endl;
        analysis_just_finished = false;
    }

    if (analyzed)
        drawTerrainData();
}

void SGOMSbot::cycle()
{


}//end SGOMSbot::update()