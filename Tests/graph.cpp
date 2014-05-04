#include "tester.h"
#include "node.h"

int RootNode(testLogger& log);
int AddNodes(testLogger& log);
int CheckResults(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Checking root node...",RootNode).RunTest();
    Test("Adding some nodes...",AddNodes).RunTest();
    Test("Printing Results...",CheckResults).RunTest();
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

    if ( node->CallCount() != count ) {
        log << "Unexpected Call Count!" << endl;
        log << "Expected: " << count << endl;
        log << "Actual : " << node->CallCount() << endl;
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

    // Add main to root
    NodePtr main = rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main
    NodePtr f1 = rootNode.AddCall(mainPath.Root(),"f1",101);

    // Add F2 and F3 to F1
    NodePtr f2 = rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    NodePtr f3_f1 = rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    NodePtr f3_main = rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);


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
       "ROOT\n"
       "    Calls: 0, Time: 0, Av. Time: 0\n"
       "    main\n"
       "        Calls: 1, Time: 100, Av. Time: 100\n"
       "        f3\n"
       "            Calls: 2, Time: 206, Av. Time: 103\n"
       "        f1\n"
       "            Calls: 2, Time: 202, Av. Time: 101\n"
       "            f2\n"
       "                Calls: 2, Time: 204, Av. Time: 102\n"
       "            f3\n"
       "                Calls: 1, Time: 103, Av. Time: 103\n";

    if ( expected != actual ) {
       log << expected <<endl;
       log << "------------------------" << endl;
       log << actual <<endl;
       return 1;
    }

    return 0;
}
