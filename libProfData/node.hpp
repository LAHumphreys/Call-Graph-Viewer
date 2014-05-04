template <class F>
void Node::ForEach(F f) {
    for ( auto& p : children ) {
        f(NodePtr(p.second));
    }
}
