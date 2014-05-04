#include "nodeSearchCache.h"
#include <iterator>
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
        this->AddTree(node);
    });
}

SearchResult SearchCache::Search ( const std::string& name) {
    auto it = theCache.find(name);
    if ( it == theCache.end() ) {
        it = theCache.find("__NULL__");
    }
    return SearchResult(it->second.begin(),it->second);
}

