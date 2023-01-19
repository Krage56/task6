//
// Created by ivan on 19.01.23.
//
#include <cstdlib>
#include "semantic_executor.h"

void* resize_array(int current_cap, float coef, void* arr, unsigned int t_size){
    int new_cap = (int)((float)current_cap * coef);
    void* new_arr = realloc(arr, t_size * new_cap);
    return new_arr;
}

void add_new_case(SwitchData* s, int condition){
    if (s->case_cond_size == s->case_cond_cap){
        s->case_cond_arr = (int*) resize_array(s->case_cond_cap,1.5,s->case_cond_arr,
                                               sizeof(int));
        s->case_cond_cap = (int)((float)s->case_cond_cap * 1.5);
    }
    s->case_cond_arr[s->case_cond_size] = condition;
    s->case_cond_size += 1;
}

void add(StStack* stack, SwitchData* data){
    if (stack->_cap == stack->_size){
        stack->data = (SwitchData**) resize_array(stack->_cap,stack->_coef,stack->data,
                                                  sizeof(SwitchData*));
        stack->_cap = (int)((float)stack->_cap * stack->_coef);
    }
    stack->data[stack->_size] = data;
    stack->_size += 1;
}

SwitchData *initSwitchData(short access) {
    SwitchData * obj = (SwitchData*)malloc(sizeof(SwitchData));
    obj->case_cond_cap = 10;
    obj->case_cond_arr = (int*) malloc(sizeof(int) * obj->case_cond_cap);
    obj->case_cond_size = 0;
    obj->access = access;
    return obj;
}

StStack *initStStack() {
    StStack* obj = (StStack*)malloc(sizeof(StStack));
    obj->data = (SwitchData**)malloc(sizeof(SwitchData*) * 10);
    obj->_cap = 10;
    obj->_size = 0;
    obj->_coef = 1.5;
    return obj;
}

SwitchData *pop(StStack *stack) {
    SwitchData* tmp = stack->data[stack->_size - 1];
    stack->_size -= 1;
    stack->data[stack->_size] = NULL;
    return tmp;
}

void free_stack(StStack *stack) {
    if (stack->_size != 0){
        for (int i = 0; i < stack->_size; i++){
            free_switch_data(stack->data[i]);
        }
    }
    free(stack->data);
    free(stack);
}

SwitchData *get(StStack *s) {
    return s->data[s->_size-1];
}

void free_switch_data(SwitchData *s) {
    free(s->case_cond_arr);
    free(s);
}
