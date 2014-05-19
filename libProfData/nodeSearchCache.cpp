#include "nodeSearchCache.h"
#include <iterator>
#include "logger.h"
#include <functional>

using namespace std;

NodePtr SearchResult::Node() {
    if ( it < v.end() && it >= v.begin() ) {
        return *it;
    } else {
        return nullptr;
    }
}

size_t SearchResult::Remaining() {
    size_t d = std::distance(it,v.end());
    // Don't count ourselves... but don't return -1 if there are no results!
    if ( d > 0 ) {
        --d;
    }
    return d;
}

SearchCache::SearchCache() {
    theCache.reserve(1000);
    // Empty vector...
    theCache["__NULL__"];
}

void SearchCache::AddTree(NodePtr& node ) {
    AddNode(node);
    node->ForEach([=] ( NodePtr&& node ) -> void {
        this->AddTree(node); });
}


SearchResult SearchCache::Search ( const std::string& name) {
    auto it = theCache.find(name);
    if ( it == theCache.end() ) {
        it = theCache.find("__NULL__");
    }
    return SearchResult(it->second.begin(),it->second);
}

RegSearch::RegSearch() : regPattern(nullptr) {
}

void RegSearch::AddTree(NodePtr& node ) {
    if ( regPattern && boost::regex_search(node->Name(),*regPattern) ) {
        this->AddNode(node);
    } else {
        SLOG_FROM(LOG_VERBOSE,"RegSearch::Search",
           "Rejected Node: " << node->Name() )
    }
    node->ForEach([=] ( NodePtr&& node ) -> void {
        this->AddTree(node); });
}

size_t RegSearch::Search(NodePtr root, const string& pattern) {
    nodes.clear();
    size_t results = 0;
    try {
        boost::regex thePattern(pattern);
        regPattern = &thePattern;

        root->ForEach([=] ( NodePtr&& node ) -> void {
            this->AddTree(node); });
    } catch ( boost::regex_error& e ) {
        SLOG_FROM(LOG_ERROR, "RegSearch::Search",
            "Invalid Regex: " << e.what()) 
        results = 0;
    } 

    regPattern = nullptr;
    return results;
}
