#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "analyzer.hpp"
#include "tokenizer.hpp"

Analyzer::Analyzer(const fs::path& _inputPath, size_t _tabWidth) :
	inputPath(_inputPath), tabWidth(_tabWidth), tokenizer(_inputPath) {
}

void Analyzer::writeXml() {
	fs::path path(inputPath);
	path.replace_extension(".xml");
	std::ofstream os(path);
	os << output;
}

std::string Analyzer::convertXmlSymbol(const std::string& symbol){
    static std::unordered_map<std::string, std::string> conversionMap {
        {">", "&gt;"},
        {"<", "&lt;"},
        {"&", "&amp;"},
    };

    if (conversionMap.count(symbol))
        return conversionMap[symbol];
    else
        return symbol;
}

/*
	parameter	: (type varName)
	type		: 'int' | 'char' | 'boolean' | className
	className	: identifier
	varName		: identifier
*/
void Analyzer::genParameter() {
	genType();
	genIdentifier();
}

/*
	parameterList : ((type varName)(',' type varName)*)?
	type		  : 'int' | 'char' | 'boolean' | className
	className	  : identifier
	varName		  : identifier
*/
void Analyzer::genParameterList() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	printLine("<parameterList>");
	++level;

	auto token = tokenizer.getNext();
	tokenizer.putBack();
	if(token.value != ")") {
		genParameter();
		while(tokenizer.hasNext()) {
			token = tokenizer.getNext();
			if(token.value == ",") {
				printLine("<symbol> , </symbol>");
				genParameter();
			}
			else {
				tokenizer.putBack();
				break;
			}
		}
	}

	--level;
	printLine("</parameterList>");
}

/*
	varDecList : type varName (',' varName)* ;
	type	   : 'int' | 'char' | 'boolean' | className
	varName	   : identifier
	className  : identifier

*/
void Analyzer::genVarDecList() {
	genType();
	genIdentifier();

	Token token;
	std::string s;

	while(tokenizer.hasNext()) {
		token = tokenizer.getNext();
		if(token.value == ",") {
			printLine("<symbol> , </symbol>");
			genIdentifier();
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

	genSymbol(";");
}

/*
	varDec : 'var' varDecList
*/
void Analyzer::genVarDec() {
	printLine("<varDec>");
	++level;

	genKeyWord({"var"});
	genVarDecList();

	--level;
	printLine("</varDec>");
}

/*
	classVarDec : ('static' | 'field') type	varName(','varName)* ';'
	type		: ('int' | 'char' | 'boolean' | className)
	className	: identifier
*/
void Analyzer::genClassVarDec() {
	printLine("<classVarDec>");
	++level;

	genKeyWord({"static", "field"});
	genVarDecList();

	--level;
	printLine("</classVarDec>");
}

/*
	class	  : 'class' className '{' classVarDec* subroutineDec* '}'
	className : identifier
*/

void Analyzer::genClass() {
	printLine("<class>");
	++level;

	genKeyWord({"class"});
	genIdentifier();
	genSymbol("{");

	// generate varDec
	while(tokenizer.hasNext()) {
		auto token = tokenizer.getNext();
		tokenizer.putBack();
		Set varDecKeywords {"field", "static"};
		if(varDecKeywords.count(token.value))
			genClassVarDec();
		else
			break;
	}

	// generate subroutineDec
	while(tokenizer.hasNext()) {
		auto token = tokenizer.getNext();
		tokenizer.putBack();
		Set subroutineDecKeywords {"constructor", "method", "function"};
		if(subroutineDecKeywords.count(token.value))
			genSubroutineDec();
		else
			break;
	}

	genSymbol("}");

	--level;
	printLine("</class>");
}

/*
	subroutineDec	: ('constructor' | 'method' | 'function')
					  ('void' | type) subroutineName '(' parameterList ')' subroutineBody
	type			: ('int' | 'char' | 'boolean' | className)
	subroutineName	: identifier
	className		: identifier
*/

void Analyzer::genSubroutineDec() {
	Token token;
	printLine("<subroutineDec>");
	++level;

	genKeyWord({"constructor", "method", "function"});
	try {
		genType();
	}
	catch (std::domain_error& ex) {
		genKeyWord({"void"});
	}

	genIdentifier();
	genSymbol("(");
	genParameterList();
	genSymbol(")");
	genSubroutineBody();

	--level;
	printLine("</subroutineDec>");
}

/*
	subroutineBody : ('{' varDec* statements '}')
*/
void Analyzer::genSubroutineBody() {
	printLine("<subroutineBody>");
	++level;

	genSymbol("{");

	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	while(tokenizer.hasNext()) {
		auto token = tokenizer.getNext();
		if(token.value == "var") {
			tokenizer.putBack();
			genVarDec();
		}
		else {
			tokenizer.putBack();
			break;
		}
	}

	genStatements();
	genSymbol("}");

	--level;
	printLine("</subroutineBody>");
}

/*
    subroutineCall : subroutineName '(' expressionList ')' |
                     (className | varName) '.' subroutineName '(' expressionList ')'
    subroutineName : indentifier
    varName        : identifier
    className      : identifier
*/
void Analyzer::genSubroutineCall() {
    genIdentifier();
    if(!tokenizer.hasNext())
        throw std::out_of_range("No more tokens");

    auto token = tokenizer.getNext();
    if(token.value == "(") {
        printLine("<symbol> ( </symbol>");
        genExpList();
        genSymbol(")");
    }
    else {
        tokenizer.putBack();
        genSymbol(".");
        genIdentifier();
        genSymbol("(");
        genExpList();
        genSymbol(")");
    }
}


/*
    expressionList : (expression (',' expression)*)?
*/
void Analyzer::genExpList() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

    printLine("<expressionList>");
	++level;

	auto token = tokenizer.getNext();
	tokenizer.putBack();
	if(token.value != ")")
	{
		genExp();
		while(tokenizer.hasNext()) {
			token = tokenizer.getNext();
			if(token.value == ",") {
				printLine("<symbol> , </symbol>");
				genExp();
			}
			else {
				tokenizer.putBack();
				break;
			}
		}
	}

    --level;
    printLine("</expressionList>");
}

/*
    expression : term (op term)*
    term       : '+' |  '-' |  '*' |  '/' |  '&' |  '|' |  '<' |  '>' |  '='
*/
void Analyzer::genExp() {
	Token token;
	Set ops {"+", "-", "*", "/", "&", "|", "<", ">", "="};

    auto prevp = printLine("<expression>");
    ++level;
    try {
        genTerm();
    }
    catch (std::exception& exp) {
        --level;
        rewind(prevp);
        throw;
    }

	while(tokenizer.hasNext()) {
		token = tokenizer.getNext();
		if(ops.count(token.value)) {
			printLine("<symbol> " + convertXmlSymbol(token.value) + " </symbol>");
            genTerm();
		}
		else {
			tokenizer.putBack();
			break;
		}
	}
    --level;
    printLine("</expression>");
}

/*
	term			: integerConstant | stringConstant | keywordConstant | varName |
					  varName '[' expresion ']' | subroutineCall | '(' expression ')' | unaryOp term
	keywordConstant : 'true' | 'false' | 'null' | 'this'
	subroutineCall	: subroutineName '(' expressionList ')' |
					  (className | varName) '.' subroutineName '(' expressionList ')'
	subroutineName	: identifier
*/
void Analyzer::genTerm() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	Set keywordConstant {"true", "false", "null", "this"};

    auto prevp = printLine("<term>");
    ++level;

	if(token.type == TokenType::INTEGER)
		printLine("<integerConstant> " + token.value + " </integerConstant>");
	else if(token.type == TokenType::STRING)
		printLine("<stringConstant> " + token.value + " </stringConstant>");
	else if(keywordConstant.count(token.value))
		printLine("<keyword> " + token.value + " </keyword>");
	else if(token.type == TokenType::IDENTIFIER) {
		printLine("<identifier> " + token.value + " </identifier>");
		if(tokenizer.hasNext()) {
			token = tokenizer.getNext();
			if(token.value == "[") {
				printLine("<symbol> [ </symbol>");
				genExp();
				genSymbol("]");
			}
			else if(token.value == "(") {
				printLine("<symbol> ( </symbol>");
				genExpList();
				genSymbol(")");
			}
			else if(token.value == ".") {
				printLine("<symbol> . </symbol>");
				genIdentifier();
				genSymbol("(");
				genExpList();
				genSymbol(")");
			}
            else
                tokenizer.putBack();
		}
	}
	else if(token.value == "(") {
        printLine("<symbol> ( </symbol>");
		genExp();
        genSymbol(")");
	}
	else if(token.value == "-" || token.value == "~") {
		printLine("<symbol> " + token.value + " </symbol>");
		genTerm();
	}
	else {
        --level;
        rewind(prevp);
        tokenizer.putBack();
		std::string msg("Unexpected token");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
    }

    --level;
    printLine("</term>");
}

/*
	letStatement : 'let' varName ('[' expression ']')? '=' expression ';'
	varName		 : identifier
*/
void Analyzer::genLetStatement() {
	printLine("<letStatement>");
	++level;

    genKeyWord({"let"});
	genIdentifier();

	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	if(token.value == "[") {
		printLine("<symbol> [ </symbol>");
		genExp();
		genSymbol("]");
	}
	else
		tokenizer.putBack();

	genSymbol("=");
	genExp();

	genSymbol(";");

	--level;
	printLine("</letStatement>");
}

/*
    doStatement : 'do' subroutineCall ';'
*/
void Analyzer::genDoStatement() {
	printLine("<doStatement>");
    ++level;

    genKeyWord({"do"});
    genSubroutineCall();
    genSymbol(";");

    --level;
	printLine("</doStatement>");
}

/*
    returnStatement : 'return' expression? ';'
*/
void Analyzer::genReturnStatement() {
	printLine("<returnStatement>");
    ++level;

    genKeyWord({"return"});
    try {
        genExp();
    }
    catch (std::domain_error& exp) { }
    genSymbol(";");

    --level;
	printLine("</returnStatement>");
}

/*
    ifStatement : 'if' '(' expression ')' '{' statements '}'
                  ('else' '{' statements '}')?
*/
void Analyzer::genIfStatement() {
	printLine("<ifStatement>");
	++level;

	genKeyWord({"if"});
	genSymbol("(");
	genExp();
	genSymbol(")");
	genSymbol("{");
	genStatements();
	genSymbol("}");

	if(tokenizer.hasNext()) {
		auto token = tokenizer.getNext();
		if(token.value == "else") {
			printLine("<keyword> else </keyword>");
			genSymbol("{");
			genStatements();
			genSymbol("}");
		}
		else
			tokenizer.putBack();
	}

	--level;
	printLine("</ifStatement>");
}

/*
	whileStatement : 'while' '(' expression ')' '{' statements '}'
*/
void Analyzer::genWhileStatement() {
	printLine("<whileStatement>");
	++level;

    genKeyWord({"while"});
	genSymbol("(");
	genExp();
	genSymbol(")");
	genSymbol("{");
	genStatements();
	genSymbol("}");

	--level;
	printLine("</whileStatement>");
}

/*
    statements : statement*
    statement  : letStatement | ifStatement | whileStatement | doStatement | returnStatement
*/
void Analyzer::genStatements() {
	Token token;
	std::unordered_map<std::string, void(Analyzer::*)()> generatorMap {
		{"do", &Analyzer::genDoStatement},
		{"if", &Analyzer::genIfStatement},
		{"let", &Analyzer::genLetStatement},
		{"return", &Analyzer::genReturnStatement},
		{"while", &Analyzer::genWhileStatement}
	};

	std::string openingTag("statements");
	auto prevp = printLine("<" + openingTag + ">");

	++level;
	int numStatements = 0;
	while(tokenizer.hasNext()) {
		token = tokenizer.getNext();
        tokenizer.putBack();
		try {
			auto generator = generatorMap.at(token.value);
			(this->*(generator))();
			++numStatements;
		}
		catch(std::out_of_range& ex) {
			break;
		}
	}
	--level;

	if(numStatements)
		printLine("</" + openingTag + ">");
	else
        rewind(prevp);
}

size_t Analyzer::printLine(const std::string& line) {
    std::string tab(tabWidth * level, ' ');
    output.append(tab + line + "\n");
    return output.size() - (line.size() + tab.size() + 1);
}

void Analyzer::rewind(size_t pos) {
    output.erase(pos);
}

void Analyzer::genIdentifier() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	if(token.type != TokenType::IDENTIFIER) {
		tokenizer.putBack();
		std::string msg("Invalid identifier");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
	}

	printLine("<identifier> " + token.value + " </identifier>");
}

void Analyzer::genType() {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	Set types {"int", "char", "boolean"};
	if(token.type != TokenType::IDENTIFIER && !types.count(token.value)) {
		tokenizer.putBack();
		std::string msg("Uknown type");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
	}

	std::string tag = (token.type == TokenType::IDENTIFIER) ? "identifier" : "keyword";
	printLine("<" + tag + "> " + token.value + " </" + tag + ">");
}

void Analyzer::genSymbol(const std::string& symbol) {
	if(!tokenizer.hasNext())
		throw std::out_of_range("No more tokens");

	auto token = tokenizer.getNext();
	if(token.type != TokenType::SYMBOL || token.value != symbol) {
		tokenizer.putBack();
		std::string msg("Uknown symbol");
		appendInputLine(msg, token.lineNo, token.columnNo);
		throw std::domain_error(msg);
	}

	printLine("<symbol> " + convertXmlSymbol(symbol) + " </symbol>");
}

void Analyzer::genKeyWord(const Set& keywords) {
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

	printLine("<keyword> " + token.value + " </keyword>");
}

void Analyzer::appendInputLine(std::string& s, size_t lineNo, size_t columnNo) {
	std::ostringstream os;
	os << " at (" << lineNo << ", " << columnNo << ")" << std::endl;
	os << tokenizer.getLine(lineNo - 1) << std::endl;
	os << std::setw(columnNo) << std::setfill('-') << "^" << std::endl;
	s.append(os.str());
}
