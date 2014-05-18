#include "node.h"
#include "logger.h"
#include <algorithm>
#include <iomanip>
#include <climits>

using namespace std;

const int INDENT_TAB_SIZE = 4;

// Root node
Node::Node()
    : sourceId(-1),
      sourceStart(INT_MAX),
      sourceEnd(-1),
      callCount(0), 
      usecs(0), 
      name("ROOT"), 
      parent(nullptr)
{
}

Node::Node(const std::string& _name, Node* _parent, long _usecs)
    : sourceId(-1),
      sourceStart(INT_MAX),
      sourceEnd(-1),
      callCount(0), 
      usecs(_usecs), 
      name(_name), 
      parent(_parent)
{
}

Node::~Node() {
    for ( auto& p: children ) {
        delete p.second;
    }
}

void Node::AddCall(long _usecs, int count) {
    callCount += count;
    usecs += _usecs;
}

// Update the node <name> with a call for <usecs>
NodePtr Node::AddCall(const std::string& name, 
                      long usecs)
{
    NodePtr child = GetChild(name);

    if ( !child.IsNull() ) {
        // Child exists - update it.
        ++child->callCount;
        child->usecs += usecs;
    } else {
        // No such child - create it
        Node* node = new Node(name,this,usecs);
        node->callCount = 1;
        children.emplace(std::string(name),
                         node);

        // The node to be returned to the caller
        child = node;
    }
    return child;
}

NodePtr Node::AddCall(Path::PathNode node,const string& name, long usecs) {
    NodePtr child = nullptr;
    if ( node.IsEnd() ) {

        // Reached the end of the path - add the call details
        child = AddCall(name,usecs);
    } else {

        // We need to go down the tree - check if the node alredy exists...
        child = GetChild(node.Name());
        if ( !child.IsNull() ) {

            // it does! - recurse into it
            child = child->AddCall(node.Next(),name,usecs);
        } else {

            // No such node - create it 
            Node* newNode = new Node(node.Name(), this, 0);
            children.emplace(node.Name(),
                             newNode);

            // Now recurse into the new node ...
            child = newNode->AddCall(node.Next(),name,usecs);
        }
    }
    return child;
}

// Make sure the child exists
NodePtr Node::MakeChild(const std::string& name) {
    NodePtr child = GetChild(name);
    if ( child.IsNull() ) {
        Node *node = new Node(name, this, 0);
        children.emplace(name, node);
        child = node;
    }
    return child;
}
// Find the node with name <name>
NodePtr Node::GetChild(const string& name ) {

    // Default to the null node
    NodePtr child = nullptr;

    // If it exists, update the pointer
    auto it = children.find(name);
    if ( it != children.end() ) {
        child = it->second;
    }

    return child;
}

std::string Node::PrintResults(unsigned int indent,
                               unsigned int depth,
                               bool printPath) 
{
    std::stringstream results("");
    PrintResults(indent,depth,"",printPath,results);
    return results.str();
}

void Node::PrintResults(unsigned int indent, 
                        unsigned int depth, 
                        const string& path,
                        bool printPath,
                        stringstream& s) {
    string myPath = ( path == "" ? 
                         name : 
                         path + "/" + name);
    // First add our info line
    s << PrintInfo(indent,myPath,printPath);

    // Now add child nodes...
    indent+=INDENT_TAB_SIZE;

    vector<PAIR> sortedNodes;
    SortByTime(sortedNodes);

    if ( depth != 0 ) {
        --depth;
        for (auto& pair: sortedNodes ) {
            pair.second->PrintResults(indent,depth,myPath,printPath,s);
        }
    }
}

std::string Node::PrintInfo(unsigned int indent,
                            const std::string& path,
                            bool printPath) {
    stringstream s("");
    s << setw(indent) << "" << name;
    if ( printPath ) { 
        s << " (" << path << ")" << endl;
    } else {
        s << endl;
    }
    indent+=INDENT_TAB_SIZE;
    s << setw(indent) << "" << "Calls: " << callCount;
    s << ", Time: " << usecs << ", Av. Time: " << (callCount == 0 ? 0: usecs/callCount) << endl;
    return s.str();
}

void Node::SortByTime(vector<PAIR>& sortedNodes) {
    sortedNodes.empty();
    sortedNodes.resize(children.size());

    partial_sort_copy(children.begin(),children.end(),
                      sortedNodes.begin(),sortedNodes.end(),
                      [] (const PAIR& lhs, const PAIR& rhs) -> bool {
                          return lhs.second->usecs>rhs.second->usecs;
                      });

}

NodePtr Node::GetNode(const Path& path) {
    return GetNode(path.Root());
}

NodePtr Node::GetNode(Path::PathNode&& pathNode) {
    NodePtr node = nullptr;

    if ( pathNode.IsEnd() ) {
        node = this;
    } else {
        node = GetChild(pathNode.Name());
        if ( !node.IsNull() ) {
            node = node->GetNode(pathNode.Next());
        } else {
            SLOG_FROM(LOG_WARNING, "Node::GetNode",
               "No such node '" << pathNode.Name() << "' in node '" 
                    << name << "'" << endl;)
        }
    } 
    return node;
}

NodePtr Node::CreateNode( const Path& path,
                          const string& name)
{
    NodePtr node = AddCall(path.Root(), name, 0);
    node->callCount = 0;
    return node;
}

void Node::AddChildren(CallCount& counter, int depth) {
    counter.AddCall(name,usecs,callCount);
    --depth;
    if ( depth > 0 ) {
        ForEach([=, &counter] (NodePtr node) -> void {
            node->AddChildren(counter,depth);
        });
    }
}

string Node::Tabulate( int depth, 
                       unsigned tableSize,
                       const string& filter ) {
    CallCount counter;

    ForEach([=, &counter] (NodePtr node) -> void {
        node->AddChildren(counter,depth);
    });

    stringstream output;

    output << PrintInfo(0,"",false);
    output << endl;
    output << endl;

    if ( filter == "" ) {
        output << counter.WidePrint(tableSize);
    } else {
        output << counter.FilteredPrint(filter,tableSize);
    }

    return output.str();
}
