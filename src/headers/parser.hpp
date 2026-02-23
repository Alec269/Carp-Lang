// src\headers\parser.hpp
#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <variant>

#include "tokeniser.hpp"
#include "utils.hpp"

/* --------------------------------------------------------------------------------------------- */

using Value = std::variant<int, std::string>;

struct Environment {
	std::unordered_map<std::string, Value> variables;
};

/* --------------------------------------------------------------------------------------------- */

// default expression type
struct Expr {
	Location m_loc{};
	virtual ~Expr() = default;	 // DESTRUCTOR
	// HELPER FOR PRINTING AST STRUCTURE
	virtual void print( int indent = 0 ) const = 0;
};

struct NumberExpr : Expr {
	std::string value;
	explicit NumberExpr( std::string val, const Location l ) : value( std::move( val ) )
	{
		m_loc = l;
	}

	void print( const int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "NumberExpr(" << YELLOW << value << CoRESET << ")\n";
	}
};

struct StringExpr : Expr {
	std::string value;
	explicit StringExpr( std::string val, const Location l ) : value( std::move( val ) )
	{
		m_loc = l;
	}

	void print( const int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "StringExpr(\"" << YELLOW << value << CoRESET << "\")\n";
	}
};

struct BoolExpr : Expr {
	bool value;
	BoolExpr( bool val, const Location l ) : value( val )
	{
		m_loc = l;
	}

	void print( const int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "BoolExpr(\"" << YELLOW << ( value ? "true" : "false" ) << CoRESET << "\")\n";
	}
};

struct IdentExpr : Expr {
	std::string name;
	explicit IdentExpr( std::string nm, const Location l ) : name( std::move( nm ) )
	{
		m_loc = l;
	}

	void print( const int indentLevel ) const override
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

	BinaryExpr( std::unique_ptr<Expr> lf, const TokenType op, std::unique_ptr<Expr> rt,
					const Location l )
		 : left( std::move( lf ) ), right( std::move( rt ) ), operatr( op )
	{
		m_loc = l;
	}

	void print( const int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "BinaryExpr(" << BLUE << tokenTypeToString( operatr ) << CoRESET << ")\n";

		left->print( indentLevel + 1 );
		right->print( indentLevel + 1 );
	}
};

/* --------------------------------------------------------------------------------------------- */

struct Stmt {
	Location m_loc{};
	virtual ~Stmt() = default;
	virtual void print( int indent = 0 ) const = 0;
};

// for variable declaration
struct VarDeclStmt : Stmt {
	TokenType type;
	std::string name;
	std::unique_ptr<Expr> expr;

	VarDeclStmt( const TokenType tp, std::string nm, std::unique_ptr<Expr> i, const Location l )
		 : type( tp ), name( std::move( nm ) ), expr( std::move( i ) )
	{
		m_loc = l;
	}

	void print( const int indentLevel ) const override
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
	AssignStmt( std::string nm, std::unique_ptr<Expr> val, const Location l )
		 : name( std::move( nm ) ), value( std::move( val ) )
	{
		m_loc = l;
	}

	void print( const int indentLevel ) const override
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

	IfStmt( std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> thenBr, const Location l,
			  std::unique_ptr<Stmt> elseBr = nullptr )
		 : condition( std::move( cond ) ), thenBranch( std::move( thenBr ) ),
			elseBranch( std::move( elseBr ) )
	{
		m_loc = l;
	}

	void print( const int indentLevel ) const override
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

	void print( const int indentLevel ) const override
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

	WhileStmt( std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> lpBody, const Location l )
		 : condition( std::move( cond ) ), loopBody( std::move( lpBody ) )

	{
		m_loc = l;
	}

	void print( const int indentLevel ) const override
	{
		indent( indentLevel );
		std::cout << "WhileLoopStmt:\n";

		indent( indentLevel + 1 );
		std::cout << "condition:\n";
		condition->print( indentLevel + 2 );

		indent( indentLevel + 1 );
		std::cout << "body:\n";
		loopBody->print( indentLevel + 2 );
	}
};

/* --------------------------------------------------------------------------------------------- */

class Parser {
 public:
	explicit Parser( const std::vector<Token>& tokens );
	std::vector<std::unique_ptr<Stmt>> parse();

	//  private:

	// parsing
	std::unique_ptr<Stmt> parseStatement();
	std::unique_ptr<Stmt> parseBlock();
	std::unique_ptr<Stmt> parseVarDecl();
	std::unique_ptr<Stmt> parseAssignment();
	std::unique_ptr<Stmt> parseIfStmt();
	std::unique_ptr<Stmt> parseWhileStmt();

	std::unique_ptr<Expr> parseExpression();
	std::unique_ptr<Expr> parsePrimary();

	std::unique_ptr<Expr> parseEquality();
	std::unique_ptr<Expr> parseComparison();
	std::unique_ptr<Expr> parseTerm();
	std::unique_ptr<Expr> parseFactor();
	std::unique_ptr<Expr> parseUnary();

 private:
	const std::vector<Token>& m_tokens;
	size_t m_pos = 0;

	// helpers
	[[nodiscard]] const Token& peek() const;
	const Token& advance();
	bool match( TokenType type );
	const Token& expect( TokenType type, const char* msg );
};
