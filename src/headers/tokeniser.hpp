#pragma once

#include <string>
#include <vector>

// strongly typed list of tokentypes
enum class TokenType	 // enum class is safer than enum & prvnts name collisions
{
	// Keywords
	T_int,

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

	// brackets
	T_LBrack,
	T_RBrack,
	T_LBrace,
	T_RBrace,
	T_LSquare,
	T_RSquare,
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

class Tokeniser {

 public:
	// constructor: takes entire file in string form
	explicit Tokeniser( std::string source );	 // e.. prvnts accidental convrs

	std::vector<Token> tokenise();

 private:
	// private helper funcs
	char peek() const;  // looks at current char
	char advance();	  // consumes current char(moves forward)
	void addToken( TokenType tType, std::string value = "", size_t startColumn = 1 );
	// stores a token

 private:
	// member vars
	std::string m_source;			// entire input file
	size_t m_index = 0;				// current position in the string
	size_t m_line = 1;				// current line number
	size_t m_column = 1;				// current column number
	std::vector<Token> m_tokens;	// output tokens
};
