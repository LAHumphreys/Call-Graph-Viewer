#include "nodeApp.h"
#include "path.h"

using namespace std;

NodeApp::NodeApp(NodePtr _root, OutputTerminal& _output) 
    : root(_root), pwd(_root), output(&_output)
{
}

int NodeApp::PWD() {
    string path = pwd->Name();
    for ( NodePtr i = pwd->Parent(); !i.IsNull(); i = i->Parent() ) {
        path = i->Name() + "/\n  " + path;
    }
    output->PutString(path);
    return 0;
}


int NodeApp::CD(const string& path_string) {
    int ret = 0;
    Path path (path_string);
    Path::PathNode rootPathNode = path.Root();
    NodePtr working = nullptr;
    if ( path_string != "" ) {
        if ( rootPathNode.Name() == "ROOT") {
            working = root->GetNode(rootPathNode.Next());
        } else {
            working = pwd->GetNode(std::move(rootPathNode));
        }
    }
    if ( !working.IsNull() ) {
        pwd = working;
    } else {
        output->PutString("error, no such node: " + path_string);
        ret = 1;
    }
    return ret;
}

int NodeApp::PUSHD(const string& path_string) {
    NodePtr startingDir = pwd;
    int ret  = CD(path_string);
    if ( ret == 0 ) {
        popdstack.push_front(startingDir);
        output->PutString(PrintPopdStack());
    }
    return ret;
}

std::string NodeApp::PrintPopdStack() {
    std::string lines;

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
    return lines;
}

int NodeApp::POPD() {
    int ret = 0;
    if ( popdstack.empty() ) {
        ret = 1;
        output->PutString("error: Node stack is empty!");
    } else {
        pwd = popdstack.front();
        popdstack.pop_front();
        output->PutString(PrintPopdStack());
    }
    return ret;
}
