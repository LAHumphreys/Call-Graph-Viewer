#include "tester.h"
#include "SourceFile.h"
#include <string>

using namespace std;

int basic(testLogger& log );
int annotate(testLogger& log );

int main(int argc, const char *argv[])
{
    Test("Print some source...",basic).RunTest();
    Test("Annotating some source...",annotate).RunTest();
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
    file.AddAnnotation(9,34235);
    file.AddAnnotation(10,342);
    file.AddAnnotation(11,999);
    string actual = file.Annotate(7,21);
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

