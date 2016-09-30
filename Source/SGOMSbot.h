#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <Windows.h>
//#include <boost/python.hpp>

// Remember not to use "Broodwar" in any global class constructor!
// Also remember only the calling thread should interact with BWAPI.

DWORD WINAPI AnalyzeThread();

class SGOMSbot : public BWAPI::AIModule
{
public:
  // Virtual functions for callbacks, leave these as they are.
  virtual void onStart(); //called only once at the beginning of the game
  virtual void onEnd(bool isWinner); //called once at the end of the match
  virtual void onFrame(); //called once for every execution of a logical frame in Broodwar.
  virtual void onSendText(std::string text); //called when user attepts to send a text message (eg: making the bot exec cmds while debugging)
  virtual void onReceiveText(BWAPI::Player player, std::string text); //when client received a msg from another player
  virtual void onPlayerLeft(BWAPI::Player player); //when a player leaves the game
  virtual void onNukeDetect(BWAPI::Position target); //when a nuke is launched somewhere on the map
  virtual void onUnitDiscover(BWAPI::Unit unit); //when a Unit becomes accessible
  virtual void onUnitEvade(BWAPI::Unit unit); //when a Unit becomes INaccessible
  virtual void onUnitShow(BWAPI::Unit unit); //when a previously invisible Unit becomes visible
  virtual void onUnitHide(BWAPI::Unit unit); //just as a visible unit is becoming invisible
  virtual void onUnitCreate(BWAPI::Unit unit); //when any unit is created
  virtual void onUnitDestroy(BWAPI::Unit unit); //Called when a unit is removed from the game either through death or other means.
  virtual void onUnitMorph(BWAPI::Unit unit); //Called when a unit changes its UnitType.
    //For example, when a Drone transforms into a Hatchery, a Siege Tank uses Siege Mode, or a Vespene Geyser receives a Refinery.//
  virtual void onUnitRenegade(BWAPI::Unit unit);
  //Called when a unit changes ownership. This occurs when the Protoss ability Mind Control is used, or if a unit changes ownership in Use Map Settings .
  virtual void onSaveGame(std::string gameName); //Called when the state of the Broodwar game is saved to file.

  virtual void onUnitComplete(BWAPI::Unit unit); //Called when the state of a unit changes from incomplete to complete
  // Everything below this line is safe to modify.

  void drawTerrainData();

};
