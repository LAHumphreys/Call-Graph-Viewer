#ifndef __PROFILER_LIBEXEC_NODE_APP_H__
#define __PROFILER_LIBEXEC_NODE_APP_H__

#include "callCount.h"
#include "node.h"
#include <string>
#include "outputTerminal.h"
#include <forward_list>
#include "Commands.h"
#include <functional>
#include "nodeSearchCache.h"

/*
 * Utility class to read in a input file and 
 * geneate the call trees
 */
class NodeApp {
public:
    /*C'tor
     *
     * Process a callgrind file to build the callgraph
     *
     *   root: The root of the callgrraph
     *   term: The output sink to use
     *
     */
    NodeApp(NodePtr root, OutputTerminal& output);
    virtual ~NodeApp();

    /***************************
     *        Commands
     **************************/


    /*
     * Write the path of the current working node to the output
     */
    int PWD();
    /*
     * Change into node at path...
     */
    int CD(std::string s);

    /*
     * Change to another directory, but add the pwd to the popd stack
     */
    int PUSHD(std::string s);
    int POPD();

    /*
     * Create a local flat view
     */
    int Flat(int depth, int max, std::string pattern);

    /*
     * Print a tree view
     */
    int Tree(int depth);

    /*
     * Search for a node...
     */
    int Search(int depth, std::string pattern);
    int Next();
    int Previous();
    int ListSearch();

    /***************************
     * Public help functions
     **************************/

    /*
     * Add application commands to a Commmands dispatcher
     * object
     */
    int RegisterCommands(Commands& dispatcher);

    NodePtr ActiveNode() {
        return pwd;
    }

    void SetOutputTerminal(OutputTerminal& term) {
        output = &term;
    }

private:
    /*
     * Utility functions
     */
    std::string PrintPopdStack();

    /*
     * Data
     */
    // location data
    NodePtr   root;
    NodePtr   pwd;

    std::forward_list<NodePtr> popdstack;

    // Where to send output to
    OutputTerminal* output;

    // Search tools
    RegSearch      finder;
    SearchResult*  result = nullptr;


    /*
     * Commands
     */
    std::function<int()> f_pwd;
    std::function<int(std::string)> f_cd;
    std::function<int(std::string)> f_pushd;
    std::function<int()> f_popd;
    std::function<int(int,int,std::string)> f_flat;
    std::function<int(int)> f_tree;
    std::function<int(int,std::string)> f_search;
};
#endif
