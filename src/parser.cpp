// src\parser.cpp

#include "headers/parser.hpp"
#include <memory>
#include <stdexcept>
#include <utility>

#include "headers/tokeniser.hpp"

/* --------------------------------------------------------------------------------------------- */

Parser::Parser( const std::vector<Token>& tokens ) : m_tokens( tokens ) {}

// To get the current token without moving, so we can decide
const Token& Parser::peek() const
{
	return m_tokens[ m_pos ];	// Returns the current token without advancing the token stream
}

// to say the current tk is valid and move on
const Token& Parser::advance()
{
	return m_tokens[ m_pos++ ];  // Consumes(returns) the current token and advances to the next one
}

bool Parser::match( const TokenType type )
{	// If the current token matches the given type, consume it and return true
	if ( peek().type == type ) {
		advance();
		return true;
	}
	return false;
}

/* --------------------------------------------------------------------------------------------- */

// to ensure the token is something we want like a name (x,y etc) after int/float/string
const Token& Parser::expect( const TokenType type, const char* msg )
{	// Ensures the current token is of the expected type.
	// Used when the grammar requires a specific token.
	// Throws an error if the grammar is violated.

	if ( peek().type != type ) {	// if the token is not what we want
		throw std::runtime_error( msg );
	}
	return advance();	 // if matches then go ahead
}

/* --------------------------------------------------------------------------------------------- */

// primary expression(numlit ,id, strlit) skeleton
std::unique_ptr<Expr> Parser::parsePrimary()
{
	// num literal
	if ( match( TokenType::T_numLit ) ) {
		const Token& num = m_tokens[ m_pos - 1 ];
		// After match() succeeds: match(TokenType::T_numLit)
		// The parser has already consumed the token.
		// So the consumed token is at: m_tokens[m_pos - 1]
		return std::make_unique<NumberExpr>( num.value, num.loc );
	}
	// identifier
	if ( match( TokenType::T_identifier ) ) {
		const Token& id = m_tokens[ m_pos - 1 ];
		return std::make_unique<IdentExpr>( id.value, id.loc );
	}
	// string literal
	if ( match( TokenType::T_strLit ) ) {
		const Token& str = m_tokens[ m_pos - 1 ];
		return std::make_unique<StringExpr>( str.value, str.loc );
	}
	if ( match( TokenType::T_LBrack ) ) {
		auto expr = parseExpression();
		expect( TokenType::T_RBrack, "Expected ')'" );
		return expr;
	}

	// bool true
	if ( match( TokenType::T_true ) ) {
		const Token& trueToken = m_tokens[ m_pos - 1 ];
		return std::make_unique<BoolExpr>( true, trueToken.loc );
	}
	if ( match( TokenType::T_false ) ) {
		const Token& falseToken = m_tokens[ m_pos - 1 ];
		return std::make_unique<BoolExpr>( false, falseToken.loc );
	}

	throw std::runtime_error( "Expected expression" );
}

/* --------------------------------------------------------------------------------------------- */

std::unique_ptr<Expr> Parser::parseExpression()
{
	return parseEquality();	 // very limited definition for now
}

std::unique_ptr<Expr> Parser::parseComparison()
{
	auto expr = parseTerm();

	while ( match( TokenType::T_GrT ) || match( TokenType::T_GrTEq ) || match( TokenType::T_LeT ) ||
			  match( TokenType::T_LeTEq ) ) {
		TokenType op = m_tokens[ m_pos - 1 ].type;
		auto right = parseTerm();
		expr = std::make_unique<BinaryExpr>( std::move( expr ), op, std::move( right ), expr->m_loc );
	}
	return expr;
}

std::unique_ptr<Expr> Parser::parseUnary()
{
	if ( match( TokenType::T_minus ) ) {
		TokenType op = m_tokens[ m_pos - 1 ].type;
		auto right = parseUnary();

		// Treat unary minus as binary (0 - expr) ; [apparently works flawlessly]
		auto zero = std::make_unique<NumberExpr>( "0", right->m_loc );
		return std::make_unique<BinaryExpr>( std::move( zero ), op, std::move( right ),
														 right->m_loc );
	}
	return parsePrimary();
}
// (* and /)
std::unique_ptr<Expr> Parser::parseFactor()
{
	auto expr = parseUnary();

	while ( match( TokenType::T_star ) || match( TokenType::T_slash ) ) {
		TokenType op = m_tokens[ m_pos - 1 ].type;
		auto right = parseUnary();
		expr = std::make_unique<BinaryExpr>( std::move( expr ), op, std::move( right ), expr->m_loc );
	}
	return expr;
}

std::unique_ptr<Expr> Parser::parseEquality()
{
	auto expr = parseComparison();

	while ( match( TokenType::T_eqEq ) || match( TokenType::T_NotE ) ) {
		TokenType op = m_tokens[ m_pos - 1 ].type;
		auto right = parseComparison();
		expr = std::make_unique<BinaryExpr>( std::move( expr ), op, std::move( right ), expr->m_loc );
	}
	return expr;
	// What this does:
	// 	Parse left side
	// 	While we see == or !=
	// 	Keep building tree left-associatively
}

// (+ and -)
std::unique_ptr<Expr> Parser::parseTerm()
{
	auto expr = parseFactor();
	while ( match( TokenType::T_plus ) || match( TokenType::T_minus ) ) {
		TokenType op = m_tokens[ m_pos - 1 ].type;
		auto right = parseFactor();
		expr = std::make_unique<BinaryExpr>( std::move( expr ), op, std::move( right ), expr->m_loc );
	}
	return expr;
}

/* --------------------------------------------------------------------------------------------- */

std::unique_ptr<Stmt> Parser::parseVarDecl()
{	// a var decl should start with a type - int/float/string
	TokenType type = advance().type;
	// ↑ Consume the current type token (int / float / string) and record its kind

	// after var type we expect identifier and then store its name
	const Token& nameToken = expect( TokenType::T_identifier, "Expected variable name" );
	// after id, we want = sign
	expect( TokenType::T_eq, "Expected '='" );
	// after this we expect an expression (number, string, identifier, etc)
	auto init = parseExpression();  // get the literal value
	// and finally a semicolon
	expect( TokenType::T_semi, "Expected ';'" );
	// after making sure the structure is correct,
	// we return the type, identifier and value of the var via a varStmt node
	return std::make_unique<VarDeclStmt>( type, nameToken.value, std::move( init ), nameToken.loc );
}

/* --------------------------------------------------------------------------------------------- */

std::unique_ptr<Stmt> Parser::parseAssignment()
{
	const Token& name = expect( TokenType::T_identifier, "Expected identifier" );
	expect( TokenType::T_eq, "Expected '='" );
	auto value = parseExpression();
	expect( TokenType::T_semi, "Expected ';'" );

	return std::make_unique<AssignStmt>( name.value, std::move( value ), name.loc );
}

/* --------------------------------------------------------------------------------------------- */

std::unique_ptr<Stmt> Parser::parseIfStmt()
{
	// consume if
	const Token& ifTok = expect( TokenType::T_if, "Expected 'if'" );
	// expect '('
	expect( TokenType::T_LBrack, "Expect '(' after 'if'" );
	// parse the condition  expr
	auto condition = parseExpression();
	// expect ')'
	expect( TokenType::T_RBrack, "Expect ')' after 'condition'" );

	auto thenBranch = parseStatement();	 // get the if body

	// optional else branch
	std::unique_ptr<Stmt> elseBranch = nullptr;
	if ( match( TokenType::T_else ) ) {
		elseBranch = parseStatement();
	}

	return std::make_unique<IfStmt>( std::move( condition ), std::move( thenBranch ), ifTok.loc,
												std::move( elseBranch ) );
}

/* --------------------------------------------------------------------------------------------- */

std::unique_ptr<Stmt> Parser::parseWhileStmt()
{
	// consume while
	const Token& whileTok = expect( TokenType::T_while, "Expected 'while'" );
	// expect '('
	expect( TokenType::T_LBrack, "Expect '(' after 'while'" );
	// parse the condition  expr
	auto condition = parseExpression();
	// expect ')'
	expect( TokenType::T_RBrack, "Expect ')' after 'condition'" );
	auto whileBody = parseStatement();	// get the body
	return std::make_unique<WhileStmt>( std::move( condition ), std::move( whileBody ),
													whileTok.loc );
}

/* --------------------------------------------------------------------------------------------- */

std::unique_ptr<Stmt> Parser::parseBlock()
{
	const Token& lbrace = expect( TokenType::T_LBrace, "Expected '{'" );

	auto block = std::make_unique<BlockStmt>();

	while ( peek().type != TokenType::T_RBrace && peek().type != TokenType::T_EOF ) {
		block->statements.push_back( parseStatement() );
	}
	expect( TokenType::T_RBrace, "Expected '}'" );

	block->m_loc = lbrace.loc;

	return block;
}

/* --------------------------------------------------------------------------------------------- */

std::unique_ptr<Stmt> Parser::parseStatement()
{
	// Dispatches to the correct statement parser based on the current token
	switch ( peek().type ) {
	case TokenType::T_int:
	case TokenType::T_float:
	case TokenType::T_string:
	case TokenType::T_bool:
		return parseVarDecl();
	case TokenType::T_identifier:
		return parseAssignment();
	case TokenType::T_if:
		return parseIfStmt();
	case TokenType::T_LBrace:
		return parseBlock();
	case TokenType::T_while:
		return parseWhileStmt();

	default:
		throw std::runtime_error( "Unknown statement" );
	}
}

/* --------------------------------------------------------------------------------------------- */

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
	std::vector<std::unique_ptr<Stmt>> stmts;
	while ( peek().type != TokenType::T_EOF ) {
		stmts.push_back( parseStatement() );
		// Parses the entire token stream into a list of top-level statements
	}
	return stmts;
}
