#ifndef __PROFILER_LIBEXEC_NODE_APP_H__
#define __PROFILER_LIBEXEC_NODE_APP_H__

#include "callCount.h"
#include "node.h"
#include <string>
#include "outputTerminal.h"
#include <forward_list>
#include "Commands.h"
#include <functional>

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
    int CD(std::string s);

    /*
     * Change to another directory, but add the pwd to the popd stack
     */
    int PUSHD(std::string s);
    int POPD();

    int RegisterCommands(Commands& dispatcher);

private:
    std::string PrintPopdStack();
    NodePtr   root;
    NodePtr   pwd;
    OutputTerminal* output;
    std::forward_list<NodePtr> popdstack;

    /*
     * Commands
     */
    std::function<int()> f_pwd;
    std::function<int(std::string)> f_cd;
    std::function<int(std::string)> f_pushd;
    std::function<int()> f_popd;
};
#endif
