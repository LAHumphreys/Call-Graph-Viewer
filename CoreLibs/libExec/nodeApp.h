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
    Path PWD();

    /**
     * Change into node at path...
     *
     * @param path    The path to change into
     *
     * @returns TRUE if the node was found and is now the pwd, FALSE otherwise
     */
    bool CD(std::string path);

    void CD(NodePtr node);

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

    /*
     * Modify program output
     */
    int Display(std::string units);

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
     * Data
     */
    // location data
    NodePtr   root;
    NodePtr   pwd;

    // Where to send output to
    OutputTerminal* output;

    // Search tools
    RegSearch      finder;
    SearchResult*  result = nullptr;

    /*
     * Commands
     */
    std::function<int(int,int,std::string)> f_flat;
    std::function<int(int)> f_tree;
    std::function<int(int,std::string)> f_search;
    std::function<int(std::string)> f_display;
};
#endif
