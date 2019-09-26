#include<stdio.h>
#include<string.h>
#include<Windows.h>
#define MAX_COUNT 1024
#define ILLEGAL_CHAR_ERR 1
#define UNKNOWN_OPERATOR_ERR 2


/*从标准输入读入第一个非空白字符(换行符除外)*/
char getnbc()
{
	char ch;
	ch = getchar();
	while (1)
	{
		if (ch == '\r' || ch == '\t' || ch == ' ')
		{
			ch = getchar();
		}
		else
		{
			break;
		}
	}
	return ch;
}
/*判断character是否为字母*/
bool letter(char character)
{
	if ((character >= 'a'&&character <= 'z') || (character >= 'A'&&character <= 'Z'))
		return true;
	else
		return false;
}
/*判断character是否为数字*/
bool digit(char character)
{
	if (character >= '0'&&character <= '9')
		return true;
	else
		return false;
}
/*回退字符*/
void retract(char& character)
{
	ungetc(character, stdin);
	character = NULL;
}
/*返回保留字的对应种别*/
int reserve(char* token)
{
	if (strcmp(token, "begin") == 0)
		return 1;
	else if (strcmp(token, "end") == 0)
		return 2;
	else if (strcmp(token, "integer") == 0)
		return 3;
	else if (strcmp(token, "if") == 0)
		return 4;
	else if (strcmp(token, "then") == 0)
		return 5;
	else if (strcmp(token, "else") == 0)
		return 6;
	else if (strcmp(token, "function") == 0)
		return 7;
	else if (strcmp(token, "read") == 0)
		return 8;
	else if (strcmp(token, "write") == 0)
		return 9;
	else
		return 0;
}
/*返回标识符的对应种别*/
int symbol()
{
	return 10;
}
/*返回常数的对应种别*/
int constant()
{
	return 11;
}
/*按照格式输出单词符号和种别*/
void output(const char* token, int kindNum)
{
	printf("%16s %2d\n", token, kindNum);
}
/*根据行号和错误码输出错误*/
bool error(int lineNum, int errNum)
{
	char* errInfo;
	switch (errNum)
	{
	case ILLEGAL_CHAR_ERR:
		errInfo = "出现字母表以外的非法字符";
		break;
	case UNKNOWN_OPERATOR_ERR:
		errInfo = "出现未知运算符";
		break;
	default:
		errInfo = "未知错误";
	}
	if (fprintf(stderr, "***LINE:%d  %s\n", lineNum, errInfo) >= 0)
		return true;
	else
		return false;
}
/*词法分析函数，每调用一次识别一个符号*/
bool LexAnalyze()
{
	static int lineNum = 1;
	char character;
	char token[17] = "";
	character = getnbc();

	switch (character)
	{
	case '\n':
		output("EOLN", 24);
		lineNum++;
		break;
	case EOF:
		output("EOF", 25);
		return false;//false表示已读到文件末尾
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	
		while (letter(character) || digit(character))
		{
			char s[2] = { character };
			strcat(token, s);
			character = getchar();
		}
		retract(character);
		int num;
		num = reserve(token);
		if (num != 0)
			output(token, num);
		else
		{
			int val;
			val = symbol();
			output(token, val);
		}
		break;
	
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		while (digit(character))
		{
			char s[2] = { character };
			strcat(token, s);
			character = getchar();
		}
		retract(character);
		int val;
		val = constant();
		output(token, val);
		break;
	case '=':
		output("=", 12);
		break;
	case '<':
		character = getchar();
		if (character == '>')
			output("<>", 13);
		else if (character == '=')
			output("<=", 14);
		else
		{
			retract(character);
			output("<", 15);
		}
		break;
	case '>':
		character = getchar();
		if (character == '=')
			output(">=", 16);
		else
		{
			retract(character);
			output(">", 17);
		}
		break;
	case '-':
		output("-", 18);
		break;
	case '*':
		output("*", 19);
		break;
	case ':':
		character = getchar();
		if (character == '=')
			output(":=", 20);
		else
			error(lineNum, 2);//输出"未知运算符"错误
		break;
	case '(':
		output("(", 21);
		break;
	case ')':
		output(")", 22);
		break;
	case ';':
		output(";", 23);
		break;
	default:
		error(lineNum, 1);//输出"出现字母表以外的非法字符"错误
	}
	return true;
}
/*获得路径*/
void getPath(char* in, char* out)
{
	char* name;
	name = strrchr(in, '\\');
	if (name != NULL)
		strncpy(out, in, strlen(in) - strlen(name) + 1);
	else
		strcpy(out, "");
}
/*获得文件名，不包括扩展*/
void getFilename(char* in, char* out)
{
	char* fullName;
	char* extension;
	fullName = strrchr(in, '\\');
	extension = strrchr(in, '.');
	if (fullName != NULL)
		strncpy(out, fullName + 1, strlen(fullName) - 1 - strlen(extension));
	else
		strncpy(out, in, strlen(in) - strlen(extension));
}
/*初始化函数，接收输入文件地址，并打开输入、输出、错误文件、将标准输入重定向到输入文件，将标准输出重定向到输出文件，标准错误重定向到错误文件*/
bool init(int argc, char* argv[])
{
	if (argc != 2)
	{
		return false;
	}
	else
	{
		char* inFilename = argv[1];//argv[1];
		char outFilename[MAX_COUNT] = "";
		char errFilename[MAX_COUNT] = "";
		char filename[MAX_COUNT] = "";
		char path[MAX_COUNT] = "";
		//获得文件名（不包括扩展名）和路径
		getFilename(inFilename, filename);
		getPath(inFilename, path);
		//生成输出文件全部路径

		strcat(outFilename, path);
		//strcat(outFilename, "\\");
		strcat(outFilename, filename);
		strcat(outFilename, ".dyd");
		//生成错误文件全部路径

		strcat(errFilename, path);
		//strcat(errFilename, "\\");
		strcat(errFilename, filename);
		strcat(errFilename, ".err");
		if (freopen(inFilename, "r", stdin) != NULL&&freopen(outFilename, "w", stdout) != NULL&&freopen(errFilename, "w", stderr) != NULL)
			return true;
		else
			return false;
	}
}

int main(int argc,char* argv[])//argv[1]是输入文件地址
{
	
	if (init(argc,argv))
	{
		while (LexAnalyze())
		{

		}
	}
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	return 0;
	
}
		

