#ifndef __PROFILER_LIBEXEC_NODE_CONFIG_H__
#define __PROFILER_LIBEXEC_NODE_CONFIG_H__

#include "stringStruct.h"
#include <string>
#include <vector>



/*
 * Defines configuration 
 */
class NodeConfig {
public:
    /*
     * Access to the single instance
     */
    static NodeConfig& Instance() {
        static NodeConfig config;
        return config;
    }

    virtual ~NodeConfig();

    struct NodeConfigCostReconfiguredError {
        std::string msg;
    };

    /*
     * Configure the cost factory used for the application. Attempting to
     * do this after it  has already been accessed will result in an
     * instance of NodeConfig::NodeConfigCostReconfiguredError being
     * thrown.
     */
    void ConfigureCostFactory(const std::string& definitions);

    /*
     * Access to the cost structs being used for this run
     */
    StringStructFactory& CostFactory(); 
    /*
     * Create a new cost struct on the heap - caller responsible for * cleaning up the memory */
    StringStruct* NewCost();

    /*
     * Which costs to print..
     */ 
    const std::vector<size_t>& DisplayIdxs() {
        return displayedCosts;
    }
    const std::vector<size_t>& ConfigureDisplay(const std::string& units);

    /*
     * Reset the state of the config object as if it has been first 
     * initialised - useful for testing where the application class may be
     * instantiated multiple times
     *
     *** WARNING ** ** WARNING ** ** WARNING ** ** WARNING ** ** WARNING **
     *** WARNING **                                           ** WARNING **
     *** WARNING **  It only makes sense to call this whilst  ** WARNING **
     *** WARNING **  testing and the whole application needs  ** WARNING **
     *** WARNING **  to be "restarted"                        ** WARNING **
     *** WARNING **                                           ** WARNING **
     *** WARNING ** ** WARNING ** ** WARNING ** ** WARNING ** ** WARNING **
     */
    void Reset();

private:
    // Singleton - private c'tor
    NodeConfig();
    NodeConfig(const NodeConfig& rhs) = delete;
    NodeConfig& operator=(const NodeConfig& rhs) = delete;

    bool costAccessed;
    StringStructFactory* costFactory;
    std::vector<size_t> displayedCosts;
};

#endif
