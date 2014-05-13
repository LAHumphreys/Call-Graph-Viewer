#include "tester.h"
#include "node.h"
#include "nodeSearchCache.h"
#include "callgrindTree.h"

int RootNode(testLogger& log);
int AddNodes(testLogger& log);
int CheckResults(testLogger& log);
int CheckShortResults(testLogger& log);
int PathAccess(testLogger& log);
int Search(testLogger& log);
int CheckRegSearch(testLogger& log);
int MakeNode(testLogger& log);
int EmptyCallgrindData(testLogger& log);
int CallgrindTree(testLogger& log);
int CallgrindTable(testLogger& log);
int LS(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Checking root node...",RootNode).RunTest();
    Test("Adding some nodes...",AddNodes).RunTest();
    Test("Checking path retrieval...",PathAccess).RunTest();
    Test("Printing Results...",CheckResults).RunTest();
    Test("Printing Short Results...",CheckShortResults).RunTest();
    Test("Seaching the graph",Search).RunTest();
    Test("Seaching the graph with regex",CheckRegSearch).RunTest();
    Test("Testing child creation",MakeNode).RunTest();
    Test("Loading an empty callggrind tree",EmptyCallgrindData).RunTest();
    Test("Loading a full callgrind tree",CallgrindTree).RunTest();
    Test("Loading a full callgrind tree and table",CallgrindTable).RunTest();
    Test("Tabulating Children (LS)...",LS).RunTest();
    return 0;
}

int CheckNode(testLogger& log,
              NodePtr node,
              bool isRoot,
              string name,
              long   count,
              long   time,
              size_t children) 
{
    log << "Checking " << node->Name() << " against " << name << endl;
    if ( isRoot != node->IsRoot() ) {
        log << "Node has incorrect root setting" << endl;
        return 1;
    }

    if ( node->Name() != name ) {
        log << "Unexpected Name!" << endl;
        log << "Expected: " << name << endl;
        log << "Actual : " << node->Name() << endl;
        return 1;
    }

    if ( node->Calls() != count ) {
        log << "Unexpected Call Count!" << endl;
        log << "Expected: " << count << endl;
        log << "Actual : " << node->Calls() << endl;
        return 1;
    }

    if ( node->RunTime() != time ) {
        log << "Unepxect Run time!" << endl;
        log << "Expected: " << time << endl;
        log << "Actual : " << node->RunTime() << endl;
        return 1;
    }
    if  ( node->NumChildren() != children ) {
        log << "Unepxect ## of children!" << endl;
        log << "Expected: " << children << endl;
        log << "Actual : " << node->NumChildren() << endl;
        return 1;
    }
    return 0;
}

int RootNode(testLogger& log) {
    Node rootNode;

    if  ( !rootNode.Parent().IsNull() ) {
        log << "Root's parent is not null!" << endl;
        return 1;
    }

    return CheckNode(log,rootNode.THIS(),true,  // isRoot
                                  "ROOT", // name
                                  0,      // count
                                  0,      // time
                                  0);     // children
}

int AddNodes(testLogger& log) {
    Node rootNode;
    Path rootPath("");
    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add F2 and F3 to F1
    NodePtr f2 = rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    NodePtr f3_f1 = rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    NodePtr f3_main = rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    // Add F1 to main
    NodePtr f1 = rootNode.AddCall(mainPath.Root(),"f1",101);

    // Add main to root
    NodePtr main = rootNode.AddCall(rootPath.Root(),"main",100);


    int ok = CheckNode(log,main, false,  // isRoot
                                 "main", // name
                                 1,      // count
                                 100,    // time
                                 2);     // children
    if ( ok == 0 ) {
        ok = CheckNode(log,f1, false,  // isRoot
                               "f1", // name
                               2,      // count
                               202,    // time
                               2);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,f2, false,  // isRoot
                               "f2", // name
                               2,      // count
                               204,    // time
                               0);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,f3_f1, false,  // isRoot
                               "f3", // name
                               1,      // count
                               103,    // time
                               0);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,f3_main, false,  // isRoot
                               "f3", // name
                               2,      // count
                               206,    // time
                               0);     // children
    }

    /*
     * Finally check the root node...
     */
    if ( ok == 0 ) {
        ok = CheckNode(log,main->Parent(), 
                               true,   // isRoot
                               "ROOT", // name
                               0,      // count
                               0,      // time
                               1);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,rootNode.THIS(), 
                               true,   // isRoot
                               "ROOT", // name
                               0,      // count
                               0,      // time
                               1);     // children
    }
    return ok;
}

int CheckResults(testLogger& log) {
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    string actual = rootNode.PrintResults();
    string expected = 
       "ROOT (ROOT)\n"
       "    Calls: 0, Time: 0, Av. Time: 0\n"
       "    main (ROOT/main)\n"
       "        Calls: 1, Time: 100, Av. Time: 100\n"
       "        f3 (ROOT/main/f3)\n"
       "            Calls: 2, Time: 206, Av. Time: 103\n"
       "        f1 (ROOT/main/f1)\n"
       "            Calls: 2, Time: 202, Av. Time: 101\n"
       "            f2 (ROOT/main/f1/f2)\n"
       "                Calls: 2, Time: 204, Av. Time: 102\n"
       "            f3 (ROOT/main/f1/f3)\n"
       "                Calls: 1, Time: 103, Av. Time: 103\n";

    if ( expected != actual ) {
       log << expected <<endl;
       log << "------------------------" << endl;
       log << actual <<endl;
       return 1;
    }

    return 0;
}

int CheckShortResults(testLogger& log) {
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    NodePtr mainNode = rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    string actual = mainNode->PrintResults(0,1);
    string expected = 
       "main (main)\n"
       "    Calls: 1, Time: 100, Av. Time: 100\n"
       "    f3 (main/f3)\n"
       "        Calls: 2, Time: 206, Av. Time: 103\n"
       "    f1 (main/f1)\n"
       "        Calls: 2, Time: 202, Av. Time: 101\n";

    if ( expected != actual ) {
       log << expected <<endl;
       log << "------------------------" << endl;
       log << actual <<endl;
       return 1;
    }

    return 0;
}

int PathAccess(testLogger& log) {
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    Path fakePath("main/f1/f2/notHere");

    NodePtr f1 = rootNode.GetNode(f1path);

    if ( f1.IsNull() ) {
        log << "f1 is a null node!" << endl;
        return 1;
    }

    int ok = CheckNode(log,f1, false,  // isRoot
                               "f1", // name
                               2,      // count
                               202,    // time
                               2);     // children
    return ok;
}

int Search(testLogger& log) {
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    SearchCache cache;
    NodePtr rootPtr = rootNode.THIS();

    cache.AddTree(rootPtr);

    SearchResult result = cache.Search("XXX");

    if ( result.Ok() ) {
        log << "Search returned ok from a stupid search! " << endl;
        return 1;
    }

    if ( result.Remaining() != 0 ) {
        log << "Search returned non-zero from a stupid search! " << endl;
        log << result.Remaining();
        return 1;
    }

    if ( !result.Node().IsNull() ) {
        log << "Result is not null from a stupid search!" << endl;
        return 1;
    }

    SearchResult f3_result = cache.Search("f3");

    if ( f3_result.Remaining() != 1 ) {
        log << "Expected only one more result!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Node()->Parent()->Name() != "main" ) {
        log << "I thought the frist result would be main :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Node()->Parent()->Name() != "f1" ) {
        log << "I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Previous().Node()->Parent()->Name() != "main" ) {
        log << "It doesn't semm to live back!" << endl;
        log << f3_result.Next().Previous().Node()->Parent()->Name() << endl;
        return 1;
    }

    ++f3_result;

    if ( f3_result.Node()->Parent()->Name() != "f1" ) {
        log << "I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Remaining() != 0 ) {
        log << "Search returned non-zero when at the end of the search!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( !f3_result.Ok() ) {
        log << "Search not ok on the last element!" << endl;
        return 1;
    }

    --f3_result;

    if ( f3_result.Remaining() != 1 ) {
        log << "--Expected only one more result!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Node()->Parent()->Name() != "main" ) {
        log << "--I thought the frist result would be main :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Node()->Parent()->Name() != "f1" ) {
        log << "--I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    ++f3_result;
    ++f3_result;

    if ( f3_result.Remaining() != 0 ) {
        log << "Search returned non-zero when past the end of a search" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Ok() ) {
        log << "Search returned ok when past the end of a search" << endl;
        return 1;
    }


    if ( !f3_result.Node().IsNull() ) {
        log << "Result is not null when past the end of the search!" << endl;
        return 1;
    }

    return 0;
}

int MakeNode (testLogger& log ) {
    Node rootNode;
    NodePtr apple = rootNode.MakeChild("apple");
    NodePtr apple2 = rootNode.MakeChild("apple");

    if ( apple.IsNull() || apple2.IsNull() )  {
        log << "MakeChild returned null!" << endl;
        return 1;
    }

    if ( apple->Name() != "apple" || apple2->Name() != "apple" ) {
        log << "MakeChild did not add the correct ndoe!" << endl;
        return 1;
    }

    if ( rootNode.NumChildren() != 1 ) {
        log << "MakeCHild did not create exactly one child" << endl;
    }

    NodePtr pair = apple->Parent()->MakeChild("pair");

    if ( pair.IsNull() || pair->Name() != "pair" ) {
        log << "Make child did not add pair! " << endl;
        return 1;
    }

    if ( rootNode.NumChildren() != 2 ) {
        log << "MakeChild did not create a new child!" << endl;
    }


    return 0;
}

int EmptyCallgrindData ( testLogger& log ) {
    CallgrindCallTree data("data/flist.csv");
    string expected = 
        "ROOT (ROOT)\n"
        "    Calls: 0, Time: 0, Av. Time: 0\n"
        "    main (ROOT/main)\n"
        "        Calls: 0, Time: 0, Av. Time: 0\n"
        "        odds (ROOT/main/odds)\n"
        "            Calls: 0, Time: 0, Av. Time: 0\n"
        "            div (ROOT/main/odds/div)\n"
        "                Calls: 0, Time: 0, Av. Time: 0\n"
        "        evens (ROOT/main/evens)\n"
        "            Calls: 0, Time: 0, Av. Time: 0\n"
        "            div (ROOT/main/evens/div)\n"
        "                Calls: 0, Time: 0, Av. Time: 0\n"
        "                pos_div2 (ROOT/main/evens/div/pos_div2)\n"
        "                    Calls: 0, Time: 0, Av. Time: 0\n"
        "                pos_div4 (ROOT/main/evens/div/pos_div4)\n"
        "                    Calls: 0, Time: 0, Av. Time: 0\n";

    string actual = data.RootNode()->PrintResults();

    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int CallgrindTree ( testLogger& log ) {
    CallgrindCallTree data("data/flist.csv");
    data.LoadCalls("data/calls.csv");
    string expected = 
        "ROOT (ROOT)\n" 
        "    Calls: 0, Time: 0, Av. Time: 0\n" 
        "    main (ROOT/main)\n" 
        "        Calls: 0, Time: 0, Av. Time: 0\n" 
        "        odds (ROOT/main/odds)\n" 
        "            Calls: 1, Time: 75, Av. Time: 75\n" 
        "            div (ROOT/main/odds/div)\n" 
        "                Calls: 3, Time: 45, Av. Time: 15\n" 
        "        evens (ROOT/main/evens)\n" 
        "            Calls: 1, Time: 65, Av. Time: 65\n" 
        "            div (ROOT/main/evens/div)\n" 
        "                Calls: 2, Time: 42, Av. Time: 21\n" 
        "                pos_div2 (ROOT/main/evens/div/pos_div2)\n" 
        "                    Calls: 1, Time: 7, Av. Time: 7\n" 
        "                pos_div4 (ROOT/main/evens/div/pos_div4)\n" 
        "                    Calls: 1, Time: 7, Av. Time: 7\n";
    string actual = data.RootNode()->PrintResults();

    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int CallgrindTable ( testLogger& log ) {
    CallgrindCallTree data("data/flist.csv");
    data.LoadCalls("data/calls.csv");
    data.LoadCosts("data/cost.csv");
    string expected = 
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Time Spent in Function                                                             -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time(us)   | us/call   |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| main                                                                  | 0      | 152        | 0         |\n"
"| div                                                                   | 5      | 87         | 17        |\n"
"| odds                                                                  | 1      | 75         | 75        |\n"
"| evens                                                                 | 1      | 65         | 65        |\n"
"| pos_div2                                                              | 1      | 7          | 7         |\n"
"| pos_div4                                                              | 1      | 7          | 7         |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Expensive Function Calls                                                           -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time(us)   | us/call   |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| odds                                                                  | 1      | 75         | 75        |\n"
"| evens                                                                 | 1      | 65         | 65        |\n"
"| div                                                                   | 5      | 87         | 17        |\n"
"| pos_div2                                                              | 1      | 7          | 7         |\n"
"| pos_div4                                                              | 1      | 7          | 7         |\n"
"| main                                                                  | 0      | 152        | 0         |\n"
"-----------------------------------------------------------------------------------------------------------\n";
    string actual = data.Counter().PrintResults();

    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int CheckRegSearch(testLogger& log) {
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    NodePtr rootPtr = rootNode.THIS();

    RegSearch finder;
    finder.Search(rootPtr, "(X|.*");
    SearchResult result = finder.Results();

    if ( result.Ok() ) {
        log << "Search returned ok from a stupid search! " << endl;
        return 1;
    }

    if ( result.Remaining() != 0 ) {
        log << "Search returned non-zero from a stupid search! " << endl;
        log << result.Remaining();
        return 1;
    }

    if ( !result.Node().IsNull() ) {
        log << "Result is not null from a stupid search!" << endl;
        return 1;
    }

    finder.Search(rootPtr,"^f3$");
    SearchResult f3_result = finder.Results();

    if ( f3_result.Remaining() != 1 ) {
        log << "Expected only one more result!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Node()->Parent()->Name() != "main" ) {
        log << "I thought the frist result would be main :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Node()->Parent()->Name() != "f1" ) {
        log << "I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Previous().Node()->Parent()->Name() != "main" ) {
        log << "It doesn't semm to live back!" << endl;
        log << f3_result.Next().Previous().Node()->Parent()->Name() << endl;
        return 1;
    }

    ++f3_result;

    if ( f3_result.Node()->Parent()->Name() != "f1" ) {
        log << "I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Remaining() != 0 ) {
        log << "Search returned non-zero when at the end of the search!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( !f3_result.Ok() ) {
        log << "Search not ok on the last element!" << endl;
        return 1;
    }

    --f3_result;

    if ( f3_result.Remaining() != 1 ) {
        log << "--Expected only one more result!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Node()->Parent()->Name() != "main" ) {
        log << "--I thought the frist result would be main :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Node()->Parent()->Name() != "f1" ) {
        log << "--I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    ++f3_result;
    ++f3_result;

    if ( f3_result.Remaining() != 0 ) {
        log << "Search returned non-zero when past the end of a search" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Ok() ) {
        log << "Search returned ok when past the end of a search" << endl;
        return 1;
    }


    if ( !f3_result.Node().IsNull() ) {
        log << "Result is not null when past the end of the search!" << endl;
        return 1;
    }


    return 0;
}

int LS(testLogger& log) {
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (3x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    NodePtr mainNode = rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);
    rootNode.AddCall(mainPath.Root(),"f1",101);

    string actual = mainNode->Tabulate();
    string actual2 = mainNode->Tabulate(2);
    string expected2 = 
        "main\n"
        "    Calls: 1, Time: 100, Av. Time: 100\n"
        "\n"
        "\n"
        "                 Most Time Spent in Function\n"
        "               ===============================\n"
        "  Calls      Time(us)      us/call        Name\n"
        "---------  -----------   -------------  --------\n"
        " 3          309           103            f3\n"
        " 3          303           101            f1\n"
        " 2          204           102            f2\n"
        "\n"
        "\n"
        "                 Most Expensive Function Calls\n"
        "               =================================\n"
        "  Calls      Time(us)      us/call        Name\n"
        "---------  -----------   -------------  --------\n"
        " 3          309           103            f3\n"
        " 2          204           102            f2\n"
        " 3          303           101            f1\n";

    string expected = 
        "main\n"
        "    Calls: 1, Time: 100, Av. Time: 100\n"
        "\n"
        "\n"
        "                 Most Time Spent in Function\n"
        "               ===============================\n"
        "  Calls      Time(us)      us/call        Name\n"
        "---------  -----------   -------------  --------\n"
        " 3          303           101            f1\n"
        " 2          206           103            f3\n"
        "\n"
        "\n"
        "                 Most Expensive Function Calls\n"
        "               =================================\n"
        "  Calls      Time(us)      us/call        Name\n"
        "---------  -----------   -------------  --------\n"
        " 2          206           103            f3\n"
        " 3          303           101            f1\n";
    if ( expected != actual ) {
       log << "ls 1 failfed! " << endl;
       log << expected <<endl;
       log << "------------------------" << endl;
       log << actual <<endl;
       return 1;
    }

    if ( expected2 != actual2 ) {
       log << mainNode->PrintResults() << endl;
       log << "ls 2 failfed! " << endl;
       log << expected2 <<endl;
       log << "------------------------" << endl;
       log << actual2 <<endl;
       return 1;
    }


    return 0;
}
