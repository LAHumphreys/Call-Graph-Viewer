#ifndef __PROFILER_LIBSOURCE_SOURCE_FILE_H__
#define __PROFILER_LIBSOURCE_SOURCE_FILE_H__

#include <vector>
#include <string>
#include <set>
#include "Annotation.h"


class SourceFile {
public:
    SourceFile(const std::string& fname);

    std::string Print(size_t start, size_t end);

    std::string Annotate(const Annotation& annotations, 
                         int start, 
                         int end);

    void Initialise();

    std::string Name() {
        return fname;
    }
private:
    std::string            fname;
    bool                   initialised;
    std::vector<std::string> lines;
};

#endif
