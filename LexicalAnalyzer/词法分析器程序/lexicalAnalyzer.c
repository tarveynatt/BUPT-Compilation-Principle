//
// Created by Tan Enhao on 2021/10/9.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexicalAnalyzer.h"

int letter(void) {
    return ((C>='a'&&C<='z')||(C>='A'&&C<='Z'))?1:0;
}

int digit(void) {
    return (C>='0'&&C<='9')?1:0;
}

void get_char() {
    C = buffer[forward];
    if (C == '\n') {
        line++;
    }
    forward++;
    if (forward == BUFFERSIZE && re_flag == 0) {
            buffer_fill(BUFFERSIZE);
    }
    else if (forward == BUFFERSIZE * 2 && re_flag == 0) {
            buffer_fill(0);
    }
    re_flag = 0;
    forward = (forward + BUFFERSIZE * 2) % (BUFFERSIZE * 2);
}

void get_nbc(void) {
    while (C == ' ' || C == '\n') {
        get_char();
    }
}

void cat(void) {
    token[pos++] = C;
}

void retract(void) {
    re_flag = 1;    
    forward = (forward + BUFFERSIZE * 2 - 1) % (BUFFERSIZE * 2);
    if (C == '\n') {
        line--;
    }
}

void combine(int tokens, double attribute) {
    token[pos] = '\0';
    binary.token = tokens;
    if (binary.token == NUMF) {
        binary.attribute.fltnum = attribute;
    }
    
    binary.attribute.intnum = (int)attribute;
    token_print();
    pos = 0;
}

void token_print() {
    token_num[binary.token]++;
    printf("\t<%s, ", token);
    switch (binary.token)
    {
    case 0:
        printf("KEY, %ld>\n", binary.attribute.intnum);
        break;
    case 1:
        printf("ID, %ld>\n", binary.attribute.intnum);
        break;
    case 2:
        printf("NUMI, %ld>\n", binary.attribute.intnum);
        break;
    case 3:
        printf("NUMF, %lf>\n", binary.attribute.fltnum);
        break;
    case 4:
        printf("REL, %ld>\n", binary.attribute.intnum);
        break;
    case 5:
        printf("ASS, %ld>\n", binary.attribute.intnum);
        break;
    case 6:
        printf("BIT, %ld>\n", binary.attribute.intnum);
        break;
    case 7:
        printf("LOG, %ld>\n", binary.attribute.intnum);
        break;
    case 8:
        printf("ARI, %ld>\n", binary.attribute.intnum);
        break;
    case 9:
        printf("SPE, %ld>\n", binary.attribute.intnum);
        break;
    case 10:
        printf("NOT, %ld>\n", binary.attribute.intnum);
        break;
    case 11:
        printf("SYM, %ld>\n", binary.attribute.intnum);
        break;
    case 12:
        printf("SEL, %ld>\n", binary.attribute.intnum);
        break;
    default:
        break;
    }
}

int reserve(void) {
    token[pos++] = '\0';
    for (int i = 0; i < KEYSIZE; i++) {
        if (strcmp(keyTable[i], token) == 0) {
            return i;
        }
    }
    return -1;
}

void table_insert(void) {
    strcpy(signTable[signPos], token);
}

void error(int log) {
    int eline;
    if (C == '\n') {
        eline = line - 1;
    }
    else {
        eline = line;
    }
    switch (log) {
        case 0:
           printf("\t小数点后必须有数字(line %d)\n", eline);
           break;
        case 1:
            printf("\t指数符号E后必须有数字(line %d)\n", eline);
            break;
        case 2:
            printf("\t正负号后必须有数字(line %d)\n", eline);
            break;
        case 3:
            printf("\t无法识别的输入(line %d)\n", eline);
    }
    pos = 0;
}

void buffer_fill(int start) {
    int i = 0;
    char c;
    while (i < BUFFERSIZE && (c = fgetc(fp)) != EOF) {
        buffer[start + i] = c;
        i++;
        total_ns++;
        if (c != ' ' && c != '\n') {
            total++;
        }
    }
    if (c == EOF) {
        buffer[start + i] = '\0';
    }
}

void lexicalAnalyzer(void) {
    state = 0;
    pos = 0;
    do {
        switch(state) {
            case 0: //初始状态
                get_char();
                get_nbc();
                if (letter() || C == '_') {
                    state = 1; //设置标识符状态
                }
                else if (digit()) {
                    state = 2; //设置常数符状态
                }
                else {
                    switch(C) {
                        case '<': state = 8; break;
                        case '>': state = 10; break;
                        case '=': state = 12; break;
                        case '!': state = 13; break;
                        case '+': state = 14; break;
                        case '-': state = 15; break;
                        case '&': state = 16; break;
                        case '|': state = 17; break;
                        case '*': state = 18; break;
                        case '/': state = 19; break;
                        case '%': state = 22; break;
                        case '^': state = 23; break;
                        case '{':
                            cat(); state = 0; combine(SYM, 0); break;
                        case '}':
                            cat(); state = 0; combine(SYM, 0); break;
                        case '(':
                            cat(); state = 0; combine(SPE, 0); break;
                        case ')':
                            cat(); state = 0; combine(SPE, 1); break;
                        case '[':
                            cat(); state = 0; combine(SPE, 2); break;
                        case ']':
                            cat(); state = 0; combine(SPE, 3); break;
                        case '.':
                            cat(); state = 0; combine(SPE, 5); break;
                        case ':':
                            cat(); state = 0; combine(SYM, 2); break;
                        case '\'':
                            cat(); state = 0; combine(SYM, 3); break;
                        case '\"':
                            cat(); state = 0; combine(SYM, 4); break;
                        case ';':
                            cat(); state = 0; combine(SYM, 5); break;
                        case '?':
                            cat(); state = 0; combine(SEL, 0); break;
                        case ',':
                            cat(); state = 0; combine(SYM, 6); break;
                        case '#':
                            cat(); state = 0; combine(SYM, 7); break;
                        default: state = 22; break;
                    }
                }
                break;

            case 1: //标识符状态
                cat();
                get_char();
                if (letter() || digit()) {
                    state = 1;
                }
                else {
                    retract();
                    state = 0;
                    isKey = reserve();
                    if (isKey != -1) {
                        combine(KEY, isKey);
                    }
                    else {
                        table_insert(); combine(ID, signPos);
                    }
                }
                break;

            case 2: //常数状态
                cat();
                get_char();
                switch (C) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': state = 2; break;
                    case '.': state = 3; break;
                    case 'E': state = 5; break;
                    default: //识别出常整数
                        retract(); state = 0; combine(NUMI, atoi(token)); break;
                }
                break;

            case 3: //小数点状态
                cat();
                get_char();
                if (digit()) {
                    state = 4;
                }
                else {
                    error(0); state = 0;
                }
                break;

            case 4: //小数状态
                cat();
                get_char();
                switch (C) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': state = 4; break;
                    case 'E': state = 5; break;
                    default: //识别出小数
                        retract(); state = 0; combine(NUMF, atof(token)); break;
                }
                break;

            case 5: //指数状态
                cat();
                get_char();
                switch (C) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': state = 7; break;
                    case '+':
                    case '-':state = 6; break;
                    default:
                        retract(); error(1); state = 0; break;
                }
                break;

            case 6:
                cat();
                get_char();
                if (digit()) {
                    state = 7;
                }
                else {
                    retract(); error(2); state = 0;
                }
                break;

            case 7:
                cat();
                get_char();
                if(digit()) {
                    state = 7;
                }
                else {
                    retract(); state = 0; combine(NUMF, atof(token));
                }
                break;

            case 8: // <
                cat();
                get_char();
                switch (C) {
                    case '=':
                        cat(); state = 0; combine(REL, 1); break; // <=
                    case '<':
                        state = 9; break; // <<?
                    default:
                        retract(); state = 0; combine(REL, 0); break; // <
                }
                break;

            case 9: // <<
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 10); // <<=
                }
                else {
                    retract(); state = 0; combine(BIT, 4); // <<
                }
                break;

            case 10: // >
                cat();
                get_char();
                switch (C) {
                    case '=':
                        cat(); state = 0; combine(REL, 3); break; // >=
                    case '<':
                        state = 11; break; // >>?
                    default:
                        retract(); state = 0; combine(REL, 2); break; // >
                }
                break;

            case 11: // >>
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 9); // >>=
                }
                else {
                    retract(); state = 0; combine(BIT, 5); // >>
                }
                break;

            case 12: // =
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(REL, 4); // ==
                }
                else {
                    retract(); state = 0; combine(ASS, 0); // =
                }
                break;

            case 13: // !
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(REL, 5); // !=
                }
                else {
                    retract(); state = 0; combine(LOG, 2); // !
                }
                break;

            case 14: // +
                cat();
                get_char();
                switch (C) {
                    case '+':
                        cat(); state = 0; combine(ARI, 5); break; // ++
                    case '=':
                        cat(); state = 0; combine(ASS, 1); break; // +=
                    default:
                        retract(); state = 0; combine(ARI, 0); break; // +
                }
                break;

            case 15: // -
                cat();
                get_char();
                switch (C) {
                    case '-':
                        cat(); state = 0; combine(ARI, 6); break; // --
                    case '=':
                        cat(); state = 0; combine(ASS, 2); break; // -=
                    case '>':
                        cat(); state = 0; combine(SPE, 4);  break; // ->
                    default:
                        retract(); state = 0; combine(ARI, 1); break; // -
                }
                break;

            case 16: // &
                cat();
                get_char();
                switch (C) {
                    case '&':
                        cat(); state = 0; combine(LOG, 0); break;// &&
                    case '=':
                        cat(); state = 0; combine(ASS, 6); break;// &=
                    default:
                        retract(); state = 0; combine(BIT, 0); break;// &
                }
                break;

            case 17: // |
                cat();
                get_char();
                switch (C) {
                    case '|':
                        cat(); state = 0; combine(LOG, 1); break;// ||
                    case '=':
                        cat(); state = 0; combine(ASS, 7); break;// |=
                    default:
                        retract(); state = 0; combine(BIT, 1); break;// |
                }
                break;

            case 18: // *
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 3); // *=
                }
                else {
                    retract(); state = 0; combine(ARI, 2); // *
                }
                break;

            case 19: // /
                cat();
                get_char();
                switch (C) {
                    case '=':
                        cat(); state = 0; combine(ASS, 4); break;// /=
                    case '*':
                        cat(); state = 20; combine(NOT, 0); break;// /*
                    case '/':
                        cat(); state = 21; combine(NOT, 1); break;// //
                    default:
                        retract(); state = 0; combine(ARI, 3); break;// /
                }
                break;

            case 20: // /* */
                get_char();
                while (C != '*') get_char();
                get_char();
                if (C == '/') {
                    state = 0;
                }
                else {
                    state = 20;
                }
                break;

            case 21: // //
                get_char();
                while (C != '\n' && C != '\0') get_char();
                if (C == '\n') {
                    state = 0;
                }
                break;

            case 22: // %
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 5); // %=
                }
                else {
                    retract(); state = 0; combine(ARI, 4); // %
                }
                break;

            case 23: // ^
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 8); // ^=
                }
                else {
                    retract(); state = 0; combine(BIT, 3); // ^
                }
                break;

            case 24: //error
                error(3);
                state = 0;
                break;
            default: break;
        }
    } while (C != '\0');
}

void outcome_print(void) {
    printf("\n\n");
    printf("该程序共有 %d 行\n", line);
    printf("该程序的字符总数为 %d / %d\n", total, total_ns);
    printf("各种记号的的个数为：\n");
    printf("\tKEY:\t%d\n", token_num[0]);
    printf("\tID:\t\t%d\n", token_num[1]);
    printf("\tNUMI:\t%d\n", token_num[2]);
    printf("\tNUMF:\t%d\n", token_num[3]);
    printf("\tREL:\t%d\n", token_num[4]);
    printf("\tASS:\t%d\n", token_num[5]);
    printf("\tBIT:\t%d\n", token_num[6]);
    printf("\tLOG:\t%d\n", token_num[7]);
    printf("\tARI:\t%d\n", token_num[8]);
    printf("\tSPE:\t%d\n", token_num[9]);
    printf("\tNOT:\t%d\n", token_num[10]);
    printf("\tSYM:\t%d\n", token_num[11]);
    printf("\tSEL:\t%d\n", token_num[12]);
}


int main(void) {

    // printf("RUNNING");

    if ((fp = fopen("HelloWorld.txt", "r")) == NULL) {
        printf("File doesn't exit.");
        return 0;
    }

    //初始化变量
    forward = 0;
    line = 1;
    total = 0;
    total_ns = 0;

    buffer_fill(0); //先读入左半边缓冲区

    printf("词法分析结果为：\n");
    
    lexicalAnalyzer(); //词法分析

    outcome_print(); //打印结果
    
    fclose(fp);
    return 0;
}
