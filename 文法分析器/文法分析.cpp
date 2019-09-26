#define  _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
using namespace std;

FILE *input, *output, *error, *preerror; //�����ļ����룬�ļ��������������ļ�ָ��
char *ch = (char*)malloc(sizeof(char) * 300); //�����ֱ�����
char b[3] = "\0"; //�����﷨��������
char forcast[3] = "\0"; //����Ԥ���������
char Word[30][17] = { "\0" }; //����洢��ʶ���ͳ�������
int aLength = 0; //��Ԫ���г���
int line = 1; //����
char analyze[3] = "\0"; // ��ǰʶ����ֱ�
int analyzeLine = 0; //��ǰ�ֱ�λ��
int step = 1; //��ǰʶ��Ĳ�����
int count = 0; //�洢��ʶ���ͳ����ĸ���
int outputCount = 0; //��ǰ��ʶ���ͳ�����λ��
int errorCount = 0; //�������
void errorFunction(int);

int levGlobe = 1; //��ǰ�����ĺ������
char levStack[11][17] = { "\0" }; //�洢����������ǰ����֮ǰ�ĺ�����
int ready = 0;

class variable //������
{
public:
	char vname[17] = "\0"; //������
	char vproc[17] = "\0"; //��������
	int vkind; //�������β�
	char vtype[17] = "\0"; //��������
	int vlev; //�������
	int vadr; //�����ڱ������е�λ��
	void createVariable(char vname[17], char vproc[17], int vkind, char vtype[17], int vlev, int vadr)
	{
		strcat(this->vname, vname);
		strcat(this->vproc, vproc);
		this->vkind = vkind;
		strcat(this->vtype, vtype);
		this->vlev = vlev;
		this->vadr = vadr;
	}
};
variable var[30];
int varLength = 0;


class process //������
{
public:
	char pname[17] = "\0"; //������
	char ptype[17] = "\0"; //��������
	int plev = -1; //���̲��
	int fadr = -1; //��һ�������ڱ������е�λ��
	int ladr = -1; //���һ�������ڱ������е�λ��
	char fatherproc[17] = "\0"; //��������
	void crreateProcess(char pname[17], char ptype[17], int plev, char fatherproc[17])
	{
		strcat(this->pname, pname);
		strcat(this->ptype, ptype);
		this->plev = plev;
		strcat(this->fatherproc, fatherproc);
	}
};
process pro[10];
int proLength = 0;

void addProcess(char pname[17], char ptype[17], int plev, char fatherproc[17]) //��ӹ���
{
	int error = 0;
	for (int i = 0; i < proLength; i++)
	{
		if (strcmp(pro[i].pname, pname) == 0 && pro[i].plev == levGlobe && strcmp(pro[i].fatherproc, fatherproc) == 0)
			error = 1;
	}
	if (error == 1) //����Ƿ����ظ��������
	{
		errorFunction(3);
	}
	pro[proLength++].crreateProcess(pname, ptype, plev, fatherproc);
	printf("dddd\n");
}

void addVariable(int n, int offset) //��ӱ���
{
	int error = 0;
	for (int i = 0; i < varLength; i++)
	{
		if (strcmp(var[i].vname, Word[outputCount - offset]) == 0 && var[i].vlev == levGlobe && strcmp(var[i].vproc, levStack[levGlobe - 1]) == 0)
		{
			error = 1;
		}
	}
	if (error == 1) //����Ƿ����ظ��������
	{
		errorFunction(1);
	}
	var[varLength++].createVariable(Word[outputCount - offset], levStack[levGlobe - 1], n, "integer", levGlobe, varLength - 1);
}

void searchFadrAndLadr(int n) //��ӹ��̵�fadr��ladr
{
	int first = 0;
	for (int i = 0; i < varLength; i++)
	{
		if (var[i].vlev == levGlobe&&strcmp(var[i].vproc, levStack[levGlobe - 1]) == 0)
		{
			pro[n].ladr = i;
			if (first == 0)
			{
				first = 1;
				pro[n].fadr = i;
			}
		}
	}
}

void searchVar(int offset) //�������Ƿ���
{
	int success = 0;
	for (int i = 0; i < varLength; i++)
	{
		if (strcmp(var[i].vname, Word[outputCount + offset]) == 0 && var[i].vlev == levGlobe && strcmp(var[i].vproc, levStack[levGlobe - 1]) == 0)
			success = 1;
	}
	for (int i = 0; i < proLength; i++)
	{
		if (strcmp(pro[i].pname, Word[outputCount + offset]) == 0 && pro[i].plev == levGlobe && strcmp(pro[i].pname, levStack[levGlobe - 1]) == 0)
			success = 1;
	}
	if (success == 0)
	{
		ready = offset;
		errorFunction(2);
		ready = 0;
		printf("  ����%s\n", Word[outputCount + offset]);
	}
}

void searchPro(int offset) //�������Ƿ���
{
	int success = 0;
	for (int i = 0; i < proLength; i++)
	{
		if (strcmp(pro[i].pname, Word[outputCount + offset]) == 0 && (strcmp(pro[i].fatherproc, levStack[levGlobe - 1])==0 || strcmp(pro[i].pname, levStack[levGlobe - 1])==0))
			success = 1;
	}
	if (success == 0)
	{
		errorFunction(4);
	}
}

void outVar() //�����������
{
	FILE *varFile = fopen("G:\\����ԭ��ʵ��\\data.var", "w+");
	for (int i = 0; i < varLength; i++)
	{
		fputs("vname:", varFile);
		fputs(var[i].vname, varFile);
		fputs("\n", varFile);

		fputs("vproc:", varFile);
		fputs(var[i].vproc, varFile);
		fputs("\n", varFile);

		fputs("vkind:", varFile);
		char temp1[3] = "\0";
		_itoa(var[i].vkind, temp1, 10);
		fputs(temp1, varFile);
		fputs("\n", varFile);

		fputs("vtype:", varFile);
		fputs(var[i].vtype, varFile);
		fputs("\n", varFile);

		fputs("vlev:", varFile);
		char temp2[3] = "\0";
		_itoa(var[i].vlev, temp2, 10);
		fputs(temp2, varFile);
		fputs("\n", varFile);

		fputs("vadr:", varFile);
		char temp3[3] = "\0";
		_itoa(var[i].vadr, temp3, 10);
		fputs(temp3, varFile);
		fputs("\n\n", varFile);
	}
	fclose(varFile);
}

void outPro() //�����������
{
	FILE *proFile = fopen("G:\\����ԭ��ʵ��\\data.pro", "w+");
	for (int i = 0; i < proLength; i++)
	{
		fputs("pname:", proFile);
		fputs(pro[i].pname, proFile);
		fputs("\n", proFile);

		fputs("ptype:", proFile);
		fputs(pro[i].ptype, proFile);
		fputs("\n", proFile);

		fputs("plev:", proFile);
		char temp1[3] = "\0";
		_itoa(pro[i].plev, temp1, 10);
		fputs(temp1, proFile);
		fputs("\n", proFile);

		fputs("fadr:", proFile);
		char temp2[3] = "\0";
		_itoa(pro[i].fadr, temp2, 10);
		fputs(temp2, proFile);
		fputs("\n", proFile);

		fputs("ladr:", proFile);
		char temp3[3] = "\0";
		_itoa(pro[i].ladr, temp3, 10);
		fputs(temp3, proFile);
		fputs("\n\n", proFile);
	}
	fclose(proFile);
}

//�ݹ��½���������
void ����();
void �ֳ���();
void ˵������();
void ˵������P();
void ˵�����();
void ��������˵��();
void ����();
void ����();
void ������();
void ִ������();
void ִ������P();
void ִ�����();
void �������ʽ();
void �������ʽP();
void ��();
void ��P();
void ����();
void �ջ��������ʽ();
void �������ʽ();
void ��ϵ�����();

void showWords(char analyze[3]) //��ʾ�Ѳ���Ĺؼ���
{
	if (strcmp(analyze, "01") == 0)
		printf("�Ѳ���ؼ���begin\n");
	else if (strcmp(analyze, "02") == 0)
		printf("�Ѳ���ؼ���end\n");
	else if (strcmp(analyze, "03") == 0)
		printf("�Ѳ���ؼ���integer\n");
	else if (strcmp(analyze, "04") == 0)
		printf("�Ѳ���ؼ���if\n");
	else if (strcmp(analyze, "05") == 0)
		printf("�Ѳ���ؼ���then\n");
	else if (strcmp(analyze, "06") == 0)
		printf("�Ѳ���ؼ���else\n");
	else if (strcmp(analyze, "07") == 0)
		printf("�Ѳ���ؼ���function\n");
	else if (strcmp(analyze, "08") == 0)
		printf("�Ѳ���ؼ���read\n");
	else if (strcmp(analyze, "09") == 0)
		printf("�Ѳ���ؼ���write\n");
	else if (strcmp(analyze, "12") == 0)
		printf("�Ѳ��������=\n");
	else if (strcmp(analyze, "13") == 0)
		printf("�Ѳ��������<>\n");
	else if (strcmp(analyze, "14") == 0)
		printf("�Ѳ��������<=\n");
	else if (strcmp(analyze, "15") == 0)
		printf("�Ѳ���ؼ���<\n");
	else if (strcmp(analyze, "16") == 0)
		printf("�Ѳ��������>=\n");
	else if (strcmp(analyze, "17") == 0)
		printf("�Ѳ��������>\n");
	else if (strcmp(analyze, "18") == 0)
		printf("�Ѳ��������-\n");
	else if (strcmp(analyze, "19") == 0)
		printf("�Ѳ��������*\n");
	else if (strcmp(analyze, "20") == 0)
		printf("�Ѳ�����:=\n");
	else if (strcmp(analyze, "21") == 0)
		printf("�Ѳ�����(\n");
	else if (strcmp(analyze, "22") == 0)
		printf("�Ѳ�����)\n");
	else if (strcmp(analyze, "23") == 0)
		printf("�Ѳ�����;\n");
	else if (strcmp(analyze, "10") == 0)
		printf("�Ѳ����ʶ��%s\n", Word[outputCount++]);
	else if (strcmp(analyze, "11") == 0)
		printf("�Ѳ�����%s\n", Word[outputCount++]);
}

void advance() //�����ֱ�ɹ���Ѱ����һ����ʶ���ֱ�
{
	showWords(analyze);
	analyze[0] = ch[analyzeLine++];
	analyze[1] = ch[analyzeLine++];
	while (strcmp(analyze, "24") == 0)
	{
		line++;
		analyze[0] = ch[analyzeLine++];
		analyze[1] = ch[analyzeLine++];
	}
}

void forcastFunction() //Ԥ������һ���ֱ���ȷ���Ƿ��˳��ݹ�
{
	int n = 0;
	forcast[0] = ch[analyzeLine + n];
	n++;
	forcast[1] = ch[analyzeLine + n];
	n++;
	while (strcmp(forcast, "24") == 0)
	{
		forcast[0] = ch[analyzeLine + n];
		n++;
		forcast[1] = ch[analyzeLine + n];
		n++;
	}
}

void enter(char start[30], char end[30], int n) //��ʾ��ǰ��ʶ��Ĺ��̺��ֱ�
{
	char show[17] = "\0";
	if (strcmp(analyze, "01") == 0)
		strcat(show, "begin");
	else if (strcmp(analyze, "02") == 0)
		strcat(show, "end");
	else if (strcmp(analyze, "03") == 0)
		strcat(show, "integer");
	else if (strcmp(analyze, "04") == 0)
		strcat(show, "if");
	else if (strcmp(analyze, "05") == 0)
		strcat(show, "then");
	else if (strcmp(analyze, "06") == 0)
		strcat(show, "else");
	else if (strcmp(analyze, "07") == 0)
		strcat(show, "function");
	else if (strcmp(analyze, "08") == 0)
		strcat(show, "read");
	else if (strcmp(analyze, "09") == 0)
		strcat(show, "write");
	else if (strcmp(analyze, "12") == 0)
		strcat(show, "=");
	else if (strcmp(analyze, "13") == 0)
		strcat(show, "<>");
	else if (strcmp(analyze, "14") == 0)
		strcat(show, "<=");
	else if (strcmp(analyze, "15") == 0)
		strcat(show, "<");
	else if (strcmp(analyze, "16") == 0)
		strcat(show, ">=");
	else if (strcmp(analyze, "17") == 0)
		strcat(show, ">");
	else if (strcmp(analyze, "18") == 0)
		strcat(show, "-");
	else if (strcmp(analyze, "19") == 0)
		strcat(show, "*");
	else if (strcmp(analyze, "20") == 0)
		strcat(show, ":=");
	else if (strcmp(analyze, "21") == 0)
		strcat(show, "(");
	else if (strcmp(analyze, "22") == 0)
		strcat(show, ")");
	else if (strcmp(analyze, "23") == 0)
		strcat(show, ";");
	else if (strcmp(analyze, "10") == 0)
		strcat(show, Word[outputCount]);
	else if (strcmp(analyze, "11") == 0)
		strcat(show, Word[outputCount]);
	else if (strcmp(analyze, "25") == 0)
		strcat(show, "#");
	if (n == 0)
		printf("���벽��%d��%s->%s, ��ǰ��ʶ�����:%s\n", step, start, end, show);
	else if (n == 1)
		printf("���ز���%d��%s->%s, ��ǰ��ʶ�����:%s\n", step, end, start, show);
	step++;
}

void errorFunction(int num) //������
{
	if (errorCount == 0)
	{
		error = fopen("G:\\����ԭ��ʵ��\\data.error", "w+");
	}
	if (num == 0)
	{
		char lineNum[3] = "\0";
		char lineNumber[3] = "\0";
		_itoa(line, lineNum, 10);
		if (line < 10)
		{
			strcat(lineNumber, "0");
		}
		strcat(lineNumber, lineNum);
		char errorInfor[60] = "\0";
		strcat(errorInfor, "***Line:");
		strcat(errorInfor, lineNumber);
		strcat(errorInfor, "   ʶ��");
		char *show = NULL;
		show = (char*)calloc(17, sizeof(char));
		if (strcmp(analyze, "01") == 0)
			strcat(show, "begin");
		else if (strcmp(analyze, "02") == 0)
			strcat(show, "end");
		else if (strcmp(analyze, "03") == 0)
			strcat(show, "integer");
		else if (strcmp(analyze, "04") == 0)
			strcat(show, "if");
		else if (strcmp(analyze, "05") == 0)
			strcat(show, "then");
		else if (strcmp(analyze, "06") == 0)
			strcat(show, "else");
		else if (strcmp(analyze, "07") == 0)
			strcat(show, "function");
		else if (strcmp(analyze, "08") == 0)
			strcat(show, "read");
		else if (strcmp(analyze, "09") == 0)
			strcat(show, "write");
		else if (strcmp(analyze, "12") == 0)
			strcat(show, "=");
		else if (strcmp(analyze, "13") == 0)
			strcat(show, "<>");
		else if (strcmp(analyze, "14") == 0)
			strcat(show, "<=");
		else if (strcmp(analyze, "15") == 0)
			strcat(show, "<");
		else if (strcmp(analyze, "16") == 0)
			strcat(show, ">=");
		else if (strcmp(analyze, "17") == 0)
			strcat(show, ">");
		else if (strcmp(analyze, "18") == 0)
			strcat(show, "-");
		else if (strcmp(analyze, "19") == 0)
			strcat(show, "*");
		else if (strcmp(analyze, "20") == 0)
			strcat(show, ":=");
		else if (strcmp(analyze, "21") == 0)
			strcat(show, "(");
		else if (strcmp(analyze, "22") == 0)
			strcat(show, ")");
		else if (strcmp(analyze, "23") == 0)
			strcat(show, ";");
		else if (strcmp(analyze, "10") == 0 || strcmp(analyze, "11") == 0)
			strcat(show, Word[outputCount]);
		strcat(errorInfor, show);
		strcat(errorInfor, "���Ź�����ƥ����ȱ�ٷ���\n");
		fputs(errorInfor, error);
		analyze[0] = ch[analyzeLine++];
		analyze[1] = ch[analyzeLine++];
		while (strcmp(analyze, "24") == 0)
		{
			line++;
			analyze[0] = ch[analyzeLine++];
			analyze[1] = ch[analyzeLine++];
		}
	}
	else if (num == 1)
	{
		char lineNum[3] = "\0";
		char lineNumber[3] = "\0";
		_itoa(line, lineNum, 10);
		if (line < 10)
		{
			strcat(lineNumber, "0");
		}
		strcat(lineNumber, lineNum);
		char errorInfor[60] = "\0";
		strcat(errorInfor, "***Line:");
		strcat(errorInfor, lineNumber);
		strcat(errorInfor, "   ");
		strcat(errorInfor, Word[outputCount]);
		strcat(errorInfor, "�����ظ�����\n");
		fputs(errorInfor, error);
	}
	else if (num == 2)
	{
		char lineNum[3] = "\0";
		char lineNumber[3] = "\0";
		_itoa(line, lineNum, 10);
		if (line < 10)
		{
			strcat(lineNumber, "0");
		}
		strcat(lineNumber, lineNum);
		char errorInfor[60] = "\0";
		strcat(errorInfor, "***Line:");
		strcat(errorInfor, lineNumber);
		strcat(errorInfor, "   ");
		strcat(errorInfor, Word[outputCount+ready]);
		strcat(errorInfor, "����δ����\n");
		fputs(errorInfor, error);
	}
	else if (num == 3)
	{
		char lineNum[3] = "\0";
		char lineNumber[3] = "\0";
		_itoa(line, lineNum, 10);
		if (line < 10)
		{
			strcat(lineNumber, "0");
		}
		strcat(lineNumber, lineNum);
		char errorInfor[60] = "\0";
		strcat(errorInfor, "***Line:");
		strcat(errorInfor, lineNumber);	
		strcat(errorInfor, "   ");
		strcat(errorInfor, Word[outputCount]);
		strcat(errorInfor, "�����ظ�����\n");
		fputs(errorInfor, error);
	}
	else if (num == 4)
	{
		char lineNum[3] = "\0";
		char lineNumber[3] = "\0";
		_itoa(line, lineNum, 10);
		if (line < 10)
		{
			strcat(lineNumber, "0");
		}
		strcat(lineNumber, lineNum);
		char errorInfor[60] = "\0";
		strcat(errorInfor, "***Line:");
		strcat(errorInfor, lineNumber);
		strcat(errorInfor, "   ");
		strcat(errorInfor, Word[outputCount-1]);
		strcat(errorInfor, "����δ����\n");
		fputs(errorInfor, error);
	}
	errorCount++;
}

void errorstrfunction(char*err, char*correct)//�ж�ȱʧ����
{
	if (errorCount == 0)
	{
		error = fopen("G:\\����ԭ��ʵ��\\data.error", "w+");
	}
	char lineNum[3] = "\0";
	char lineNumber[3] = "\0";
	_itoa(line, lineNum, 10);
	if (line < 10)
	{
		strcat(lineNumber, "0");
	}
	strcat(lineNumber, lineNum);
	char errorInfor[50] = "\0";
	strcat(errorInfor, "***Line:");
	strcat(errorInfor, lineNumber);
	strcat(errorInfor, "   ԭ");
	strcat(errorInfor, err);
	strcat(errorInfor, "����Ӧ��Ϊ");
	strcat(errorInfor, correct);
	strcat(errorInfor, "\n");
	fputs(errorInfor, error);
	errorCount++;
}

void repeatErr(char *infor)//�ж��ظ�����
{
	if (errorCount == 0)
	{
		error = fopen("G:\\����ԭ��ʵ��\\data.error", "w+");
	}
	char lineNum[3] = "\0";
	char lineNumber[4] = "\0";
	_itoa(line , lineNum, 10);
	if (line < 10)
	{
		strcat(lineNumber, "0");
	}
	strcat(lineNumber, lineNum);
	char errorInfor[50] = "\0";
	strcat(errorInfor, "***Line:");
	strcat(errorInfor, lineNumber);
	strcat(errorInfor, "  ");
	strcat(errorInfor, infor);
	strcat(errorInfor, "\n");
	fputs(errorInfor, error);
	errorCount++;
}

void ����()
{
	enter("����", "�ֳ���", 0);
	�ֳ���();
	enter("����", "�ֳ���", 1);
	printf("�ķ������ѽ���\n");
	if (errorCount > 0)
	{
		fclose(error);
		printf("������%d��������ǰ�������ļ��в鿴\n", errorCount);
		system("start G:\\����ԭ��ʵ��\\data.error");
		return;
	}
	if (strcmp(analyze, "25") == 0)
	{
		printf("�ķ������ɹ�\n");
	}
}


void �ֳ���()
{
	if (strcmp(analyze,"10")==0)
	{
		if (strcmp(Word[outputCount], "b") == 0 || strcmp(Word[outputCount], "be") == 0 || strcmp(Word[outputCount], "beg") == 0 || strcmp(Word[outputCount], "begi") == 0 || 1 == 1)
		{
			errorstrfunction(Word[outputCount], "begin");
			goto A;
		}
	}
	if (strcmp(analyze, "01") == 0)
	{
A:		int addr = proLength;
		//pro[proLength++].crreateProcess("mainFunction", "void", levGlobe, "root");
		addProcess("mainFunction", "void", levGlobe, "root");
		strcat(levStack[levGlobe - 1], "mainFunction");
		advance();
		while (strcmp(analyze, "01") == 0)
		{
			repeatErr("begin�ظ�ʹ��");
			advance();
		}
		enter("�ֳ���", "˵������", 0);
		˵������();
		enter("�ֳ���", "˵������", 1);
		if (strcmp(analyze, "10") == 0)
		{
			errorstrfunction(Word[outputCount], ";");
			goto B;
		}
		if (strcmp(analyze, "23") == 0)
		{
B:			advance();
			while (strcmp(analyze, "23") == 0)
			{
				repeatErr(";�ظ�ʹ��");
				advance();
			}
			enter("�ֳ���", "ִ������", 0);
			ִ������();
			enter("�ֳ���", "ִ������", 1);
			if (strcmp(analyze, "10") == 0)
			{
				errorstrfunction(Word[outputCount], "end");
				goto C;
			}
			if (strcmp(analyze, "02") == 0)
			{
C:				searchFadrAndLadr(addr);
				advance();
				while (strcmp(analyze, "02") == 0)
				{
					repeatErr("end�ظ�ʹ��");
					advance();
				}
			}
			else errorFunction(0);
		}
		else errorFunction(0);
	}
	else errorFunction(0);
}

void ˵������()
{
	enter("˵������", "˵�����", 0);
	˵�����();
	enter("˵������", "˵�����", 1);
	enter("˵������", "˵������P", 0);
	˵������P();
	enter("˵������", "˵������P", 1);
}

void ˵������P()
{
	if (strcmp(analyze, "23") == 0)
	{
		forcastFunction(); //��Ԥ���������ֱ��Ծ����Ƿ��˳�����˵�����
		if (strcmp(forcast, "08") == 0 || strcmp(forcast, "09") == 0 ||
			strcmp(forcast, "04") == 0 || strcmp(forcast, "10") == 0)
		{
			printf("����Ϊ��\n");
			return;
		}
		advance();
		enter("˵������P", "˵�����", 0);
		˵�����();
		enter("˵������P", "˵�����", 1);
		enter("˵������P", "˵������P", 0);
		˵������P();
		enter("˵������P", "˵������P", 1);
	}
	else printf("����Ϊ��\n");
}

void ˵�����()
{
	if (strcmp(analyze, "10") == 0)
	{
		errorstrfunction(Word[outputCount], "integer");
		goto D;
	}
	if (strcmp(analyze, "03") == 0)
	{
	D:		advance();
		while (strcmp(analyze, "03") == 0)
		{
			repeatErr("integer�ظ�ʹ��");
			advance();
		}
		enter("˵�����", "��������˵��", 0);
		��������˵��();
		enter("˵�����", "��������˵��", 1);
	}
	else errorFunction(0);
}


void ��������˵��()
{
	while (strcmp(analyze, "03") == 0)
	{
		repeatErr("integer�ظ�ʹ��");
		advance();
	}
	forcastFunction();
	if (strcmp(forcast, "10") == 0)
	{
		if (strcmp(analyze, "07") == 0)
		{
			goto Z;
		}
		errorstrfunction(Word[outputCount], "function");
		goto Z;
	}
	if (strcmp(analyze, "10") == 0)
	{

		enter("��������˵��", "����", 0);
		addVariable(0, 0);
		//var[varLength++].createVariable(Word[outputCount ], pro[levGlobe - 1].pname, 0, "integer", levGlobe, varLength - 1);
		����();
		enter("��������˵��", "����", 1);
	}
	else if (strcmp(analyze, "07") == 0)
	{
Z:		advance();
		if (strcmp(analyze, "10") == 0)
		{
			advance();
			if (strcmp(analyze, "21") == 0)
			{
				advance();
				enter("��������˵��", "����", 0);
				addVariable(1, 0);
				//var[varLength++].createVariable(Word[outputCount], pro[levGlobe - 1].pname, 1, "integer", levGlobe, varLength - 1);
				����();
				enter("��������˵��", "����", 1);
				if (strcmp(analyze, "22") == 0)
				{
					advance();
					if (strcmp(analyze, "23") == 0)
					{
						advance();
						enter("��������˵��", "������", 0);
						levGlobe++;
						//pro[proLength++].crreateProcess(Word[outputCount - 2], "integeter", levGlobe, levStack[levGlobe - 2]);
						addProcess(Word[outputCount - 2], "integeter", levGlobe, levStack[levGlobe - 2]);
						strcat(levStack[levGlobe - 1], Word[outputCount - 2]);
						������();
						levGlobe--;
						enter("��������˵��", "������", 1);
					}
					else errorFunction(0);
				}
				else errorFunction(0);
			}
			else errorFunction(0);
		}
		else errorFunction(0);
	}
	else errorFunction(0);
}

void ����()
{
	enter("����", "����", 0);
	����();
	enter("����", "����", 1);
}

void ����()
{
	if (strcmp(analyze, "10") == 0)
	{
		advance();
	}
	else errorFunction(0);
}

void ������()
{
	if (strcmp(analyze, "10") == 0)
	{
		errorstrfunction(Word[outputCount], "begin");
		goto E;
	}
	if (strcmp(analyze, "01") == 0)
	{
E:		int addr = proLength - 1;
		advance();
		while (strcmp(analyze, "01") == 0)
		{
			repeatErr("begin�ظ�ʹ��");
			advance();
		}
		enter("������", "˵������", 0);
		˵������();
		enter("������", "˵������", 1);
		if (strcmp(analyze, "10") == 0)
		{
			errorstrfunction(Word[outputCount], ";");
			goto F;
		}
		if (strcmp(analyze, "23") == 0)
		{
		F:			advance();
			while (strcmp(analyze, "23") == 0)
			{
				repeatErr(";�ظ�ʹ��");
				advance();
			}
			enter("������", "ִ������", 0);
			ִ������();
			enter("������", "ִ������", 1);
			if (strcmp(analyze, "10") == 0)
			{
				errorstrfunction(Word[outputCount], "end");
				goto G;
			}
			if (strcmp(analyze, "02") == 0)
			{
G:				searchFadrAndLadr(addr);
				advance();
				while (strcmp(analyze, "02") == 0)
				{
					repeatErr("end�ظ�ʹ��");
					advance();
				}
			}
			else errorFunction(0);
		}
		else errorFunction(0);
	}
	else errorFunction(0);
}

void ִ������()
{
	enter("ִ������", "ִ�����", 0);
	ִ�����();
	enter("ִ������", "ִ�����", 1);
	enter("ִ������", "ִ������P", 0);
	ִ������P();
	enter("ִ������", "ִ������P", 1);
}

void ִ������P()
{
	if (strcmp(analyze, "23") == 0)
	{
		forcastFunction(); //��Ԥ���������ֱ��Ծ����Ƿ��˳�����ִ�����
		if (strcmp(forcast, "03") == 0)
		{
			repeatErr("ִ�������治��ִ��˵�����");
			return;
		}
		advance();
		enter("ִ������P", "ִ�����", 0);
		ִ�����();
		enter("ִ������P", "ִ�����", 1);
		enter("ִ������P", "ִ������P", 0);
		ִ������P();
		enter("ִ������P", "ִ������P", 1);
	}
	else printf("����Ϊ��\n");
}

void ִ�����()
{
	/*if (strcmp(analyze, "08") == 0)
	{
		advance();
		if (strcmp(analyze, "10") == 0)
		{
			errorstrfunction(Word[outputCount], "(");
			goto H;
		}
		if (strcmp(analyze, "21") == 0)
		{
		H:			advance();
			while (strcmp(analyze, "21") == 0)
			{
				repeatErr("(�ظ�ʹ��");
				advance();
			}
			enter("ִ�����", "����", 0);
			addVariable(0, 0);
			����();
			enter("ִ�����", "����", 1);
			if (strcmp(analyze, "10") == 0)
			{
				errorstrfunction(Word[outputCount], ")");
				goto I;
			}
			if (strcmp(analyze, "22") == 0)
			{
			I:				advance();
				while (strcmp(analyze, "22") == 0)
				{
					repeatErr(")�ظ�ʹ��");
					advance();
				}
			}
			else errorFunction(0);
		}
		else errorFunction(0);
	}*/
	if (strcmp(analyze, "09") == 0|| strcmp(analyze, "08") == 0)
	{
		advance();
		if (strcmp(analyze, "10") == 0)
		{
			errorstrfunction(Word[outputCount], "(");
			goto J;
		}
		if (strcmp(analyze, "21") == 0)
		{
		J:			advance();
			while (strcmp(analyze, "21") == 0)
			{
				repeatErr("(�ظ�ʹ��");
				advance();
			}
			enter("ִ�����", "����", 0);
			//�жϱ����Ƿ���
			searchVar(0);
			����();
			enter("ִ�����", "����", 1);
			if (strcmp(analyze, "10") == 0)
			{
				errorstrfunction(Word[outputCount], ")");
				goto K;
			}
			if (strcmp(analyze, "22") == 0)
			{
			K:				advance();
				while (strcmp(analyze, "22") == 0)
				{
					repeatErr(")�ظ�ʹ��");
					advance();
				}
			}
			else errorFunction(0);
		}
		else errorFunction(0);
	}
	else if (strcmp(analyze, "04") == 0)
	{
		advance();
		enter("ִ�����", "�������ʽ", 0);
		�������ʽ();
		enter("ִ�����", "�������ʽ", 1);
		if (strcmp(analyze, "10") == 0)
		{
			errorstrfunction(Word[outputCount], "then");
			goto L;
		}
		if (strcmp(analyze, "05") == 0)
		{
		L:			advance();
			while (strcmp(analyze, "05") == 0)
			{
				repeatErr("then�ظ�ʹ��");
				advance();
			}
			enter("ִ�����", "ִ�����", 0);
			ִ�����();
			enter("ִ�����", "ִ�����", 1);
			if (strcmp(analyze, "10") == 0)
			{
				errorstrfunction(Word[outputCount], "else");
				goto M;
			}
			if (strcmp(analyze, "06") == 0)
			{
			M:				advance();
				while (strcmp(analyze, "06") == 0)
				{
					repeatErr("else�ظ�ʹ��");
					advance();
				}
				enter("ִ�����", "ִ�����", 0);
				ִ�����();
				enter("ִ�����", "ִ�����", 1);
			}
			else errorFunction(0);
		}
		else errorFunction(0);
	}
	else if (strcmp(analyze, "10") == 0)
	{
		enter("ִ�����", "����", 0);
		//�жϱ����Ƿ���
		searchVar(0);
		����();
		enter("ִ�����", "����", 1);
		if (strcmp(analyze, "10") == 0)
		{
			errorstrfunction(Word[outputCount], ":=");
			goto N;
		}
		if (strcmp(analyze, "20") == 0)
		{
		N:			advance();
			while (strcmp(analyze, "20") == 0)
			{
				repeatErr(":=�ظ�ʹ��");
				advance();
			}
			enter("ִ�����", "�������ʽ", 0);
			�������ʽ();
			enter("ִ�����", "�������ʽ", 1);
		}
		else errorFunction(0);
	}
	else errorFunction(0);
}

void �������ʽ()
{
	enter("�������ʽ", "��", 0);
	��();
	enter("�������ʽ", "��", 1);
	enter("�������ʽ", "�������ʽP", 0);
	�������ʽP();
	enter("�������ʽ", "�������ʽP", 1);
}

void �������ʽP()
{
	if (strcmp(analyze, "18") == 0)
	{
		advance();
		enter("�������ʽP", "��", 0);
		��();
		enter("�������ʽP", "��", 1);
		enter("�������ʽP", "�������ʽP", 0);
		�������ʽP();
		enter("�������ʽP", "�������ʽP", 1);
	}
	else printf("����Ϊ��\n");
}

void ��()
{
	enter("��", "����", 0);
	����();
	enter("��", "����", 1);
	enter("��", "��P", 0);
	��P();
	enter("��", "��P", 1);
}

void ��P()
{
	if (strcmp(analyze, "19") == 0)
	{
		advance();
		enter("��P", "����", 0);
		����();
		enter("��P", "����", 1);
		enter("��P", "��P", 0);
		��P();
		enter("��P", "��P", 1);
	}
	else printf("����Ϊ��\n");
}

void ����()
{
	if (strcmp(analyze, "11") == 0)
	{
		advance();
	}
	else if (strcmp(analyze, "10") == 0)
	{
		advance();
		enter("����", "�ջ��������ʽ", 0);
		�ջ��������ʽ();
		enter("����", "�ջ��������ʽ", 1);
	}
	else errorFunction(0);
}

void �ջ��������ʽ()
{
	if (strcmp(analyze, "21") == 0)
	{
		advance();
		//�������Ƿ���
		searchPro(-1);
		enter("�ջ��������ʽ", "�������ʽ", 0);
		�������ʽ();
		enter("�ջ��������ʽ", "�������ʽ", 1);
		if (strcmp(analyze, "22") == 0)
		{
			advance();
		}
		else errorFunction(0);
	}
	else
	{
		searchVar(-1);
		printf("%s\n\n", Word[outputCount - 1]);
		printf("����Ϊ��\n");
	}
}

void �������ʽ()
{
	enter("�������ʽ", "�������ʽ", 0);
	�������ʽ();
	enter("�������ʽ", "�������ʽ", 1);
	enter("�������ʽ", "��ϵ�����", 0);
	��ϵ�����();
	enter("�������ʽ", "��ϵ�����", 1);
	enter("�������ʽ", "�������ʽ", 0);
	�������ʽ();
	enter("�������ʽ", "�������ʽ", 1);
}

void ��ϵ�����()
{
	if (strcmp(analyze, "12") == 0 || strcmp(analyze, "13") == 0 ||
		strcmp(analyze, "14") == 0 || strcmp(analyze, "15") == 0 ||
		strcmp(analyze, "16") == 0 || strcmp(analyze, "17") == 0)
	{
		advance();
	}
	else errorFunction(0);
}

int main()
{
	system("G:\\����ԭ��ʵ��\\�ʷ�����.exe");
	preerror = fopen("G:\\����ԭ��ʵ��\\data.error", "r");
	if (preerror != NULL)
	{
		fclose(preerror);
		remove("G:\\����ԭ��ʵ��\\data.error");
	}
	input = fopen("G:\\����ԭ��ʵ��\\data.dyd", "r");
	if (input == NULL)
	{
		printf("�ʷ������ļ������ڣ����Ƚ��дʷ�����\n");
		getchar();
		return 0;
	}
	while (1) //��ȡ�ʷ������Ķ�Ԫ����
	{
		if (aLength == 0)
		{
			fseek(input, 17, SEEK_CUR);
		}
		else
			fseek(input, 19, SEEK_CUR);
		b[0] = fgetc(input);
		b[1] = fgetc(input);
		ch[aLength++] = b[0];
		ch[aLength++] = b[1];
		if (strcmp(b, "10") == 0 || strcmp(b, "11") == 0)
		{
			char store[17] = "\0";
			fseek(input, -18, SEEK_CUR);
			int tempLineLength = 0;
			char m = fgetc(input);
			while (m == ' ')
			{
				m = fgetc(input);
			}
			store[tempLineLength++] = m;
			m = fgetc(input);
			while (m != ' ')
			{
				store[tempLineLength++] = m;
				m = fgetc(input);
			}
			strcat(Word[::count++], store);
			fseek(input, 2, SEEK_CUR);
		}
		if (strcmp(b, "25") == 0)
		{
			fclose(input);
			break;
		}
	}
	analyze[0] = ch[analyzeLine++];
	analyze[1] = ch[analyzeLine++];
	while (strcmp(analyze, "24") == 0)
	{
		line++;
		analyze[0] = ch[analyzeLine++];
		analyze[1] = ch[analyzeLine++];
	}
	����();
	if (errorCount == 0)
	{
		outVar();
		outPro();
	}
	getchar();
	return 0;
}