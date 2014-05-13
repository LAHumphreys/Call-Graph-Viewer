#ifndef __PROFILER_LIBPROFDATA_NODE_H__
#define __PROFILER_LIBPROFDATA_NODE_H__
#include <string>
#include <unordered_map>
#include "path.h"
#include <sstream>
#include "callCount.h"

class Node;

class NodePtr {
public:
    NodePtr(Node* theNode): p (theNode) {}

    inline Node* operator->() {return p;}
    bool operator==(const NodePtr&rhs) { return p == rhs.p; }
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
    const long& Calls() const { return callCount; }
    const long& RunTime() const { return usecs; }
    size_t NumChildren() const { return children.size(); }

    // element access
    NodePtr Parent() const { return parent; }
    NodePtr THIS() { return this;}

    /*
     * Add call 
     */
    void AddCall(long usecs, int count = 1);

    /*
     * Navigate to the leaf node at the end of the path, and then add a function
     * call to <name> for duration <usecs>
     *
     * Return a pointer to the updated (or created) node.
     */
    NodePtr AddCall(Path::PathNode node,const std::string& name, long usecs);

    NodePtr CreateNode(const Path& path, const std::string& name);

    /*
     * Return a pointer to the child name name. 
     *
     * If the node does not exist it is created, otherwise a pointer to the
     * existing node is returned
     */
    NodePtr MakeChild(const std::string& name);

    /*
     * Navigate to the leaf node at the end of the path, and return it
     *
     * If there is no such node, the nullptr is returned
     */
    NodePtr GetNode(const Path& path);

    /*
     * Advance to the end of the path...
     */
    NodePtr GetNode(Path::PathNode&& pathNode);

    std::string PrintResults(unsigned int indent=0, 
                             unsigned int depth=10,
                             bool printPath = true);

    // Add all of our children to the counter;
    void AddChildren(CallCount& counter, int depth);

    /*
     * Return a string detailing the children of this node...
     */
    std::string Tabulate(int depth = 1);

    /*
     * Call a function of the form f(NodePtr p) for each descedent element.
     *
     * The order of calls is undefined.
     */
    template <class F>
    void ForEach(F f);

private:

    using PAIR = std::pair<std::string,Node*>;
    // Populata sorted Node with the element in desceding order of time
    void SortByTime(std::vector<PAIR>& sortedNode);

    void PrintResults(unsigned int indent, 
                      unsigned int depth, 
                      const std::string& path,
                      bool  printPath,
                      std::stringstream& s);

    /*
     * Print information about this node...
     */
    std::string PrintInfo(unsigned int indent, 
                          const std::string& path,
                          bool printPath);

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

#include "node.hpp"

#endif
