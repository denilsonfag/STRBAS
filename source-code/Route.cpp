/* 
 * File:   Route.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "Route.hpp"

Route::Route(){}
Route::~Route(){
    delete route;
}
void Route::create(int size_route){
    route = new IntList();
    route->create(size_route);
    total_cost_route = 0.0;
    temporal_cost_route = 0.0;
}

void Route::reset(){
    route->clear();
    total_cost_route = 0.0;
    temporal_cost_route = 0.0;
}
bool Route::add_node(int new_node, FloatMatrix* &cost_matrix){
    float cost;
    if (!route->is_empty()){  // se a rota não estiver vazia
        int last_node = this->last_node();
        cost = cost_matrix->get_value(last_node, new_node);
    }
    else{
        cost = 0.0;
    }
    if (route->add_value(new_node)){  // a rota ainda não está cheia
        total_cost_route = total_cost_route + cost;    
        return true;
    }
    else{  // erro: a rota já está cheia
        return false;
    }
}
bool Route::add_node_with_temp_cost(int new_node, FloatMatrix* &cost_matrix, float temporal_cost){
    if (this->add_node(new_node, cost_matrix)){ // a rota ainda não está cheia
        temporal_cost_route = temporal_cost_route + temporal_cost;
        return true;
    }
    else{  // erro: a rota já está cheia
        return false;
    }
}

int Route::n_nodes(){ 
    return route->n_items();
}
int Route::node(int index_node) {
    return route->value(index_node); 
}
int Route::last_node(){ 
    return route->last_value();
}
bool Route::is_empty(){
    if (route->is_empty()) return true;
    else return false;
}
float Route::total_cost(){ 
    return total_cost_route; 
}
float Route::temporal_cost(){
    return temporal_cost_route;
}

void Route::save(ofstream& file_out){
    int n_nodes = route->n_items();
    file_out << "nodes: " << n_nodes;
    file_out << "\ttotal cost: " << total_cost_route;
    file_out << "\ttemporal cost: " << temporal_cost_route;
    file_out << "\troute: "; this->save_nodes_route(file_out);
    file_out << "\r\n";
}
void Route::save_nodes_route(ofstream& file_out){
    route->save(file_out);
}
void Route::print(){
    int n_nodes = route->n_items();
    cout << "nodes: " << n_nodes;
    cout << "\ttotal cost: " << total_cost_route;
    cout << "\ttemporal cost: " << temporal_cost_route;
    cout << "\troute: "; this->print_nodes_route();
    cout << "\r\n";
}
void Route::print_nodes_route(){
    route->print();
}
