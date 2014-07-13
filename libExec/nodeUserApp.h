
#ifndef __PROFILER_LIBEXEC_NODE_USER_APP_H__
#define __PROFILER_LIBEXEC_NODE_USER_APP_H__

#include "callCount.h"
#include "node.h"
#include "outputTerminal.h"
#include "Commands.h"
#include "nodeApp.h"
#include "configFile.h"


class NodeUserApp {
public:
    NodeUserApp(NodePtr root);

    /*
     * Run the main loop, executing commands until 
     * the user exits the application
     */
    int Run();

    /*
     * Run a script
     */
    int Execute(const CommandScript& script);

    /* 
     * Change the terminal output is sent to. Intended for testing
     * use only.
     */
    void SetMainTerminal(OutputTerminal& _term) {
        term = &_term;
    }
    NodeUserApp(NodePtr root, OutputTerminal& term);
private:
    int Execute(const std::string& command);
    OutputTerminal* term;
    Commands dispatcher;
    NodeApp  application;
};
#endif
