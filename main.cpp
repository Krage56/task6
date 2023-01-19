#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include "syntax_analyzer.h"
#define buf_size 1024+1

//Сюда записывается считываемая лексема
char buf[buf_size];

enum TOKENS{
    SWITCH, //0
    CASE,//1
    DEFAULT,//2
    BREAK,//3
    PRINT,//4
    INT,//5
    SEMI,//6
    FLBRACE,//7
    FRBRACE,//8
    COLON,//9
    LBRACE,//10
    RBRACE//11
};

FILE * ptrFile = fopen("../prog.txt" , "r");

int symbol;

int t = ' ';
long scan(){
    enum TOKENS current_token;
    while(1){
        if (t == ' ' || t == '\t' || t == '\n' || t == '\r') t = fgetc(ptrFile);
        else if (t == '(') {
            t = fgetc(ptrFile);
            current_token = LBRACE;
            buf[0] = '(';
            buf[1] = '\0';
            return current_token;
        }
        else if (t == ')') {
            t = fgetc(ptrFile);
            current_token = RBRACE;
            buf[0] = ')';
            buf[1] = '\0';
            return current_token;
        }
        else if (t == '{') {
            t = fgetc(ptrFile);
            current_token = FLBRACE;
            buf[0] = '{';
            buf[1] = '\0';
            return current_token;
        }
        else if (t == '}') {
            t = fgetc(ptrFile);
            current_token = FRBRACE;
            buf[0] = '}';
            buf[1] = '\0';
            return current_token;
        }
        else if (t >= '0' && t <= '9'){
            long num = LONG_MIN;
            int count = 0;
            memset(buf, 0, buf_size);
            while (t >= '0' && t <= '9'){
                buf[count] = t;
                t = fgetc(ptrFile);
                ++count;
            }
            buf[count] = '\0';
            num = strtol(buf, NULL, 0);
            current_token = INT;
            return  current_token;
        }
        else if (t == ';') {
            t = fgetc(ptrFile);
            current_token = SEMI;
            buf[0] = ';';
            buf[1] = '\0';
            return current_token;
        }
        else if (t == ':') {
            t = fgetc(ptrFile);
            current_token = COLON;
            buf[0] = ':';
            buf[1] = '\0';
            return current_token;
        }
        else if (t == EOF) return EOF;
        else{
            memset(buf, 0, buf_size);
            int count = 0;
            while ((t != ' ' && t != '\t' && t != '\n' && t != '\r' && t != '(' && t != ')' && t != '{' &&
                    t != '}' && t != ';' && t != ':') && (count < buf_size)){
                buf[count] = t;
                t = fgetc(ptrFile);
                ++count;
            }
            buf[count % buf_size] = '\0';
            const char* operators[] = {
                    "SWITCH",
                    "CASE",
                    "DEFAULT",
                    "BREAK",
                    "print"
            };
            TOKENS start = SWITCH;
            for (int i = start; i < start + 5; ++i){
                if (strcmp((operators[i - start]), buf) == 0){
                    return i;
                }
            }
            printf("Unresolved operator %s\n", buf);
            return EOF;
        }
    }
}

void payload(){
    printf("payload -> ");

    while (symbol == SWITCH || symbol == PRINT || symbol == BREAK){
        if (symbol == SWITCH){
            statement();
        }
        else if (symbol == PRINT){
            symbol = scan();
            if (symbol != LBRACE){
                printf("Incorrect expression: must be left brace, %s received\n", buf);
                return;
            }

            symbol = scan();
            if (symbol != INT){
                printf("Incorrect expression: must be integer value, %s received\n", buf);
                return;
            }

            symbol = scan();
            if (symbol != RBRACE){
                printf("Incorrect expression: must be right brace, %s received\n", buf);
                return;
            }

            symbol = scan();
            if (symbol != SEMI){
                printf("Incorrect expression: must be semicolon, %s received\n", buf);
                return;
            }
        }
        else{

            symbol = scan();
            if (symbol != SEMI) {
                printf("Incorrect expression: must be semicolon, %s received\n", buf);
                return;
            }
        }
        symbol = scan();
    }
}

void st_case(){
    printf("st_case -> ");

    symbol = scan();
    if (symbol != INT){
        printf("Incorrect expression: must be integer value, %s received\n", buf);
        return;
    }

    symbol = scan();
    if (symbol != COLON){
        printf("Incorrect expression: must be colon, %s received\n", buf);
        return;
    }
    symbol = scan();
    payload();
}

void st_default(){
    printf("default -> ");

    symbol = scan();
    if (symbol != COLON){
        printf("Incorrect expression: must be colon, %s received\n", buf);
        return;
    }
    symbol = scan();
    payload();
}

void case_arr(){
    printf("case_arr -> ");
    while (symbol == CASE){
        st_case();
    }
    if (symbol == DEFAULT){
        st_default();
    }
}

void statement(){
    printf("statement aka st_switch -> ");
    char loc_buf[buf_size];
    if (symbol == SWITCH){
        symbol = scan();
        if (symbol != LBRACE){
            printf("Incorrect expression: must be left brace, %s received\n", buf);
            return;
        }

        symbol = scan();
        if (symbol != INT){
            printf("Incorrect expression: must be integer value, %s received\n", buf);
            return;
        }
        //Если успешно считался INT, то в буфере лежит число
        strcpy(loc_buf, buf);
        symbol = scan();
        if (symbol != RBRACE){
            printf("Incorrect expression: must be right brace, %s received\n", buf);
            return;
        }

        symbol = scan();
        if (symbol != FLBRACE){
            printf("Incorrect expression: must be left figure brace, %s received\n", buf);
            return;
        }

        symbol = scan();
        case_arr();
        if (symbol != FRBRACE){
            printf("Incorrect expression: must be right figure brace to close statement\n");
            return;
        }

    }
}

int main() {
    symbol = scan();
    statement();
    fclose(ptrFile);
    return 0;
}
