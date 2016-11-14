#pragma once

#include "Common.h"
#include "Agent.h"
#include "Context.h"
#include "Awareness.h"
#include "Goal.h"


using namespace SGOMS;

Agent::Agent()
{
    initialize();
	//do nothing for now
}

void Agent::initialize()
{
    monitor.initialize();
    context.initialize();

    //do nothing for now
}

void Agent::update()
{
    /*
    *   SGOMS Loop
    */

    //Choose Planning Unit
    {
        //Retrieve next unit task (if/then conditions)
        {//while still have unit tasks
            //retrieve next Operator / Method
            {
                //Execute operator(s)

            }

            //Update situation knowledge (from Parallel external monitoring)

            //Evaluate situation (determine arousal / emergency
            { //while problem solving is an option
                //Is everything okay?
                {
                    //If not, consider problem solving (depends on risk/arousal)

                    //Problem solve?
                    {
                        //doProblemSolve()
                    }
                }

            }//while problem solving an option

            //if emergency, break

        }//while still have unit tasks
    }//end choose planning unit   

    //do nothing for now
}