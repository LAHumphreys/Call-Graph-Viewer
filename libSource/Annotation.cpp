#include "Annotation.h"
#include <climits>

Annotation::Annotation() 
    : start(INT_MAX), stop(0)
{
}

void Annotation::AddAnnotation(const int& lineno, const long& cost) {
    GetLine(lineno).cost += cost;

    if ( lineno < start ) {
        start = lineno;
    } else if ( lineno > stop ) {
        stop  = lineno;
    }
}


Annotation::Line& Annotation::GetLine(const int& lineno) {
    return annotations[lineno];
}

Annotation::Line Annotation::CheckLine(const int& lineno) const {
    auto it = annotations.find(lineno);
    if ( it != annotations.end() ) {
        return it->second;
    } else {
        return Line();
    }
}

long Annotation::Sum() const {
    long total = 0;
    for ( auto& p : annotations ) {
        total += p.second.cost;
    }
    return total;
}
