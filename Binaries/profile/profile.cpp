#include "logger.h"
#include "buildCallGraph.h"
#include <iostream>
#include <sstream>
#include <string>
#include "util_time.h"
#include "nodeSearchCache.h"

using namespace std;

CallProfile profile;
SearchCache* cache = nullptr;
SearchResult* result = nullptr;

// Utils
string GetCommand(NodePtr& activeNode);
void GetHelp(NodePtr& activeNode, stringstream& command);

// Searching
void AdvanceSearch(NodePtr& activeNode, short direction);
void ListSearch();
void Search(NodePtr& activeNode, stringstream& command);

// Navigation
void GoTo(NodePtr& activeNode, stringstream& command);
void GoToParent(NodePtr& activeNode, stringstream& command);
void LS(NodePtr& activeNode, stringstream& command);
void PWD (NodePtr& activeNode);
void PrintTable(NodePtr& activeNode, stringstream& command);
void PrintTree(NodePtr& activeNode, stringstream& command);

int main(int argc, const char *argv[])
{
    if ( argc != 2 ) {
        cout << "Usage: profile <data file>" << endl;
        cout << "Usage: profile <data file> count" << endl;
        return 1;
    }

    LOG_FROM(LOG_OVERVIEW, "main", "Building Profile...")
    cout << "Building call graph...";
    cout.flush();
    Time start;
    profile.ProcessFile(argv[1]);
    Time end;
    cout << "done" << endl;
    cout << "Process started up in " << end.DiffSecs(start) << " seconds" << endl;
    LOG_FROM(LOG_OVERVIEW, "main", "Profile built - program ready!")


    // start at the top of the tree
    NodePtr activeNode = profile.RootNode();

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
        } else if ( action == "table" ) {
            PrintTable(activeNode, command);
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
        } else if ( action == "search" ) {
            Search(activeNode, command);
        } else if ( action == "this" ) {
            AdvanceSearch(activeNode, 0);
        } else if ( action == "previous" || action == "p" ) {
            AdvanceSearch(activeNode, -1);
        } else if ( action == "next"  || action == "n") {
            AdvanceSearch(activeNode, 1);
        } else if ( action == "list" ) {
            ListSearch();
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
    cout << endl << "|" << activeNode->Name() << "> ";
    cin.getline(buf,10240);
    return string(buf);
}

/*
 * Display the usage text
 */
void GetHelp(NodePtr& activeNode, stringstream& command) {
    cout << "help              Display this help message" << endl;
    cout << "exit              Quit the application" << endl;
    cout << "table [max]       Print the flat table for the full program" << endl;
    cout << "tree  [depth=5]   Print the tree for the current node" << endl;
    cout << "ls                List the child nodes" << endl;
    cout << "cd                Jump to this node" << endl;
    cout << "..                Go to the parent node" << endl;
    cout << "pwd               Get the address of the current node" << endl;
    cout << "search <name>     All calls to function <name>" << endl;
    cout << "  next            Go to the next search result" << endl;
    cout << "  previous        Go to the previous search result" << endl;
    cout << "  this            Go back to the current search result" << endl;
    cout << "  list            List the current search results" << endl;
}

/*
 * Callback triggered by a new search
 */
void Search(NodePtr& activeNode, stringstream& command) {
    string name = "";
    command >> name;

    if ( name == "" ) {
        cout << "usage: search <function name>" << endl;
    } else {
        if ( cache == nullptr ) {
            cache = new SearchCache();
            cout << "Building one time search cache.. " << endl;
            Time start;
            NodePtr ptr = profile.RootNode();
            cache->AddTree(ptr);
            Time end;
            cout << "done" << endl;
            cout << "Search cache built in " << end.DiffSecs(start) << " seconds" << endl;
        }
        delete result;
        result = new SearchResult(cache->Search(name));
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
            cout << " : " << node->RunTime() << " / " << node->CallCount();
            cout << " (" << ( node->CallCount() == 0 ? 
                                   0 : 
                                   node->RunTime() / node->CallCount() 
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
    cout << profile.PrintResults(rows) << endl;
}

/*
 * Print the tree for the current node
 */
void PrintTree(NodePtr& activeNode, stringstream& command) {
    int depth = 5;
    command >> depth;
    cout << activeNode->PrintResults(2,depth) << endl;
}

/*
 * Show the direct descendents of the current node
 */
void LS(NodePtr& activeNode, stringstream& command) {
    cout << activeNode->PrintResults(2,1) << endl;
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
        path = i->Name() + "/" + path;
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
    command >> path_string;
    Path path (path_string);
    Path::PathNode rootNode = path.Root();

    if ( path_string == ".." ) {
        GoToParent(activeNode,command); } else { // Find the node...
        NodePtr node = nullptr;
        if ( rootNode.Name() == "ROOT" ) {
            node = profile.RootNode()->GetNode(rootNode.Next());
        } else {
            node = activeNode->GetNode(std::move(rootNode));
        }

        if ( !node.IsNull() ) {
            activeNode = node;
        } else {
            cout << "Error: no such node!" << endl;
        }
    }
}

