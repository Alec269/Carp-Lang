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
	std::unique_ptr<Expr> expr;

	VarDeclStmt( TokenType tp, std::string nm, std::unique_ptr<Expr> i )
		 : type( tp ), name( std::move( nm ) ), expr( std::move( i ) )
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
		std::cout << "initExpr:\n";
		expr->print( indentLevel + 2 );
	}
};

struct AssignStmt : Stmt {
	std::string name;
	std::unique_ptr<Expr> value;
	AssignStmt( std::string nm, std::unique_ptr<Expr> val )
		 : name( std::move( nm ) ), value( std::move( val ) )
	{
	}

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "AssignStmt\n";

		indent( indentLevel + 1 );
		std::cout << "name: " << GREEN << name << CoRESET << "\n";

		indent( indentLevel + 1 );
		std::cout << "value:\n";
		value->print( indentLevel + 2 );
	}
};

struct IfStmt : Stmt {
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> thenBranch;
	std::unique_ptr<Stmt> elseBranch;

	IfStmt( std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> thenBr,
			  std::unique_ptr<Stmt> elseBr = nullptr )
		 : condition( std::move( cond ) ), thenBranch( std::move( thenBr ) ),
			elseBranch( std::move( elseBr ) )
	{
	}

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "IfStmt\n";

		indent( indentLevel + 1 );
		std::cout << "condition:\n";
		condition->print( indentLevel + 2 );

		indent( indentLevel + 1 );
		std::cout << "then:\n";
		thenBranch->print( indentLevel + 2 );

		if ( elseBranch ) {
			indent( indentLevel + 1 );
			std::cout << "else:" << '\n';
			elseBranch->print( indentLevel + 2 );
		}
	}
};

struct BlockStmt : Stmt {
	std::vector<std::unique_ptr<Stmt>> statements;

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "BlockStmt" << '\n';
		for ( auto& st : statements ) {
			st->print( indentLevel + 1 );
		}
	}
};

struct WhileStmt : Stmt {
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> loopBody;

	WhileStmt( std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> lpBody )
		 : condition( std::move( cond ) ), loopBody( std::move( lpBody ) )

	{
	}

	void print( int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "WHileLoopStmt:\n";

		indent( indentLevel + 1 );
		std::cout << "condition:\n";
		condition->print( indentLevel + 2 );

		indent( indentLevel + 1 );
		std::cout << "body:\n";
		loopBody->print( indentLevel + 2 );
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
	std::unique_ptr<Stmt> parseBlock();
	std::unique_ptr<Stmt> parseVarDecl();
	std::unique_ptr<Stmt> parseAssignment();
	std::unique_ptr<Stmt> parseIfStmt();
	std::unique_ptr<Stmt> parseWhileStmt();

	std::unique_ptr<Expr> parseExpression();
	std::unique_ptr<Expr> parsePrimary();
};
