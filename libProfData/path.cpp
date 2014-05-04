#include "path.h"

using namespace std;

Path::Path(const std::string& path) {
    size_t i=-1, j;
    for ( j=path.find_first_of("/");
          j != std::string::npos;
          j = path.find_first_of("/",j+1))
    {
        this->path.push_back(path.substr(i+1,(j-i-1)));
        // Finally store the old value
        i = j;
    }
    if ( i != (path.length()-1) ) {
        this->path.push_back(path.substr(i+1,path.length()-i-1));
    }
}
Path::Path(const std::vector<std::string>::const_iterator& beg, 
           const std::vector<std::string>::const_iterator& end)
    : path(beg,end)
{
}

Path::Path(const Path& rhs) 
    :path(rhs.path)
{
}

Path::Path(Path&& rhs) 
    :path(std::move(rhs.path))
{
}

Path& Path::operator=(Path&& rhs) {
    path = std::move(rhs.path);
    return *this;
}
Path::Path(const Path& rhs, const std::string& next) 
    :path(rhs.path)
{
    this->path.push_back(next);
}

Path::PathNode Path::Root() const {
    return PathNode(path.begin(),path);
}

Path Path::PathNode::GetPath() const {
    return Path(path.begin(),current+1);
}

