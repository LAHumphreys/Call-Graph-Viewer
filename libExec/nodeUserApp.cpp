#include "nodeUserApp.h"
#include "screen.h"

using namespace std;

NodeUserApp::NodeUserApp(NodePtr root) 
   : term(&Screen::Instance().MainTerminal()),
     application(root,*term)
{
    application.RegisterCommands(dispatcher);
}

NodeUserApp::NodeUserApp(NodePtr root, OutputTerminal& _term) 
   : term(&_term),
     application(root,*term)
{
    application.RegisterCommands(dispatcher);
}

int NodeUserApp::Execute(const CommandScript& script) {
    int ret = 0;
    for ( const string& command: script.Commands() ) {
        ret = dispatcher.Execute(command);
    }
    return ret;
}

int NodeUserApp::Execute(const string& command) {
    return dispatcher.Execute(command);
}
