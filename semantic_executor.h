//
// Created by ivan on 19.01.23.
//

#ifndef SCAN_RECURCEPREDICT_SEMANTIC_EXECUTOR_H
#define SCAN_RECURCEPREDICT_SEMANTIC_EXECUTOR_H
struct SwitchData {
    //Массив условий из case-ов
    int* case_cond_arr;
    //Условие для данного switch
    int condition;
    //Количество case в данном switch
    int case_cond_size;
    //Размер массива для записи условий case-ов
    int case_cond_cap;
    //Разрешено ли воспроизведение в теле оператора
    // 0 - запрещено, пока не найден case с условием,
    //совпадающим с условием switch;
    // 1 - разрешено, пока не встретился оператор break;
    //-1 - запрещение; возникает, если при access == 1
    // встретился break, или если switch является вложенным
    // в другой свитч, в котором на текущий момент access == 0 или -1.
    short access;
};

//Фабричная функция
SwitchData* initSwitchData(short access = 0);

//Функция добавления нового case оператора в рассмотрение
void add_new_case(SwitchData* s, int condition);

void free_switch_data(SwitchData* s);

//Стек для разбора семантики оператора switch
struct StStack {
    SwitchData** data;
    //Физический размер массива
    int _cap;
    //Количество элементов в массиве
    int _size;
    //Коэффициент, с которым перевыделяется память
    float _coef;
};

//Фабричная функция
StStack* initStStack();

//Положить switch на вершину стека
void add(StStack* stack, SwitchData* data);

//Снять switch с вершины стека
SwitchData* pop(StStack* stack);

//Получить элемент с вершины стека
SwitchData* get(StStack* s);

//Очистить массив прочитанных данных
void free_stack(StStack* stack);

#endif //SCAN_RECURCEPREDICT_SEMANTIC_EXECUTOR_H
