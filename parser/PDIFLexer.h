#ifndef PDIFLEXER_H
#define PDIFLEXER_H
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

enum class token_type 
{
    //literal
    value_string,     
    value_integer,    
    value_float,      
    begin_array,      
    begin_object,    
    end_array,        
    end_object,       
    component, // COMPONENT
    environment,    // ENVIRONMENT
    pdifrev,        // PDIFvrev
    program,        // Program
    dbtype,         // DBtype
    dbvrev,         // DBvrev
    dbtime,         // DBtime
    dbunit,         // DBunit
    dbgrid,         // DBgrid
    lyrstr,         // Lyrstr
    user_literal,        
    view,         
    mode,         
    vw,         
    gs,         
    /*display_literal,*/
    symbol,
    pin_def,
    pkg,
    rdl,
    sd,
    pic,
    atr,
    in,
    org,
    detail,
    annotate,
    l,
    net_def,
    n,
    dg,
    w,
    subcomp,
    comp_def,
    p,
    pt,
    lq,
    ploc,
    pid,
    c,
    i,
    cn,
    pl,
    ro,
    arc,
    lv,         // lv
    ty,
    ly,
    ls,
    wd,
    ts,
    tj,
    tr,
    tm,
    t,
    fr,
    r,
    pnl,
    at,
    ex,
    nn,
    un,
    v,
    ns,
    vr,
    rd,
    pn,
    asg,
    sc,
    mr,
    nl,

    uninitialized,
    parse_error,      ///< indicating a parse error
    end_of_input,     ///< indicating the end of the input buffer
    //literal_or_value,  ///< a literal or the begin of a value (only for diagnostics)
};

/**
 * @brief **Carriage position in pdf file**
 */
struct position_t
{
    size_t line_number;
    size_t symbol_number;
};

/**
 * @brief **Lexical analyzer**
 * @details The task of the lexical analyzer is to split the file into lexemes (reserved words (class token_type), string and numeric values)
 */
class PDIFLexer {
private:
    ifstream ia; ///<input file stream
    int current; ///< current symbol
    position_t position {1,0}; ///< current carriage position
    string string_value; ///< string representation of the current token or string value
    int integer_value; ///< integer value
    float float_value; ///< float value
    token_type token_type_value; ///< current token value
    
    
    ///< string representation of token
    std::vector<string> lexemes { "VALUE_STRING","VALUE_INTEGER","VALUE_FLOAT","BEGIN_ARRAY","BEGIN_OBJECT","END_ARRAY",
    "END_OBJECT","COMPONENT", "ENVIRONMENT", "PDIFvrev","Program","DBtype","DBvrev",
    "DBtime","DBunit","DBgrid","Lyrstr","USER", "VIEW", "Mode","Vw","Gs",/*"DISPLAY",*/"SYMBOL",
    "PIN_DEF","PKG","Rdl","Sd","PIC", "ATR", "IN","Org","DETAIL","ANNOTATE","L","NET_DEF","N","DG",
    "W","SUBCOMP","COMP_DEF", "P", "Pt","Lq","Ploc","Pid","C","I","CN","Pl","Ro","Arc","Lv",
    "Ty", "Ly", "Ls","Wd","Ts","Tj","Tr","Tm","T","Fr","R","Pnl","At","Ex","Nn","Un","V","Ns","Vr","Rd","Pn","ASG","Sc","Mr","Nl"};

    /**
     * @brief **Сhecking that the string is a number**
     * @details This is not a complete check on the number. Can only be used for PDF file
     * @param str 
     * @return true 
     * @return false 
     */
    bool is_number(string str);

    /**
     * @brief **Сhecking that the string is a float**
     * @details This is not a complete check on the float. Can only be used for PDF file
     * @param str 
     * @return true 
     * @return false 
     */
    bool is_float(string str);

    /**
     * @brief **Skip whitespace**
     * 
     */
    void skip_whitespace();

    /**
     * @brief **Get symbol from stream. Update current carriage position**
     * 
     * @return int 
     */
    int get();

    token_type scan_string_with_quote();
    token_type scan_string_without_quote();
    token_type get_values_from_scan_string();
    bool scan_comment();

    /**
     * @brief Add a character to string_value
     * 
     * @param symbol 
     */
    void add(char symbol);
      
public:
    PDIFLexer(string ifile);
    ~PDIFLexer();

    /**
     * @brief Get lexem (token, string and numeric values) from PDF file
     * 
     * @return token_type 
     */
    token_type scan();

    /**
     * @brief Сhecking whether a string is a reserved PDF word
     * 
     * @return true 
     * @return false 
     */
    bool is_key_words(token_type);

    /**
     * @brief Get the string_value
     * 
     * @return string 
     */
    string get_string_value();

    /**
     * @brief Get the integer_value
     * 
     * @return int 
     */
    int get_integer_value();

    /**
     * @brief Get the float_value
     * 
     * @return float 
     */
    float get_float_value();

    /**
     * @brief Get the carriage position
     * 
     * @return position_t 
     */
    position_t get_position();   
};
#endif