#ifndef PDIFEXCEPTION_H
#define PDIFEXCEPTION_H
#include <exception>
#include <string>
#include "PDIFLexer.h"

/**
 * @brief Сontains general information about the error and its position
 * 
 */
class PDIFException : public std::exception
{
private:
	PDIFLexer* lexer;
	std::string addMessage; 
	token_type expected_token_value;
public:

	PDIFException(PDIFLexer* lexer):expected_token_value(token_type::uninitialized) {
		this->lexer = lexer;
	}

	PDIFException(PDIFLexer* lexer,std::string addMessage) :expected_token_value(token_type::uninitialized) {
		this->lexer = lexer;
		this->addMessage = addMessage;
	}

	PDIFException(PDIFLexer* lexer, token_type expected_token_value) {
		this->lexer = lexer;
		this->expected_token_value = expected_token_value;
		
		if(expected_token_value == token_type::value_string) addMessage = std::string("expected string value");
		else if(expected_token_value == token_type::value_integer) addMessage = std::string("expected integer value");
		else if(expected_token_value == token_type::value_float) addMessage = std::string("expected float value");
		else if (lexer->is_key_words(expected_token_value)) addMessage = std::string("expected key word");
	}

	const char* what() const throw() {
		return (new std::string("Error: Unexpected token: " + lexer->get_string_value() +
			" position: " + std::to_string(lexer->get_position().line_number) +
			"," + std::to_string(lexer->get_position().symbol_number) + " " + addMessage))->c_str();
	}
};
#endif