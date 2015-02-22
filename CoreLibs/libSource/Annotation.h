#ifndef __PROFILER_LIBSOURCE_ANNOTATION
#define __PROFILER_LIBSOURCE_ANNOTATION

#include <unordered_map>
#include "stringStruct.h"


class Annotation {
public:
    Annotation();
    ~Annotation();
    typedef StringStruct Line;
    using Lines = std::unordered_map<int,Line>;

    void AddAnnotation(const int& lineno, const Line& line);

    const Line& CheckLine(const int& lineno) const;

    const int& Start() const { return start;}
    const int& Stop() const { return stop;}

    long Sum() const;

private:
    Line& NullLine() const;
    int   start;
    int   stop;
    Line& GetLine(const int& lineno);
    Lines annotations;
    mutable Line*  nullLine;
   
};

#endif
