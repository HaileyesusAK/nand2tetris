#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "code_generator.hpp"
#include "tokenizer.hpp"

CodeGenerator::CodeGenerator(const fs::path& _inputPath) : inputPath(_inputPath), tokenizer(_inputPath) { }

/*
    doStatement : 'do' subroutineCall ';'
*/
void CodeGenerator::genDoStatement() {
    getKeyWord({"do"});
    genSubroutineCall();
    getSymbol(";");
}

/*
	class	  : 'class' className '{' classVarDec* subroutineDec* '}'
	className : identifier
*/
void CodeGenerator::genClass() {
	Set varDecKeywords {"field", "static"};
	Set subroutineDecKeywords {"constructor", "method", "function"};
	Token token;

	getKeyWord({"class"});
	auto identifier = getIdentifier();
	getSymbol("{");

	// generate varDec
	while(tokenizer.hasNext()) {
		try {
			token = getKeyWord(varDecKeywords);
		}
		catch (std::domain_error& err) {
			break;
		}

		genVarDecList(token.value);
	}

	// generate subroutineDec
	while(tokenizer.hasNext()) {
		try {
			token = getKeyWord(subroutineDecKeywords);
		}
		catch (std::domain_error& err) {
			break;
		}

		genSubroutineDec();
	}

	getSymbol("}");
}

/*
    expression : term (op term)*
    term       : '+' |  '-' |  '*' |  '/' |  '&' |  '|' |  '<' |  '>' |  '='
*/
void CodeGenerator::genExp() {
	Token token;
	Set ops {"+", "-", "*", "/", "&", "|", "<", ">", "="};

    genTerm();
	while(tokenizer.hasNext()) {
		token = tokenizer.getNext();
		if(ops.count(token.value)) {
            genTerm();
			output.push_back(getBinOpInst(token.value));
		}
		else {
			tokenizer.putBack();
			break;
		}
	}
}

/*
    expressionList : (expression (',' expression)*)?
*/
uint16_t CodeGenerator::genExpList() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	uint16_t expCount = 0;
	auto token = tokenizer.getNext();
	tokenizer.putBack();
	if(token.value != ")")
	{
		genExp();
		expCount = 1;
		while(tokenizer.hasNext()) {
			token = tokenizer.getNext();
			if(token.value == ",") {
				++expCount;
				genExp();
			}
			else {
				tokenizer.putBack();
				break;
			}
		}
	}

	return expCount;
}

/*
    ifStatement : 'if' '(' expression ')' '{' statements '}'
                  ('else' '{' statements '}')?
*/
void CodeGenerator::genIfStatement() {
	static uint32_t i;
	std::string endLabel {"(IF_END_" + std::to_string(i)};
	std::string elseLabel {"(ELSE_" + std::to_string(i)};

	getKeyWord({"if"});
	getSymbol("(");
	genExp();
	output.push_back("neg");	//Easier to work after negating the expression's result

	getSymbol(")");
	output.push_back("if-goto " + elseLabel);
	getSymbol("{");
	genStatements();
	getSymbol("}");
	output.push_back("(" + elseLabel + ")");
	if(tokenizer.hasNext()) {
		auto token = tokenizer.getNext();
		if(token.value == "else") {
			getSymbol("{");
			genStatements();
			getSymbol("}");
		}
		else
			tokenizer.putBack();
	}
	output.push_back("(" + endLabel + ")");
	++i;
}

/*
	letStatement : 'let' varName ('[' expression ']')? '=' expression ';'
	varName		 : identifier
*/
void CodeGenerator::genLetStatement() {
    getKeyWord({"let"});
	auto identifier = getIdentifier();

	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto symbol = resolveSymbol(identifier.value);
	output.push_back("push " + symbol.kind + " " + std::to_string(symbol.index));

	auto token = tokenizer.getNext();
	if(token.value == "[") { // Array element assignment
		genExp();
		output.push_back("add");	// Add array base address and index expression
		getSymbol("]");
	}
	else
		tokenizer.putBack();

	output.push_back("pop pointer 1"); // Save the variable's or array element's address

	getSymbol("=");
	genExp();
	output.push_back("pop that 0"); // Assign the evaluated expression
	getSymbol(";");
}


/*
	parameterList : ((type varName)(',' type varName)*)?
	type		  : 'int' | 'char' | 'boolean' | className
	className	  : identifier
	varName		  : identifier
*/
void CodeGenerator::genParameterList() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	tokenizer.putBack();
	if(token.value != ")") {
		uint16_t index = 0;
		auto type = getType();
		auto identifier = getIdentifier();
		subroutineSymbols[identifier.value] = {identifier.value, type.value, "argument", index};

		while(tokenizer.hasNext()) {
			token = tokenizer.getNext();
			if(token.value == ",") {
				getSymbol(",");
				type = getType();
				identifier = getIdentifier();
				subroutineSymbols[identifier.value] = {identifier.value, type.value, "argument", ++index};
			}
			else {
				tokenizer.putBack();
				break;
			}
		}
	}
}

/*
    returnStatement : 'return' expression? ';'
*/
void CodeGenerator::genReturnStatement() {
    getKeyWord({"return"});
    try {
        genExp();
    }
    catch (std::domain_error& exp) { }
    getSymbol(";");
}

/*
    statements : statement*
    statement  : letStatement | ifStatement | whileStatement | doStatement | returnStatement
*/
void CodeGenerator::genStatements() {
	Token token;
	std::unordered_map<std::string, void(CodeGenerator::*)()> generatorMap {
		{"do", &CodeGenerator::genDoStatement},
		{"if", &CodeGenerator::genIfStatement},
		{"let", &CodeGenerator::genLetStatement},
		{"return", &CodeGenerator::genReturnStatement},
		{"while", &CodeGenerator::genWhileStatement}
	};

	while(tokenizer.hasNext()) {
		token = tokenizer.getNext();
        tokenizer.putBack();
		try {
			auto generator = generatorMap.at(token.value);
			(this->*(generator))();
		}
		catch(std::out_of_range& ex) {
			break;
		}
	}
}

/*
	subroutineBody : ('{' varDec* statements '}')
*/
void CodeGenerator::genSubroutineBody() {
	getSymbol("{");

	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	while(tokenizer.hasNext()) {
		auto token = tokenizer.getNext();
		if(token.value == "var") {
			genVarDecList("local");
		}
		else {
			tokenizer.putBack();
			break;
		}
	}

	genStatements();
	getSymbol("}");
}

/*
    subroutineCall : subroutineName '(' expressionList ')' |
                     (className | varName) '.' subroutineName '(' expressionList ')'
    subroutineName : indentifier
    varName        : identifier
    className      : identifier
*/
void CodeGenerator::genSubroutineCall() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	std::string methodName;
	uint16_t nArgs;

    auto token = getIdentifier();
	methodName = token.value;
    token = tokenizer.getNext();
    if(token.value == "(") {
        nArgs = genExpList();
        getSymbol(")");
    }
    else {
        tokenizer.putBack();
		getSymbol(".");
        token = getIdentifier();
        getSymbol("(");
        nArgs = genExpList();
		methodName += "." + token.value;
        getSymbol(")");
    }

	output.push_back("call " + methodName + " " + std::to_string(nArgs));
}

/*
	subroutineDec	: ('constructor' | 'method' | 'function')
					  ('void' | type) subroutineName '(' parameterList ')' subroutineBody
	type			: ('int' | 'char' | 'boolean' | className)
	subroutineName	: identifier
	className		: identifier
*/
void CodeGenerator::genSubroutineDec() {
	Token token;

	subroutineSymbols.clear();
	try {
		getType();
	}
	catch (std::domain_error& err) {
		getKeyWord({"void"});
	}

	token = getIdentifier();
	getSymbol("(");
	genParameterList();
	getSymbol(")");
	genSubroutineBody();
}

/*
	term			: integerConstant | stringConstant | keywordConstant | varName |
					  varName '[' expresion ']' | subroutineCall | '(' expression ')' | unaryOp term
	keywordConstant : 'true' | 'false' | 'null' | 'this'
	subroutineCall	: subroutineName '(' expressionList ')' |
					  (className | varName) '.' subroutineName '(' expressionList ')'
	subroutineName	: identifier
*/
void CodeGenerator::genTerm() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	Set keywordConstant {"true", "false", "null", "this"};
	auto token = tokenizer.getNext();
	if(token.type == TokenType::INTEGER)
		output.push_back("push constant " + static_cast<uint16_t>(std::stoul(token.value)));
	else if(token.type == TokenType::STRING) {
		//TODO: how to push a string literal
	}
	else if(keywordConstant.count(token.value)) {
		//TODO: how to push a keywordConstant
	}
	else if(token.type == TokenType::IDENTIFIER) {
		const auto& symbol = resolveSymbol(token.value);
		if(tokenizer.hasNext()) {
			token = tokenizer.getNext();
			if(token.value == "[") { // Array evaluation
				output.push_back("push " + symbol.kind + " " + std::to_string(symbol.index));
				genExp();
				output.push_back("add");
				getSymbol("]");
			}
			else if(token.value == "(" || token.value == ".") { // Subroutine call
				tokenizer.putBack(); // Put back the symbol
				tokenizer.putBack(); // Put back the identifier
				genSubroutineCall();
			}
            else {
				output.push_back("push " + symbol.kind + " " + std::to_string(symbol.index));
                tokenizer.putBack();
			}
		}
		else {
			output.push_back("push " + symbol.kind + " " + std::to_string(symbol.index));
		}
	}
	else if(token.value == "(") {
		genExp();
        getSymbol(")");
	}
	else if(token.value == "-") {
		genTerm();
		output.push_back("neg");
	}
	else if(token.value == "~") {
		genTerm();
		output.push_back("not");
	}
	else {
        tokenizer.putBack();
		std::string msg("Unexpected token");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
    }
}

/*
	varDecList : type varName (',' varName)* ;
	type	   : 'int' | 'char' | 'boolean' | className
	varName	   : identifier
	className  : identifier
*/
void CodeGenerator::genVarDecList(const std::string& kind) {
	uint16_t index = 0;
	Token token;
	auto type = getType();
	auto identifier = getIdentifier();

	subroutineSymbols[identifier.value] = {identifier.value, type.value, kind, index};

	while(tokenizer.hasNext()) {
		token = tokenizer.getNext();
		if(token.value == ",") {
			identifier = getIdentifier();
			subroutineSymbols[identifier.value] = {identifier.value, type.value, kind, ++index};
		}
		else if(token.value == ";") {
			tokenizer.putBack();
			break;
		}
		else {
			std::string msg("Unexpected token in variable declaration");
			appendInputLine(msg, token.lineNo, token.columnNo);
			throw std::domain_error(msg);
		}
	}

	getSymbol(";");
}


void CodeGenerator::genWhileStatement() {
	static uint32_t i;
	std::string whileBeginLabel {"BEGIN_WHILE_" + std::to_string(i)};
	std::string whileEndLabel {"END_WHILE_" + std::to_string(i)};

    getKeyWord({"while"});
	getSymbol("(");
	output.push_back("(" + whileBeginLabel + ")");
	genExp();
	output.push_back("neg");	//Easier to work after negating the expression's result
	getSymbol(")");
	output.push_back("if-goto " + whileEndLabel);
	getSymbol("{");
	genStatements();
	output.push_back("(" + whileBeginLabel +")");
	getSymbol("}");
	output.push_back("(" + whileEndLabel +")");
	++i;
}

std::string CodeGenerator::getBinOpInst(const std::string& op) {
	static const std::unordered_map<std::string, std::string> opInst {
		{"+", "add"},
		{"-", "sub"},
		{"*", "mul"},
		{"&", "and"},
		{"|", "or"},
		{"<", "lt"},
		{">", "gt"},
		{"=", "eq"}
	};

	if(op == "/") {
		return ""; //TODO: return commands that implement division
	}
	else {
		return opInst.at(op);
	}
}

Token CodeGenerator::getIdentifier() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	if(token.type != TokenType::IDENTIFIER) {
		tokenizer.putBack();
		std::string msg("Invalid identifier");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
	}
	return token;
}

Token CodeGenerator::getType() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	static Set types {"int", "char", "boolean"};
	if(token.type != TokenType::IDENTIFIER && !types.count(token.value)) {
		tokenizer.putBack();
		std::string msg("Uknown type");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
	}
	return token;
}

Token CodeGenerator::getSymbol(const std::string& symbol) {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	if(token.type != TokenType::SYMBOL || token.value != symbol) {
		tokenizer.putBack();
		std::string msg("Uknown symbol");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
	}

	return token;
}

Token CodeGenerator::getKeyWord(const Set& keywords) {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	if(token.type != TokenType::KEYWORD) {
		tokenizer.putBack();
		std::string msg("Invalid keyword");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
	}

	if(!keywords.count(token.value)) {
		tokenizer.putBack();
		std::string msg("Unexpected token");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
	}

	return token;
}

void CodeGenerator::appendInputLine(std::string& s, size_t lineNo, size_t columnNo) {
	std::ostringstream os;
	os << " at (" << lineNo << ", " << columnNo << ")" << std::endl;
	os << tokenizer.getLine(lineNo - 1) << std::endl;
	os << std::setw(columnNo) << std::setfill('-') << "^" << std::endl;
	s.append(os.str());
}
