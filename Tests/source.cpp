#include "tester.h"
#include "SourceFile.h"
#include <string>
#include "callgrindTree.h"

using namespace std;

int basic(testLogger& log );
int annotate(testLogger& log );
int load_ids(testLogger& log);
int annotate_native(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Print some source...",basic).RunTest();
    Test("Annotating some source...",annotate).RunTest();
    Test("Checking file names...",load_ids).RunTest();
    Test("Annotating Source...",annotate_native).RunTest();
    return 0;
}

int basic (testLogger& log ) {
    SourceFile file("data/graph.cpp");
    string actual = file.Print(5,20);

    string expected = 
"    6: int RootNode(testLogger& log);\n"
"    7: int AddNodes(testLogger& log);\n"
"    8: int CheckResults(testLogger& log);\n"
"    9: int CheckShortResults(testLogger& log);\n"
"   10: int PathAccess(testLogger& log);\n"
"   11: int Search(testLogger& log);\n"
"   12: int CheckRegSearch(testLogger& log);\n"
"   13: int MakeNode(testLogger& log);\n"
"   14: int EmptyCallgrindData(testLogger& log);\n"
"   15: int EmptyCallgrindData_Native(testLogger& log);\n"
"   16: int CallgrindTree(testLogger& log);\n"
"   17: int CallgrindTree_Native(testLogger& log);\n"
"   18: int CallgrindTable(testLogger& log);\n"
"   19: int CallgrindTable_Native(testLogger& log);\n"
"   20: int LS(testLogger& log);\n";

    if ( actual != expected ) {
       log << "Invalid source file!" << endl;
       log << "Expected: " << expected <<  endl;
       log << "Actual: " << actual <<  endl;
        return 1;
    }
    return 0;
}

int annotate (testLogger& log ) {
    SourceFile file("data/graph.cpp");
    Annotation a;
    a.AddAnnotation(9,34235);
    a.AddAnnotation(10,342);
    a.AddAnnotation(11,999);
    string actual = file.Annotate(a,7,21);
    string expected = 
"    7:           : int RootNode(testLogger& log);\n"
"    8:           : int AddNodes(testLogger& log);\n"
"    9:      34235: int CheckResults(testLogger& log);\n"
"   10:        342: int CheckShortResults(testLogger& log);\n"
"   11:        999: int PathAccess(testLogger& log);\n"
"   12:           : int Search(testLogger& log);\n"
"   13:           : int CheckRegSearch(testLogger& log);\n"
"   14:           : int MakeNode(testLogger& log);\n"
"   15:           : int EmptyCallgrindData(testLogger& log);\n"
"   16:           : int EmptyCallgrindData_Native(testLogger& log);\n"
"   17:           : int CallgrindTree(testLogger& log);\n"
"   18:           : int CallgrindTree_Native(testLogger& log);\n"
"   19:           : int CallgrindTable(testLogger& log);\n"
"   20:           : int CallgrindTable_Native(testLogger& log);\n"
"   21:           : int LS(testLogger& log);\n";

    if ( actual != expected ) {
       log << "Invalid source file!" << endl;
       log << "Expected: >" << expected <<  "<" << endl;
       log << "Actual: " << ">" << actual <<  "<" << endl;
        return 1;
    }
    return 0;
}

int load_ids(testLogger& log ) {
    CallgrindNative native("data/native/flist_files.callgrind");
    NodePtr div = native.RootNode()->GetNode(Path("main/evens/div"));

    if ( div.IsNull() ) {
        log << " Failed to build tree " << endl;
        log << native.RootNode()->PrintResults() << endl;
        return 1;
    }
    if ( div->SourceId() != 194 ) {
        log << "div doesn't live in 194!:  " << div->SourceId() << endl;
        return 1;
    }

    if ( native.GetFile(div->SourceId()).Name() != "data/graph2.cpp" ) {
        log << "Main doesn't live in the correct file:  >" << native.GetFile(div->SourceId()).Name() << "<" << endl;
        return 1;
    }
    return 0;
}

int annotate_native(testLogger& log ) {
    CallgrindNative native("data/native/flist_files.callgrind");
    NodePtr div = native.RootNode()->GetNode(Path("main/evens"));


    if ( div->SourceStart() != 25 ) {
        log << " Invalid SourceStart: " << div->SourceStart() << endl;
        return 1;
    }

    string actual = native.Annotate(div);
    string expected = 
"   22:           : \n"
"   23:           : int main(int argc, const char *argv[])\n"
"   24:           : {\n"
"   25:         63: \n"
"   26:          7:     Test(\"Checking root node...\",RootNode).RunTest();\n"
"   27:           :     Test(\"Adding some nodes...\",AddNodes).RunTest();\n"
"   28:           :     Test(\"Checking path retrieval...\",PathAccess).RunTest();\n"
"   29:           :     Test(\"Printing Results...\",CheckResults).RunTest();\n"
"   30:           :     Test(\"Printing Short Results...\",CheckShortResults).RunTest();\n"
"   31:           :     Test(\"Seaching the graph\",Search).RunTest();\n"
"   32:           :     Test(\"Seaching the graph with regex\",CheckRegSearch).RunTest();\n"
"   33:           :     Test(\"Testing child creation\",MakeNode).RunTest();\n"
"   34:           :     Test(\"Loading an empty callggrind tree\",EmptyCallgrindData).RunTest();\n"
"   35:           :     Test(\"Loading an empty callggrind tree from native file strings\",EmptyCallgrindData_Native).RunTest();\n"
"   36:           :     Test(\"Loading a full callgrind tree\",CallgrindTree).RunTest();\n"
"   37:           :     Test(\"Loading a full callgrind tree from native file strings\",CallgrindTree_Native).RunTest();\n"
"   38:           :     Test(\"Loading a full callgrind tree and table\",CallgrindTable).RunTest();\n"
"   39:           :     Test(\"Loading a full callgrind tree and table from native file strings\",CallgrindTable_Native).RunTest();\n"
"   40:          7:     Test(\"Tabulating Children (LS)...\",LS).RunTest();\n"
"   41:           :     return 0;\n"
"   42:           : }\n"
"   43:           : \n";
    if (actual != expected ) {
        log << "Annotation: " << native.Annotate(div);
        return 1;
    }

    return 0;
}
