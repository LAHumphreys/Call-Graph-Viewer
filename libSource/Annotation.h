#ifndef __PROFILER_LIBSOURCE_ANNOTATION
#define __PROFILER_LIBSOURCE_ANNOTATION

#include <unordered_map>


class Annotation {
public:
    Annotation();
    struct Line {
        long cost;
    };
    using Lines = std::unordered_map<int,Line>;

    void AddAnnotation(const int& lineno, const long& cost);

    Line CheckLine(const int& lineno) const;

    const int& Start() const { return start;}
    const int& Stop() const { return stop;}

    long Sum() const;

private:
    int   start;
    int   stop;
    Line& GetLine(const int& lineno);
    Lines    annotations;
   
};

#endif
