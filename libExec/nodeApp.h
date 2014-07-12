#ifndef __PROFILER_LIBEXEC_NODE_APP_H__
#define __PROFILER_LIBEXEC_NODE_APP_H__

#include "callCount.h"
#include "node.h"
#include <string>
#include "outputTerminal.h"
#include <forward_list>

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

    /*
     * Write the path of the current working node to the output
     */
    int PWD();

    /*
     * Change into node at path...
     */
    int CD(const std::string& s);

    /*
     * Change to another directory, but add the pwd to the popd stack
     */
    int PUSHD(const std::string& s);

    int POPD();
private:
    std::string PrintPopdStack();
    NodePtr   root;
    NodePtr   pwd;
    OutputTerminal* output;
    std::forward_list<NodePtr> popdstack;
};
#endif
