#include "nodeUserApp.h"
#include "screen.h"

using namespace std;

NodeUserApp::NodeUserApp(NodePtr root) 
   : term(&Screen::Instance().MainTerminal()),
     application(root,*term),
     exit(false)
{
    Initialise();
}

NodeUserApp::NodeUserApp(NodePtr root, OutputTerminal& _term) 
   : term(&_term),
     application(root,*term),
     exit(false)
{
    Initialise();
}

void NodeUserApp::Initialise() {
    dispatcher.AddCommand("exit",std::bind(&NodeUserApp::Exit,this));
    dispatcher.AddCommand("output_on",std::bind(&NodeUserApp::OutputOn,this));
    dispatcher.AddCommand("output_off",std::bind(&NodeUserApp::OutputOff,this));
    dispatcher.AddCommand("clear",std::bind(&NodeUserApp::Clear,this));
    dispatcher.AddCommand("help",std::bind(&NodeUserApp::Help,this));
    application.RegisterCommands(dispatcher);
    OutputOn();
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

int NodeUserApp::Run() {
    exit = false;
    while ( !exit) {
        string name = application.ActiveNode()->Name();
        string shortName = name.substr(0,name.find("("));
        string cmd = term->GetLine("|" + shortName + "> ");
        try {
            Execute(cmd);
        } catch ( Commands::ExecutionError& e ) {
            term->PutString(e.msg + "\n");
        }
    }
    return 0;
}

int NodeUserApp::OutputOn() {
    output = term;
    application.SetOutputTerminal(*term);
    return 0;
}

int NodeUserApp::OutputOff() {
    output = &silentSink;
    application.SetOutputTerminal(silentSink);
    return 0;
}

int NodeUserApp::Clear() {
    output->Clear();
    return 0;
}

int NodeUserApp::Help() {
    output->PutString(dispatcher.Usage() + "\n");
    return 0;
}

