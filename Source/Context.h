#pragma once
#include <BWAPI.h>
#include <BWTA.h>

// Remember not to use "Broodwar" in any global class constructor!






class Context
{
    public:
	    Context();
	    ~Context(){};

        void initialize();
        void update();
        void updateSupplyInfo();
        bool isSupplyBlocked(){return supplyBlocked;};

        //Unit Management globals
        BWAPI::Unitset infantry;
        BWAPI::Unitset workersUnion;
        BWAPI::Unitset workersMinerals;
        BWAPI::Unitset workersGas;
        BWAPI::Unitset availableWorkers;

        BWAPI::Unit constructionWorker_p = nullptr;
        BWAPI::Unit supplyMaker_p = nullptr;

        BWAPI::Unitset marinesUnion;
        BWAPI::UnitType supplyProviderType = NULL;

        //Building Management globals
        int lastChecked = 0; // last game frame we checked if supply blocked

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

        //State transition globals
        bool makingDepot = false;
        bool makingBarracks = false;
        bool isConstruction = false;
        bool supplyBlocked = false;

        //Combat globals
        bool attackingBase = false;


        int numWorkers = 0;
        int numMarines = 0;
        int nextUnit = 0;
           
};


