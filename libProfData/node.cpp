#include "node.h"
#include "logger.h"
#include <algorithm>
#include <iomanip>

using namespace std;

const int INDENT_TAB_SIZE = 4;

// Root node
Node::Node()
    : callCount(0), 
      usecs(0), 
      name("ROOT"), 
      parent(nullptr)
{
}

Node::Node(const std::string& _name, Node* _parent, long _usecs)
    : callCount(0), 
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

std::string Node::PrintResults(unsigned int indent) {
    std::stringstream results("");
    PrintResults(indent,results);
    return results.str();
}

void Node::PrintResults(unsigned int indent, stringstream& s) {
    // First add our info line
    s << PrintInfo(indent);

    // Now add child nodes...
    indent+=INDENT_TAB_SIZE;

    vector<PAIR> sortedNodes;
    SortByTime(sortedNodes);

    for (auto& pair: sortedNodes ) {
        pair.second->PrintResults(indent,s);
    }
}

std::string Node::PrintInfo(unsigned int indent) {
    stringstream s("");
    s << setw(indent) << "" << name << endl;
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
