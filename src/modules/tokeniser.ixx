module;

#include <cctype>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

export module tokeniser;

// # Global Data
//
// strongly typed list of token types
export enum class TokenType	 // enum class is safer than enum & prevents name collisions
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
	{ "int", TokenType::T_int },		  { "long", TokenType::T_long },
	{ "char", TokenType::T_char },	  { "string", TokenType::T_string },
	{ "float", TokenType::T_float },	  { "double", TokenType::T_double },
	{ "any", TokenType::T_any },		  { "if", TokenType::T_if },
	{ "else", TokenType::T_else },	  { "while", TokenType::T_while },
	{ "return", TokenType::T_return },	//{ "for", TokenType::T_for },
};

// where token appears
export struct Location {
	size_t line;
	size_t column;
};	 // will be Used for error messages later

// What a full token is
export struct Token {
	TokenType type;	  // TokenType = what type is
	std::string value;  // value = what text it came from
	Location loc;		  // Location = where it is
};

// # End of Global data

export class Tokeniser {

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

// ditto mark ('') are used in comments: means same as the line above

/* Lexer rule of thumb
Exactly ONE token per loop iteration
*/

// constructor init
Tokeniser::Tokeniser( std::string source ) : m_source( std::move( source ) ) {}
// moving the text input to the source member var

char Tokeniser::peek() const
{
	if ( m_index >= m_source.size() ) {	 // if we've reached the end
		return '\0';
	}

	return m_source[ m_index ];  // otherwise return current char for use
}

char Tokeniser::advance()
{
	const char c = m_source[ m_index ];	 // consume current char for use
	m_index++;									 // increment
	m_column++;									 // ''
	return c;									 // return the consumed char for use
}
// currently using peek() and advance() for their side effects, not their return values.
/* Example future use:

string literals "hello"
escape sequences \n
comments // or block comments /
*/

void Tokeniser::addToken( const TokenType tType, std::string value, const size_t startColumn )
{
	m_tokens.push_back( { tType, std::move( value ), { m_line, startColumn } } );
	// add tokens to the dynamic array with their type, text and pos
	// must use startcolumn meaning the column u see in IDE,
	// when cursor is before a char
}

std::vector<Token> Tokeniser::tokenise()
{
	while ( m_index < m_source.size() ) {
		const char c = peek();
		//* whitespace
		if ( c == ' ' || c == '\t' || c == '\r' ) {
			advance();
			continue;
			// Ignore 'spaces', 'tabs', 'carriage return'
			// Just advance and continue.
			// continue means “I’m DONE handling this character/token.
			// 						Go back to the top of the loop.”
			// one could instead write else-ifs but those can be janky for tokenisers
		}

		//* new line: advance, increment line,reset column, continue
		if ( c == '\n' ) {
			advance();
			m_line++;
			m_column = 1;
			continue;
		}

		//* numbers
		if ( std::isdigit( static_cast<unsigned char>( c ) ) ) {	 // detect the start of a num
			// undefined behaviour without static cast
			const size_t startIndex = m_index;	  // remember where the num start,
			const size_t startColumn = m_column;  // ''
			// ↓ consume all digits
			while ( std::isdigit( static_cast<unsigned char>( peek() ) ) ) {
				advance();
			}

			addToken( TokenType::T_numLit, m_source.substr( startIndex, m_index - startIndex ),
						 startColumn );  // extract the num tex ↑
			continue;
		}

		//* ids/keywords
		if ( std::isalpha( static_cast<unsigned char>( c ) ) || c == '_' ) {
			// ↑ consume letters , digit and underscores
			const size_t startIndex = m_index;
			const size_t startColumn = m_column;
			while ( std::isalnum( static_cast<unsigned char>( peek() ) ) || peek() == '_' ) {
				advance();
			}
			// if we get non-quoted text the extract it for comparison
			std::string txt = m_source.substr( startIndex, m_index - startIndex );
			// if the piece of text matches then add correct token
			//@ match Tokens
			if ( auto tryKey = g_keywords.find( txt ); tryKey != g_keywords.end() ) {
				addToken( tryKey->second, txt, startColumn );
				// second means the 2nd part of a key of the map
			} else {
				addToken( TokenType::T_identifier, txt, startColumn );
			}
			continue;
		}
		// one might wonder why use isalpha in parent if and isalnum in the child
		/* In c-like langs
		the start of an identifier must not be a num
		the outer condition ensures that
		and in the inner "if" we're asking "can this char continue the id name"
		*/

		//* string literals
		if ( c == '"' ) {
			const size_t startColumn = m_column;
			advance();									// consume opening quote
			const size_t startIndex = m_index;	// define startIndex where the string actually starts

			while ( peek() != '"' && peek() != '\0' ) {	// look ahead without moving
				if ( peek() == '\n' ) {
					throw std::runtime_error( "Unterminated string literal" );	// error if newline
				}
				advance();	// keep consuming chars until we reach char before either " or \0
			}
			if ( peek() == '\0' ) {	 // checks the next char
				throw std::runtime_error( "Unterminated string literal" );
				// If the loop finishes because it hit the end of the file (\0) rather than a closing
				// quote, it’s a syntax error.
			}	// if this doesn't trigger the next char is "
			const std::string value = m_source.substr( startIndex, m_index - startIndex );
			advance();	// consume closing "

			addToken( TokenType::T_strLit, value, startColumn );
			continue;
		}

		// here we compare chars for token assignment
		// {} bcs of needing to define startcolumn per case
		switch ( c ) {
		case '+': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_plus, "+", startColumn );
			break;
		}
		case '-': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_minus, "-", startColumn );
			break;
		}
		case '*': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_star, "*", startColumn );
			break;
		}
		case '/': {
			const size_t startColumn = m_column;
			advance();
			if ( peek() == '/' ) {
				// single line
				while ( peek() != '\n' && peek() != '\0' ) {
					advance();
				}
			} else if ( peek() == '*' ) {
				// multiline
				advance();
				while ( true ) {
					if ( peek() == '\0' ) {
						throw std::runtime_error( "Unterminated comment" );
					}
					if ( peek() == '*' && m_source[ m_index + 1 ] == '/' ) {
						advance();
						advance();
						break;
					}
					if ( peek() == '\n' ) {
						m_line++;
						m_column = 1;
					}
					advance();
				}

			} else {

				addToken( TokenType::T_slash, "/", startColumn );
			}
			break;
		}
		case ';': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_semi, ";", startColumn );
			break;
		}
		case '(': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_LBrack, "(", startColumn );
			break;
		}
		case ')': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_RBrack, ")", startColumn );
			break;
		}
		case '{': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_LBrace, "{", startColumn );
			break;
		}
		case '}': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_RBrace, "}", startColumn );
			break;
		}
		case '[': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_LSquare, "[", startColumn );
			break;
		}
		case ']': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_RSquare, "]", startColumn );
			break;
		}
		case ',': {
			const size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_comma, ",", startColumn );
			break;
		}
		case '=': {
			const size_t startColumn = m_column;
			advance();
			if ( peek() == '=' ) {
				advance();
				addToken( TokenType::T_eqEq, "==", startColumn );
			} else {
				addToken( TokenType::T_eq, "=", startColumn );
			}
			break;
		}
		case '!': {
			const size_t startColumn = m_column;
			advance();
			if ( peek() == '=' ) {
				advance();
				addToken( TokenType::T_NotE, "!=", startColumn );
			} else {
				throw std::runtime_error( "Unexpected '!'" );
			}
			break;
		}
		case '<': {
			const size_t startColumn = m_column;
			advance();
			if ( peek() == '=' ) {
				advance();
				addToken( TokenType::T_LeTEq, "<=", startColumn );
			} else {
				addToken( TokenType::T_LeT, "<", startColumn );
			}
			break;
		}
		case '>': {
			const size_t startColumn = m_column;
			advance();
			if ( peek() == '=' ) {
				advance();
				addToken( TokenType::T_GrTEq, ">=", startColumn );
			} else {
				addToken( TokenType::T_GrT, ">", startColumn );
			}
			break;
		}
		default:
			// for now crashes if a symbol doesn't match (lose text etc. becomes ids)
			throw std::runtime_error( "Unknown character at " + std::to_string( m_line ) + ":" +
											  std::to_string( m_column ) );
		}
	}

	addToken( TokenType::T_EOF, "", m_column );	// end of file
	return m_tokens;	// return the tokens for further use, such as parsing
}
