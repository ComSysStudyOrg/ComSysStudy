#include "CompilationEngine.h"

CompilationEngine::CompilationEngine(filesystem::path fpath) {
	tokenizer = new JackTokenizer(fpath);
	//tokenizer->test();//��ūȭ ��� �׽�Ʈ
	
	filesystem::path outpath = fpath.parent_path();
	outpath += "\\test\\";
	outpath += fpath.stem().concat(".xml");
	outputStream.open(outpath);

	CompileClass();
}

void CompilationEngine::getToken() {
	if (tokenizer->hasMoreTokens())
		tokenizer->advance();
}

void CompilationEngine::CompileClass() {
	outputStream << "<class>\n";
	cout << "<class>\n";

	getToken();
	keyword();// 'class'
	getToken();												// ������� �׳� advance�� �κе� �� getToken���� �ٲٰų� hasmoreToken ����� ��
	identifier();// className
	getToken();
	symbol();// '{'
	while (true) {
		getToken();
		if (tokenizer->tokenType() == KEYWORD) {
			switch (tokenizer->keyword()) {
			case STATIC:
			case FIELD:
				CompileClassVarDec();// classVarDec*
				break;
			case CONSTRUCTOR:
			case FUNCTION:
			case METHOD:
				CompileSubroutine();// subroutineDec*
				break;
			default: break;
			}
		}
		else break;
	}
	symbol();// '}'

	outputStream << " </class>";
	cout << " </class>";
	outputStream.close();
}

void CompilationEngine::CompileClassVarDec() {
	outputStream << "<classVarDec>\n";

	keyword();// 'static'|'field'
	//type
	getToken();
	if (tokenizer->tokenType() == KEYWORD) keyword();// 'int'|'char'|'boolean'
	else identifier();// className

	//varName (',' varName)*
	do {
		getToken();
		identifier();// varName
		getToken();
		if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ';') break;
		symbol();// ','
	} while (true);
	symbol();// ';'

	outputStream << "</classVarDec>\n";
}

void CompilationEngine::CompileSubroutine() {
	outputStream << "<subroutineDec>\n";

	keyword();// 'constructor'|'function'|'method'
	// ('void'|type)
	getToken();
	if (tokenizer->tokenType() == KEYWORD) keyword();// 'void'|'int'|'char'|'boolean'
	else identifier();// className

	getToken();
	identifier();// subroutineName
	getToken();
	symbol();// '('
	compileParameterList();// parameterList
	symbol();// ')'

	// subroutineBody
	outputStream << "<subroutineBody>\n";
	getToken();
	symbol();// '{'
	// varDec*
	while (true) {
		getToken();
		if (tokenizer->tokenType() == KEYWORD && tokenizer->keyword() == VAR) {
			compileVarDec();
		}
		else break;
	}
	// ��ū �ϳ� �̸� ��������
	// statements
	compileStatements();
	symbol();// '}'
	outputStream << "</subroutineBody>\n";

	outputStream << "</subroutineDec>\n";
}

void CompilationEngine::compileParameterList() {
	outputStream << "<parameterList>\n";

	while (true) {
		getToken();
		if (tokenizer->tokenType() == SYMBOL)
			if (tokenizer->symbol() == ')') {
				outputStream << "</parameterList>\n";
				return;// �Ű����� ���� ���
			}
			else if (tokenizer->symbol() == ',') {
				symbol();// ','
				getToken();
			}
		// type
		if (tokenizer->tokenType() == KEYWORD) keyword();// 'int'|'char'|'boolean'
		else identifier();// className;
		// varName
		getToken();
		identifier();
	}
}

void CompilationEngine::compileVarDec() {
	outputStream << "<varDec>\n";

	keyword();// 'var'
	// type
	getToken();
	if (tokenizer->tokenType() == KEYWORD) keyword();// 'int'|'char'|'boolean'
	else identifier();// className;
	// varName
	getToken();
	identifier();
	while (true) {
		getToken();
		if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ';') break;
		symbol();// ','
		getToken();
		identifier();// varName
	}
	symbol();// ';'

	outputStream << "</varDec>\n";
}

void CompilationEngine::compileStatements() {
	outputStream << "<statements>\n";

	// statement*
	while (true) {
		// statement
		switch (tokenizer->keyword()) {
		case LET:
			compileLet();
			getToken();
			break;
		case IF:
			compileIf();// if���� ��� else Ȯ�� ������ �̹� ���� ��ū advance��
			break;
		case WHILE:
			compileWhile();
			getToken();
			break;
		case DO:
			compileDo();
			getToken();
			break;
		case RETURN:
			compileReturn();
			getToken();
			break;
		}
		if (tokenizer->tokenType() == SYMBOL) break;// statements ��
	}

	outputStream << "</statements>\n";
}

void CompilationEngine::compileDo() {
	outputStream << "<doStatement>\n";

	keyword();// 'do'
	// subroutineCall
	getToken();
	identifier();// subroutineName|className|varName
	getToken();
	if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == '.') {// className|varName
		symbol();// '.'
		getToken();
		identifier();// subroutineName
		getToken();
	}
	symbol();// '('
	CompileExpressionList();// expressionList
	symbol();// ')'
	getToken();
	symbol();// ';'

	outputStream << "</doStatement>\n";
}

void CompilationEngine::compileLet() {
	outputStream << "<letStatement>\n";

	keyword();// 'let'
	getToken();
	identifier();// varName
	getToken();
	if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == '[') {
		symbol();// '['
		CompileExpression();// expression
		getToken();;// ������ �������� ����!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		symbol();// ']'
		getToken();
	}
	symbol();// '='
	CompileExpression();// expression
	getToken();// ������ �������� ����!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	symbol();// ';'

	outputStream << "</letStatement>\n";
}

void CompilationEngine::compileWhile() {
	outputStream << "<whileStatement>\n";

	keyword();// 'while'
	// (expression)
	getToken();
	symbol();// '('
	CompileExpression();// expression
	getToken();// ������ �������� ����!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	symbol();// ')'

	// {statements}
	getToken();
	symbol();// '{'
	getToken();
	compileStatements();// statements
	symbol();// '}'

	outputStream << "</whileStatement>\n";
}

void CompilationEngine::compileReturn() {
	outputStream << "<returnStatement>\n";

	keyword();// 'return'
	// expression?
	getToken();
	if (tokenizer->tokenType() == SYMBOL) {
		symbol();// ';'
		outputStream << "</returnStatement>\n";
		return;
	}
	outputStream << "<expression>\n";
	outputStream << "<term>\n";
	identifier();
	outputStream << "</term>\n";
	outputStream << "</expression>\n";
	//CompileExpression();// expression
	getToken();// ������ �������� ����!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	symbol();// ';'

	outputStream << "</returnStatement>\n";
}

void CompilationEngine::compileIf() {
	outputStream << "<ifStatement>\n";

	keyword();// 'if'
	// (expression)
	getToken();
	symbol();// '('
	CompileExpression();// expression
	getToken();// ������ �������� ����!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	symbol();// ')'

	// {statements}
	getToken();
	symbol();// '{'
	getToken();
	compileStatements();// statements
	symbol();// '}'

	// (else {statements})?
	getToken();
	if (tokenizer->tokenType() == KEYWORD && tokenizer->keyword() == ELSE) {
		keyword();// 'else'
		// {statements}
		getToken();
		symbol();// '{'
		getToken();
		compileStatements();// statements
		symbol();// '}'

		getToken();// statements �Լ����� ���� ��ū �о���� �� �����ֱ� ����
	}

	outputStream << "</ifStatement>\n";
}

void CompilationEngine::CompileExpression() {
	//return�� ¥�鼭 ���ϱ� expression�� ��ū �ϳ� ���� ���¿��� �����ϵ��� ¥���� ��
	//expression ȣ���ϴ� �κе� ����!
	outputStream << "<expression>\n";
	outputStream << "<term>\n";
	getToken();
	identifier();// ǥ���� ���� ��� �׽�Ʈ �뵵
	outputStream << "</term>\n";
	outputStream << "</expression>\n";
}

void CompilationEngine::CompileTerm() {}

void CompilationEngine::CompileExpressionList() {
	outputStream << "<expressionList>\n";
	while (true) {
		getToken();
		if (tokenizer->tokenType() == IDENTIFIER) {
			outputStream << "<expression>\n";
			outputStream << "<term>\n";
			identifier();
			outputStream << "</term>\n";
			outputStream << "</expression>\n";
		}
		else if (tokenizer->tokenType() == KEYWORD) {
			outputStream << "<expression>\n";
			outputStream << "<term>\n";
			keyword();
			outputStream << "</term>\n";
			outputStream << "</expression>\n";
		}
		else if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ',') symbol();// ','
		else break;
	}// ǥ���� ���� ��� �׽�Ʈ �뵵
	outputStream << "</expressionList>\n";
}

void CompilationEngine::keyword() {
	switch (tokenizer->keyword()) {
	case CLASS:
		outputStream << "<keyword> class </keyword>\n";
		cout << "<keyword> class </keyword>\n";
		break;
	case METHOD:
		outputStream << "<keyword> method </keyword>\n";
		cout << "<keyword> method </keyword>\n";
		break;
	case FUNCTION:
		outputStream << "<keyword> function </keyword>\n";
		cout << "<keyword> function </keyword>\n";
		break;
	case CONSTRUCTOR:
		outputStream << "<keyword> constructor </keyword>\n";
		cout << "<keyword> constructor </keyword>\n";
		break;
	case INT:
		outputStream << "<keyword> int </keyword>\n";
		cout << "<keyword> int </keyword>\n";
		break;
	case BOOLEAN:
		outputStream << "<keyword> boolean </keyword>\n";
		cout << "<keyword> boolean </keyword>\n";
		break;
	case CHAR:
		outputStream << "<keyword> char </keyword>\n";
		cout << "<keyword> char </keyword>\n";
		break;
	case VOID:
		outputStream << "<keyword> void </keyword>\n";
		cout << "<keyword> void </keyword>\n";
		break;
	case VAR:
		outputStream << "<keyword> var </keyword>\n";
		cout << "<keyword> var </keyword>\n";
		break;
	case STATIC:
		outputStream << "<keyword> static </keyword>\n";
		cout << "<keyword> static </keyword>\n";
		break;
	case FIELD:
		outputStream << "<keyword> field </keyword>\n";
		cout << "<keyword> field </keyword>\n";
		break;
	case LET:
		outputStream << "<keyword> let </keyword>\n";
		cout << "<keyword> let </keyword>\n";
		break;
	case DO:
		outputStream << "<keyword> do </keyword>\n";
		cout << "<keyword> do </keyword>\n";
		break;
	case IF:
		outputStream << "<keyword> if </keyword>\n";
		cout << "<keyword> if </keyword>\n";
		break;
	case ELSE:
		outputStream << "<keyword> else </keyword>\n";
		cout << "<keyword> else </keyword>\n";
		break;
	case WHILE:
		outputStream << "<keyword> while </keyword>\n";
		cout << "<keyword> while </keyword>\n";
		break;
	case RETURN:
		outputStream << "<keyword> return </keyword>\n";
		cout << "<keyword> return </keyword>\n";
		break;
	case TRUE:
		outputStream << "<keyword> true </keyword>\n";
		cout << "<keyword> true </keyword>\n";
		break;
	case FALSE:
		outputStream << "<keyword> false </keyword>\n";
		cout << "<keyword> false </keyword>\n";
		break;
	case KEY_NULL:
		outputStream << "<keyword> null </keyword>\n";
		cout << "<keyword> null </keyword>\n";
		break;
	case THIS:
		outputStream << "<keyword> this </keyword>\n";
		cout << "<keyword> this </keyword>\n";
		break;

	}
}

void CompilationEngine::symbol() {
	outputStream << "<symbol> " << tokenizer->symbol() << " </symbol>\n";
	cout << "<symbol> " << tokenizer->symbol() << " </symbol>\n";
}

void CompilationEngine::integerConstant() {
	outputStream << "<integerConstant> " << tokenizer->intVal() << " </integerConstant>\n";
	cout << "<integerConstant> " << tokenizer->intVal() << " </integerConstant>\n";
}

void CompilationEngine::stringConstant() {
	outputStream << "<stringConstant> " << tokenizer->stringVal() << " </stringConstant>\n";
	cout << "<stringConstant> " << tokenizer->stringVal() << " </stringConstant>\n";
}

void CompilationEngine::identifier() {
	outputStream << "<identifier> " << tokenizer->identifier() << " </identifier>\n";
	cout << "<identifier> " << tokenizer->identifier() << " </identifier>\n";
}