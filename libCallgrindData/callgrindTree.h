#include <string>
#include "node.h"
#include <unordered_map>
#include "callCount.h"
#include "SourceFile.h"

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

    void SetCalls (const std::string& line);

    void AddCost (const std::string& line);

    void ChangeFile (const std::string& line);
    void SetChildFile (const std::string& line);

    void AddFile (const int& id, const std::string& path);

    SourceFile& GetFile(const int& id);

    std::string Annotate(NodePtr node, int threshold = 101);

private:
    Node           root;
    NodePtr        child;
    NodePtr        current;
    CallCount      counter;
    int            numCalls;
    int            currentFile;
    int            childFile;
    int            currentLine;
    int            currentActiveLine;
    std::unordered_map<int,SourceFile> sources;
    std::unordered_map<int,NodePtr> idMap;
};
