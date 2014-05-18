#include "logger.h"
#include "buildCallGraph.h"
#include <iostream>
#include <sstream>
#include <string>
#include "util_time.h"
#include "nodeSearchCache.h"
#include "callgrindTree.h"

using namespace std;

CallgrindNative* data = nullptr;
CallProfile profile;
CallCount* counter = nullptr;
SearchCache* cache = nullptr;
SearchResult* result = nullptr;
RegSearch  finder;

NodePtr rootNode = nullptr;

// Utils
string GetCommand(NodePtr& activeNode);
void GetHelp(NodePtr& activeNode, stringstream& command);

// Searching
void AdvanceSearch(NodePtr& activeNode, short direction);
void ListSearch();
void Search(NodePtr& activeNode, stringstream& command);
void SearchChildren(NodePtr& activeNode, stringstream& command);

// Navigation
void GoTo(NodePtr& activeNode, stringstream& command);
void GoToParent(NodePtr& activeNode, stringstream& command);
void LS(NodePtr& activeNode, stringstream& command);
void PWD (NodePtr& activeNode);

// Information
void PrintFilteredTable(NodePtr& activeNode, stringstream& command);
void PrintTable(NodePtr& activeNode, stringstream& command);
void PrintTree(NodePtr& activeNode, stringstream& command);
void PrintWideTable(NodePtr& activeNode, stringstream& command);
void PrintAnnotation(NodePtr& activeNode);

int main(int argc, const char *argv[])
{
    NodePtr activeNode = nullptr;

    if ( argc != 2 && argc != 4) {
        cout << "Usage: profile <data file>" << endl;
        cout << "Usage: profile <flist file> <calls file> <cost file>" << endl;
        return 1;
    }

    LOG_FROM(LOG_OVERVIEW, "main", "Building Profile...")
    cout << "Building call graph...";
    cout.flush();
    Time start;

    data = new CallgrindNative(argv[1]);

    Time end;
    cout << "done" << endl;
    cout << "Process started up in " << end.DiffUSecs(start)/1000 << " mili-seconds" << endl;
    LOG_FROM(LOG_OVERVIEW, "main", "Profile built - program ready!")

    // start at the top of the tree
    rootNode = data->RootNode();
    counter = &data->Counter();

    // start at the top of the tree
    activeNode = rootNode;



    /*
     * Main Loop
     */
    while ( 1 ) {
        stringstream command(GetCommand(activeNode));
        std::string action;
        command >> action;
        SLOG_FROM(LOG_OVERVIEW, "main", "Got action: '" << action 
                                           << "' from command '" << command.str() << "'")
        if ( action == "help" ) {
            GetHelp(activeNode, command);
        } else if ( action == "exit" ) {
            break;
        } else if ( action == "table" || action == "t" ) {
            PrintTable(activeNode, command);
        } else if ( action == "widetable" || action == "wt" ) {
            PrintWideTable(activeNode, command);
        } else if ( action == "searchtable" || action == "st" ) {
            PrintFilteredTable(activeNode, command);
        } else if ( action == "tree" ) {
            PrintTree(activeNode, command);
        } else if ( action == "cd" ) {
            GoTo(activeNode, command);
        } else if ( action == ".." ) {
            GoToParent(activeNode, command);
        } else if ( action == "pwd" ) {
            PWD(activeNode);
        } else if ( action == "ls" ) {
            LS(activeNode, command);
        } else if ( action == "search"  || action == "s") {
            Search(activeNode, command);
        } else if ( action == "searchchildren" || action == "sc" ) {
            SearchChildren(activeNode, command);
        } else if ( action == "searchroot" || action == "sr" ) {
            activeNode = rootNode;
            SearchChildren(activeNode, command);
        } else if ( action == "this" ) {
            AdvanceSearch(activeNode, 0);
        } else if ( action == "previous" || action == "p" ) {
            AdvanceSearch(activeNode, -1);
        } else if ( action == "next"  || action == "n") {
            AdvanceSearch(activeNode, 1);
        } else if ( action == "list" ) {
            ListSearch();
        } else if ( action == "annotate" || action == "a" ) {
            PrintAnnotation(activeNode);
        } else {
            cout << "Unknown command!" << endl;
            GetHelp(activeNode,command);
        }
    }


    return 0;
}

/*
 * Get the next command from the user ( a single line of text)
 */
string GetCommand(NodePtr& activeNode) {
    char buf[10240];
    string name = activeNode->Name();
    string shortName = name.substr(0,name.find("("));
    cout << endl << "|" << shortName << "> ";
    cin.getline(buf,10240);
    return string(buf);
}

/*
 * Display the usage text
 */
void GetHelp(NodePtr& activeNode, stringstream& command) {
    cout << "help                   Display this help message" << endl;
    cout << "exit                   Quit the application" << endl;
    cout << endl;
    cout << "  Flat View" << endl;
    cout << "-------------" << endl;
    cout << "table [max]                (t) Print the flat table for the full program" << endl;
    cout << "searchtable <regex>        (st) Filter the flat table by a regular expression" << endl;
    cout << "widetable [max] [regex]    (wt) Print the flat table for the full program with full names" << endl;
    cout << "ls [max] [depth=1] [regex] Create a local flat view" << endl; 
    cout << endl;
    cout << "  Searching" << endl;
    cout << "-------------" << endl;
    cout << "search <name>              (s) All calls to function <name>" << endl;
    cout << "searchroot <regex>         (sr) Search child nodes of root for children matching <regex>" << endl;
    cout << "searchchildren <regex>     (sc) Search child nodes for children matching <regex>" << endl;
    cout << "  next                     Go to the next search result" << endl;
    cout << "  previous                 Go to the previous search result" << endl;
    cout << "  this                     Go back to the current search result" << endl;
    cout << "  list                     List the current search results" << endl;
    cout << endl;
    cout << "  Tree Navigation" << endl;
    cout << "------------------" << endl;
    cout << "tree  [depth=5]            Print the tree for the current node" << endl;
    cout << "cd                         Jump to this node" << endl;
    cout << "..                         Go to the parent node" << endl;
    cout << "pwd                        Get the address of the current node" << endl;
}

/*
 * Callback triggered by a new search
 */
void Search(NodePtr& activeNode, stringstream& command) {
    string name = "";
    getline(command,name);

    if ( name == "" ) {
        cout << "usage: search <function name>" << endl;
    } else {
        if ( cache == nullptr ) {
            cache = new SearchCache();
            cout << "Building one time search cache.. " << endl;
            Time start;
            NodePtr ptr = rootNode;
            cache->AddTree(ptr);
            Time end;
            cout << "done" << endl;
            cout << "Search cache built in " << end.DiffSecs(start) << " seconds" << endl;
        }
        delete result;
        result = new SearchResult(cache->Search(name.substr(1)));
        AdvanceSearch(activeNode,0);
    }
}
void SearchChildren(NodePtr& activeNode, stringstream& command) {
    string pattern = "";
    getline(command,pattern);

    if ( pattern == "" ) {
        cout << "usage: searchchildren <pattern to search>" << endl;
        cout << "usage: sc <pattern to search>" << endl;
    } else {

        delete result;
        finder.Search(activeNode,pattern.substr(1));
        result = new SearchResult(finder.Results());
        AdvanceSearch(activeNode,0);
    }
}

/*
 * Jump to a search result
 *     1:  Go to the next result
 *     0:  Jump back to the current result
 *     -1: Jump back to the previous result
 */
void AdvanceSearch(NodePtr& activeNode, short direction) {
    if ( result == nullptr ) {
        cout << "Error: No active search!" << endl;
    } else {
        if ( direction > 0 ) {
            ++(*result);
        } else if ( direction < 0 ) {
            --(*result);
        }

        if ( !result->Node().IsNull() ) {
            activeNode = result->Node();
            ListSearch();
            cout << "There are " << result->Remaining() << " more results" << endl;
        } else {
            // unwind the change
            if ( direction > 0 ) {
                --(*result);
            } else if ( direction < 0 ) {
                ++(*result);
            }
            cout << "Error: No nodes found" << endl;
        }
    }
}

/*
 * List the search results
 */
void ListSearch() {
    if ( result == nullptr ) {
        cout << "Error: No active search!" << endl;
    } else {
        for ( SearchResult it(result->First()); it.Ok(); ++it) {
            NodePtr node = it.Node();
            if (  node == result->Node() ) {
                cout << " --> ";
            } else {
                cout << "     ";
            }
            cout << node->Parent()->Name() << "/" << node->Name();
            cout << " : " << node->RunTime() << " / " << node->Calls();
            cout << " (" << ( node->Calls() == 0 ? 
                                   0 : 
                                   node->RunTime() / node->Calls() 
                            ) << ")" << endl;
        }
    }
}

/*
 * Print the flat table of results.
 */
void PrintTable(NodePtr& activeNode, stringstream& command) {
    int rows = 0;
    command >> rows;
    cout << counter->PrintResults(rows) << endl;
}
void PrintWideTable(NodePtr& activeNode, stringstream& command) {
    int rows = 0;
    command >> rows;
    string pattern;
    getline(command,pattern);
    if ( pattern == "" ) {
        cout << counter->WidePrint(rows) << endl;
    } else {
        pattern = pattern.substr(1);
        cout << "Searching flat table for: '" << pattern << "'" << endl;
        cout << counter->FilteredPrint(pattern,rows) << endl;
    }
}
void PrintFilteredTable(NodePtr& activeNode, stringstream& command) {
    string pattern = "";
    getline(command,pattern);
    pattern = pattern.substr(1);
    cout << "Searching flat table for: '" << pattern << "'" << endl;
    cout << counter->FilteredPrint(pattern) << endl;
}

/*
 * Print the tree for the current node
 */
void PrintTree(NodePtr& activeNode, stringstream& command) {
    int depth = 5;
    command >> depth;
    cout << activeNode->PrintResults(2,depth,false) << endl;
}

/*
 * Show the direct descendents of the current node
 */
void LS(NodePtr& activeNode, stringstream& command) {
    int max = 0;
    int depth = 1;
    command >> max;
    command >> depth;

    string pattern = "";
    getline(command,pattern);

    if ( pattern != "" ) {
        pattern = pattern.substr(1);
    }
    cout << activeNode->Tabulate(depth,max,pattern) << endl;
}


/*
 * Jump to the parent node
 */
void GoToParent(NodePtr& activeNode, stringstream& command) {
    NodePtr parent = activeNode->Parent();
    if ( parent.IsNull() ) {
        cout << "Error: No such node!" << endl;
    } else {
        activeNode = parent;
    }
}

/*
 * Build the path string of the current node
 */
void PWD (NodePtr& activeNode) {
    string path = activeNode->Name();
    for ( NodePtr i = activeNode->Parent();
          !i.IsNull();
          i = i->Parent() ) {
        path = i->Name() + "/\n  " + path;
    }
    cout << path << endl;
}


/*
 * Jump to another node.
 *
 * ..         : Go to the parent
 * ROOT/...   : An absolute path
 * (other)    : A relative path
 */
void GoTo(NodePtr& activeNode, stringstream& command) {
    // Read the path
    std::string path_string;
    getline(command,path_string);
    path_string = path_string.substr(1);
    Path path (path_string);
    Path::PathNode rootPathNode = path.Root();

    if ( path_string == ".." ) {
        GoToParent(activeNode,command); } else { // Find the node...
        NodePtr node = nullptr;
        if ( rootPathNode.Name() == "ROOT" ) {
            node = rootNode->GetNode(rootPathNode.Next());
        } else {
            node = activeNode->GetNode(std::move(rootPathNode));
        }

        if ( !node.IsNull() ) {
            activeNode = node;
        } else {
            cout << "Error: no such node!" << endl;
        }
    }
}

void PrintAnnotation(NodePtr& activeNode) {
    if ( data == nullptr ) {
            cout << "Error: annotations are unavailable" << endl;
    } else {
        string results = data->Annotate(activeNode);
        if ( results == "" ) {
            results = "Error: annotations are unavailable for this node";
        }

        cout << results << endl;
    }
}


