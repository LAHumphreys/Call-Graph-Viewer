#ifndef __PROFILER_LIBPROFDATA_NODE_H__
#define __PROFILER_LIBPROFDATA_NODE_H__
#include <string>
#include <unordered_map>
#include "path.h"
#include <sstream>

class Node;

class NodePtr {
public:
    NodePtr(Node* theNode): p (theNode) {}

    inline Node* operator->() {return p;}
    inline bool IsNull() { return p == nullptr;} 

private: 
    Node*    p;
};

/*
 * A node in the call tree. 
 *
 * The root node is identified by the fact parent is null.
 */
class Node {
public:
    // Initialise as a root node
    Node();

    virtual ~Node();

    bool IsRoot() const { return parent==nullptr; }

    // data access
    const std::string& Name() const { return name; }
    const long& CallCount() const { return callCount; }
    const long& RunTime() const { return usecs; }
    size_t NumChildren() const { return children.size(); }

    // element access
    NodePtr Parent() const { return parent; }
    NodePtr THIS() { return this;}

    /*
     * Navigate to the leaf node at the end of the path, and then add a function
     * call to <name> for duration <usecs>
     *
     * Return a pointer to the updated (or created) node.
     */
    NodePtr AddCall(Path::PathNode node,const std::string& name, long usecs);

    std::string PrintResults(unsigned indent=0);

private:
    using PAIR = std::pair<std::string,Node*>;
    // Populata sorted Node with the element in desceding order of time
    void SortByTime(std::vector<PAIR>& sortedNode);

    void PrintResults(unsigned int indent, std::stringstream& s);

    std::string PrintInfo(unsigned int indent);

    // Element Modification
    NodePtr AddCall(const std::string& name, long usecs);

    // Find the child node with name, or return null
    NodePtr GetChild(const std::string& name);

    // Used to initialise as a child node
    Node(const std::string& name, Node* parent, long _usecs);

    long                                 callCount;
    long                                 usecs;
    std::string                          name;
    Node*                                parent;
    std::unordered_map<std::string,
                       Node*>            children;
};


#endif
