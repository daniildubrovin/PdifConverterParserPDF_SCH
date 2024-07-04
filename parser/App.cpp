#pragma once
#include "App.h"
#include "SCHParser.h"

void App::run(int argc, char** argv) {

    bool running_with_parameters = argc > 1;

    /*while (true) {
        restart_parameters();

        if (running_with_parameters) {
            for (size_t i = 1; i < argc; i++) {
                current_command_str += argv[i];
                current_command_str += " ";
            }
        }
        else {          
            cout << "pdif2svg: ";
            getline(cin, current_command_str);
        }

        cout << current_command_str << endl;
                
        get_tokens(current_command_str);

        if (current_command_str == "q") break;
        if (current_command_str == "--help") {
            cout << help_output;
            continue;
        }

        if (parsing()) {
            try
            {
                PDIFParser parser(input_file_name);
                PDIFType pdif = parser.parse(scale);
                pdif.toSVG(output_file_name);

                cout << "a file " + output_file_name + " was added\n\n";               
            }
            catch (const PDIFException& e)
            {
                cout << "an error was detected when parsing the file " + input_file_name + " :\n";
                cout << e.what() << endl;
                break;
            }
            
        }

        if (running_with_parameters) break;
    }*/

   // SCHParser sch_parser("data/1RECT.SCH");
    //SCHParser sch_parser("data/MIDIBOY.SCH");
    //SCHParser sch_parser("data/1N4007.SYM");
    //SCHParser sch_parser("data/ISA12.SYM");
    //SCHParser sch_parser("data/KT368.PRT");
  //  SCHParser sch_parser("data/L22D.SCH");
   // SCHParser sch_parser("data/TEST.PLC");
   // SCHParser sch_parser("data/TEST04.PCB");

    //SCHParser sch_parser("data/TEST26S.SCH");
    //SCHParser sch_parser("data/L22D.SCH");
   // sch_parser.to_svg("data/1RECT.SVG", 0.1);

  //  sch_parser.clear_memory();
  //  pdif_type.SCALE = 0.01;

     SCHParser sch_parser("data/L22D.SCH", 2);
     sch_parser.to_svg("data/L22D.SVG");
     sch_parser.clear_memory();
  
     PDIFParser pdif_parser("data/L22D.PDF");
     PDIFType pdif = pdif_parser.parse();
     pdif.toSVG("data/L22D2.SVG", 2);

     int a = 5;

   // PDIFParser pdif_parser("data/L22.BAK");
   // PDIFParser pdif_parser("data/TEST26S.PDF");
  //  pdif.toSVG("data/TEST26S_PDF.SVG");
  
}

void App::restart_parameters() {
    input_file_name = "";
    output_file_name = "";
    force = false;
    is_scale = false;
    scale = 2.0;
    token_index = 0;
    current_token = "";
    current_command_str = "";
    state = converter_cli::input_file;
}

bool App::is_digit(string value) {
    int dot_count = 0;
    bool isdigit = true;
    int start = value[0] == '+' || value[0] == '-' ? 1 : 0;
    for (int i = start; i < value.size(); i++)
    {
        char c = value[i];
        if (c == '.') dot_count++;
        else if (c < 48 || c > 57) {
            isdigit = false;
            break;
        }
    }
    if (dot_count > 1 || !isdigit) return false;
    return true;
}

bool App::next_token() {
    if (token_index < tokens.size()) {
        current_token = tokens[token_index];
        token_index++;
        return true;
    }
    return false;
}

void App::get_tokens(string& command_temp) {
    tokens.clear();
    istringstream input(command_temp);
    while (!input.eof()) {
        string substring; input >> substring;
        if(!substring.empty()) tokens.push_back(substring);
    }
}

string App::get_unknown_command_output(string& unknown_command) {
    return "\nunknown command: " + unknown_command + "\nenter --help for list of command\n\n";
}

void App::get_new_copy_file_name() {
    ifstream ifs(output_file_name);
    string temp;
    int i = 1;
    while (ifs.is_open()) {
        ifs.close();
        size_t len = output_file_name.length();
        ostringstream ostr; ostr << output_file_name.substr(0, len - 4) << "(" << i << ")" << output_file_name.substr(len-4);
        temp = ostr.str();    
        ifs.open(temp);
        i++;
    } 

    output_file_name = temp;
    ifs.close();
}

void App::check_overwriting_output_file()
{
    if (output_file_name.empty()) output_file_name = get_svg_filename();

    ifstream ifs(output_file_name);
    if (ifs.is_open() && !force) {
        cout << "outputfile " + output_file_name + " was found. Overwrite it? (Y\\N) : ";
        string t;
        while (true) {
            cin >> t;
            cin.ignore(256, '\n');
            if (t == "Y" || t == "y" || t == "�") break;
            else if (t == "N" || t == "n") {
                get_new_copy_file_name();
                break;
            }
            else cout << "only (Y\\N) :";
        }
    }
    ifs.close();
}

string App::get_svg_filename()
{
    return input_file_name.substr(0, input_file_name.length() - 3) + "svg";
}

bool App::check_state_input_file() {
    ifstream ifs(current_token);
    if (!ifs.is_open()) {
        cout << "\nfile \"" + current_token + "\" not found; \nenter --help for list of command\n\n";
        ifs.close();
        return false;
    }
    else {
        input_file_name = current_token;
        ifs.close();
        return true;
    }
}

bool App::check_state_value_scale() {
    bool flag = false;

    if (is_digit(current_token)) {
        stringstream ss; ss << current_token;
        ss >> scale;
        if(scale > 0) flag = true;
    }
  
    if (!flag) cout << current_token << " : " << bad_number_output;
    return flag;
}

bool App::check_state_additional_parameters() {
    bool flag = true;
    if (current_token == "--scale") {
        if (is_scale) flag = false;
        else {
            is_scale = true;
            state = converter_cli::value_scale;
        }
    }
    else if (current_token == "--force") {
        if (force) flag = false;
        else {
            force = true;
            state = converter_cli::additional_parameters;
        }
    }
    else flag = false;

    return flag;
}

bool App::parsing() {
    while (next_token()) {
        if (state == converter_cli::input_file) {
            if (check_state_input_file()) state = converter_cli::output_file;
            else return false;
        }
        else if (state == converter_cli::output_file) {
            if (current_token == "--scale" || current_token == "--force") token_index--;
            else output_file_name = current_token;

            state = converter_cli::additional_parameters;
        }
        else if (state == converter_cli::additional_parameters) {
            if (!check_state_additional_parameters()) {
                cout << get_unknown_command_output(current_token);
                return false;
            }
        }
        else if (state == converter_cli::value_scale) {
            if (check_state_value_scale()) state = converter_cli::additional_parameters;
            else return false;
        }
    }

    check_overwriting_output_file();
    
    return true;
}