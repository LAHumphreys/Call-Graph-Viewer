#ifndef __PROFILER_LIBEXEC_BUILD_CALL_GRAPH_H__
#define __PROFILER_LIBEXEC_BUILD_CALL_GRAPH_H__

#include "callCount.h"

/*
 * Utility class to read in a input file and 
 * geneate the call trees
 */
class CallProfile {
public:
    void ProcessFile(const std::string& datafile);

    std::string PrintResults() {
        return counter.PrintResults();
    }
private:
    CallCount counter;
};
#endif
