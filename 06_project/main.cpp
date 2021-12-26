#include <iostream>
#include <fstream>
#include <bitset>
#include "Parser.h"
#include "Code.h"
#include "SymbolTable.h"

using namespace std;

int main() {
	/*Parser: ���� �ҷ��ͼ� �б� ����, �ʵ�� ��ȣ�� �и�*/
	/*Code: string ���� ��ȣ�� ��ȯ*/
	/*SymbolTable: ��ȣ ���̺��� ���ο� ��ȣ�� �߰��ϰų� ã��..*/

	Parser first_parse = Parser();
	SymbolTable sb_table = SymbolTable();

	int addressnum = 0;
	//1pass: ��ȣ ���̺� ����
	while (first_parse.hasMoreCommands()) {
		string lines = first_parse.advance();
		if (first_parse.commandType(lines) == 'L') {
			sb_table.addEntry(first_parse.symbol(), addressnum);
		}
		else if (first_parse.commandType(lines) == 'C' || first_parse.commandType(lines) == 'A') {
			addressnum++;
		}
	}
	//2pass
	Parser second_parse = Parser();
	Code code = Code();
	ofstream fout;
	
	int newaddnum = 16;
	string out_file_name = second_parse.filename.substr(0, second_parse.filename.length() - 4) + ".hack";
	fout.open(out_file_name);

	while (second_parse.hasMoreCommands()) {
		string lines = second_parse.advance();
		if (second_parse.commandType(lines) == 'A') {
			//fout << second_parse.commandType(lines) << "\n";
			fout << "0";
			if (second_parse.symbol().find_first_not_of("0123456789") == string::npos) {
				fout << bitset<15>(stoull(second_parse.symbol(), nullptr)).to_string();
			}
			else {
				sb_table.addEntry(second_parse.symbol(), newaddnum++);
				fout << bitset<15>(static_cast<unsigned long long>(sb_table.GetAddress(second_parse.symbol()))).to_string();
			}
			fout << endl;
		}
		else if (second_parse.commandType(lines) == 'C') {
			//fout << second_parse.commandType(lines) << "\n";
			fout << "111";
			fout << code.comp(second_parse.comp());
			fout << code.dest(second_parse.dest());
			fout << code.jump(second_parse.jump());
			fout << endl;
		}
	
	}

	fout.close();
	return 0;
}