#ifndef __PROFILER_LIBEXEC_NODE_CONFIG_H__
#define __PROFILER_LIBEXEC_NODE_CONFIG_H__

#include "stringStruct.h"
#include <string>

/*
 * Defines configuration 
 */
class NodeConfig {
public:
    static NodeConfig& Instance() {
        static NodeConfig config;
        return config;
    }

    StringStructFactory& CostFactory() { return costFactory; } 
    StringStruct* NewCost();
private:
    // Singleton - private c'tor
    NodeConfig();
    NodeConfig(const NodeConfig& rhs) = delete;
    NodeConfig& operator=(const NodeConfig& rhs) = delete;

    StringStructFactory costFactory;
};

#endif
