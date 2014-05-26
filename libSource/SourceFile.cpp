#include "SourceFile.h"
#include <fstream>
#include <sstream>
#include "logger.h"
#include <iomanip>

using namespace std;

SourceFile::SourceFile(const string& _fname ) 
    : fname(_fname), initialised(false)
{
}

string SourceFile::Print(size_t start, size_t end) {
    stringstream output;
    if ( !initialised ) {
        Initialise();
    }
    
    if ( start <= end && end < lines.size() ) {
        for ( size_t i = start; i < end; ++i ) {
            output << setw(5) << i+1 << ": " << lines[i+1] << endl;
        }
    } else {
        SLOG_FROM(LOG_ERROR,"SourceFile::Print",
                      "Invalid Index: " << start << " -> " << end << endl
                      << "Size of lines: " << lines.size())
    }

    return output.str();
}

string SourceFile::Annotate(const Annotation& annotations,
                            int start, 
                            int stop,
                            long total,
                            int threshold) 
{
    stringstream output;

    if ( !initialised ) {
        Initialise();
    }
    
    if ( stop  >=  static_cast<int>(lines.size()) ) {
        stop = lines.size()-1;
    }

    if ( total != 0 ) {
        // Calculate the real total...
        total =  annotations.Sum();
    }

    if ( start > 0 && start <= stop ) {
        for ( int i = start; i <= stop; ++i ) {
            output << setw(5) << i << ": ";
            const auto& l = annotations.CheckLine(i);
            if ( l.cost != 0 ) { 
                output << setw(10) << l.cost;
            } else {
                output << setw(10) << "";
            }
            if (total != 0 ) {
                int pcnt = ( l.cost*100 / total);
                if ( pcnt < threshold ) {
                    output << " (" << setw(2) <<  pcnt << "%)";
                } else {
                    output << " !" << setw(2) <<  pcnt << "%!";
                }
            }
            output << ": " << lines[i-1] << endl;
        }
    } else {
        SLOG_FROM(LOG_ERROR,"SourceFile::Print",
                      "Invalid Index: " << start << " -> " << stop << endl
                      << "Size of lines: " << lines.size())
    }

    return output.str();
}

void SourceFile::Initialise() {
    ifstream file(fname);

    while ( file.good() ) {
        string line;
        std::getline(file,line);
        if ( line.length() > 80 ) {
            lines.emplace_back(line.substr(0,77) + "...");
        } else {
            lines.emplace_back(std::move(line));
        }
    }

    initialised = true;
    file.close();
}
