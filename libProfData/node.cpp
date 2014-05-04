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
    : callCount(1), 
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

NodePtr Node::AddCall(const std::string& name, 
                      long usecs)
{
    auto it = children.find(name);

    Node* node = nullptr; 

    if ( it != children.end() ) {
        node = it->second;
        ++(it->second->callCount);
        it->second->usecs += usecs;
    } else {
        node = new Node(name,this,usecs);
        children.emplace(std::string(name),
                         node);
    }
    return node;
}

NodePtr Node::AddCall(Path::PathNode node,const string& name, long usecs) {
    NodePtr child = nullptr;
    if ( node.IsEnd() ) {
        child = AddCall(name,usecs);
    } else {
        child = GetChild(node.Name());
        if ( !child.IsNull() ) {
            child = child->AddCall(node.Next(),name,usecs);
        } else {
            SLOG_FROM(LOG_ERROR,"Node::AddCall",
                "Failed to add function call because node " << this->name
                    << " does not have a node called " << node.Name())
        }
    }
    return child;
}

NodePtr Node::GetChild(const string& name ) {
    auto it = children.find(name);
    NodePtr child = nullptr;
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
