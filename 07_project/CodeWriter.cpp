#include "CodeWriter.h"

using namespace std;

CodeWriter::CodeWriter(string f_name) {
	filename = f_name;
}
void CodeWriter::setFileName() {
	string out_file_name = filename.substr(0, filename.length() - 3) + ".asm";
	fout.open(out_file_name);
}
void CodeWriter::writerArithmetic(string command) {
	if (command == "add" || command == "sub" || command == "and" || command == "or"){
		stk_pop();
		decSP();
		op(command);
		incSP();
	}
	else if (command == "neg") {
		stk_pop();
		fout << "D=-D" << endl << "M=D" << endl;
		incSP();
	}
	else if (command == "not") {
		stk_pop();
		fout << "D=!D" << endl << "M=D" << endl;
		incSP();
	}
	else if (command == "eq") {
		/*(eq/lt/gt)Num�� ����� ����*/
		eqNum++;
		stk_pop();
		decSP();
		fout << "D=M-D" << endl;
		fout << "@EQ" << eqNum << endl;
		fout << "D;JEQ" << endl;

		/*������ �������� �ʴ� ��� - ���ÿ� 0 �ֱ�(���� ���� �ǳʶٴ� ���)*/
		stk_push(0);
		fout << "@ENDEQ" << eqNum << endl;
		fout << "0;JMP" << endl;

		/*���� �����ϸ� ���ÿ� -1 �ֱ�*/
		fout << "(EQ" << eqNum << ")" << endl;
		stk_push(-1);

		/*���� �����ϵ� �� �ϵ� ����, �׳� ǥ�ø�(���ٸ� �ڵ� �ʿ�ġ X)*/
		fout << "(ENDEQ" << eqNum << ")" << endl;
	}
	else if (command == "gt") {
		gtNum++;
		stk_pop();
		decSP();
		fout << "D=M-D" << endl;
		fout << "@GT" << gtNum << endl;
		fout << "D;JGT" << endl;
		stk_push(0);
		fout << "@ENDGT" << gtNum << endl;
		fout << "0;JMP" << endl;
		fout << "(GT" << gtNum << ")" << endl;
		stk_push(-1);
		fout << "(ENDGT" << gtNum << ")" << endl;
	}
	else if (command == "lt") {
		ltNum++;
		stk_pop();
		decSP();
		fout << "D=M-D" << endl;
		fout << "@LT" << ltNum << endl;
		fout << "D;JLT" << endl;
		stk_push(0);
		fout << "@ENDLT" << ltNum << endl;
		fout << "0;JMP" << endl;
		fout << "(LT" << ltNum <<")"<< endl;
		stk_push(-1);
		fout << "(ENDLT" << ltNum << ")" << endl;
	}
}
void CodeWriter::stk_push(int x) {
	fout << "@SP" << endl << "A=M" << endl;
	fout << "M=" << x << endl;
	incSP();
}
void CodeWriter::stk_pop() {
	decSP();
	fout << "D=M" << endl;
}
void CodeWriter::decSP() {
	fout << "@SP" << endl << "AM=M-1" << endl;
}
void CodeWriter::incSP() {
	fout << "@SP" << endl << "AM=M+1" << endl;
}
void CodeWriter::op(string command) {
	/*sub�� ��� M-D�ӿ� ����!*/
	if (command == "add") {
		fout<<"D=D"<<"+"<<"M"<<endl<<"M=D"<<endl;
	}
	else if (command == "sub") {
		fout << "D=M" << "-" << "D" << endl << "M=D" << endl;
	}
	else if (command == "and") {
		fout << "D=D" << "&" << "M" << endl << "M=D" << endl;
	}
	else if (command == "or") {
		fout << "D=D" << "|" << "M" << endl << "M=D" << endl;
	}
}
void CodeWriter::WritePushPop(string command, string segment, int index) {
	if (command == "C_PUSH") {
		if(segment == "argument"){
			fout << "@ARG"<< endl << "D = M" << endl;
			fout << "@" << index << endl << "D=D+A" << endl << "A=D" << endl << "D=M" << endl;// -> D�� push�ؾߵ� ���� �������. ��, (ARG+index)�� ����� ��
			fout << "@SP" << endl << "A=M" << endl << "M=D" << endl; //���ÿ� D�� push
			incSP();
		}
		else if (segment == "local") {
			fout << "@LCL" << endl << "D = M" << endl;
			fout << "@" << index << endl << "D=D+A" << endl << "A=D" << endl << "D=M" << endl;// -> D�� push�ؾߵ� ���� �������
			fout << "@SP" << endl << "A=M" << endl << "M=D" << endl;
			incSP();
		}
		else if (segment == "static") {
			fout << "@" << filename << "." << index << endl << "D=M" << endl;
			fout << "@SP" << endl << "A=M" << endl << "M=D" << endl;
			incSP();
		}
		else if (segment == "constant") {
			fout << "@"<<index << endl << "D=A" << endl; //���� D�� ������ ���� ����
			fout << "@SP" << endl << "A=M" << endl << "M=D" << endl; //stk_push(D)
			incSP();
		}
		else if (segment == "this") {
			fout << "@THIS" << endl << "D=M" << endl;
			fout << "@" << index << endl << "D=D+A" << endl << "A=D" << endl << "D=M" << endl;// -> D�� push�ؾߵ� ���� �������
			fout << "@SP" << endl << "A=M" << endl << "M=D" << endl;
			incSP();
		}
		else if (segment == "that") {
			fout << "@THAT" << endl << "D=M" << endl;
			fout << "@" << index << endl << "D=D+A" << endl << "A=D" << endl << "D=M" << endl;// -> D�� push�ؾߵ� ���� �������
			fout << "@SP" << endl << "A=M" << endl << "M=D" << endl;
			incSP();
		}
		else if (segment == "pointer") {
			fout << "@3" << endl << "D=A" << endl;
			fout << "@" << index << endl << "D=D+A" << endl << "A=D" << endl << "D=M" << endl;// -> D�� push�ؾߵ� ���� �������
			fout << "@SP" << endl << "A=M" << endl << "M=D" << endl;
			incSP();
		}
		else if (segment == "temp") {
			fout << "@5" << endl << "D=A" << endl;
			fout << "@" << index << endl << "D=D+A" << endl << "A=D" << endl << "D=M" << endl;// -> D�� push�ؾߵ� ���� �������
			fout << "@SP" << endl << "A=M" << endl << "M=D" << endl;
			incSP();
		}
	}
	else if (command == "C_POP") {
		if (segment == "argument") {
			fout << "@ARG" << endl << "D = M" << endl << "@" << index << endl << "D = D + A" << endl; //D�� ������ �ּ�
			fout << "@R13" << endl << "M=D" << endl; //�ӽ�����
			stk_pop();
			fout << "@R13" << endl << "A=M" << endl << "M=D" << endl; //D�� ���� ������
		}
		else if (segment == "local") {
			fout << "@LCL" << endl << "D = M" << endl << "@" << index << endl << "D = D + A" << endl;
			fout << "@R13" << endl << "M=D" << endl;
			stk_pop();
			fout << "@R13" << endl << "A=M" << endl << "M=D" << endl;
		}
		else if (segment == "static") {
			fout << "@" << filename << "." << index << endl;
			fout << "D=A" << endl << "@R13" << endl << "M=D" << endl; //R[13]�� D �ӽ�����
			stk_pop(); //���� ����
			fout << "@R13" << endl << "A=M" << endl << "M=D" << endl; //�װ��� �ٽ� R13�� ����
		}
		else if (segment == "this") {
			fout << "@THIS" << endl << "D = M" << endl << "@" << index << endl << "D = D + A" << endl;
			fout << "@R13" << endl << "M=D" << endl;
			stk_pop();
			fout << "@R13" << endl << "A=M" << endl << "M=D" << endl;
		}
		else if (segment == "that") {
			fout << "@THAT" << endl << "D = M" << endl << "@" << index << endl << "D = D + A" << endl;
			fout << "@R13" << endl << "M=D" << endl;
			stk_pop();
			fout << "@R13" << endl << "A=M" << endl << "M=D" << endl;
		}
		else if (segment == "pointer") {
			fout << "@3" << endl << "D = A" << endl << "@" << index << endl << "D = D + A" << endl;
			fout << "@R13" << endl << "M=D" << endl;
			stk_pop();
			fout << "@R13" << endl << "A=M" << endl << "M=D" << endl;
		}
		else if (segment == "temp") {
			fout << "@5" << endl << "D = A" << endl << "@" << index << endl << "D = D + A" << endl;
			fout << "@R13" << endl << "M=D" << endl;
			stk_pop();
			fout << "@R13" << endl << "A=M" << endl << "M=D" << endl;
		}
	}
}
void CodeWriter::Close() {
	fout << "(END)" << endl << "@END" << endl << "0;JMP" << endl;
	fout.close();
}