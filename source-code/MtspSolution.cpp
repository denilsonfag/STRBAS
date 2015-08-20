/* 
 * File:   MtspSolution.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "MtspSolution.hpp"

MtspSolution::MtspSolution(){}
MtspSolution::~MtspSolution(){
    delete [] routes;
}
void MtspSolution::create(int n_nodes_instance, int number_routes){
    n_routes = number_routes;
    routes = new Route[n_routes];
    for (int k=0; k < n_routes; k++){
        routes[k].create(n_nodes_instance+1);
    }
    total_cost_solution = 0.0;
    longest_route_cost_solution = 0.0;
    temp_total_cost_solution = 0.0;
    temp_longest_route_cost = 0.0;      
    complete_solution = false;
}

void MtspSolution::reset(){
    for (int k=0; k < n_routes; k++){
        routes[k].reset();
    }
    total_cost_solution = 0.0;
    longest_route_cost_solution = 0.0;
    temp_total_cost_solution = 0.0;
    temp_longest_route_cost = 0.0; 
    complete_solution = false;
}
bool MtspSolution::add_node(int k, int new_node, FloatMatrix* &cost_matrix){
    if (routes[k].add_node(new_node, cost_matrix)){
        return true;
    }
    else{  // erro: a rota já está cheia
        return false;
    }
}
bool MtspSolution::add_node_with_temp_cost(int k, int new_node, FloatMatrix* &cost_matrix, float temporal_cost){
    if (routes[k].add_node_with_temp_cost(new_node, cost_matrix, temporal_cost)){
        return true;
    }
    else{  // erro: a rota já está cheia
        return false;
    }
}
void MtspSolution::finalize_solution(int cycle_sol, int time_exec_sol, long random_seed_exec){
    cycle = cycle_sol;
    time_exec = time_exec_sol;
    random_seed = random_seed_exec;
    this->calcule_total_costs_solution();
    this->update_longest_route_costs();
}
void MtspSolution::calcule_total_costs_solution(){
    total_cost_solution = 0.0;
    temp_total_cost_solution = 0.0;
    for (int k=0; k < n_routes; k++){
        total_cost_solution = total_cost_solution + routes[k].total_cost();
        temp_total_cost_solution = temp_total_cost_solution + routes[k].temporal_cost();
    }
}
void MtspSolution::update_longest_route_costs(){
    for (int k=0; k < n_routes; k++){
        if (k==0){
            longest_route_cost_solution = routes[k].total_cost();
            temp_longest_route_cost = routes[k].temporal_cost();
        }
        else{
            if (routes[k].total_cost() > longest_route_cost_solution){
                longest_route_cost_solution = routes[k].total_cost();
            }
            if (routes[k].temporal_cost() > temp_longest_route_cost){
                temp_longest_route_cost = routes[k].temporal_cost();
            }
        }
    }
}
void MtspSolution::set_random_seed (long new_seed){
    random_seed = new_seed;
}

int MtspSolution::n_routes_sol(){
    return n_routes;
}
int MtspSolution::n_nodes_route(int k){
    return routes[k].n_nodes();
}
int MtspSolution::node_route(int k, int index_node){
    return routes[k].node(index_node);
}
int MtspSolution::last_node_route(int k){
    return routes[k].last_node();
}
int MtspSolution::next_node_route(int k, int current_node){
    int next_node;
    int size_route = routes[k].n_nodes();
    for (int i=0; i < size_route; i++){
        int node_route = routes[k].node(i);
        if (node_route == current_node){
            next_node = routes[k].node(i+1);
            i = size_route;
        }
    }
    return next_node;
}
float MtspSolution::total_cost_sol(){ return total_cost_solution; }
float MtspSolution::temporal_cost_route(int k){
    return routes[k].temporal_cost();
}
float MtspSolution::longest_route_cost_sol(){ return longest_route_cost_solution; }
float MtspSolution::cost_route(int k){
    return routes[k].total_cost();
}
Route* MtspSolution::pointer_routes(){
    return routes;
}
float MtspSolution::cost_by_objective(int mtsp_objective){
    if (mtsp_objective == 1){ return longest_route_cost_solution; }  // atualizar pela longest_route 
    else{ return total_cost_solution; }  // atualizar pelo total da solução
}

int MtspSolution::cycle_sol(){
    return cycle;
}
int MtspSolution::time_exec_sol(){
    return time_exec;
}
long MtspSolution::random_seed_sol(){
    return random_seed;
}

void MtspSolution::save(ofstream& file_out){
    file_out << "longest: " << longest_route_cost_solution << "  \ttotal: " << total_cost_solution << "  \troutes: ";
    for (int k=0; k < n_routes; k++){
        this->save_route(k, file_out);
        file_out << "   ";
    }
    file_out << "\r\n";
}
void MtspSolution::save_route(int k, ofstream& file_out){
    routes[k].save_nodes_route(file_out);
}
void MtspSolution::save_to_plot(ofstream& file_out, Node*& nodes){
    file_out << setiosflags (ios::fixed) << setprecision(0);
    if (random_seed != -1) file_out << "\r\nseed execution: " << random_seed << "\r\n";
    for (int k=0; k < n_routes; k++){
        int size_route = routes[k].n_nodes();
        for (int i=0; i < size_route; i++){
            int current_node = routes[k].node(i);
            file_out << current_node << "\t";
            file_out << nodes[current_node].x() << "\t";
            file_out << nodes[current_node].y() << "\r\n";
        }
        file_out << "-1\r\n";
    }
    file_out << setiosflags (ios::fixed) << setprecision(2);
}
void MtspSolution::save_longest_cost(ofstream& file_out){
    file_out << longest_route_cost_solution << "\t";
}
void MtspSolution::save_total_cost(ofstream& file_out){
    file_out << total_cost_solution << "\t";
}
void MtspSolution::print(){
    cout << "longest: " << longest_route_cost_solution << " \ttotal: " << total_cost_solution << " \troutes: ";
    for (int k=0; k < n_routes; k++){
        routes[k].print_nodes_route();
        cout << "   ";
    }
    cout << "\r\n";
}
void MtspSolution::print_route(int k){
    routes[k].print();
}
