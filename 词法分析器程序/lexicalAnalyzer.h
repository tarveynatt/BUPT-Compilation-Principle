//
// Created by Tan Enhao on 2021/10/9.
//

#ifndef UNTITLED2_LEXICALANALYZER_H
#define UNTITLED2_LEXICALANALYZER_H

#define KEYSIZE 33
#define BUFFERSIZE 1000

#define KEY 0
#define ID 1
#define NUMI 2
#define NUMF 3
#define REL 4
#define ASS 5
#define BIT 6
#define LOG 7
#define ARI 8
#define SPE 9
#define NOT 10
#define SYM 11
#define SEL 12

int token_num[13]; //存放各种记号单词的数量

union attributes {
        long intnum;
        double fltnum;
}; //存放二元组中的属性

struct tokens {
    int token;
    union attributes attribute; 
};
struct tokens binary; //存放当前单词二元组

char signTable[1000][32]; //存放用户自定义字符的符号表
int signPos; //符号表当前末尾位置
char keyTable[KEYSIZE][10] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
                         "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register",
                         "restrict", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
                         "union", "unsigned", "void", "volatile"}; //存放关键字字符
int state; //当前状态指示
char C; //存放当前读入的字符
int isKey; //值为-1表示识别出的单词是用户自定义的标识符，否则，表示识别出的单词是关键字，其值为关键字的记号
char token[1000]; //存放当前正在识别的单词字符串
int pos; //token中的字符串尾部；
int forward; //向前指针
int line; //当前行数
int total; //不带换行和空格的字符总数
int total_ns; //带换行和空格的字符总数
char buffer[BUFFERSIZE * 2]; //输入缓冲区
FILE* fp; //文件指针
int re_flag; //向前指针回退时的标记，避免重复加载缓冲区

void get_char(); //根据向前指针forward的指示从输入缓冲区中读一个字符放入变量C中，然后移动forward，使之指向下一个字符
void get_nbc(void); //检查C中的字符是否为空格，若是，则反复调用get_char，直到C中进入一个非空字符为止
void cat(void); //把C中字符链接在token中的字符串后面
void retract(void); //向前指针forward后退一个字符
void combine(int token, double attribute); //把单词的记号和属性组合成一个二元组
void error(int log); //对发现的错误进行相应的处理
int letter(void); //判断C中的字符是否为字母，若是则返回1，否则返回0
int digit(void); //判断C中的字符是否为数字，若是则返回1，否则返回0
int reserve(void); //根据token中的单词查关键字表，若token中的单词是关键字，则返回该关键字的记号，否则，返回值“-1”
void table_insert(void); //将识别出来的用户自定义标识符，即token中的单词，插入符号表，返回该单词在符号表中的位置指针；
void token_print(void); //打印识别出来的记号
void buffer_fill(int start); //填充一半的缓冲区
void outcome_print(void); //打印统计结果

#endif //UNTITLED2_LEXICALANALYZER_H
