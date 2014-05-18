#ifndef __PROFILER_LIBPROFDATA_PATH_H__
#define __PROFILER_LIBPROFDATA_PATH_H__

#include <vector>
#include <string>


/*
 * Utility class representing a path into the call graph.
 * 
 * The PathNode accessor is used to talk the path.
 */

class Path {
public:
    /*
     * Initialise the object with a '/' separated route.
     *  e.g "main/test"
     */
    Path(const std::string& path);

    /*
     * Initialise the object as a copy of another path
     */
    Path(const Path& rhs);
    Path(Path&& rhs);

    Path& operator=(Path&& rhs);
    Path& operator=(const Path& rhs);

    /*
     * Initialise the object as a copy of another path,
     * but with a single extra node on the end - next;
     */
    Path(const Path& rhs, const std::string& next);

    /*
     * Extend the path
     */
    void Extend(const std::string& next ) {
        path.push_back(next);
    }

    /*
     * Used to take a splice of a path
     */
    Path(const std::vector<std::string>::const_iterator& beg, 
         const std::vector<std::string>::const_iterator& end);

    /*
     * This is the iterator for walking the call graph
     */
    class PathNode {
    public:
        PathNode(
            std::vector<std::string>::const_iterator c,
            const std::vector<std::string>& v)
          : current(c) ,path(v) {}

        // Return a Path object terminating at this node
        Path GetPath() const;

        std::string Name() const { return *current; }
        PathNode Previous() const {return PathNode(current-1,path);}
        PathNode Next() const { return PathNode(current+1,path);}

        PathNode& operator++() { ++current; return *this;}
        PathNode& operator--() { --current; return *this;}

        bool IsEnd() const { return current==path.end();}
        bool IsBegining() const { return current==path.begin();}
    private:
        // data
        std::vector<std::string>::const_iterator current;
        const std::vector<std::string>& path;
    };

    /*
     * Return the number of items in this path
     */
    size_t Length() const { 
        return path.size();
    }

    /*
     * Return a node pointer for walking the tree
     */
    PathNode Root() const;

private:
    std::vector<std::string> path;
};
#endif
