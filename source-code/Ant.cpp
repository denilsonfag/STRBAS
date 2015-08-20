/* 
 * File:   Ant.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "Ant.hpp"

Ant::Ant(){}
Ant::~Ant(){}

void Ant::reset (int start_node, int end_node){
    this->starting_node = start_node;
    this->ending_node = end_node;
    this->curr_node = starting_node;
}
void Ant::move(int next_node){
    curr_node = next_node;
}

int Ant::start_node(){ return starting_node; }
int Ant::end_node(){ return ending_node; }
int Ant::current_node(){ return curr_node; }

void Ant::print_ant(int id_ant){
    cout << "ant:" << id_ant;
    cout << "  n_star:" << starting_node;
    cout << "  n_end:" << ending_node;
    cout << "  n_cur:" << curr_node;
}
