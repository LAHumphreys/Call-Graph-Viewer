#ifndef __PROFILER_LIBPROFDATA_NODE_SEARCH_CACHE_H__
#define __PROFILER_LIBPROFDATA_NODE_SEARCH_CACHE_H__
#include <vector>
#include "node.h"
#include <string>
#include <boost/regex.hpp>

class SearchResult {
public:
    using NodeList = std::vector<NodePtr>; 
    SearchResult(NodeList::const_iterator _it,
                 const NodeList& _v): it(_it), v(_v) {}

    
    NodePtr Node();

    SearchResult& operator++() { 
        ++it; 
        return *this;
    }
    SearchResult& operator--() { 
        --it; 
        return *this;
    }

    SearchResult  First() { 
        return SearchResult(v.begin(),v); 
    }

    SearchResult  Next() { 
        return SearchResult(it+1,v); 
    }

    SearchResult  Previous() { 
        return SearchResult(it-1,v); 
    }

    bool Ok () { 
        return it < v.end() && it >= v.begin(); 
    }

    size_t Remaining ();

private:
    NodeList::const_iterator it;
    const  NodeList& v;
};

class RegSearch {
public:
    using NodeList = std::vector<NodePtr>; 

    RegSearch();

    void AddTree(NodePtr& node);

    size_t Search(NodePtr root, const std::string& pattern);

    SearchResult Results() const {
        return SearchResult(nodes.begin(), nodes);
    }

    // Add a single node to the cache
    void AddNode(NodePtr& node) {
        nodes.emplace_back(node);
    }

private:
    boost::regex* regPattern;
    NodeList nodes;
};

class SearchCache {
    using NodeList = std::vector<NodePtr>; 
public:
    SearchCache();

    // Add a single node to the cache
    void AddNode(NodePtr& node) {
        theCache[node->Name()].push_back(node);
    }

    // Add a node, and all of its descedents to the cache
    void AddTree(NodePtr& node);

    // Search for entries in the tree
    SearchResult Search ( const std::string& name);

private:
    std::unordered_map<std::string,NodeList>   theCache;
};

#endif
