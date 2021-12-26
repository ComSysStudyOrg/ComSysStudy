#include "Parser.h"

using namespace std;
/*
	������ �Լ� �̸� �� ����� å+a
	in: file�� �о�鿩�� ����� ����� �ʵ�� ��ȣ�� ������
	out: 1. ����� �ּ� ����
		 2. Parser�� ����� ������ ������ ��� ���� -> dest, jump, comp, 
*/
Parser::Parser() {
	//������ open�ϰ� �б� ������
	// .asm Ȯ�������� Ȯ���ϰ� �ƴϸ� �ٽ� input �ޱ� -> fileopen
	string file_name;
	cin >> file_name;
	///���ڿ� ��ġ �˻� rfind �̿�, Ȯ���� ã��
	while (file_name.rfind(".asm") == string::npos) {
		cout << "�ùٸ� ���ϸ��� �ƴ�. �ٽ� �Է��ϼ���\n";
		cin >> file_name;
	}

	filename = file_name;
	file.open(file_name);
	
}
bool Parser::hasMoreCommands() {
	//eof�̸� false, eof�� �ƴϸ� true
	return !file.eof();
}
string Parser::advance() {
	//hasMoreCommands�� ���̸�
	//���� �ٷ� �Ѿ
	//���� �ؼ��ϰ� �ִ� line ������ �ӽ÷� ��Ƶ�
	//��, line���� �ּ��� ������ ������
	string line;
	if(hasMoreCommands()){
		getline(file, line);
		line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
		//cout << line<<'\n';
		return line;
	}
}

char Parser::commandType(string line) {
	//"//"�� �����ϴ� ���, �ּ�ó��(X)
	unsigned long commentPos = line.find("//");
	if (commentPos != string::npos) {
		line.erase(commentPos, line.length() - commentPos);
		command_type = 'X';
		return command_type;
	}
	currentLine = line;
	//line[0]�� "@"�� ���, A��ɾ�
	if (line.front() == '@') {
		command_type = 'A';
		return command_type;

	}
	//line[0]�� "("�� ���, L��ɾ�
	else if (line.front() == '(') {
		command_type = 'L';
		return command_type;
	}
	//else, C��ɾ�
	else
	{
		command_type = 'C';
		return command_type;
	}
}

string Parser::symbol() {
	//A�� �� @����
	if (command_type == 'A') {
		return currentLine.substr(1, currentLine.length() - 1);
	}
	//L�� �� ()����
	unsigned long leftPos = currentLine.rfind('(');
	unsigned long rightPos = currentLine.find(')');
	if (leftPos != string::npos && rightPos != string::npos && command_type == 'L') {
		return currentLine.substr(leftPos + 1, rightPos - leftPos - 1);
	}
	return "";
}
/*
dest=cmp, dest=comp;jump, comp;jump�� ��� ����
;������ jump ����
=������ dest ����
*/
string Parser::dest() {
	if (command_type == 'C') {
		unsigned long equalPos;
		equalPos = currentLine.find("=");
		if (equalPos != string::npos) {
			return currentLine.substr(0, equalPos);
		}
	}
	return "";
}
string Parser::comp() {
	if (command_type == 'C') {
		unsigned long equalPos;
		unsigned long semicPos;
		
		equalPos = currentLine.find('=');
		semicPos = currentLine.find(';');
		
		if (equalPos != string::npos) {
			if (semicPos != string::npos) {
				return currentLine.substr(equalPos + 1, semicPos - equalPos - 1);
			}
			else {
				return currentLine.substr(equalPos + 1, currentLine.length() - equalPos - 1);
			}
		}
		else if (semicPos != string::npos) {
			return currentLine.substr(0, semicPos);
		}
	}
	return "";
}
string Parser::jump() {
	if (command_type == 'C') {
		unsigned long semicPos;
		semicPos = currentLine.find(';');
		if (semicPos != string::npos) {
			return currentLine.substr(semicPos + 1, currentLine.length() - semicPos - 1);
		}
	}
	return "";
}
/*
int main() {
	Parser parse = Parser();
	while (parse.hasMoreCommands()) {
		string lines = parse.advance();
		
		cout<<parse.commandType(lines)<<'\n';
		cout << parse.currentLine << '\n';
		cout << parse.symbol() << '\n';
		cout << parse.dest() << '\n';
		cout << parse.comp() << '\n';
		cout << parse.jump() << '\n';
	}
}
*/