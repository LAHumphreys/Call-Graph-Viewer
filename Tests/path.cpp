#include "tester.h"
#include "path.h"

using namespace std;

int FromString(testLogger& log);
int ToString(testLogger& log);
int Copy(testLogger& log);
int TrailingSlash(testLogger& log);
int Extend(testLogger& log);
int SplicingPath(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Creating Path From String..",FromString).RunTest(); 
    Test("Creating string from path",ToString).RunTest();
    Test("Creating Path From String, testing trailing '/'..",TrailingSlash).RunTest(); 
    Test("Creating Path From Copy..",Copy).RunTest(); 
    Test("Creating Path with extension...",Extend).RunTest(); 
    Test("Creating Path from subpath...",SplicingPath).RunTest(); 
    return 0;
}

const std::string refPath = "main/TestFunc/OtherFunc";

int CheckRefPath(testLogger& log, const Path& path) {
    if ( path.Length() != 3 ) {
        log << "Invalid path length: " << path.Length();
        return 1;
    }
    auto currentNode = path.Root();

    if (currentNode.Name() != "main") {
        log << "Invalid root node name: " << currentNode.Name() << endl;;
        return 1;
    }

    if (currentNode.Next().Previous().Name() != "main") {
        log << "Invalid (returned) root node name: " << currentNode.Next().Previous().Name() << endl;;
        return 1;
    }

    if (currentNode.Next().Name() != "TestFunc") {
        log << "Invalid name for next node: " << currentNode.Next().Name() << endl;;
        return 1;
    }

    if (currentNode.Next().Next().Name() != "OtherFunc") {
        log << "Invalid name for other node: " << currentNode.Next().Next().Name() << endl;;
        return 1;
    }

    if (currentNode.Name() != "main") {
        log << "The iterator has been moved!" << currentNode.Name() << endl;;
        return 1;
    }

    // Walk to the end of the path
    while ( !currentNode.Next().IsEnd() ) {
        ++currentNode;
    }

    if ( currentNode.Name() != "OtherFunc" ) {
        log << "Error: Should be at OtherFunc! - " << currentNode.Name();
    }

    // Walk back to the start
    while ( !currentNode.IsBegining() ) {
        --currentNode;
    }

    if (currentNode.Name() != "main") {
        log << "Not at the fron of the path!" << currentNode.Name() << endl;;
        return 1;
    }
    return 0;
}

int FromString(testLogger& log) {
    Path path(refPath);
    return CheckRefPath(log,path);
}

int ToString(testLogger& log) {
    Path path(refPath);
    Path newPath(path.ToString());

    if (refPath != path.ToString()) {
        log << "Unexpected string for path: " << path.ToString();
        return 0;
    }
    return CheckRefPath(log,newPath);
}

int TrailingSlash(testLogger& log) {
    Path path("main/TestFunc/OtherFunc");
    return CheckRefPath(log,path);
}

int Copy(testLogger& log) {
    Path path(refPath);
    Path path2(path);
    return CheckRefPath(log,path2);
}

int Extend(testLogger& log) {
    Path path("main/TestFunc");
    Path path2(path,"OtherFunc");
    return CheckRefPath(log,path2);
}

int SplicingPath(testLogger& log) {
    Path path("main/TestFunc/OtherFunc/AnotherCall/AndAnother");
    auto rootNode = path.Root();
    auto otherNode = rootNode.Next().Next();
    return CheckRefPath(log,otherNode.GetPath());
}
