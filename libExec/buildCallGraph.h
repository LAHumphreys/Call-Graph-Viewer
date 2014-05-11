#ifndef __PROFILER_LIBEXEC_BUILD_CALL_GRAPH_H__
#define __PROFILER_LIBEXEC_BUILD_CALL_GRAPH_H__

#include "callCount.h"
#include "node.h"

/*
 * Utility class to read in a input file and 
 * geneate the call trees
 */
class CallProfile {
public:
    void ProcessFile(const std::string& datafile);

    std::string PrintResults(unsigned tableSize=0) {
        return counter.PrintResults(tableSize);
    }

    std::string PrintGraph(unsigned int depth=500) {
        return callGraph.PrintResults(0,depth);
    }

    NodePtr RootNode () { return callGraph.THIS();}

    CallCount& Count() { return counter;}

private:
    CallCount counter;
    Node      callGraph;
};
#endif
