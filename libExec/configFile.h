
#ifndef __PROFILER_LIBEXEC_CONFIG_FILE_H__
#define __PROFILER_LIBEXEC_CONFIG_FILE_H__
#include <string>
#include <vector>
#include <fstream>

/*
 * A very simplistic parser for basic config files,
 * each line in which is a command.
 *
 * A comment char of // is supported, but it must start 
 * the line:
 *
 * // ------ Config Start --------
 * // Initialise the side bar
 * sidebar annotate
 *
 * // Start in the main function
 * searchroot main
 * // ------ Config End --------
 *
 */
class CommandFile { 
public:
    /*
     * Read the config file
     */
    CommandFile(const std::string& fname);

    /*
     * The list of commands configured
     */
    const std::vector<std::string>& Commands() const {
        return commands;
    }

    /*
     * Check the command file initialised ok
     */
    bool IsOk() const {
        return ok;
    }
private: 
    /*
     * Load the next command from file into the output
     * buffer. 
     *
     * Return a bool indicating if a command was found
     */
    bool GetNextCommand(std::string& buf);

    /*
     * Return a bool indicating if the supplied line is a valid
     * command
     */
    bool LineIsCommand(const std::string& line);

    /*
     * Return a bool indicating if the supplied line is a comment
     */
    bool LineIsComment(const std::string& line);

    /*
     * Data
     */
    std::vector<std::string>   commands;
    std::ifstream              file;
    bool                       ok;
};

#endif
