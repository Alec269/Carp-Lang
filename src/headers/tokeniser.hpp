// src\headers\tokeniser.hpp
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

// # Global Data

// strongly typed list of tokentypes
enum class TokenType	 // enum class is safer than enum & prvnts name collisions
{
	// Keywords
	T_int,
	T_long,
	T_char,
	T_string,
	T_float,
	T_double,
	T_any,
	T_if,
	T_else,
	T_while,
	T_return,
	// T_for,
	T_bool,
	T_true,
	T_false,
	// Ids and Litereals
	T_identifier,
	T_numLit,
	T_strLit,

	// symbols
	T_plus,
	T_minus,
	T_star,	 // multiplication symbol *
	T_slash,	 // division symbol /
	T_eq,
	T_semi,
	T_comma,

	// doubled operators
	T_eqEq,
	T_NotE,
	T_PlusE,
	T_MinusE,
	T_LeT,
	T_LeTEq,
	T_GrT,
	T_GrTEq,

	// brackets
	T_LBrack,
	T_RBrack,
	T_LBrace,
	T_RBrace,
	T_LSquare,
	T_RSquare,

	// end of file
	T_EOF
};

// this way we won't have to do manual comparisons
static const std::unordered_map<std::string, TokenType> g_keywords = {
	{ "int", TokenType::T_int },
	// { "long", TokenType::T_long },
	// { "char", TokenType::T_char },
	{ "string", TokenType::T_string },
	// { "float", TokenType::T_float },
	// { "double", TokenType::T_double },
	{ "bool", TokenType::T_bool },
	{ "true", TokenType::T_true },
	{ "false", TokenType::T_false },
	// { "any", TokenType::T_any },
	{ "if", TokenType::T_if },
	{ "else", TokenType::T_else },
	{ "while", TokenType::T_while },
	// { "return", TokenType::T_return },
	//{ "for", TokenType::T_for },
};

// where token appears
struct Location {
	size_t line;
	size_t column;
};	 // will be Used for error messages later

// What a full token is
struct Token {
	TokenType type;	  // TokenType = what type is
	std::string value;  // value = what text it came from
	Location loc;		  // Location = where it is
};

// # End of Global data

class Tokeniser {

 public:
	// constructor: takes entire file in string form
	explicit Tokeniser( std::string source );	 // e.. prvnts accidental convrs

	std::vector<Token> tokenise();

 private:
	// private helper funcs
	[[nodiscard]] char peek() const;	 // looks at current char
	char advance();						 // consumes current char(moves forward)
	void addToken( TokenType tType, std::string value = "", size_t startColumn = 1 );
	// stores a token

	// member vars
	std::string m_source;			// entire input file
	size_t m_index = 0;				// current position in the string
	size_t m_line = 1;				// current line number
	size_t m_column = 1;				// current column number
	std::vector<Token> m_tokens;	// output tokens
};
