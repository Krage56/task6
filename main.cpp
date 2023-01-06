#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <climits>

#define buf_size 1024+1
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

long symbol;

int t = ' ';
long scan(){
    enum TOKENS current_token;
    while(1){
        if (t == ' ' || t == '\t' || t == '\n' || t == '\r') t = fgetc(ptrFile);
        else if (t == '(') {
            t = fgetc(ptrFile);
            current_token = LBRACE;
            return current_token;
        }
        else if (t == ')') {
            t = fgetc(ptrFile);
            current_token = RBRACE;
            return current_token;
        }
        else if (t == '{') {
            t = fgetc(ptrFile);
            current_token = FLBRACE;
            return current_token;
        }
        else if (t == '}') {
            t = fgetc(ptrFile);
            current_token = FRBRACE;
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
            return current_token;
        }
        else if (t == ':') {
            t = fgetc(ptrFile);
            current_token = COLON;
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
            printf("Unresolved operator %s", buf);
            return EOF;
        }
    }
}

int main() {
    int tmp;
    while ((tmp = scan()) != EOF){
        continue;
    }
    fclose(ptrFile);
    return 0;
}
