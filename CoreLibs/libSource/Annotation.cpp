#include "Annotation.h"
#include <climits>
#include "nodeConfig.h"

Annotation::Annotation() 
    : start(INT_MAX), 
      stop(0), 
      nullLine(nullptr)
{
}

Annotation::~Annotation() {
    delete nullLine;
}

void Annotation::AddAnnotation(const int& lineno, const Line& line) {
    GetLine(lineno) += line;
    if ( lineno < start ) {
        start = lineno;
    } else if ( lineno > stop ) {
        stop  = lineno;
    }
}


Annotation::Line& Annotation::GetLine(const int& lineno) {
    auto it  = annotations.find(lineno);
    if ( it == annotations.end() ) {
        auto t = annotations.emplace(
            lineno,
            NodeConfig::Instance().CostFactory().New()
        );
        it = t.first;
    }
    return it->second;
}

const Annotation::Line& Annotation::CheckLine(const int& lineno) const {
    auto it = annotations.find(lineno);
    if ( it != annotations.end() ) {
        return it->second;
    } else {
        return NullLine();
    }
}

long Annotation::Sum() const {
    long total = 0;
    for ( auto& p : annotations ) {
        total += p.second[0];
    }
    return total;
}

Annotation::Line& Annotation::NullLine() const {
    if ( nullLine == nullptr ) {
        nullLine = NodeConfig::Instance().NewCost();
    }
    return *nullLine;
}
