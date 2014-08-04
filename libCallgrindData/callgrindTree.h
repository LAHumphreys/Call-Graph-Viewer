#include <string>
#include "node.h"
#include <unordered_map>
#include "callCount.h"
#include "SourceFile.h"
#include "stringStruct.h"

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
    ~CallgrindNative ();
    /**********************************************
     *      Results Access
     **********************************************/

    /*
     * Retrun the root node of the call graph
     */
    NodePtr RootNode() {
        return &root;
    }
    /*
     * Returns the global flat view
     */
    CallCount& Counter() {
        return counter;
    };

    /*
     * Apply annotations to the node, based on the parsed input file
     */
    std::string Annotate(NodePtr node, int threshold = 101);

    /*
     * Return the source file with id
     */
    SourceFile& GetFile(const int& id);

    /*
     * What is the index into the results data you will find unit at?
     *
     * In there was no events line it will return INT_MAX, but node
     * that there will be no nodes to look up anyway...
     */
    size_t GetCostIdx(const std::string& unit) const;
    /*
     * Return a new struct with the costs of this line
     */
    using CostStruct = StringStruct;
    CostStruct MakeCosts(const char* line) const;
private:

    /**********************************************
     *      Handle lines in the input file
     **********************************************/

    /*
     * Change the source file the next defined function will be found in
     */
    void ChangeFile (const std::string& line);

    /*
     * Called at the start of a new fn block in the input file
     */ 
    void SetCurrentFunction ( const std::string& line);

    /*
     * Called before a child-call if it belongs to a different source file,
     * triggered by cfl/cfi
     */ 
    void SetChildFile (const std::string& line);
    /*
     * Triggered by a child call (cfn) in the input file
     */ 
    void CallChild ( const std::string& line);


    /**********************************************
     *      Record Costs
     **********************************************/

    /*
     * Add a new source file to the cache
     */
    void AddFile (const int& id, const std::string& path);

    /*
     * Target of a for each loop for adding function calls to the flat 
     * view
     */
    void AddCalls(NodePtr node);

    /*
     * Add costs for the current node...
     */
    void SetCalls (const std::string& line);
    void AddCost (const std::string& line);

    Node           root;
    NodePtr        child;
    NodePtr        current;
    CallCount      counter;
    int            numCalls;
    int            currentFile;
    int            childFile;
    int            currentLine;
    int            currentActiveLine;
    StringStruct*  currentCost;
    std::unordered_map<int,SourceFile> sources;
    std::unordered_map<int,NodePtr> idMap;
};
