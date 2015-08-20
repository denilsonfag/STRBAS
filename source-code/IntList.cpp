/* 
 * File:   IntList.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 18 de julho de 2015, 16:11
 */

#include "IntList.hpp"

IntList::IntList(){}
IntList::~IntList(){
    delete [] values;
}
void IntList::create(int size_list){
    size = size_list;
    values = new int[size];
    index = 0;
}

void IntList::clear(){ 
    index = 0; 
}
bool IntList::add_value(int value){
    if (index >= size){
        return false;  // erro: lista cheia
    }
    else{
        values[index] = value;
        index++;
        return true;
    }
}
bool IntList::change_values(int index_a, int index_b){
    if ((index_a >= index) || (index_b >= index) || (index_a < 0) || (index_b < 0)){  // índices fora do número de itens da lista
        return false;
    }
    else{
        int aux_value = values[index_a];
        values[index_a] = values[index_b];
        values[index_b] = aux_value;
        return true;
    }
}
bool IntList::reverse(int index_start, int index_end){
    if ((index_end <= index_start) || (index_start >= index) || (index_end >= index) || (index_start < 0) || (index_end < 0)){  // index_end <= index_start ou índices fora do número de itens da lista
        return false;
    }
    else{
        int *new_list = new int[size];
        int aux_index_end = index_end;
        for (int i=0; i < size; i++){
            if ((i < index_start) || (i > index_end)) { // copiar valores normalmente
                new_list[i] = values[i];
            }
            else{ // inverter valores
                new_list[i] = values[aux_index_end];
                aux_index_end--;
            }
        }
        delete [] values;
        values = new_list;
        return true;
    }
}

int IntList::size_list() { 
    return size; 
}
int IntList::n_items(){ 
    return index; 
}
int IntList::value(int index_value) {
    if ((index_value >= index) || (index_value < 0)){
        return -1;
    }
    else{
        return values[index_value];
    }
}  
int IntList::last_value(){
        return values[(index-1)];
}
bool IntList::in_the_list(int value){
    int i = 0;
    while (i < index){
        if (value == values[i]){
            return true;
        }
        i++;
    }
    return false;
}
bool IntList::is_full(){
    if (index == size) return true;
    else return false;
}
bool IntList::is_empty(){
    if (index == 0) return true;
    else return false;
}

void IntList::save(ofstream& file_out){
    if (index == 0){
        file_out << "empty list";
    }
    else{
        for (int i=0; i < index; i++){
            if (values[i] < 10) file_out << " ";
            file_out << values[i] << " ";
        }
    }
}
void IntList::print(){
    if (index == 0){
        cout << "empty list";
    }
    else{
        for (int i=0; i < index; i++){
            if (values[i] < 10) cout << " ";
            cout << values[i] << " ";
        }
    }
}
void IntList::print_adding_1(){
    if (index == 0){
        cout << "empty list";
    }
    else{
        for (int i=0; i < index; i++){
            if (values[i] < 10) cout << " ";
            cout << (values[i]+1) << " ";
        }
    }
}
