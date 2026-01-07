#include <cctype>
#include <stdexcept>

#include "headers/tokeniser.hpp"

// ditto mark ('') are used in comments: means same as the line above

/* Lexer rule of thumb
Exactly ONE token per loop iteration
*/

// constructor init
Tokeniser::Tokeniser( std::string src ) : m_source( std::move( src ) ) {}
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
	char c = m_source[ m_index ];	 // consume current char for use
	m_index++;							 // increment
	m_column++;							 // ''
	return c;							 // return the consumed char for use
}
// currently using peek() and advance() for their side effects, not their return values.
/* Example future use:

string literals "hello"
escape sequences \n
comments // or /* /
*/

void Tokeniser::addToken( TokenType tType, std::string value, size_t startColumn )
{
	m_tokens.push_back( { tType, std::move( value ), { m_line, startColumn } } );
	// add tokens to the dynamic array with their type, text and pos
	// must use startcolumn meaning the column u see in IDE,
	// when cursor is before a char
}

std::vector<Token> Tokeniser::tokenise()
{
	while ( m_index < m_source.size() ) {
		char c = peek();
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
			// undefined behavior without static cast
			size_t startIndex = m_index;	  // remember where the num start,
			size_t startColumn = m_column;  // ''
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
			size_t startIndex = m_index;
			size_t startColumn = m_column;
			while ( std::isalnum( static_cast<unsigned char>( peek() ) ) || peek() == '_' ) {
				advance();
			}
			// if we get non-quoted text the extract it for comparison
			std::string txt = m_source.substr( startIndex, m_index - startIndex );
			// if the piece of text matches then add correct token
			//@ match Tokens
			auto tryKey = g_keywords.find( txt );
			if ( tryKey != g_keywords.end() ) {
				addToken( tryKey->second, txt, startColumn );
				// second means the 2nd part of a key of the map
			} else {
				addToken( TokenType::T_identifier, txt, startColumn );
			}
			continue;
		}
		// one might wonder why use isalpha in parent if and isalnum in the child
		/* In c-like langs
		the start of a identifier must not be a num
		the outer condition ensures that
		and in the inner "if" we're asking "can this char continue the id name"
		*/

		//* string literals
		if ( c == '"' ) {
			size_t startColumn = m_column;
			advance();							// consume opening quote
			size_t startIndex = m_index;	// define startIndex where the string actually starts

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
			std::string value = m_source.substr( startIndex, m_index - startIndex );
			advance();	// consume closing "

			addToken( TokenType::T_strLit, value, startColumn );
			continue;
		}

		// here we compare chars for token assignment
		// {} bcs of needing to define startcolumn per case
		switch ( c ) {
		case '+': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_plus, "+", startColumn );
			break;
		}
		case '-': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_minus, "-", startColumn );
			break;
		}
		case '*': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_star, "*", startColumn );
			break;
		}
		case '/': {
			size_t startColumn = m_column;
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
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_semi, ";", startColumn );
			break;
		}
		case '(': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_LBrack, "(", startColumn );
			break;
		}
		case ')': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_RBrack, ")", startColumn );
			break;
		}
		case '{': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_LBrace, "{", startColumn );
			break;
		}
		case '}': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_RBrace, "}", startColumn );
			break;
		}
		case '[': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_LSquare, "[", startColumn );
			break;
		}
		case ']': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_RSquare, "]", startColumn );
			break;
		}
		case ',': {
			size_t startColumn = m_column;
			advance();
			addToken( TokenType::T_comma, ",", startColumn );
			break;
		}
		case '=': {
			size_t startColumn = m_column;
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
			size_t startColumn = m_column;
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
			size_t startColumn = m_column;
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
			size_t startColumn = m_column;
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
