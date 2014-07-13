
#ifndef __PROFILER_LIBEXEC_CONFIG_FILE_H__
#define __PROFILER_LIBEXEC_CONFIG_FILE_H__
#include <string>
#include <vector>
#include <fstream>

class CommandScript {
public:

    CommandScript(std::istream& input);
    CommandScript(const std::string script);
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
protected:
    CommandScript();
    void Initialise(std::istream& input);
    bool                       ok;
private: 
    /*
     * Load the next command from file into the output
     * buffer. 
     *
     * Return a bool indicating if a command was found
     */
    bool GetNextCommand(std::string& buf, std::istream& input);

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
};

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
class CommandFile: public CommandScript  { 
public:
    /*
     * Read the config file
     */
    CommandFile(const std::string& fname);

    bool Ok() const {
        return file.good();
    }
private:
     std::fstream file;

};

#endif
