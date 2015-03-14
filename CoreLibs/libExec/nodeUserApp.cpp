#include "nodeUserApp.h"
#include "screen.h"
#include "util_time.h"

using namespace std;
using namespace std::placeholders;

NodeUserApp::NodeUserApp(NodePtr root, OutputTerminal* _term)
   : term(_term? _term: &Screen::Instance().MainTerminal()),
     application(root,*term),
     exit(false),
     f_pwd(std::bind(&NodeUserApp::PWD,this)),
     f_cd(std::bind(&NodeUserApp::CD,this,_1)),
     f_pushd(std::bind(&NodeUserApp::PUSHD,this,_1)),
     f_popd(std::bind(&NodeUserApp::POPD,this))
{
    Initialise();
}

void NodeUserApp::Initialise() {
    dispatcher.AddCommand("exit",std::bind(&NodeUserApp::Exit,this));
    dispatcher.AddCommand("output_on",std::bind(&NodeUserApp::OutputOn,this));
    dispatcher.AddCommand("output_off",std::bind(&NodeUserApp::OutputOff,this));
    dispatcher.AddCommand("clear",std::bind(&NodeUserApp::Clear,this));
    dispatcher.AddCommand("help",std::bind(&NodeUserApp::Help,this));
    dispatcher.AddCommand("pwd",f_pwd);
    dispatcher.AddCommand("cd",f_cd, "cd <node name>");
    dispatcher.AddCommand("pushd",f_pushd,"pushd <node name>");
    dispatcher.AddCommand("popd",f_popd);
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

int NodeUserApp::CD(std::string s) {
    int ret = 0;
    if ( application.CD(s))
    {
        // found target node
    }
    else
    {
        // fall back to a search
        return application.Search(1,"^"+s);
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
            Time start;
            Execute(cmd);
            Time stop;
            stringstream s;
            s << "\"" + cmd + "\" completed in ";
            s << stop.DiffUSecs(start) /1000 << "ms" << endl;;
            term->PutString(s.str());
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


int NodeUserApp::PWD() {
    Path path = application.PWD();
    bool first = true;
    string pathStr = "";
    for ( Path::PathNode n = path.Root(); !n.IsEnd(); ++n) {
        if ( first ) {
            first = false;
            pathStr = "\n" + n.Name();
        } else {
            pathStr = pathStr + "/\n  " + n.Name();
        }
    }
    output->PutString(pathStr + "\n");
    return 0;
}

int NodeUserApp::PUSHD(string path_string) {
    NodePtr pwd = application.ActiveNode();
    NodePtr startingDir = pwd;
    int ret  = CD(path_string);
    if ( ret == 0 ) {
        popdstack.push_front(startingDir);
        output->PutString(PrintPopdStack());
    }
    return ret;
}

std::string NodeUserApp::PrintPopdStack() {
    std::string lines;

    NodePtr pwd = application.ActiveNode();
    if ( pwd->Parent().IsNull() ) {
        lines += pwd->Name();
    } else {
        lines += pwd->Parent()->Name() + "/" + pwd->Name();
    }

    for ( const NodePtr& node: popdstack ) {
        lines += "\n";
        if ( node->Parent().IsNull() ) {
            lines += node->Name();
        } else {
            lines += node->Parent()->Name() + "/" + node->Name();
        }
    }
    lines += "\n";
    return lines;
}

int NodeUserApp::POPD() {
    int ret = 0;
    if ( popdstack.empty() ) {
        ret = 1;
        output->PutString("error: Node stack is empty!\n");
    } else {
        NodePtr pwd = popdstack.front();
        popdstack.pop_front();
        application.CD(pwd);
        output->PutString(PrintPopdStack());
    }
    return ret;
}

