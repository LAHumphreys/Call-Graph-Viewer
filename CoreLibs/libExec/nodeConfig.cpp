#include "nodeConfig.h"
#include "logger.h"


NodeConfig::NodeConfig() 
    : costAccessed(false), 
      costFactory(new StringStructFactory("Time")),
      displayedCosts({0})
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
        displayedCosts = costFactory->GetIdxs(definitions);
    }
}

void NodeConfig::Reset() {
    LOG_FROM(LOG_WARNING,"NodeConfig::Reset",
               "**WARNING** The global Config object has been reset, "
               "it is extremely unlikely you wanted to do this unless"
               " you are testing the application, and know"
               "exactly what you are doing **WARNING**")
    delete costFactory;
    costFactory = new StringStructFactory("Time");
    costAccessed = false;
    displayedCosts.clear();
    displayedCosts.push_back(0);
}

const vector<size_t>& NodeConfig::ConfigureDisplay(const string& units) {
    displayedCosts = costFactory->GetIdxs(units);
    return DisplayIdxs();
}
