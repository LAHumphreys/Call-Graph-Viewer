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

Path::Path(const std::vector<std::string>::const_reverse_iterator& beg,
           const std::vector<std::string>::const_reverse_iterator& end)
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

std::string Path::ToString() const {
    static std::string pwd;
    pwd.clear();

    bool first = true;
    for ( PathNode n = Root(); !n.IsEnd(); ++n) {
        if ( first ) {
            first = false;
            pwd = n.Name();
        } else {
            pwd += "/" + n.Name();
        }
    }

    return pwd;
}

Path& Path::operator=(Path&& rhs) {
    path = std::move(rhs.path);
    return *this;
}

Path& Path::operator=(const Path& rhs) {
    path = rhs.path;
    return *this;
}

Path::Path(const Path& rhs, const std::string& next) 
    :path(rhs.path)
{
    Extend(next);
}

Path::PathNode Path::Root() const {
    return PathNode(path.begin(),path);
}

Path Path::PathNode::GetPath() const {
    return Path(path.begin(),current+1);
}

void Path::GetPath(std::vector<std::string>& dest) {
    dest = path;
}
