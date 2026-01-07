#include <stdexcept>

#include "headers/parser.hpp"

Parser::Parser( const std::vector<Token>& tokens ) : m_tokens( tokens ) {}

// To get the current token without moving, so we can decide
const Token& Parser::peek() const
{
	return m_tokens[ m_pos ];	// Returns the current token without advancing the token stream
}

// to say the current tk is valid and move on
const Token& Parser::advance()
{
	return m_tokens[ m_pos++ ];  // Consumes the current token and advances to the next one
}

bool Parser::match( TokenType type )
{	// If the current token matches the given type, consume it and return true
	if ( peek().type == type ) {
		advance();
		return true;
	}
	return false;
}
// to ensure the token is something we want like a name (x,y etc) after int/float/string
const Token& Parser::expect( TokenType type, const char* msg )
{	// Ensures the current token is of the expected type.
	// Used when the grammar requires a specific token.
	// Throws an error if the grammar is violated.

	if ( peek().type != type ) {	// if the token is not what we want
		throw std::runtime_error( msg );
	}
	return advance();	 // if matches then go ahead
}

// primary expression skeleton
std::unique_ptr<Expr> Parser::parsePrimary()
{
	if ( match( TokenType::T_numLit ) ) {
		return std::make_unique<NumberExpr>( m_tokens[ m_pos - 1 ].value );
		// Create a NumberExpr AST node using the value of the previously consumed token
		// basically NumberExpr("5") from int x = 5 ;
		// m_pos - 1 refers to the token that was just consumed by match()
	}
	if ( match( TokenType::T_identifier ) ) {
		return std::make_unique<IdentExpr>( m_tokens[ m_pos - 1 ].value );
	}
	if ( match( TokenType::T_strLit ) ) {
		return std::make_unique<StringExpr>( m_tokens[ m_pos - 1 ].value );
	}

	throw std::runtime_error( "Expected expression" );
}

std::unique_ptr<Expr> Parser::parseExpression()
{
	return parsePrimary();	// very limited definition for now
}

std::unique_ptr<Stmt> Parser::parseVarDecl()

{	// a var decl should start with a type - int/float/string
	TokenType type = advance().type;
	// ↑ Consume the current type token (int / float / string) and record its kind

	// after var type we expect identifier and then store its name
	const Token& nameToken = expect( TokenType::T_identifier, "Expected variable name" );
	// after id we want = sign
	expect( TokenType::T_eq, "Expected '='" );
	// after this we expect an expression (number, string, identifier, etc)
	auto init = parseExpression();  // get the literal value
	// and finally a semicolon
	expect( TokenType::T_semi, "Expected ';'" );
	// after making sure the structure is correct,
	// we return the type, identifier and value of the var via a varStmt node
	return std::make_unique<VarDeclStmt>( type, nameToken.value, std::move( init ) );
}

std::unique_ptr<Stmt> Parser::parseStatement()
{
	// Dispatches to the correct statement parser based on the current token
	switch ( peek().type ) {
	case TokenType::T_int:
	case TokenType::T_float:
	case TokenType::T_string:
		return parseVarDecl();

	default:
		throw std::runtime_error( "Unknown statement" );
	}
}

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
	std::vector<std::unique_ptr<Stmt>> stmts;
	while ( peek().type != TokenType::T_EOF ) {
		stmts.push_back( parseStatement() );
		// Parses the entire token stream into a list of top-level statements
	}
	return stmts;
}

/*

#@ `std::make_unique<NumberExpr>( m_tokens[m_pos - 1].value )`

This does three things:

- Allocates a new NumberExpr on the heap
- Copies the string value (e.g. "5") into it
- Returns std::unique_ptr<Expr>

Important:

- The AST node does not reference the token
- The token can disappear later
- The AST survives independently

## Why returning a token pointer would be WRONG

very common beginner mistake:

```cpp
struct NumberExpr : Expr {
	const Token& tok;
};
```

Why this breaks later

- Tokens are a temporary frontend structure
- AST is a long-lived semantic structure
- Later passes (type checking, codegen) should NOT depend on lexer state

Real compilers follow this rule:

- Tokens are input
- AST is owned data

Once parsing is done, the tokeniser & tokens can be thrown away
*/
