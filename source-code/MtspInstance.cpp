/* 
 * File:   MtspInstance.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "MtspInstance.hpp"

MtspInstance::MtspInstance(){}
MtspInstance::~MtspInstance(){
    delete final_nodes_routes;
    delete initial_nodes_routes;
    delete valid_nodes;
    delete [] plan_nodes_vector;
    delete cost_matrix;
}

void MtspInstance::load_benchmark_instance(int n_salesmen, int code_instance, int integer_euc_mat, int type_mtsp_sols, int depot_node, int test){
    this->create_nodes_benchmark_instance(code_instance, test);
    cost_matrix = new FloatMatrix(n_nodes);
    this->calcule_euclidean_matrix(integer_euc_mat, 1);
    this->configure_salesmen(n_salesmen, type_mtsp_sols, depot_node);
}
void MtspInstance::create_nodes_benchmark_instance(int code_instance, int n_nodes_test){
    const char* name_file_in;
    switch (code_instance){ 
        case 1: {  // instância Eil51 da TSPLIB
            name_file_in = "inputs/benchmark-instances/eil51.txt";
            break;
        }
        case 2: {  // instância Eil76 da TSPLIB
            name_file_in = "inputs/benchmark-instances/eil76.txt";
            break;
        }
        case 3: {  // instância Eil101 da TSPLIB
            name_file_in = "inputs/benchmark-instances/eil101.txt";
            break;
        }
        case 4: {  // instância pr76 da TSPLIB
            name_file_in = "inputs/benchmark-instances/pr76.txt";
            break;
        }
        case 5: {  // instância pr1002 da TSPLIB
            name_file_in = "inputs/benchmark-instances/pr1002.txt";
            break;
        }
        case 6: {  // instância sgb128
            name_file_in = "inputs/benchmark-instances/sgb128.txt";
            break;
        }
    }
    
    ifstream file_in;
    file_in.open(name_file_in, ios::in);
    int vertex_read;
    float cx_read;
    float cy_read;
    
    // contando o número de nós da instância:
    if (n_nodes_test != -1 ) n_nodes = n_nodes_test;
    else{
        n_nodes = 0;
        do{
            file_in >> vertex_read;
            file_in >> cx_read;
            file_in >> cy_read;
            n_nodes++;
        } while(vertex_read != -1);
        n_nodes--;
    }

    plan_nodes_vector = new Node[n_nodes];
    valid_nodes = new IntList();
    valid_nodes->create(n_nodes);
    
    file_in.close();
    file_in.open(name_file_in, ios::in);
    for (int i=0; i < n_nodes; i++){
        file_in >> vertex_read;
        file_in >> cx_read;
        file_in >> cy_read;
        plan_nodes_vector[i].create(cx_read, cy_read);
        valid_nodes->add_value(i);
    }
    file_in.close();
}
void MtspInstance::configure_salesmen(int n_salesmen, int type_mtsp_sols, int depot_node){
    this->n_salesmen = n_salesmen;
    initial_nodes_routes = new IntList();
    initial_nodes_routes->create(n_salesmen);
    final_nodes_routes = new IntList();
    final_nodes_routes->create(n_salesmen);
    // definindo os nos iniciais e finais das rotas:
    for (int k=0; k < n_salesmen; k++){
        if (type_mtsp_sols == 1){  // rotas fechadas: todos partem e retornam a um depósito comum
            initial_nodes_routes->add_value(depot_node);
            final_nodes_routes->add_value(depot_node);
        }
        else{  // rotas abertas, partindo dos nós 0,1,2... e retornando para n,n-1,n-2...
            initial_nodes_routes->add_value(k); 
            final_nodes_routes->add_value(n_nodes - k);  
        }
    }
}

void MtspInstance::create_empty_instance(int n_nodes, int n_salesmen){
    this->n_nodes = n_nodes;
    this->n_salesmen = n_salesmen;
    cost_matrix = new FloatMatrix(n_nodes);
    plan_nodes_vector = new Node[n_nodes];
    index_last_nodes_vector = -1;
    valid_nodes = new IntList();
    valid_nodes->create(n_nodes);
    initial_nodes_routes = new IntList();
    initial_nodes_routes->create(n_salesmen);
    final_nodes_routes = new IntList();
    final_nodes_routes->create(n_salesmen);
}
bool MtspInstance::add_plan_node(int coord_x, int coord_y){
    if ((index_last_nodes_vector+1) >= n_nodes){
        return false;
    }
    else{
        index_last_nodes_vector++;
        plan_nodes_vector[index_last_nodes_vector].create(coord_x, coord_y);
    }
}
void MtspInstance::set_value_cost_matrix(int i, int j, float cost){
    if ((cost == 0.0) && (i!=j)) cout << "\r\nCost 0.0: edge " << i << " " << j << "\r\n";
    cost_matrix->set_value(i, j, cost);
}
void MtspInstance::reset_valid_nodes_instance(){
    valid_nodes->clear();
}
bool MtspInstance::add_valid_node(int new_node){
    if (valid_nodes->add_value(new_node)){
        return true;
    }
    else{  // erro: a lista já está cheia
        return false;
    }
}
void MtspInstance::reset_initial_nodes_routes(){
    initial_nodes_routes->clear();
}
bool MtspInstance::add_initial_node_route(int new_node){
    if (initial_nodes_routes->add_value(new_node)){
        return true;
    }
    else{  // erro: a lista já está cheia
        return false;
    }
}
void MtspInstance::reset_final_nodes_routes(){
    final_nodes_routes->clear();
}
bool MtspInstance::add_final_node_route(int new_node){
    if (final_nodes_routes->add_value(new_node)){
        return true;
    }
    else{  // erro: a lista já está cheia
        return false;
    }
}

void MtspInstance::calcule_euclidean_matrix(bool integer_euclid_matrix, int divisor){
    float aux1;
    float aux2;
    float dist;
    for (int i=0; i < n_nodes; i++){
        for (int j=0; j < n_nodes; j++){
            
            aux1 = plan_nodes_vector[i].x() - plan_nodes_vector[j].x();
            aux2 = plan_nodes_vector[i].y() - plan_nodes_vector[j].y();
            dist = sqrt(pow(aux1,2) + pow(aux2,2));
            dist = dist / divisor;
            
            if (integer_euclid_matrix){    // matriz de distâncias inteiras (arredondamento)
                dist = floor(dist + 0.5);  // floor trunca para baixo 
            }
            
//            if ((dist == 0.0) && (i!=j)) { cout << "\r\nDistance 0.0: edge " << i << " " << j << "\r\n"; }
            cost_matrix->set_value(i,j,dist);
        }
    } 
}

int MtspInstance::get_n_nodes() {
    return n_nodes;
}
int MtspInstance::get_n_salesmen(){
    return n_salesmen;
}
Node* MtspInstance::get_plan_nodes_vector(){
    return plan_nodes_vector;
}
int MtspInstance::get_n_valid_nodes(){
    return valid_nodes->n_items();
}
int MtspInstance::get_valid_node(int index){
    return valid_nodes->value(index);
}
IntList* MtspInstance::get_valid_nodes_instance(){
    return valid_nodes;
}
float MtspInstance::get_value_cost_matrix(int i, int j){
    return cost_matrix->get_value(i,j);
}
FloatMatrix* MtspInstance::get_cost_matrix(){
    return cost_matrix;
}
IntList* MtspInstance::get_initial_nodes_routes(){
    return initial_nodes_routes;
}
IntList* MtspInstance::get_final_nodes_routes(){
    return final_nodes_routes;
}


void MtspInstance::save_instance(ofstream& file_out){
    file_out << "\r\nCurrent instance:\r\n";
    this->save_valid_nodes(file_out);
    this->save_initial_nodes_routes(file_out);
}
void MtspInstance::save_valid_nodes(ofstream& file_out){
    file_out << "Valid nodes: "; 
    valid_nodes->save(file_out); 
    file_out << "\r\n";
}
void MtspInstance::save_initial_nodes_routes(ofstream& file_out){
    file_out << "Initial nodes routes: "; 
    initial_nodes_routes->save(file_out); 
    file_out << "\r\n";
}

void MtspInstance::print_instance(){
    cout << "\r\nCurrent instance:\r\n";
    this->print_valid_nodes();
    this->print_initial_nodes_routes();
}
void MtspInstance::print_valid_nodes(){
    cout << "Valid nodes: "; 
    valid_nodes->print(); 
    cout << "\r\n";
}
void MtspInstance::print_initial_nodes_routes(){
    cout << "Initial nodes routes: ";  
    initial_nodes_routes->print(); 
    cout << "\r\n";
}

void MtspInstance::print_cost_matrix(){
    cout << "\r\nCosts matrix:\r\n";
    cost_matrix->print();
}
