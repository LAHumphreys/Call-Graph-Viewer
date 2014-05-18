#include <string>
#include "node.h"
#include <unordered_map>
#include "callCount.h"

/*
 * Load a pre-processed callgrind file from 3 csv files... 
 */
class CallgrindCallTree {
public:
    CallgrindCallTree ( const std::string& fname);

    void LoadCalls(const std::string& fname);

    void LoadCosts(const std::string& fname);

    CallCount& Counter() {
        return counter;
    };

    void AddCalls(NodePtr node);

    NodePtr RootNode() {
        return &root;
    }
private:
    Node       root;
    CallCount  counter;
    std::unordered_map<int,NodePtr> idMap;
};

/*
 * Process a raw callgrind file
 */
class CallgrindNative {
public:
    CallgrindNative ( const std::string& fname);

    CallCount& Counter() {
        return counter;
    };

    void AddCalls(NodePtr node);

    NodePtr RootNode() {
        return &root;
    }

    void SetCurrentFunction ( const std::string& line);

    void CallChild ( const std::string& line);
private:
    Node       root;
    NodePtr    child;
    NodePtr    current;
    CallCount  counter;
    std::unordered_map<int,NodePtr> idMap;
};
