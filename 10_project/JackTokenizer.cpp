#include "JackTokenizer.h"

using namespace std;

JackTokenizer::JackTokenizer(string filename) {
	file.open(filename);
}

bool JackTokenizer::hasMoreLines(){
	//������ ������ �κ��� �ƴϸ鼭, ���� ���� ������ ���� ���
	return (!file.eof());
}

void JackTokenizer::advance() {
	//���� ��ū�� �޾Ƽ� ���� ��ū���� ����
	getline(file, line);

	int end = line.length();
	int start = 0, div = 0;
	line_tokens.clear();
	
	while (div < end) {
		if (isdigit(line[div]) || isalpha(line[div]) || line[div] == '"') {
			if (line[div] == '"') {
				int first_pos = line.find('"');
				int second_pos = line.rfind('"');
				token = line.substr(first_pos, second_pos - first_pos + 1);
				line_tokens.push_back(token);
				div = div + (second_pos - first_pos + 1);
				start = div;
			}
			else
			{
				div++;
			}
		}
		else {
			if (div != start) {
				token = line.substr(start, div - start);
				line_tokens.push_back(token);
				start = div;
			}
			else {
				if (!isspace(line[div])) {
					token = (line[div]);
					line_tokens.push_back(token);
				}
				div++;
				start = div;
			}
		}
	}
}

string JackTokenizer::tokenType(){
	auto a = find(keyword_token.begin(), keyword_token.end(), cur_token);
	if (a != keyword_token.end()) return "KEYWORD";

	auto b = find(symbol_token.begin(), symbol_token.end(), cur_token[0]);
	if (b != symbol_token.end()) return "SYMBOL";
	
	if ('0' <= cur_token[0] && cur_token[0] <= '9')	return "INT_CONST";

	else if (cur_token[0] == '"') {
		cur_token = cur_token.substr(1, cur_token.length() - 2);
		return "STRING_CONST";
	}

	return "IDENTIFIER";
}

string JackTokenizer::keyword() {
		return cur_token;
}
char JackTokenizer::symbol(){
	//if (tokenType()=="SYMBOL") {
		return cur_token[0];
	//}
}
string JackTokenizer::identifier() {
	//if (tokenType()=="IDENTIFIER") {
		return cur_token;
	//}
}
int JackTokenizer::intVal() {
	//if (tokenType()=="INT_CONST") {
		return stoi(cur_token);
	//}
}
string JackTokenizer::stringVal() {
	//if (tokenType()=="STRING_CONST") {
		return cur_token;
	//}
}

string JackTokenizer::curToken() {
	if (hasMoreToken()) {
		cur_token = line_tokens.front();
		line_tokens.erase(line_tokens.begin());
		return cur_token;
	}
	return "";
}

bool JackTokenizer::hasMoreToken() {
	return(!line_tokens.empty());
}

int main() {
	JackTokenizer jt = JackTokenizer("Main.jack");
	while (jt.hasMoreLines()) {
		//�� �� �о��, ��ū ����Ǿ�����
		jt.advance();

		cout << "print line tokens: ";
		while (jt.hasMoreToken()) {
			jt.curToken();

			//cout <<jt.cur_token<<" ";
			if(jt.tokenType()=="KEYWORD")	cout<<jt.keyword();
			if (jt.tokenType() == "SYMBOL")	cout<<jt.symbol();
			if (jt.tokenType() == "IDENTIFIER")	cout<<jt.identifier();
			if (jt.tokenType() == "INT_CONST")	cout<<jt.intVal();
			if (jt.tokenType() == "STRING_CONST")cout << jt.stringVal();
			cout << endl;
		}
		cout << endl;
	}
	return 0;
}