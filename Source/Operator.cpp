#include "Operator.h"
#include <BWAPI.h>
#include "Common.h"

using namespace OperatorTypes;

Operator::Operator()
{

}

Operator::Operator(OperatorTypes::OpCode code)
{
    myOp = code;
}

Operator::~Operator()
{
}

void Operator::execute()
{
    switch (myOp)
    {
    case OperatorTypes::Train:
        subject->train(objectType);
        break;
    default:
        ;
    }
}
