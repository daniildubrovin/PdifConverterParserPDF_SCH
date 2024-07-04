#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "PDIFLexer.h"
#include "PDIFParser.h"
#include "PDIFType.h"
#include "PDIFException.h"
#include "GraphElement.h"

/**
 * @brief a set of states used when processing a command in the console
 */
enum class converter_cli {
    //name of the pdf file
     input_file, 
     //name of the generated svg file
     output_file, 
     //the scaling value of the svg file
     value_scale, 
     //optional parameter. For example --force or --scale
     additional_parameters
};

/**
 * @brief **Application class**
 * @details Processing console commands and converting a pdf file to an svg file
 */
class App
{
private:
    string help_output = "\n<inputfile> [outputfile] [--scale <scale>] [--force]\nto exit: q\n\n";
    string bad_number_output = "the entered value cannot be used as a scale.\nenter --help for list of command\n\n";

    string input_file_name;  ///< name of the pdf file
    string output_file_name; ///< name of the svg file
    double scale; ///< svg file scale
    bool force; ///< force flag. If installed, there will be no request to overwrite the svg file
    bool is_scale; ///< scale flag
    converter_cli state = converter_cli::input_file; ///< current state
    string current_command_str; 
    string current_token; ///< current token
    vector<string> tokens; ///< vector storing tokens
    int token_index; ///< index of the current token
    
    /**
     * @brief **Reset parameters**
     */
    void restart_parameters();

    /**
     * @brief **Checks whether the string is a number**
     * @param value the string being checked
     * @return true if the string is a number
     * @return false if the string is not a number
     */
    bool is_digit(string value); 

    /**
     * @brief **Checks if there are still raw tokens in App::tokens.**
     * @details Checks if there are still raw tokens in App::token. If there is, updates App::token_index and App::current_token.
     * @return true if there are raw tokens
     * @return false if there are no raw tokens
     */
    bool next_token();

    /**
     * @brief **Get tokens from command line**
     * @details Get tokens from command line. The received tokens are added to the App::tokens
     * @param command_temp entered command
     */
    void get_tokens(string& command_temp);

    /**
     * @brief **Get the unknown command output**
     * @param unknown_command 
     * @return string containing an unknown command and additional information
     */
    string get_unknown_command_output(string& unknown_command);

    /**
     * @brief **Сheck overwriting output file**
     * @details Сheck overwriting output file. If the file exists, it requests the possibility of overwriting or creating a copy.
     */
    void check_overwriting_output_file();

    /**
     * @brief **Get the new copy file name**
     * @details a copy number is appended to the file name. For example "file(1).svg"
     */
    void get_new_copy_file_name();

    /**
     * @brief **Get the svg filename from PDF file name**
     * @return string name of generated svg file
     */
    string get_svg_filename();

    bool check_state_command();
    bool check_state_input_file();
    bool check_state_value_scale();
    bool check_state_additional_parameters();

    /**
     * @brief **Checking and processing the entered command**
     * @return true if the command is successfully processed
     * @return false if there are errors
     */
    bool parsing();

public:
    /**
     * @brief **Processes console commands**
     * @details the main method of the application class that starts processing the received console command. 
     * The result can be either a generated svg file or an error message on the console. If there are no command line arguments, i.e. argc = 0, 
     * then the command line interface is started
     * @param argc number of command line arguments
     * @param argv command line arguments
     */
    void run(int argc = 0, char** argv = nullptr);
};