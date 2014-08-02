#include "nodeConfig.h"


NodeConfig::NodeConfig() 
    : costFactory("Time")
{
}

StringStruct* NodeConfig::NewCost() {
    return new StringStruct(CostFactory().New());
}
