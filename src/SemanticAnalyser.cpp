// src\SemanticAnalyser.cpp
#include "headers/SemanticAnalyser.hpp"
#include <stdexcept>
#include "headers/parser.hpp"
#include "headers/tokeniser.hpp"

/* --------------------------------------------------------------------------------------------- */

SemanticAnalyser::SemanticAnalyser() = default;

void SemanticAnalyser::enterScope()
{
	scopeStack.emplace_back();	 // add new empty scope object in scope vector
	// we fill it when we reach "stmts", in stmt funcs like declare within visitStmt()
}

void SemanticAnalyser::exitScope()
{
	scopeStack.pop_back();	// remove the last element [meaning exit]
}

[[noreturn]]
void SemanticAnalyser::error( const Location& loc, const std::string& msg )
{
	throw std::runtime_error( "Error at " + std::to_string( loc.line ) + ":" +
									  std::to_string( loc.column ) + " -> " + msg );
}
/* --------------------------------------------------------------------------------------------- */

// returns a pointer to the Symbol of an identifier, if that id exists in any active scope.
Symbol* SemanticAnalyser::lookup( const std::string& name )
{
	// Search for this variable starting from the closest scope, outward
	for ( auto rIter = scopeStack.rbegin(); rIter != scopeStack.rend(); ++rIter ) {
		// we're using reverse iteration to start from innermost scope to outermost scope
		// from child to parent, eventually to global scope
		auto found = rIter->symbols.find( name );
		// ↑ try to find id via name among symbols of that scope and return an iterator
		if ( found != rIter->symbols.end() ) {
			return &found->second;	// pointer to the Symbol stored in the map
		}
	}
	return nullptr;

	// rbegin() = Give me an iterator that starts at the LAST element
	// rend() = starts before first
}

/* --------------------------------------------------------------------------------------------- */

// to keep track of declarations
void SemanticAnalyser::declare( const std::string& name, const TokenType type )
{
	auto& curent = scopeStack.back().symbols;	 // get the latest scope

	if ( curent.contains( name ) ) {
		throw std::runtime_error( "Variable redeclared: " + name );
	}
	curent[ name ] = Symbol{ type };	 // if not redeclared, mk key and assign it the type

	/*
		curent[name]
			If key exists → returns reference to value
			If key doesn’t exist → creates it
		Symbol{ type }
			Aggregate initialisation
			Equivalent to:
				Symbol sym;
				sym.tType = type;
	*/
}

/* --------------------------------------------------------------------------------------------- */

// answers: What type does this expression evaluate to?
TokenType SemanticAnalyser::visitExpr( const Expr* expr )
{
	// # Number
	if ( auto num = dynamic_cast<const NumberExpr*>( expr ) ) {
		return TokenType::T_int;
	}
	// # String
	if ( auto str = dynamic_cast<const StringExpr*>( expr ) ) {
		return TokenType::T_string;
	}
	// # Identifier
	if ( const auto id = dynamic_cast<const IdentExpr*>( expr ) ) {  // if it has id
		const Symbol* sym = lookup( id->name );							  // check the entire scope-stack
		// ↑ get id [Pointer lets you express absence (nullptr)]
		if ( !sym ) {
			error( id->m_loc, "Use of undeclared variable: " + id->name );
		}
		return sym->tType;
		// basically, if identifier(symbol) exists return its type
	}
	// # bool
	if ( auto b = dynamic_cast<const BoolExpr*>( expr ) ) {
		return TokenType::T_bool;
	}

	// # Binary
	if ( const auto bin = dynamic_cast<const BinaryExpr*>( expr ) ) {
		const TokenType leftType = visitExpr( bin->left.get() );
		const TokenType rightType = visitExpr( bin->right.get() );
		// ↑ recursively ask what type, the stuff on both side is | (x+3)

		switch ( bin->operatr ) {
			// Arithmatic
		case TokenType::T_plus:
		case TokenType::T_minus:
		case TokenType::T_star:
		case TokenType::T_slash:
			if ( leftType != TokenType::T_int || rightType != TokenType::T_int ) {
				error( bin->m_loc, "Arithmetic operators require int operands" );
			}
			return TokenType::T_int;
		// Comparison
		case TokenType::T_GrT:
		case TokenType::T_LeT:
		case TokenType::T_GrTEq:
		case TokenType::T_LeTEq:
			if ( leftType != TokenType::T_int || rightType != TokenType::T_int ) {
				error( bin->m_loc, "Comparison requires int operands" );
			}
			return TokenType::T_bool;
		// Equality
		case TokenType::T_eqEq:
		case TokenType::T_NotE:
			if ( leftType != rightType ) {
				error( bin->m_loc, "Equality operands must be same Type" );
			}
			return TokenType::T_bool;
		default:
			error( bin->m_loc, "Unknown Binary Operator" );
		}
	}

	error( expr->m_loc, "Unknown expression type" );
}
/* --------------------------------------------------------------------------------------------- */
// This is a dispatcher that walks the AST and enforces semantic rules.
//  Semantics = meaning.

void SemanticAnalyser::visitStmt( const Stmt* stmt )
{
	// # declaration
	//  here it tries to cast, if it passes then we know it is varDecl. if not then ship to next 'if'
	if ( const auto v = dynamic_cast<const VarDeclStmt*>( stmt ) ) {
		const TokenType exprType = visitExpr( v->expr.get() );  // visitExpr returns a TokenType
		// get the expr type (like intLit/strLit etc) and compare
		if ( exprType != v->type ) {	// here v->type is the type decl like int,string,float
			error( v->m_loc, "Type mismatch in declaration of " + v->name );
		}
		declare( v->name, v->type );	// this stores it in current scope within scope-stack
		return;
	}
	// # assignment
	if ( const auto a = dynamic_cast<const AssignStmt*>( stmt ) ) {
		const Symbol* sym = lookup( a->name );	 // check if id exists or not
		if ( !sym ) {
			error( a->m_loc, "Assignment to undeclared variable: " + a->name );
		}
		const TokenType valueType = visitExpr( a->value.get() );	 // get the expr
		if ( valueType != sym->tType ) {
			error( a->m_loc, "Type mismatch in assignment to " + a->name );
		}
		return;
	}
	// # blocks
	if ( const auto b = dynamic_cast<const BlockStmt*>( stmt ) ) {
		enterScope();
		for ( auto& st : b->statements ) {
			visitStmt( st.get() );	// recursively check the inner stmts

			// What .get() does
			// Returns the raw pointer inside unique_ptr.
			// Ownership does not change.
		}
		exitScope();
		return;
	}
	// if
	if ( const auto i = dynamic_cast<const IfStmt*>( stmt ) ) {
		const auto condType = visitExpr( i->condition.get() );
		if ( condType != TokenType::T_bool ) {
			error( i->m_loc, "condition expression must evaluate to a boolean" );
		}
		visitStmt( i->thenBranch.get() );
		if ( i->elseBranch ) {
			visitStmt( i->elseBranch.get() );
		}
		return;
	}
	// while
	if ( const auto w = dynamic_cast<const WhileStmt*>( stmt ) ) {
		const auto condType = visitExpr( w->condition.get() );
		if ( condType != TokenType::T_bool ) {
			error( w->m_loc, "condition expression must evaluate to a boolean" );
		}
		visitStmt( w->loopBody.get() );
		return;
	}
	throw std::runtime_error( "Unknown Statement type" );
}

/* --------------------------------------------------------------------------------------------- */

void SemanticAnalyser::analyse( const std::vector<std::unique_ptr<Stmt>>& program )
{
	enterScope();	// global scope
	for ( auto& stmt : program ) {
		visitStmt( stmt.get() );
	}
	exitScope();
}
