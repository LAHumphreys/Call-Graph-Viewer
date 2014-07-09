#include "tester.h"
#include "configFile.h"

int LoadValidConfig(testLogger& log);
int LoadNonFile(testLogger& log);
int LoadEmptyFile(testLogger& log);

int main(int argc, const char *arg[]) {
    Test("Loading a non existent file",LoadNonFile).RunTest();
    Test("Loading an empty file",LoadEmptyFile).RunTest();
    Test("Loading a valid file",LoadValidConfig).RunTest();
    return 0;
}

int LoadNonFile(testLogger& log ) {
    CommandFile commands("data/NotAFile.cfg");

    if ( commands.Commands().size() != 0 ) {
        log << "Non existent file did not return no commands!";
        return 1;
    }

    if ( commands.IsOk() ) {
        log << "Non existent file was marked as OK!";
        return 2;
    }
    return 0;
}

int LoadEmptyFile(testLogger& log ) {
    CommandFile commands("data/empty.cfg");

    if ( commands.Commands().size() != 0 ) {
        log << "Empty file did not return no commands!";
        return 1;
    }

    if ( !commands.IsOk() ) {
        log << "Empty file was not marked as OK!";
        return 2;
    }
    return 0;
}

int LoadValidConfig(testLogger& log ) {
    CommandFile commands("data/example.cfg");

    if ( commands.Commands().size() != 3 ) {
        log << "Command file should have had 3 commands, actually " 
            << commands.Commands().size();
        return 1;
    }
    std::string expected[] = {
     "command1 argument1 argument2",
     "command2 argument1 argument2",
     "command2 argument1 // argument2",
    };

    for ( int i =0; i<3; i++ ) {
        if ( commands.Commands()[i] != expected[i] ) {
            log << "Command missmatch for command " << i << endl;
            log << "Expected: >" << expected[i] << "<" << endl;
            log << "Actual: >" << commands.Commands()[i] << "<" << endl;
            return 2;
        }
    }
    return 0;
}
