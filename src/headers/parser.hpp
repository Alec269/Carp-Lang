#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "tokeniser.hpp"

// # Global Data
inline void indent( int n )
{
	for ( int i = 0; i < n; ++i ) {
		std::cout << " ";
	}
}

inline std::string tokenTypeToString( TokenType t )
{
	switch ( t ) {
	case TokenType::T_int:
		return "int";
	case TokenType::T_float:
		return "float";
	case TokenType::T_string:
		return "string";

	case TokenType::T_identifier:
		return "identifier";
	case TokenType::T_numLit:
		return "number";
	case TokenType::T_strLit:
		return "string literal";

	case TokenType::T_eq:
		return "=";
	case TokenType::T_eqEq:
		return "==";
	case TokenType::T_NotE:
		return "!=";
	case TokenType::T_LeT:
		return "<";
	case TokenType::T_LeTEq:
		return "<=";
	case TokenType::T_GrT:
		return ">";
	case TokenType::T_GrTEq:
		return ">=";

	case TokenType::T_plus:
		return "+";
	case TokenType::T_minus:
		return "-";
	case TokenType::T_star:
		return "*";
	case TokenType::T_slash:
		return "/";

	case TokenType::T_semi:
		return ";";
	case TokenType::T_comma:
		return ",";

	case TokenType::T_LBrack:
		return "(";
	case TokenType::T_RBrack:
		return ")";
	case TokenType::T_LBrace:
		return "{";
	case TokenType::T_RBrace:
		return "}";
	case TokenType::T_LSquare:
		return "[";
	case TokenType::T_RSquare:
		return "]";

	case TokenType::T_EOF:
		return "<EOF>";

	default:
		return "<unknown token>";
	}
}

// default expression type
struct Expr {
	virtual ~Expr() = default;	 // DESTRUCTOR
	// HELPER FOR PRINTING AST STRUCTURE
	virtual void print( int indent = 0 ) const = 0;
};

struct NumberExpr : Expr {
	std::string value;
	NumberExpr( std::string val ) : value( std::move( val ) ) {}

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "NumberExpr(" << value << ")\n";
	}
};

struct StringExpr : Expr {
	std::string value;
	StringExpr( std::string val ) : value( std::move( val ) ) {}

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "StringExpr(\"" << value << "\")\n";
	}
};

struct IdentExpr : Expr {
	std::string name;
	IdentExpr( std::string nm ) : name( std::move( nm ) ) {}

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "IdentExpr(" << name << ")\n";
	}
};

// == != >= <= etc
struct BinaryExpr : Expr {
	std::unique_ptr<Expr> left;
	std::unique_ptr<Expr> right;
	TokenType operatr;

	BinaryExpr( std::unique_ptr<Expr> lf, TokenType op, std::unique_ptr<Expr> rt )
		 : left( std::move( lf ) ), operatr( op ), right( std::move( rt ) )
	{
	}
};

struct Stmt {
	virtual ~Stmt() = default;
	virtual void print( int indent = 0 ) const = 0;
};

// for variable declaration
struct VarDeclStmt : Stmt {
	TokenType type;
	std::string name;
	std::unique_ptr<Expr> init;

	VarDeclStmt( TokenType tp, std::string nm, std::unique_ptr<Expr> i )
		 : type( tp ), name( std::move( nm ) ), init( std::move( i ) )
	{
	}

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "VarDeclStmt\n";

		indent( indentLevel + 1 );
		std::cout << "type: " << tokenTypeToString( type ) << "\n";

		indent( indentLevel + 1 );
		std::cout << "name: " << name << "\n";

		indent( indentLevel + 1 );
		std::cout << "init:\n";
		init->print( indentLevel + 2 );
	}
};

// # End of Global data

class Parser {
 public:
	explicit Parser( const std::vector<Token>& tokens );
	std::vector<std::unique_ptr<Stmt>> parse();

 private:
	const std::vector<Token>& m_tokens;
	size_t m_pos = 0;

	// helpers
	const Token& peek() const;
	const Token& advance();
	bool match( TokenType type );
	const Token& expect( TokenType type, const char* msg );

	// parsing
	std::unique_ptr<Stmt> parseStatement();
	std::unique_ptr<Stmt> parseVarDecl();

	std::unique_ptr<Expr> parseExpression();
	std::unique_ptr<Expr> parsePrimary();
};
