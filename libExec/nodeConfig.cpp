#include "nodeConfig.h"
#include "logger.h"


NodeConfig::NodeConfig() 
    : costAccessed(false), costFactory(new StringStructFactory("Time"))
{
}

NodeConfig::~NodeConfig() {
    delete costFactory;
}

StringStruct* NodeConfig::NewCost() {
    costAccessed = true;
    return new StringStruct(CostFactory().New());
}

StringStructFactory& NodeConfig::CostFactory() {
    costAccessed = true;
    return *costFactory;
}

void NodeConfig::ConfigureCostFactory(const std::string& definitions) {
    if ( costAccessed ) {
        throw NodeConfigCostReconfiguredError{ 
                 "Attempted to configure costFactory after first access!"
        };
    } else {
        delete costFactory;
        costFactory = new StringStructFactory(definitions);
    }
}

void NodeConfig::Reset() {
    LOG_FROM(LOG_WARNING,"NodeConfig::Reset",
               "**WARNING** Global Config object has been reset, "
               "it is unlikely you wanted to do this unles you are "
               "testing the application **WARNING**")
    delete costFactory;
    costFactory = new StringStructFactory("Time");
    costAccessed = false;
}
