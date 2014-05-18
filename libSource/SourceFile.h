#include <vector>
#include <string>
#include <set>
#include "Annotation.h"


class SourceFile {
public:
    SourceFile(const std::string& fname);

    std::string Print(size_t start, size_t end);

    std::string Annotate(int start, int end);

    void AddAnnotation(const int& lineno, const long& cost) {
        annotations.AddAnnotation(lineno,cost);
    }

    void Initialise();
private:
    Annotation             annotations;
    std::string            fname;
    bool                   initialised;
    std::vector<std::string> lines;
};
