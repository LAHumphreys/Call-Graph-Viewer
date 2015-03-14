
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
    NodeUserApp(NodePtr root, OutputTerminal* _term = nullptr);

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

    int OutputOn();
    int OutputOff();
    int Help();

    /*
     * Accessors
     */
    NodePtr ActiveNode() {
        return application.ActiveNode();
    }
    Commands& CommandDispatcher() { return dispatcher;}

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

private:
    /*
     * Utility functions
     */
    std::string PrintPopdStack();

    void Initialise();
    // Call this to exit run next time it drops back to the main loop
    int Exit() { 
        exit = true; 
        return 0;
    }

    int Clear();
    int Execute(const std::string& command);
    OutputTerminal* output;
    OutputTerminal* term;
    TestTerminal    silentSink;
    Commands dispatcher;
    NodeApp  application;
    bool     exit;

    std::forward_list<NodePtr> popdstack;


    /***************************
     *        Commands
     **************************/
    std::function<int()> f_pwd;
    std::function<int(std::string)> f_cd;
    std::function<int(std::string)> f_pushd;
    std::function<int()> f_popd;

};
#endif
