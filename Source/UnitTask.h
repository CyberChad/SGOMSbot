#pragma once

// Remember not to use "Broodwar" in any global class constructor!

//namespace SGOMSbot

class UnitTask
{
    public:
	    UnitTask();
	    ~UnitTask()
	    {
		    //do nothing
	    };

	/*Contaner for Generic UnitTask (extended by Building/Combat/Unit/Worker commands)

	To be contained by Planning Unit type
	Contains a list of Methods/Operators (Queue)
		
	Contains a pointer to SituationKnowledge


	*/
};


