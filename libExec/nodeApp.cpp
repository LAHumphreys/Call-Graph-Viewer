#include "nodeApp.h"
#include "path.h"
#include "nodeConfig.h"
#include <iomanip>

using namespace std;
using namespace std::placeholders;

NodeApp::NodeApp(NodePtr _root, OutputTerminal& _output) 
    : root(_root), pwd(_root), output(&_output),
     f_pwd(std::bind(&NodeApp::PWD,this)),
     f_cd(std::bind(&NodeApp::CD,this,_1)),
     f_pushd(std::bind(&NodeApp::PUSHD,this,_1)),
     f_popd(std::bind(&NodeApp::POPD,this)),
     f_flat(std::bind(&NodeApp::Flat,this,_1,_2,_3)),
     f_tree(std::bind(&NodeApp::Tree,this,_1)),
     f_search(std::bind(&NodeApp::Search,this,_1,_2)),
     f_display(std::bind(&NodeApp::Display,this,_1))
{
}
NodeApp::~NodeApp() {
    delete result;
}

int NodeApp::RegisterCommands(Commands& dispatcher) {
    dispatcher.AddCommand("pwd",f_pwd);
    dispatcher.AddCommand("cd",f_cd, "cd <node name>");
    dispatcher.AddCommand("pushd",f_pushd,"pushd <node name>");
    dispatcher.AddCommand("popd",f_popd);
    dispatcher.AddCommand("flat",f_flat, "flat <max depth> <max rows> [pattern]");
    dispatcher.AddCommand("tree",f_tree, "tree <max depth>");
    dispatcher.AddCommand("search",f_search, "search <max depth> <pattern>");
    dispatcher.AddCommand("next",std::bind(&NodeApp::Next,this));
    dispatcher.AddCommand("display",f_display,"display <units to print>");
    return 0;
}

int NodeApp::PWD() {
    string path = pwd->Name();
    for ( NodePtr i = pwd->Parent(); !i.IsNull(); i = i->Parent() ) {
        path = i->Name() + "/\n  " + path;
    }
    output->PutString(path + "\n");
    return 0;
}


int NodeApp::CD(string path_string) {
    int ret = 0;
    NodePtr working = nullptr;
    if ( path_string == "ROOT" ) {
        working = root;
    } else  {
        Path path (path_string);
        Path::PathNode rootPathNode = path.Root();
        if ( path_string != "" ) {
            if ( rootPathNode.Name() == "ROOT") {
                working = root->GetNode(rootPathNode.Next());
            } else {
                working = pwd->GetNode(std::move(rootPathNode));
            }
        }
    }
    if ( !working.IsNull() ) {
        pwd = working;
    } else {
        Search(1,"^" + path_string);
        //output->PutString("error, no such node: " + path_string + "\n");
        //ret = 1;
    }
    return ret;
}

int NodeApp::PUSHD(string path_string) {
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
    lines += "\n";
    return lines;
}

int NodeApp::POPD() {
    int ret = 0;
    if ( popdstack.empty() ) {
        ret = 1;
        output->PutString("error: Node stack is empty!\n");
    } else {
        pwd = popdstack.front();
        popdstack.pop_front();
        output->PutString(PrintPopdStack());
    }
    return ret;
}

int NodeApp::Flat(int depth, int max, std::string pattern) {
    output->PutString(pwd->Tabulate(depth,max,pattern) + "\n");
    return 0;
}

int NodeApp::Tree(int depth) {
    output->PutString(pwd->PrintResults(2,depth,false) + "\n");
    return 0;
}

int NodeApp::Search(int depth, string pattern) {
    delete result;
    finder.Search(pwd,pattern.substr(1),depth);
    result = new SearchResult(finder.Results());
    if ( !result->Node().IsNull() ) {
        pwd = result->Node();
        ListSearch();
    } else {
        output->PutString("error, no such node: " + pattern + "\n");
    }
    return 0;
}

int NodeApp::Previous() {
    if ( result == nullptr ) {
        output->PutString("Error: No active search!\n");
    } else {
        --(*result);
        if ( !result->Node().IsNull() ) {
            pwd = result->Node();
            ListSearch();
            stringstream buf;
            buf << "There are " << result->Remaining() << " more results" << endl;
            output->PutString(buf.str());
        } else {
            // unwind the change
            ++(*result);
            output->PutString("Error: already at the first result");
        }
    }
    return 0;
}

int NodeApp::Next() {
    if ( result == nullptr ) {
        output->PutString("Error: No active search!\n");
    } else {
        ++(*result);
        if ( !result->Node().IsNull() ) {
            pwd = result->Node();
            ListSearch();
            stringstream buf;
            buf << "There are " << result->Remaining() << " more results" << endl;
            output->PutString(buf.str());
        } else {
            // unwind the change
            --(*result);
            output->PutString("Error: No nodes found\n");
        }
    }
    return 0;
}

/*
 * List the search results
 */
int NodeApp::ListSearch() {
    if ( result == nullptr ) {
        output->PutString("Error: No active search!\n");
    } else {
        stringstream results;
        for ( SearchResult it(result->First()); it.Ok(); ++it) {
            NodePtr node = it.Node();
            if (  node == result->Node() ) {
                results << " --> ";
            } else {
                results << "     ";
            }
            results << node->Parent()->Name() << "/" << node->Name();
            results <<  " : " <<  scientific << (float) node->RunTime() << " / " << node->Calls();
            results << " (" << ( node->Calls() == 0 ? 
                                   0 : 
                                   node->RunTime() / node->Calls() 
                            ) << ")" << endl;
        }
        output->PutString(results.str() + "\n");
    }
    return 0;
}

int NodeApp::Display(std::string units) {
    if ( units != "" ) {
        NodeConfig::Instance().ConfigureDisplay(units);
    }

    static map<string,string> unitMap({
        {"Ir",   "Total Instructions Read" },
        {"I1mr", "Instruction reads which missed L1 cache" },
        {"ILmr", "Instruction reads which missed the last line of cache" },
        {"Dr",   "Total memory reads" },
        {"D1mr", "Memory reads which missed L1 cache" },
        {"DLmr", "Memory reads which missed the last line of cache" },
        {"Dw",   "Total memory writes" },
        {"D1mw", "Memory writes which missed L1 cache" },
        {"DLmw", "Memory writes which missed the last line of cache" },
        {"Bc",   "Total conditional branches executed" },
        {"Bcm",  "Mispredicted conditional branches" },
        {"Bi",   "Indirect branches (jumps to an unknown location)" },
        {"Bim",  "Indirect branches mispredicted" }
    });

    stringstream results;
    results << "        Enabled"   << endl;
    results << "      -----------" << endl;
    results << left << setw(15) << " Unit Name "  << setw(60) << " Description ";
    results << endl;
    results << left << setw(15) << "-----------"  << setw(60) << "-------------";
    results << endl;
    for ( const size_t& i : NodeConfig::Instance().DisplayIdxs() ) {
        const string& unitName = NodeConfig::Instance().CostFactory()
                                    .GetName(i);
        results << left << setw(15) << " " + unitName;
        results << left << setw(60) << unitMap[unitName];
        results << endl;
    }
    results << "        Available"   << endl;
    results << "      -------------" << endl;
    results << left << setw(15) << " Unit Name "  << setw(60) << " Description ";
    results << endl;
    results << left << setw(15) << "-----------"  << setw(60) << "-------------";
    results << endl;
    for ( size_t i = 0; 
          i < NodeConfig::Instance().CostFactory().NumElements();
          ++i)
    {
        const string& unitName = NodeConfig::Instance().CostFactory()
                                    .GetName(i);
        results << left << setw(15) << " " + unitName;
        results << left << setw(60) << unitMap[unitName];
        results << endl;
    }
    output->PutString(results.str() + "\n");
    return 0;
}
