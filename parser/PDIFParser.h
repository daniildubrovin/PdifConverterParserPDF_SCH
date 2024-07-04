#ifndef PDIFPARSER_H
#define PDIFPARSER_H

#include <string>
#include <stack>
#include "PDIFType.h"
#include "PDIFLexer.h" 



/**
 * @brief A set of states used when parsing a PDF file
 * 
 */
enum class states {
	START, ///< initial state
	BEGIN_OBJECT, ///< beginning of the object {
	BEGIN_ARRAY, ///< beginning of the array  [
	END_OBJECT, ///< end of object }
	END_ARRAY, ///< end of array ]
	LITERALS,  ///< literal
	VALUE,  ///< string or number value
	LITERALS_ARRAY, ///< literal after "{" 
	VALUE_ARRAY, ///< literal after "["
	FINISH ///< final state
};

/**
 * @brief 
 * 
 */
struct Flags {
	bool Component = false;
	//ENVIRONMENT
	bool ENVIRONMENT = false;
	bool PDIFvrev = false;
	bool Program = false;
	bool DBtype = false;
	bool DBvrev = false;
	bool DBtime = false;
	bool DBunit = false;
	bool DBgrid = false;
	bool Lyrstr = false;
	int countLyrStr = 0;
	//SKIPPING
	token_type UnsupportedToken = token_type::uninitialized;
	//GRAPHIC ELEMENTS
	bool Line = false, Arc = false, Circle = false, Rect = false, Frect = false, Text = false, Wire = false;
	//PARAMETRS
	int countParametrs = 0;
	bool first_pos = 0;
	bool second_pos = 0;
	//LAYER
	Layer current_layer;
	bool Ly = false, Ls = false, Wd = false, Ts = false, Tj = false, Tr = false, Tm = false;
	//USER
	bool USER = false, Lv = false, Gs = false, Mode = false;
	//SYMBOL
	bool Symbol = false;
	bool PIN_DEF = false, P = false, Pt = false, Lq = false, Ploc = false;
	bool PIC = false;
	bool PKG = false;
	bool ATR = false;
	bool IN = false, EX = false;
	bool Rdl = false, Pnl = false, Sd = false, Org = false, Ty = false, At = false;
	bool pid = false;
	//DETAIL
	bool ANNOTATE = false;
	bool NET_DEF = false;
	bool DG = false;
	bool N = false, N_value = false, V = false, Nn = false, Ns = false, Un = false;
	bool COMP_DEF = false, COMP_DEF_value = false, Vr = false, SUBCOMP = false;
	bool I = false,I_new = false, CN = false, Rd = false, Pn = false, Pl = false, ASG = false, Sc = false, Ro = false, Mr = false, Nl = false;
};

/**
 * @brief The class is intended for syntactic analysis of the received tokens and creation of the PDIF object model.
 * 
 */
 class PDIFParser
{
private:
	PDIFLexer* lexer;
	token_type last_token;
	std::stack<token_type> stack;
	states state;
	Flags read_flags;
	vector<string> colors {"black","lime","red","yellow","blue","aqua","fuchsia","rgba(85,85,85,255)",
	"rgba(0,85,0,255)","rgba(255,170,0,255)","rgba(170,85,0,255)","rgba(0,170,255,255)","rgba(255,170,170,255)",
	"rgba(255,0,170,255)","rgba(170,170,170,255)","chartreuse"};

	/**
	 * @brief Returns the next token and assigns it to last_token.
	 * 
	 * @return token_type 
	 */
	token_type next_token();

	// ***Magic***

	bool is_skiping_token();
	void print_unsupported_message(string file = "");
	void update_flags(token_type token);
	void parse_user(PDIFType& pdif, string mode);
	void parse_environment(PDIFType& pdif, string mode);
	void parse_graphical_elements(PDIFType& pdif, string mode);
	void parse_layer(PDIFType& pdif, string mode);
	void parse_pin_def(PDIFType& pdif, string mode);
	void parse_symbol(PDIFType& pdif, string mode);
	void parse_nef_def(PDIFType& pdif, string mode);
	void parse_comp_def(PDIFType& pdif, string mode);
	void parse_i(PDIFType& pdif, string mode);

public:
	 PDIFParser(std::string ifile);
	/**
	 * @brief The method starts parsing (parsing) of the input document and returns object.
	 * 
	 * @param scale 	Svg file scale
	 * @return PDIFType 	Object representation of a PDF file
	 */
	 PDIFType parse();
};
#endif