#define  _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
using namespace std;

FILE *input, *output, *error, *preerror; //定义文件输入，文件输出，错误输出文件指针
char *ch = (char*)malloc(sizeof(char) * 300); //定义种别数组
char b[3] = "\0"; //定义语法分析数组
char forcast[3] = "\0"; //定义预测分析数组
char Word[30][17] = { "\0" }; //定义存储标识符和常量数组
int aLength = 0; //二元序列长度
int line = 1; //行数
char analyze[3] = "\0"; // 当前识别的种别
int analyzeLine = 0; //当前种别位置
int step = 1; //当前识别的步骤数
int count = 0; //存储标识符和常量的个数
int outputCount = 0; //当前标识符和常量的位置
int errorCount = 0; //错误个数
void errorFunction(int);

int levGlobe = 1; //当前所属的函数深度
char levStack[11][17] = { "\0" }; //存储主函数到当前函数之前的函数名
int ready = 0;

class variable //变量类
{
public:
	char vname[17] = "\0"; //变量名
	char vproc[17] = "\0"; //所属过程
	int vkind; //变量或形参
	char vtype[17] = "\0"; //变量类型
	int vlev; //变量层次
	int vadr; //变量在变量表中的位置
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


class process //过程类
{
public:
	char pname[17] = "\0"; //过程名
	char ptype[17] = "\0"; //过程类型
	int plev = -1; //过程层次
	int fadr = -1; //第一个变量在变量表中的位置
	int ladr = -1; //最后一个变量在变量表中的位置
	char fatherproc[17] = "\0"; //父进程名
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

void addProcess(char pname[17], char ptype[17], int plev, char fatherproc[17]) //添加过程
{
	int error = 0;
	for (int i = 0; i < proLength; i++)
	{
		if (strcmp(pro[i].pname, pname) == 0 && pro[i].plev == levGlobe && strcmp(pro[i].fatherproc, fatherproc) == 0)
			error = 1;
	}
	if (error == 1) //检测是否有重复定义过程
	{
		errorFunction(3);
	}
	pro[proLength++].crreateProcess(pname, ptype, plev, fatherproc);
	printf("dddd\n");
}

void addVariable(int n, int offset) //添加变量
{
	int error = 0;
	for (int i = 0; i < varLength; i++)
	{
		if (strcmp(var[i].vname, Word[outputCount - offset]) == 0 && var[i].vlev == levGlobe && strcmp(var[i].vproc, levStack[levGlobe - 1]) == 0)
		{
			error = 1;
		}
	}
	if (error == 1) //检测是否有重复定义变量
	{
		errorFunction(1);
	}
	var[varLength++].createVariable(Word[outputCount - offset], levStack[levGlobe - 1], n, "integer", levGlobe, varLength - 1);
}

void searchFadrAndLadr(int n) //添加过程的fadr和ladr
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

void searchVar(int offset) //检测变量是否定义
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
		printf("  出错%s\n", Word[outputCount + offset]);
	}
}

void searchPro(int offset) //检测过程是否定义
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

void outVar() //输出变量名表
{
	FILE *varFile = fopen("G:\\编译原理实验\\data.var", "w+");
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

void outPro() //输出过程名表
{
	FILE *proFile = fopen("G:\\编译原理实验\\data.pro", "w+");
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

//递归下降分析函数
void 程序();
void 分程序();
void 说明语句表();
void 说明语句表P();
void 说明语句();
void 变量或函数说明();
void 参数();
void 变量();
void 函数体();
void 执行语句表();
void 执行语句表P();
void 执行语句();
void 算术表达式();
void 算术表达式P();
void 项();
void 项P();
void 因子();
void 空或算术表达式();
void 条件表达式();
void 关系运算符();

void showWords(char analyze[3]) //显示已捕获的关键字
{
	if (strcmp(analyze, "01") == 0)
		printf("已捕获关键字begin\n");
	else if (strcmp(analyze, "02") == 0)
		printf("已捕获关键字end\n");
	else if (strcmp(analyze, "03") == 0)
		printf("已捕获关键字integer\n");
	else if (strcmp(analyze, "04") == 0)
		printf("已捕获关键字if\n");
	else if (strcmp(analyze, "05") == 0)
		printf("已捕获关键字then\n");
	else if (strcmp(analyze, "06") == 0)
		printf("已捕获关键字else\n");
	else if (strcmp(analyze, "07") == 0)
		printf("已捕获关键字function\n");
	else if (strcmp(analyze, "08") == 0)
		printf("已捕获关键字read\n");
	else if (strcmp(analyze, "09") == 0)
		printf("已捕获关键字write\n");
	else if (strcmp(analyze, "12") == 0)
		printf("已捕获运算符=\n");
	else if (strcmp(analyze, "13") == 0)
		printf("已捕获运算符<>\n");
	else if (strcmp(analyze, "14") == 0)
		printf("已捕获运算符<=\n");
	else if (strcmp(analyze, "15") == 0)
		printf("已捕获关键字<\n");
	else if (strcmp(analyze, "16") == 0)
		printf("已捕获运算符>=\n");
	else if (strcmp(analyze, "17") == 0)
		printf("已捕获运算符>\n");
	else if (strcmp(analyze, "18") == 0)
		printf("已捕获运算符-\n");
	else if (strcmp(analyze, "19") == 0)
		printf("已捕获运算符*\n");
	else if (strcmp(analyze, "20") == 0)
		printf("已捕获界符:=\n");
	else if (strcmp(analyze, "21") == 0)
		printf("已捕获界符(\n");
	else if (strcmp(analyze, "22") == 0)
		printf("已捕获界符)\n");
	else if (strcmp(analyze, "23") == 0)
		printf("已捕获界符;\n");
	else if (strcmp(analyze, "10") == 0)
		printf("已捕获标识符%s\n", Word[outputCount++]);
	else if (strcmp(analyze, "11") == 0)
		printf("已捕获常数%s\n", Word[outputCount++]);
}

void advance() //检测该种别成功并寻找下一个待识别种别
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

void forcastFunction() //预测下下一个种别已确定是否退出递归
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

void enter(char start[30], char end[30], int n) //显示当前待识别的过程和种别
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
		printf("进入步骤%d，%s->%s, 当前待识别符号:%s\n", step, start, end, show);
	else if (n == 1)
		printf("返回步骤%d，%s->%s, 当前待识别符号:%s\n", step, end, start, show);
	step++;
}

void errorFunction(int num) //出错处理
{
	if (errorCount == 0)
	{
		error = fopen("G:\\编译原理实验\\data.error", "w+");
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
		strcat(errorInfor, "   识别");
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
		strcat(errorInfor, "符号过程中匹配错或缺少符号\n");
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
		strcat(errorInfor, "变量重复定义\n");
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
		strcat(errorInfor, "变量未定义\n");
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
		strcat(errorInfor, "函数重复定义\n");
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
		strcat(errorInfor, "函数未定义\n");
		fputs(errorInfor, error);
	}
	errorCount++;
}

void errorstrfunction(char*err, char*correct)//判断缺失错误
{
	if (errorCount == 0)
	{
		error = fopen("G:\\编译原理实验\\data.error", "w+");
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
	strcat(errorInfor, "   原");
	strcat(errorInfor, err);
	strcat(errorInfor, "符号应改为");
	strcat(errorInfor, correct);
	strcat(errorInfor, "\n");
	fputs(errorInfor, error);
	errorCount++;
}

void repeatErr(char *infor)//判断重复错误
{
	if (errorCount == 0)
	{
		error = fopen("G:\\编译原理实验\\data.error", "w+");
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

void 程序()
{
	enter("程序", "分程序", 0);
	分程序();
	enter("程序", "分程序", 1);
	printf("文法分析已结束\n");
	if (errorCount > 0)
	{
		fclose(error);
		printf("共发现%d个错误，请前往错误文件中查看\n", errorCount);
		system("start G:\\编译原理实验\\data.error");
		return;
	}
	if (strcmp(analyze, "25") == 0)
	{
		printf("文法分析成功\n");
	}
}


void 分程序()
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
			repeatErr("begin重复使用");
			advance();
		}
		enter("分程序", "说明语句表", 0);
		说明语句表();
		enter("分程序", "说明语句表", 1);
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
				repeatErr(";重复使用");
				advance();
			}
			enter("分程序", "执行语句表", 0);
			执行语句表();
			enter("分程序", "执行语句表", 1);
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
					repeatErr("end重复使用");
					advance();
				}
			}
			else errorFunction(0);
		}
		else errorFunction(0);
	}
	else errorFunction(0);
}

void 说明语句表()
{
	enter("说明语句表", "说明语句", 0);
	说明语句();
	enter("说明语句表", "说明语句", 1);
	enter("说明语句表", "说明语句表P", 0);
	说明语句表P();
	enter("说明语句表", "说明语句表P", 1);
}

void 说明语句表P()
{
	if (strcmp(analyze, "23") == 0)
	{
		forcastFunction(); //需预测下两个种别以决定是否退出分析说明语句
		if (strcmp(forcast, "08") == 0 || strcmp(forcast, "09") == 0 ||
			strcmp(forcast, "04") == 0 || strcmp(forcast, "10") == 0)
		{
			printf("捕获为空\n");
			return;
		}
		advance();
		enter("说明语句表P", "说明语句", 0);
		说明语句();
		enter("说明语句表P", "说明语句", 1);
		enter("说明语句表P", "说明语句表P", 0);
		说明语句表P();
		enter("说明语句表P", "说明语句表P", 1);
	}
	else printf("捕获为空\n");
}

void 说明语句()
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
			repeatErr("integer重复使用");
			advance();
		}
		enter("说明语句", "变量或函数说明", 0);
		变量或函数说明();
		enter("说明语句", "变量或函数说明", 1);
	}
	else errorFunction(0);
}


void 变量或函数说明()
{
	while (strcmp(analyze, "03") == 0)
	{
		repeatErr("integer重复使用");
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

		enter("变量或函数说明", "变量", 0);
		addVariable(0, 0);
		//var[varLength++].createVariable(Word[outputCount ], pro[levGlobe - 1].pname, 0, "integer", levGlobe, varLength - 1);
		变量();
		enter("变量或函数说明", "变量", 1);
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
				enter("变量或函数说明", "参数", 0);
				addVariable(1, 0);
				//var[varLength++].createVariable(Word[outputCount], pro[levGlobe - 1].pname, 1, "integer", levGlobe, varLength - 1);
				参数();
				enter("变量或函数说明", "参数", 1);
				if (strcmp(analyze, "22") == 0)
				{
					advance();
					if (strcmp(analyze, "23") == 0)
					{
						advance();
						enter("变量或函数说明", "函数体", 0);
						levGlobe++;
						//pro[proLength++].crreateProcess(Word[outputCount - 2], "integeter", levGlobe, levStack[levGlobe - 2]);
						addProcess(Word[outputCount - 2], "integeter", levGlobe, levStack[levGlobe - 2]);
						strcat(levStack[levGlobe - 1], Word[outputCount - 2]);
						函数体();
						levGlobe--;
						enter("变量或函数说明", "函数体", 1);
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

void 参数()
{
	enter("参数", "变量", 0);
	变量();
	enter("参数", "变量", 1);
}

void 变量()
{
	if (strcmp(analyze, "10") == 0)
	{
		advance();
	}
	else errorFunction(0);
}

void 函数体()
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
			repeatErr("begin重复使用");
			advance();
		}
		enter("函数体", "说明语句表", 0);
		说明语句表();
		enter("函数体", "说明语句表", 1);
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
				repeatErr(";重复使用");
				advance();
			}
			enter("函数体", "执行语句表", 0);
			执行语句表();
			enter("函数体", "执行语句表", 1);
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
					repeatErr("end重复使用");
					advance();
				}
			}
			else errorFunction(0);
		}
		else errorFunction(0);
	}
	else errorFunction(0);
}

void 执行语句表()
{
	enter("执行语句表", "执行语句", 0);
	执行语句();
	enter("执行语句表", "执行语句", 1);
	enter("执行语句表", "执行语句表P", 0);
	执行语句表P();
	enter("执行语句表", "执行语句表P", 1);
}

void 执行语句表P()
{
	if (strcmp(analyze, "23") == 0)
	{
		forcastFunction(); //需预测下两个种别以决定是否退出分析执行语句
		if (strcmp(forcast, "03") == 0)
		{
			repeatErr("执行语句后面不能执行说明语句");
			return;
		}
		advance();
		enter("执行语句表P", "执行语句", 0);
		执行语句();
		enter("执行语句表P", "执行语句", 1);
		enter("执行语句表P", "执行语句表P", 0);
		执行语句表P();
		enter("执行语句表P", "执行语句表P", 1);
	}
	else printf("捕获为空\n");
}

void 执行语句()
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
				repeatErr("(重复使用");
				advance();
			}
			enter("执行语句", "变量", 0);
			addVariable(0, 0);
			变量();
			enter("执行语句", "变量", 1);
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
					repeatErr(")重复使用");
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
				repeatErr("(重复使用");
				advance();
			}
			enter("执行语句", "变量", 0);
			//判断变量是否定义
			searchVar(0);
			变量();
			enter("执行语句", "变量", 1);
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
					repeatErr(")重复使用");
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
		enter("执行语句", "条件表达式", 0);
		条件表达式();
		enter("执行语句", "条件表达式", 1);
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
				repeatErr("then重复使用");
				advance();
			}
			enter("执行语句", "执行语句", 0);
			执行语句();
			enter("执行语句", "执行语句", 1);
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
					repeatErr("else重复使用");
					advance();
				}
				enter("执行语句", "执行语句", 0);
				执行语句();
				enter("执行语句", "执行语句", 1);
			}
			else errorFunction(0);
		}
		else errorFunction(0);
	}
	else if (strcmp(analyze, "10") == 0)
	{
		enter("执行语句", "变量", 0);
		//判断变量是否定义
		searchVar(0);
		变量();
		enter("执行语句", "变量", 1);
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
				repeatErr(":=重复使用");
				advance();
			}
			enter("执行语句", "算术表达式", 0);
			算术表达式();
			enter("执行语句", "算术表达式", 1);
		}
		else errorFunction(0);
	}
	else errorFunction(0);
}

void 算术表达式()
{
	enter("算术表达式", "项", 0);
	项();
	enter("算术表达式", "项", 1);
	enter("算术表达式", "算术表达式P", 0);
	算术表达式P();
	enter("算术表达式", "算术表达式P", 1);
}

void 算术表达式P()
{
	if (strcmp(analyze, "18") == 0)
	{
		advance();
		enter("算术表达式P", "项", 0);
		项();
		enter("算术表达式P", "项", 1);
		enter("算术表达式P", "算术表达式P", 0);
		算术表达式P();
		enter("算术表达式P", "算术表达式P", 1);
	}
	else printf("捕获为空\n");
}

void 项()
{
	enter("项", "因子", 0);
	因子();
	enter("项", "因子", 1);
	enter("项", "项P", 0);
	项P();
	enter("项", "项P", 1);
}

void 项P()
{
	if (strcmp(analyze, "19") == 0)
	{
		advance();
		enter("项P", "因子", 0);
		因子();
		enter("项P", "因子", 1);
		enter("项P", "项P", 0);
		项P();
		enter("项P", "项P", 1);
	}
	else printf("捕获为空\n");
}

void 因子()
{
	if (strcmp(analyze, "11") == 0)
	{
		advance();
	}
	else if (strcmp(analyze, "10") == 0)
	{
		advance();
		enter("因子", "空或算术表达式", 0);
		空或算术表达式();
		enter("因子", "空或算术表达式", 1);
	}
	else errorFunction(0);
}

void 空或算术表达式()
{
	if (strcmp(analyze, "21") == 0)
	{
		advance();
		//检测过程是否定义
		searchPro(-1);
		enter("空或算术表达式", "算术表达式", 0);
		算术表达式();
		enter("空或算术表达式", "算术表达式", 1);
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
		printf("捕获为空\n");
	}
}

void 条件表达式()
{
	enter("条件表达式", "算术表达式", 0);
	算术表达式();
	enter("条件表达式", "算术表达式", 1);
	enter("条件表达式", "关系运算符", 0);
	关系运算符();
	enter("条件表达式", "关系运算符", 1);
	enter("条件表达式", "算术表达式", 0);
	算术表达式();
	enter("条件表达式", "算术表达式", 1);
}

void 关系运算符()
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
	system("G:\\编译原理实验\\词法分析.exe");
	preerror = fopen("G:\\编译原理实验\\data.error", "r");
	if (preerror != NULL)
	{
		fclose(preerror);
		remove("G:\\编译原理实验\\data.error");
	}
	input = fopen("G:\\编译原理实验\\data.dyd", "r");
	if (input == NULL)
	{
		printf("词法分析文件不存在，请先进行词法分析\n");
		getchar();
		return 0;
	}
	while (1) //提取词法分析的二元序列
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
	程序();
	if (errorCount == 0)
	{
		outVar();
		outPro();
	}
	getchar();
	return 0;
}