#include "PDIFLexer.h"
#include <sstream>
#include "PDIFException.h"
    
PDIFLexer::PDIFLexer(string ifile)
{ 
    ia.open(ifile);
    if (!ia.is_open()) throw PDIFException(this, "file don't open");
}

token_type PDIFLexer::scan()
{
    skip_whitespace();
    while (current == '%')
    {
        if (!scan_comment()) return token_type::parse_error;
        skip_whitespace();
    }
   
    switch (current)
    {
        // structural characters
    case '[':
        string_value = '[';
        return token_type::begin_array;
    case ']':
        string_value = ']';
        return token_type::end_array;
    case '{':
        string_value = '{';
        return token_type::begin_object;
    case '}':
        string_value = '}';
        return token_type::end_object;

        // string
    case '\"':
        return scan_string_with_quote();

    default:
        return scan_string_without_quote();
    }

    return token_type::uninitialized;
}

bool PDIFLexer::is_number(string str)
{
    for (char& symbol : str) {
        if (!isdigit(symbol) && symbol != '.' && symbol != '-') return false;
    }
    return true;
}

void PDIFLexer::skip_whitespace()
{
    do { get(); } 
    while (current == ' ' || current == '\t' || current == '\n' || current == '\r');
}

int PDIFLexer::get()
{
    if (current == '\n') {
        position.line_number++;
        position.symbol_number = 1;
    }
    else position.symbol_number++;

    return current = ia.get();
}

token_type PDIFLexer::scan_string_with_quote() {
    string_value = "";

    if (current == '\"') {
        while (true)
        {
            // get next character
            switch (get())
            {
                // end of file while parsing string
                case std::char_traits<char>::eof():
                {
                    return token_type::parse_error;
                }
                case '&':
                {
                    string_value += "&amp;";
                    break;
                }
                // closing quote
                case '\"':
                {
                    return token_type_value = token_type::value_string;
                }
                // escapes
                case '\\':
                {
                    switch (get())
                    {
                        // quotation mark
                    case '\"':
                        add('\"');
                        break;
                        // reverse solidus
                    case '\\':
                        add('\\');
                        break;
                        // solidus
                    case '/':
                        add('/');
                        break;
                        // backspace
                    case 'b':
                        add(' ');
                        break;
                        // form feed
                    case 'f':
                        add('\f');
                        break;
                        // line feed
                    case 'n':
                        add('\n');
                        break;
                        // carriage return
                    case 'r':
                        add('\r');
                        break;
                        // tab
                    case 't':
                        add('\t');
                        break;
                    case '(':
                        add('(');
                        break;
                    case ')':
                        add(')');
                        break;

                        // other characters after escape
                    default:
                        return token_type::parse_error;
                    }

                    break;
                }

                default: add(current);
            }
        }
    }

    return token_type::parse_error;
}


token_type PDIFLexer::scan_string_without_quote() {
    string_value = "";

    while (true) {
        switch (current)
        {
            case std::char_traits<char>::eof():
            case '\0': 
                return token_type::end_of_input;
            case '}': 
            case ']': 
                ia.unget();
            case ' ':
            case '\n':
            case '\t':
            case '\r': 
                return get_values_from_scan_string();
            default:
                add(current);
                break;
        }
        get();
    }

    return token_type::parse_error;
}

token_type PDIFLexer::get_values_from_scan_string()
{
    if (is_number(string_value)) {
        std::stringstream ss(string_value);
        if (is_float(string_value)) {
            float_value = 0; ss >> float_value;
            return token_type_value = token_type::value_float;
        }

        integer_value = 0; ss >> integer_value;
        return token_type_value = token_type::value_integer;
    }

    for (int i = 0; i < lexemes.size(); i++)
    {
        if (lexemes[i] == string_value) return token_type_value = (token_type)(i);
    }

    return token_type_value = token_type::value_string;
}

bool PDIFLexer::scan_comment()
{
    // single-line comments skip input until a newline or EOF is read
    while (true)
    {
        switch (get())
        {
        case '\n':    
        case '\r':          
        case '\0':
            return true;

        default:
            break;
        }
    }
    return false;
}

void PDIFLexer::add(char symbol)
{
    string_value += symbol;
}

bool PDIFLexer::is_float(string str)
{
    for (char symbol : str) {
        if (symbol == '.') return true;
    }
    return false;
}

string PDIFLexer::get_string_value()
{
    return string_value;
}

bool PDIFLexer::is_key_words(token_type token)
{
    return (int)token >= 7 && (int)token <= 76;
}

int PDIFLexer::get_integer_value()
{
    if (token_type_value != token_type::value_integer) throw PDIFException(this, token_type::value_integer);
    return integer_value;
}
float PDIFLexer::get_float_value()
{
    if (token_type_value != token_type::value_float) throw PDIFException(this, token_type::value_float);
    return float_value;
}
PDIFLexer::~PDIFLexer()
{
    ia.close();
}

position_t PDIFLexer::get_position()
{
    return position;
}
