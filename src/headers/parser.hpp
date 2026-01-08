#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "tokeniser.hpp"
#include "utils.hpp"

// # Global Data

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
		std::cout << "NumberExpr(" << YELLOW << value << CoRESET << ")\n";
	}
};

struct StringExpr : Expr {
	std::string value;
	StringExpr( std::string val ) : value( std::move( val ) ) {}

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "StringExpr(\"" << YELLOW << value << CoRESET << "\")\n";
	}
};

struct IdentExpr : Expr {
	std::string name;
	IdentExpr( std::string nm ) : name( std::move( nm ) ) {}

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "IdentExpr(" << YELLOW << name << CoRESET << ")\n";
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
		std::cout << "type: " << BLUE << tokenTypeToString( type ) << CoRESET << "\n";

		indent( indentLevel + 1 );
		std::cout << "name: " << GREEN << name << CoRESET << "\n";

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
