#include "logger.h"
#include "buildCallGraph.h"
#include <iostream>
#include <sstream>
#include <string>
#include "util_time.h"
#include "nodeSearchCache.h"
#include "callgrindTree.h"
#include "screen.h"
#include <cstdlib>

/*
 * Helper macro to print to terminal
 */
#define COUT(line) \
    {   \
        stringstream __buf; \
        __buf << line; \
        term.PutString(__buf.str()); \
    }   \

using namespace std;

CallgrindNative* data = nullptr;
CallProfile profile;
CallCount* counter = nullptr;
SearchCache* cache = nullptr;
SearchResult* result = nullptr;
RegSearch  finder;

NodePtr rootNode = nullptr;

struct BarProperties {
    string  command = "";
    bool    showing = false;
    bool    locked = false;
} topbarProps, sidebarProbs;

// Utils
string GetCommand(Terminal& term, NodePtr& activeNode);
void GetHelp(Terminal& term, NodePtr& activeNode, stringstream& command);
void Clear(Terminal& term, NodePtr& activeNode);
void DoCommand(Terminal& term, NodePtr& node, stringstream& command, string action = "");

// Searching
void AdvanceSearch(Terminal& term, NodePtr& activeNode, short direction);
void ListSearch(Terminal& term);
void Search(Terminal& term, NodePtr& activeNode, stringstream& command);
void SearchChildren(Terminal& term, NodePtr& activeNode, stringstream& command);

// Navigation
void GoTo(Terminal& term, NodePtr& activeNode, stringstream& command);
void GoToParent(Terminal& term, NodePtr& activeNode, stringstream& command);
void LS(Terminal& term, NodePtr& activeNode, stringstream& command);
void PWD (Terminal& term, NodePtr& activeNode);

// Information
void PrintFilteredTable(Terminal& term, NodePtr& activeNode, stringstream& command);
void PrintTable(Terminal& term, NodePtr& activeNode, stringstream& command);
void PrintTree(Terminal& term, NodePtr& activeNode, stringstream& command);
void PrintWideTable(Terminal& term, NodePtr& activeNode, stringstream& command);
void PrintAnnotation(Terminal& term, NodePtr& activeNode);

// Windows
void DoTopCommand(NodePtr& node, stringstream& command);
void DoSideCommand(NodePtr& node, stringstream& command);

int main(int argc, const char *argv[])
{
    NodePtr activeNode = nullptr;

    Terminal& term = Screen::Instance().MainTerminal();

    if ( argc != 2 && argc != 4) {
        COUT ( "Usage: profile <data file>" << endl)
        COUT ( "Usage: profile <flist file> <calls file> <cost file>" << endl)
        return 1;
    }

    LOG_FROM(LOG_OVERVIEW, "main", "Building Profile...")
    Screen::Instance().MainTerminal().PutString("Building call graph...");
    Time start;

    data = new CallgrindNative(argv[1]);

    Time end;
    Screen::Instance().MainTerminal().PutString("done\n");
    Screen::Instance().MainTerminal().PutString("done\n");
    COUT ( "Process started up in " << end.DiffUSecs(start)/1000 << " mili-seconds" << endl;)
    LOG_FROM(LOG_OVERVIEW, "main", "Profile built - program ready!")

    // start at the top of the tree
    rootNode = data->RootNode();
    counter = &data->Counter();

    // start at the top of the tree
    activeNode = rootNode;

    /*
     * Main Loop
     */
    NodePtr oldNode = activeNode;
    while ( 1 ) {
        stringstream command(GetCommand(term, activeNode));
        DoCommand(term, activeNode,command);
        if ( ! (activeNode==oldNode) ) {
            if ( topbarProps.showing && !topbarProps.locked ) {
                stringstream command(topbarProps.command);
                DoTopCommand(activeNode,command);
            }
            if ( sidebarProbs.showing && !sidebarProbs.locked ) {
                stringstream command(sidebarProbs.command);
                DoSideCommand(activeNode,command);
            }
        }
    }
    return 0;
}

void DoTopCommand(NodePtr& node, stringstream& command) {
    std::string action;
    command >> action;
    topbarProps.showing = true;
    // Ignore an additional topbar instruction...
    while ( action == "topbar"  || action == "to" ) {
        action.clear();
        command >> action;
    }
    SLOG_FROM(LOG_OVERVIEW, "DoTopCommand", "Got action: '" << action 
                                       << "' from command '" << command.str() << "'")
    // Clear the topbar
    if ( action == "height" ) {
        int lines = 0;
        command >> lines;
        if ( lines != 0 ) {
            Screen::Instance().SetTopBarHeight(lines);
        }
    } else if ( action == "lock" ) {
        topbarProps.locked = true;
    } else if ( action == "unlock" ) {
        topbarProps.locked = false;
    } else {
        Screen::Instance().TopBar().Clear();
        // Execute the command in the topbar
        DoCommand(Screen::Instance().TopBar(),node,command, action);
        topbarProps.command = command.str();
    }
}

void DoSideCommand(NodePtr& node, stringstream& command) {
    std::string action;
    command >> action;
    sidebarProbs.showing = true;
    // Ignore an additional topbar instruction...
    while ( action == "sidebar"  || action == "si") {
        action.clear();
        command >> action;
    }
    SLOG_FROM(LOG_OVERVIEW, "DoSideCommand", "Got action: '" << action 
                                       << "' from command '" << command.str() << "'")
    // Clear the topbar
    if ( action == "width" ) {
        int cols = 0;
        command >> cols;
        if ( cols != 0 ) {
            Screen::Instance().SetSideBarWidth(cols);
        }
    } else if ( action == "lock" ) {
        sidebarProbs.locked = true;
    } else if ( action == "unlock" ) {
        sidebarProbs.locked = false;
    } else {
        Screen::Instance().SideBar().Clear();
        // Execute the command in the topbar
        DoCommand(Screen::Instance().SideBar(),node,command, action);
        sidebarProbs.command = command.str();
    }
}

void DoCommand(Terminal& term, NodePtr& activeNode, stringstream& command, string action) {

    if ( action == "" ) {
        command >> action;
    }

    SLOG_FROM(LOG_OVERVIEW, "DoCommand", "Got action: '" << action 
                                       << "' from command '" << command.str() << "'")

    if ( action == "topbar" || action == "to" ) {
        DoTopCommand(activeNode, command);
    } else if ( action == "sidebar" || action == "si" ) {
        DoSideCommand(activeNode, command);
    } else {
        if ( action == "help" ) {
            GetHelp(term, activeNode, command);
        } else if ( action == "exit" ) {
            exit(0);
        } else if ( action == "table" || action == "t" ) {
            PrintTable(term, activeNode, command);
        } else if ( action == "widetable" || action == "wt" ) {
            PrintWideTable(term, activeNode, command);
        } else if ( action == "searchtable" || action == "st" ) {
            PrintFilteredTable(term, activeNode, command);
        } else if ( action == "tree" ) {
            PrintTree(term, activeNode, command);
        } else if ( action == "cd" ) {
            GoTo(term, activeNode, command);
        } else if ( action == ".." ) {
            GoToParent(term, activeNode, command);
        } else if ( action == "pwd" ) {
            PWD(term, activeNode);
        } else if ( action == "ls" ) {
            LS(term, activeNode, command);
        } else if ( action == "clear" ) {
            Clear(term, activeNode);
        } else if ( action == "search"  || action == "s") {
            Search(term, activeNode, command);
        } else if ( action == "searchchildren" || action == "sc" ) {
            SearchChildren(term, activeNode, command);
        } else if ( action == "searchroot" || action == "sr" ) {
            activeNode = rootNode;
            SearchChildren(term, activeNode, command);
        } else if ( action == "this" ) {
            AdvanceSearch(term, activeNode, 0);
        } else if ( action == "previous" || action == "p" ) {
            AdvanceSearch(term, activeNode, -1);
        } else if ( action == "next"  || action == "n") {
            AdvanceSearch(term, activeNode, 1);
        } else if ( action == "list" ) {
            ListSearch(term);
        } else if ( action == "annotate" || action == "a" ) {
            PrintAnnotation(term, activeNode);
        } else {
            COUT ( "Unknown command!" << endl;)
            GetHelp(term, activeNode,command);
        }
    }
}

/*
 * Get the next command from the user ( a single line of text)
 */
string GetCommand(Terminal& term, NodePtr& activeNode) {
    string name = activeNode->Name();
    string shortName = name.substr(0,name.find("("));
    return Screen::Instance().MainTerminal().GetLine("|" + shortName + "> ");
}

/*
 * Display the usage text
 */
void GetHelp(Terminal& term, NodePtr& activeNode, stringstream& command) {
    stringstream helptext;
    helptext << "help                   Display this help message" << endl;
    helptext << "clear                  Clear the display" << endl;
    helptext << "exit                   Quit the application" << endl;
    helptext << endl;
    helptext << "  Flat View" << endl;
    helptext << "-------------" << endl;
    helptext << "table [max]                (t) Print the flat table for the full program" << endl;
    helptext << "searchtable <regex>        (st) Filter the flat table by a regular expression" << endl;
    helptext << "widetable [max] [regex]    (wt) Print the flat table for the full program with full names" << endl;
    helptext << "ls [max] [depth=1] [regex] Create a local flat view" << endl; 
    helptext << endl;
    helptext << "  Source Code" << endl;
    helptext << "---------------" << endl;
    helptext << "annotate                   (a) Print and annotate the source for thie function" << endl;
    helptext << endl;
    helptext << "  Searching" << endl;
    helptext << "-------------" << endl;
    helptext << "search <name>              (s) All calls to function <name>" << endl;
    helptext << "searchroot <regex>         (sr) Search child nodes of root for children matching <regex>" << endl;
    helptext << "searchchildren <regex>     (sc) Search child nodes for children matching <regex>" << endl;
    helptext << "  next                     Go to the next search result" << endl;
    helptext << "  previous                 Go to the previous search result" << endl;
    helptext << "  this                     Go back to the current search result" << endl;
    helptext << "  list                     List the current search results" << endl;
    helptext << endl;
    helptext << "  Tree Navigation" << endl;
    helptext << "------------------" << endl;
    helptext << "tree  [depth=5]            Print the tree for the current node" << endl;
    helptext << "cd                         Jump to this child node" << endl;
    helptext << "..                         Go to the parent node" << endl;
    helptext << "pwd                        Get the address of the current node" << endl;
    helptext << endl;
    helptext << "  Windows " << endl;
    helptext << "-----------" << endl;
    helptext << "topbar <cmd>               (to) Run <cmd> in the topbar on node change" << endl;
    helptext << "topbar lock                (to) Lock the contents of the topbar" << endl;
    helptext << "topbar unlock              (to) Start running commands in the topbar again" << endl;
    helptext << "topbar height              (to) Adjust the height of the topbar" << endl;
    helptext << "sidebar <cmd>              (si) Run <cmd> in the sidebar on node change" << endl;
    helptext << "sidebar lock               (si) Lock the contents of the sidebar" << endl;
    helptext << "sidebar unlock             (si) Start running commands in the sidebar again" << endl;
    helptext << "sidebar width <cols>       (si) Adjust the width of the topbar" << endl;
    helptext << endl;
    Screen::Instance().MainTerminal().PutString(helptext.str());
}
void Clear(Terminal& term, NodePtr& activeNode) {
    Screen::Instance().MainTerminal().Clear();
}

/*
 * Callback triggered by a new search
 */
void Search(Terminal& term, NodePtr& activeNode, stringstream& command) {
    string name = "";
    getline(command,name);

    if ( name == "" ) {
        COUT ( "usage: search <function name>" << endl;)
    } else {
        if ( cache == nullptr ) {
            cache = new SearchCache();
            COUT ( "Building one time search cache.. " << endl;)
            Time start;
            NodePtr ptr = rootNode;
            cache->AddTree(ptr);
            Time end;
            COUT ( "done" << endl;)
            COUT ( "Search cache built in " << end.DiffSecs(start) << " seconds" << endl;)
        }
        delete result;
        result = new SearchResult(cache->Search(name.substr(1)));
        AdvanceSearch(term, activeNode,0);
    }
}
void SearchChildren(Terminal& term, NodePtr& activeNode, stringstream& command) {
    string pattern = "";
    getline(command,pattern);

    if ( pattern == "" ) {
        COUT ( "usage: searchchildren <pattern to search>" << endl;)
        COUT ( "usage: sc <pattern to search>" << endl;)
    } else {

        delete result;
        finder.Search(activeNode,pattern.substr(1));
        result = new SearchResult(finder.Results());
        AdvanceSearch(term, activeNode,0);
    }
}

/*
 * Jump to a search result
 *     1:  Go to the next result
 *     0:  Jump back to the current result
 *     -1: Jump back to the previous result
 */
void AdvanceSearch(Terminal& term, NodePtr& activeNode, short direction) {
    if ( result == nullptr ) {
        COUT ( "Error: No active search!" << endl;)
    } else {
        if ( direction > 0 ) {
            ++(*result);
        } else if ( direction < 0 ) {
            --(*result);
        }

        if ( !result->Node().IsNull() ) {
            activeNode = result->Node();
            ListSearch(term);
            COUT ( "There are " << result->Remaining() << " more results" << endl;)
        } else {
            // unwind the change
            if ( direction > 0 ) {
                --(*result);
            } else if ( direction < 0 ) {
                ++(*result);
            }
            COUT ( "Error: No nodes found" << endl;)
        }
    }
}

/*
 * List the search results
 */
void ListSearch(Terminal& term) {
    if ( result == nullptr ) {
        COUT ( "Error: No active search!" << endl;)
    } else {
        for ( SearchResult it(result->First()); it.Ok(); ++it) {
            NodePtr node = it.Node();
            if (  node == result->Node() ) {
                COUT ( " --> ";)
            } else {
                COUT ( "     ";)
            }
            COUT ( node->Parent()->Name() << "/" << node->Name();)
            COUT ( " : " << node->RunTime() << " / " << node->Calls();)
            COUT ( " (" << ( node->Calls() == 0 ? 
                                   0 : 
                                   node->RunTime() / node->Calls() 
                            ) << ")" << endl);
        }
    }
}

/*
 * Print the flat table of results.
 */
void PrintTable(Terminal& term, NodePtr& activeNode, stringstream& command) {
    int rows = 0;
    command >> rows;
    COUT ( counter->PrintResults(rows) << endl;)
}
void PrintWideTable(Terminal& term, NodePtr& activeNode, stringstream& command) {
    int rows = 0;
    command >> rows;
    string pattern;
    getline(command,pattern);
    if ( pattern == "" ) {
        COUT ( counter->WidePrint(rows) << endl;)
    } else {
        pattern = pattern.substr(1);
        COUT ( "Searching flat table for: '" << pattern << "'" << endl;)
        COUT ( counter->FilteredPrint(pattern,rows) << endl;)
    }
}
void PrintFilteredTable(Terminal& term, NodePtr& activeNode, stringstream& command) {
    string pattern = "";
    getline(command,pattern);
    pattern = pattern.substr(1);
    COUT ( "Searching flat table for: '" << pattern << "'" << endl;)
    COUT ( counter->FilteredPrint(pattern) << endl;)
}

/*
 * Print the tree for the current node
 */
void PrintTree(Terminal& term, NodePtr& activeNode, stringstream& command) {
    int depth = 5;
    command >> depth;
    COUT ( activeNode->PrintResults(2,depth,false) << endl;)
}

/*
 * Show the direct descendents of the current node
 */
void LS(Terminal& term, NodePtr& activeNode, stringstream& command) {
    int max = 0;
    int depth = 1;
    command >> max;
    command >> depth;

    string pattern = "";
    getline(command,pattern);

    if ( pattern != "" ) {
        pattern = pattern.substr(1);
    }
    COUT ( activeNode->Tabulate(depth,max,pattern) << endl;)
}


/*
 * Jump to the parent node
 */
void GoToParent(Terminal& term, NodePtr& activeNode, stringstream& command) {
    NodePtr parent = activeNode->Parent();
    if ( parent.IsNull() ) {
        COUT ( "Error: No such node!" << endl;)
    } else {
        activeNode = parent;
    }
}

/*
 * Build the path string of the current node
 */
void PWD (Terminal& term, NodePtr& activeNode) {
    string path = activeNode->Name();
    for ( NodePtr i = activeNode->Parent();
          !i.IsNull();
          i = i->Parent() ) {
        path = i->Name() + "/\n  " + path;
    }
    COUT ( path << endl;)
}


/*
 * Jump to another node.
 *
 * ..         : Go to the parent
 * ROOT/...   : An absolute path
 * (other)    : A relative path
 */
void GoTo(Terminal& term, NodePtr& activeNode, stringstream& command) {
    // Read the path
    std::string path_string;
    getline(command,path_string);
    path_string = path_string.substr(1);
    Path path (path_string);
    Path::PathNode rootPathNode = path.Root();

    if ( path_string == ".." ) {
        GoToParent(term, activeNode,command); 
    } else { // Find the node...
        NodePtr node = nullptr;
        if ( rootPathNode.Name() == "ROOT" ) {
            node = rootNode->GetNode(rootPathNode.Next());
        } else {
            node = activeNode->GetNode(std::move(rootPathNode));
        }

        if ( !node.IsNull() ) {
            activeNode = node;
        } else {
            COUT ( "Error: no such node!" << endl;)
        }
    }
}

void PrintAnnotation(Terminal& term, NodePtr& activeNode) {
    if ( data == nullptr ) {
            COUT ( "Error: annotations are unavailable" << endl;)
    } else {
        string results = data->Annotate(activeNode);
        if ( results == "" ) {
            results = "Error: annotations are unavailable for this node";
        }

        COUT ( results << endl;)
    }
}


