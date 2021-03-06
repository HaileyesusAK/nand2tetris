#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "code_generator.hpp"
#include "symbol_table.hpp"
#include "tokenizer.hpp"
#include "vm_writer.hpp"

CodeGenerator::CodeGenerator(const fs::path& inputPath) :
	tokenizer(inputPath),
	vmWriter(inputPath) {}

void CodeGenerator::generate() {
    vmWriter.write();
}

/*
    doStatement : 'do' subroutineCall ';'
*/
void CodeGenerator::genDoStatement() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

    getKeyWord({"do"});
    genSubroutineCall();
	vmWriter.writePop(Segment::TEMP, 0);	//TODO: verify that the subroutine's return type is void
    getSymbol(";");
}

/*
	class	  : 'class' className '{' classVarDec* subroutineDec* '}'
	className : identifier
*/
void CodeGenerator::genClass() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	Set varDecKeywords {"field", "static"};
	Set subroutineDecKeywords {"constructor", "method", "function"};
	Token token;

	getKeyWord({"class"});
	auto identifier = getIdentifier();
    className = identifier.value;
	getSymbol("{");

	// generate varDec
	while(tokenizer.hasNext()) {
		try {
			token = getKeyWord(varDecKeywords);
			tokenizer.putBack();
		}
		catch (std::domain_error& err) {
			break;
		}

		genClassVarDec();
	}

	// generate subroutineDec
	while(tokenizer.hasNext()) {
		try {
			token = getKeyWord(subroutineDecKeywords);
			tokenizer.putBack();
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
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	Token token;
	Set ops {"+", "-", "*", "/", "&", "|", "<", ">", "="};

    genTerm();
	while(tokenizer.hasNext()) {
		token = tokenizer.getNext();
		if(ops.count(token.value)) {
            genTerm();
			if(token.value == "*")
				vmWriter.writeCall("Math.multiply", 2);
			else if(token.value == "/")
				vmWriter.writeCall("Math.divide", 2);
			else
				vmWriter.writeArithmetic(getArithCmd(token.value));
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
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

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
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif
	
	++ifCount; // Enter into a new scope

	std::string ifTrueLabel {"IF_TRUE" + std::to_string(ifCount)};
	std::string ifFalseLabel {"IF_FALSE" + std::to_string(ifCount)};
	std::string ifEndLabel {"IF_END" + std::to_string(ifCount)};

	getKeyWord({"if"});
	getSymbol("(");
	genExp();
	getSymbol(")");
    vmWriter.writeIf(ifTrueLabel);
	vmWriter.writeGoto(ifFalseLabel);
	vmWriter.writeLabel(ifTrueLabel);
	getSymbol("{");
	genStatements();
	getSymbol("}");

	if(tokenizer.hasNext()) {
		auto token = tokenizer.getNext();
		if(token.value == "else") {
			vmWriter.writeGoto(ifEndLabel);
			vmWriter.writeLabel(ifFalseLabel);
			getSymbol("{");
			genStatements();
			vmWriter.writeLabel(ifEndLabel);
			getSymbol("}");
		}
		else {
			vmWriter.writeLabel(ifFalseLabel);
			tokenizer.putBack();
		}
	}
	else
		vmWriter.writeLabel(ifFalseLabel);
}

/*
	letStatement : 'let' varName ('[' expression ']')? '=' expression ';'
	varName		 : identifier
*/
void CodeGenerator::genLetStatement() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

    getKeyWord({"let"});
	auto identifier = getIdentifier();

	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

    SymbolTableEntry symbolEntry = symbolTable.getEntry(identifier.value);
    Segment segment = kindToSegment(symbolEntry.kind);

	auto token = tokenizer.getNext();
	if(token.value == "[") { // Array element assignment
		genExp();
        vmWriter.writePush(segment, symbolEntry.index);
        vmWriter.writeArithmetic(Command::ADD); // Add array base address and index expression
		getSymbol("]");
		getSymbol("=");
		genExp();
		vmWriter.writePop(Segment::TEMP, 0);
        vmWriter.writePop(Segment::POINTER, 1);
		vmWriter.writePush(Segment::TEMP, 0);
        vmWriter.writePop(Segment::THAT, 0); // Assign the evaluated expression
	}
	else {
		tokenizer.putBack();
		getSymbol("=");
		genExp();
        vmWriter.writePop(segment, symbolEntry.index);
	}
	getSymbol(";");
}


/*
	parameterList : ((type varName)(',' type varName)*)?
	type		  : 'int' | 'char' | 'boolean' | className
	className	  : identifier
	varName		  : identifier
*/
void CodeGenerator::genParameterList() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	getSymbol("(");
	auto token = tokenizer.getNext();
	tokenizer.putBack();
	if(token.value != ")") {
		auto type = getType();
		auto identifier = getIdentifier();
        symbolTable.insert(identifier.value, type.value, SymbolKind::ARGUMENT);

		while(tokenizer.hasNext()) {
			token = tokenizer.getNext();
			if(token.value == ",") {
				type = getType();
				identifier = getIdentifier();
                symbolTable.insert(identifier.value, type.value, SymbolKind::ARGUMENT);
			}
			else {
				tokenizer.putBack();
				break;
			}
		}
	}
	getSymbol(")");
}

/*
    returnStatement : 'return' expression? ';'
*/
void CodeGenerator::genReturnStatement() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

    getKeyWord({"return"});
	if(currentSubroutineType == SubroutineType::CONSTRUCTOR) {
		getKeyWord({"this"});
		vmWriter.writePush(Segment::POINTER, 0);
	}
	else {
		try {
			genExp();
		}
		catch (std::domain_error& exp) {
			vmWriter.writePush(Segment::CONST, 0);	//value for void return type
		}
	}
    getSymbol(";");

	vmWriter.writeReturn();
}

/*
    statements : statement*
    statement  : letStatement | ifStatement | whileStatement | doStatement | returnStatement
*/
void CodeGenerator::genStatements() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

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
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	getSymbol("{");

	uint16_t nLocals = 0;
	while(tokenizer.hasNext()) {
		auto token = tokenizer.getNext();
		tokenizer.putBack();
		if(token.value == "var") {
			nLocals += genVarDec();
		}
		else {
			break;
		}
	}

	vmWriter.writeFunction(currentSubroutineName, nLocals);

	switch(currentSubroutineType) {
		case SubroutineType::CONSTRUCTOR: {
			auto nFields = symbolTable.count(SymbolKind::FIELD);
			vmWriter.writePush(Segment::CONST, nFields);
			vmWriter.writeCall("Memory.alloc", 1);
			vmWriter.writePop(Segment::POINTER, 0); // Set's the base address of THIS
		}
		break;

		case SubroutineType::METHOD: {
			/*
				When compiling a Jack method into a VM function, the compiler must insert
				VM code that sets the base of the this segment properly.
			*/
			vmWriter.writePush(Segment::ARGUMENT, 0);
			vmWriter.writePop(Segment::POINTER, 0);
		}
		break;

		default:
			break;
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
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	std::string subroutineName, identifier;
	uint16_t nArgs;
    auto token = getIdentifier();
	identifier = token.value;

	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

    token = tokenizer.getNext();
    if(token.value == "(") {
		//A call to the current object's method
		vmWriter.writePush(Segment::POINTER, 0); // push 'this' as the first argument
        nArgs = genExpList() + 1; // +1 for 'this'
		subroutineName = className + "." + identifier;
    }
    else {
        tokenizer.putBack();
		getSymbol(".");
        auto methodName = getIdentifier().value;
        getSymbol("(");

		try {
			auto symbolEntry = symbolTable.getEntry(identifier);
			subroutineName = symbolEntry.type + "." + methodName;
			vmWriter.writePush(Segment::THIS, symbolEntry.index);	// push a reference to the object as a first argument
			nArgs = genExpList() + 1; // +1 for the object pushed as the first argument
		}
		catch (std::out_of_range& err) {
			//At this point 'identifier' is assumed to be a class Name
			subroutineName = identifier + "." + methodName;
			nArgs = genExpList();
		}
    }

	getSymbol(")");
	//TODO: verify the number and types of the arguments
    vmWriter.writeCall(subroutineName, nArgs);
}

/*
	subroutineDec	: ('constructor' | 'method' | 'function')
					  ('void' | type) subroutineName '(' parameterList ')' subroutineBody
	type			: ('int' | 'char' | 'boolean' | className)
	subroutineName	: identifier
	className		: identifier
*/
void CodeGenerator::genSubroutineDec() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	static Set keywords {"constructor", "method", "function"};
	static std::unordered_map<std::string, SubroutineType> subroutineTypeMap {
		{"constructor", SubroutineType::CONSTRUCTOR},
		{"method", SubroutineType::METHOD},
		{"function", SubroutineType::FUNCTION}
	};
	auto keyword = getKeyWord(keywords);

	Token type;
	symbolTable.clear(Scope::SUBROUTINE);
	ifCount = -1; 
	whileCount = -1; 

	try {
		type = getType();
	}
	catch (std::domain_error& err) {
		type = getKeyWord({"void"});
	}

	auto token = getIdentifier();
    currentSubroutineName = className + "." + token.value;
	switch(subroutineTypeMap[keyword.value]) {
		case SubroutineType::CONSTRUCTOR: {
			genParameterList();
			currentSubroutineType = SubroutineType::CONSTRUCTOR;
		}
		break;

		case SubroutineType::FUNCTION: {
			genParameterList();
			currentSubroutineType = SubroutineType::FUNCTION;
		}
		break;

		case SubroutineType::METHOD: {
			symbolTable.insert("this", className, SymbolKind::ARGUMENT);
			genParameterList();
			currentSubroutineType = SubroutineType::METHOD;
		}
		break;
	}

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
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	static Set keywordConstant {"true", "false", "null", "this"};

	auto token = tokenizer.getNext();
	switch(token.type) {
		case TokenType::INTEGER:
			vmWriter.writePush(Segment::CONST, std::stoul(token.value));
		break;

		case TokenType::KEYWORD:
			if(keywordConstant.count(token.value)) {
				if(token.value == "true") {
					vmWriter.writePush(Segment::CONST, 1);
					vmWriter.writeArithmetic(Command::NEG);
				}
				else if(token.value == "false" || token.value == "null")
					vmWriter.writePush(Segment::CONST, 0);
				else
					vmWriter.writePush(Segment::POINTER, 0);
			}
			else {
				tokenizer.putBack();
				std::string msg("Unexpected token");
				appendInputLine(msg, token.lineNo, token.columnNo);
				throw std::domain_error(msg);
			}
		break;
		
		case TokenType::STRING:
			vmWriter.writePush(Segment::CONST, token.value.size());
			vmWriter.writeCall("String.new", 1);
			for(const auto& c : token.value) {
				vmWriter.writePush(Segment::CONST, static_cast<uint16_t>(c));
				vmWriter.writeCall("String.appendChar", 2); // First argument is for 'this' of the string object
			}
		break;

		case TokenType::IDENTIFIER:
			if(tokenizer.hasNext()) {
				std::string identifier {token.value};
				token = tokenizer.getNext();
				if(token.value == "(" || token.value == ".") { // Subroutine call
					tokenizer.putBack(); // Put back the symbol
					tokenizer.putBack(); // Put back the identifier
					genSubroutineCall();
				}
				else {
					auto symbolEntry = symbolTable.getEntry(identifier);
					auto segment = kindToSegment(symbolEntry.kind);

					if(token.value == "[") { // Array evaluation
						genExp();
						vmWriter.writePush(segment, symbolEntry.index);
						vmWriter.writeArithmetic(Command::ADD);
						vmWriter.writePop(Segment::POINTER, 1);	// Set array element address
						vmWriter.writePush(Segment::THAT, 0); // Assign the evaluated expression
						getSymbol("]");
					}
					else {
						vmWriter.writePush(segment, symbolEntry.index);
						tokenizer.putBack();
					}
				}
			}
			else {
				auto symbolEntry = symbolTable.getEntry(token.value);
				auto segment = kindToSegment(symbolEntry.kind);
				vmWriter.writePush(segment, symbolEntry.index);
			}
		break;

		default:	// TokenType::SYMBOL:
			if(token.value == "(") {
				genExp();
				getSymbol(")");
			}
			else if(token.value == "-") {
				genTerm();
				vmWriter.writeArithmetic(Command::NEG);
			}
			else if(token.value == "~") {
				genTerm();
				vmWriter.writeArithmetic(Command::NOT);
			}
			else {
				tokenizer.putBack();
				std::string msg("Unexpected token");
				appendInputLine(msg, token.lineNo, token.columnNo);
				throw std::domain_error(msg);
			}
		break;
	}
}

/*
	classVarDec	: (static | field) type varName (',' varName)* ;
	type		: 'int' | 'char' | 'boolean' | className
	varName		: identifier
	className	: identifier
*/
uint16_t CodeGenerator::genClassVarDec() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	auto keyword = getKeyWord({"static", "field"});
    if(keyword.value == "static")
        return genVarDecList(SymbolKind::STATIC);
    else
        return genVarDecList(SymbolKind::FIELD);
}

/*
	varDec	   : var type varName (',' varName)* ;
	type	   : 'int' | 'char' | 'boolean' | className
	varName	   : identifier
	className  : identifier
*/
uint16_t CodeGenerator::genVarDec() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	getKeyWord({"var"});
	return genVarDecList(SymbolKind::LOCAL);
}

/*
	varDecList : type varName (',' varName)* ;
	type	   : 'int' | 'char' | 'boolean' | className
	varName	   : identifier
	className  : identifier
*/
uint16_t CodeGenerator::genVarDecList(const SymbolKind& kind) {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	Token token;
	auto type = getType();
	auto identifier = getIdentifier();
	uint16_t nVars = 1;

	symbolTable.insert(identifier.value, type.value, kind);

	while(tokenizer.hasNext()) {
		token = tokenizer.getNext();
		if(token.value == ",") {
			identifier = getIdentifier();
	        symbolTable.insert(identifier.value, type.value, kind);
			nVars++;
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

	return nVars;
}

void CodeGenerator::genWhileStatement() {
	#ifdef DEBUG
		report(__PRETTY_FUNCTION__);
	#endif

	++whileCount; // Enter into a new scope
	std::string whileBeginLabel {"WHILE_EXP" + std::to_string(whileCount)};
	std::string whileEndLabel {"WHILE_END" + std::to_string(whileCount)};

    getKeyWord({"while"});
	getSymbol("(");
    vmWriter.writeLabel(whileBeginLabel);
	genExp();
    vmWriter.writeArithmetic(Command::NOT); //Easier to work after negating the expression's result
	getSymbol(")");
    vmWriter.writeIf(whileEndLabel);
	getSymbol("{");
	genStatements();
    vmWriter.writeGoto(whileBeginLabel);
	getSymbol("}");
    vmWriter.writeLabel(whileEndLabel);
}

Command CodeGenerator::getArithCmd(const std::string& op) {
	static const std::unordered_map<std::string, Command> opInst {
		{"+", Command::ADD},
		{"-", Command::SUB},
		{"&", Command::AND},
		{"|", Command::OR},
		{"<", Command::LT},
		{">", Command::GT},
		{"=", Command::EQ}
	};

    return opInst.at(op);
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
		std::string msg("expected '" + symbol + "' but found '" + token.value + "'");
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

Segment CodeGenerator::kindToSegment(const SymbolKind& kind) {
    Segment segment;

    switch(kind) {
        case SymbolKind::STATIC:
            segment = Segment::STATIC;
        break;

        case SymbolKind::LOCAL:
            segment = Segment::LOCAL;
        break;

        case SymbolKind::ARGUMENT:
            segment = Segment::ARGUMENT;
        break;

        case SymbolKind::FIELD:
            segment = Segment::THIS;
        break;
    }

    return segment;
}

void CodeGenerator::report(const std::string& caller) {
	std::cout << "Called " << caller;
	if(tokenizer.hasNext()) {
		auto token = tokenizer.getNext();
		tokenizer.putBack();
		std::cout << " with token " << token << std::endl;
	}
	else
		std::cout << std::endl;
}
