#include <string>
#include "node.h"
#include <unordered_map>
#include "callCount.h"

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
