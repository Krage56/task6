#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include "syntax_analyzer.h"
#include "semantic_executor.h"
#define buf_size 1024 + 1

//Сюда записывается считываемая лексема
char buf[buf_size];
/*
<statement> ::= <st_switch>
<st_switch> ::= SWITCH(INT)<body_switch>
<body_switch> ::= {<case_arr>}
<case_arr> ::= <st_case><case_arr> | <st_default> | e
<st_case> ::= CASE INT:<payload>
<payload> ::= <statement><payload> | print(INT); <payload> | BREAK; <payload> | e
<st_default> ::= DEFAULT:<payload>
*/
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

StStack* stack = initStStack();

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
    while (symbol == SWITCH || symbol == PRINT || symbol == BREAK){
        if (symbol == SWITCH){
            //Если на вершине стека есть свитч с access == 1, то
            //добавляем на вершину новый свитч с access == 0, иначе с access == -1
            SwitchData* current = get(stack);
            add(stack, initSwitchData(current->access == 1 ? 0: -1));
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
            //Если у свитча на вершине стека access == 1, то запоминаем
            //число из буфера в локальный буфер
            int num = strtol(buf, NULL, 0);

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
            //Если у свитча на вершине стека access == 1, то выводим
            //число из локального буфера
            SwitchData* current = get(stack);
            if (current->access == 1){
                printf("%d\n", num);
            }
        }
        //нашли break
        else{
            symbol = scan();
            //Если у свитча на вершине стека access == 1, то меняем на -1
            SwitchData* tmp = get(stack);
            if (tmp->access == 1){
                tmp->access = -1;
            }
            if (symbol != SEMI) {
                printf("Incorrect expression: must be semicolon, %s received\n", buf);
                return;
            }
        }
        symbol = scan();
    }
}

void st_case(){
    symbol = scan();
    if (symbol != INT){
        printf("Incorrect expression: must be integer value, %s received\n", buf);
        return;
    }
    //Взяли с вершины стека свитч, дописали в него ещё один обрабатываемый кейс
    SwitchData* current_switch = get(stack);
    add_new_case(current_switch, strtol(buf, NULL, 0));
    //Сравнили последний кейс с условием свичта
    //Если условие кейса совпало с условием свитча и access == 0, то access = 1
    if ((current_switch->condition == current_switch->case_cond_arr[current_switch->case_cond_size - 1]) &&
    (current_switch->access == 0)){
        current_switch->access = 1;
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

    symbol = scan();
    if (symbol != COLON){
        printf("Incorrect expression: must be colon, %s received\n", buf);
        return;
    }
    //Если у свитча на вершине стека access == 0, меняем на 1
    if (get(stack)->access == 0){
        get(stack)->access = 1;
    }
    symbol = scan();
    payload();
}

void case_arr(){
    while (symbol == CASE){
        st_case();
    }
    if (symbol == DEFAULT){
        st_default();
    }
}

void statement(){
    if (symbol == SWITCH){
        int num = 0;
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
        num = strtol(buf, NULL, 0);
        get(stack)->condition = num;

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
        //если считалась закрывающая фигурная скобка, то switch на вершине стека выполнен
        free_switch_data(pop(stack));
    }
}


int main() {
    symbol = scan();
    add(stack, initSwitchData());
    statement();

    fclose(ptrFile);
    free_stack(stack);
    return 0;
}
