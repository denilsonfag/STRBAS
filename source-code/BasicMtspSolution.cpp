/* 
 * File:   MtspSolution.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "BasicMtspSolution.hpp"

    BasicMtspSolution::BasicMtspSolution(int n, int m, int depot_node){
        depot = depot_node;
        size = n + (m*2);  // tamanho máximo para soluções com finais fechados (para o caso de posições atuais iguais)
        nodes_list = new IntList();
        nodes_list->create(size);
        total_cost = 0.0;
        longest_route_cost = 0.0;
        iteration = -1;
        time_exec = -1;
        random_seed = -1;
    }
    BasicMtspSolution::~BasicMtspSolution(){
        delete nodes_list;
    }
    
    void BasicMtspSolution::reset(){
        nodes_list->clear();
        total_cost = 0.0;
        longest_route_cost = 0.0;
        iteration = -1;
        time_exec = -1;
        random_seed = -1;
    }
    void BasicMtspSolution::add(int node, double cost){
        nodes_list->add_value(node);
        total_cost = total_cost + cost;
    }
        
    int BasicMtspSolution::n_nodes(){ return nodes_list->n_items(); }
    int BasicMtspSolution::node(int ind) {return nodes_list->value(ind); }
    int BasicMtspSolution::last_node(){ return nodes_list->last_value(); }
    double BasicMtspSolution::get_total_cost(){ return total_cost; }
    double BasicMtspSolution::get_longest_route(){ return longest_route_cost; }
    
     void BasicMtspSolution::set_total_cost(double t_cost){  // apenas para cópia de soluções
        total_cost = t_cost;
    }
     void BasicMtspSolution::set_longest_route(double cost){
        longest_route_cost = cost;
    }
    
   
    // troca os nós das posições enviadas e recalcula a solução
    void BasicMtspSolution::change_nodes(int index_i, int index_j, FloatMatrix* &cost_matrix){
        // alterando os nós na lista de nós:
        nodes_list->change_values(index_i, index_j);
        
        if ((index_i == depot) || (index_j == depot)){  // a troca envolveu o depósito: reorganizar a solução
//-----            cout << "solution a reorg:\t\t\t\t\t"; this->print();
            IntList* copy_values = new IntList();
            copy_values->create(size);
            bool flag = false;
            int index_first_depot;
            for (int i=0; i < size-1; i++){
                if ((nodes_list->value(i) == depot) && (!flag)) {
                    index_first_depot = i;
                    flag = true;
                }
                if (flag) copy_values->add_value(nodes_list->value(i));
            }
            
            for (int i=0; i < index_first_depot; i++) copy_values->add_value(nodes_list->value(i));
            //----- copy_values->add(depot);
            
            delete nodes_list;
            nodes_list = copy_values;
        }        
        // recalculando a solução:
        this->recalculate_solution(cost_matrix);
    }
    
    // atualiza total_cost e longest_route_cost
    void BasicMtspSolution::recalculate_solution(FloatMatrix* &cost_matrix){
        total_cost = 0.0;
        longest_route_cost = 0.0;

        double current_route_cost = 0.0;  // armazena o custo da rota que está sendo calculada
       
        int size_solution = nodes_list->n_items();
        for (int i=1; i < size_solution; i++){       // começa do segundo nó da lista
            int node_i = nodes_list->value(i);
            int node_j = nodes_list->value(i-1);
            double current_cost = cost_matrix->get_value(node_i, node_j);
            total_cost = total_cost + current_cost;
            current_route_cost = current_route_cost + current_cost;
            if (nodes_list->value(i) == depot){  // concluiu uma rota
                if ((longest_route_cost == 0.0) || (current_route_cost > longest_route_cost)){
                    longest_route_cost = current_route_cost;
                }
                current_route_cost = 0.0;  // resetando o custo da rota atual
            }
        }
    }
 
    void BasicMtspSolution::print(){
        cout << "longest: " << longest_route_cost<< "\ttotal cost: " << total_cost << "  \tsolution:";
        nodes_list->print();
        cout << "\r\n";
    }
/*    
    // inverte a solução a partir de index_start (inclusive) a index_end (inclusive) e a reacalcula
    void BasicMtspSolution::reverse(int index_start, int index_end, DoubleMatrix* &cost_matrix){
        nodes_list->reverse(index_start, index_end);  // reversão sem considerar nós depósitos
//-----        nodes_list->reverse_no_depot(index_start, index_end);  // reversão mantendo os depositos em suas posições na solução
        // recalculando a solução:
        this->recalculate_solution(cost_matrix);
    }
    
    // retorna verdadeiro se a solução for válida:
    bool BasicMtspSolution::test_validate(){
        for (int i=1; i < nodes_list->n_items(); i++){
            if (nodes_list->value(i) == depot){
                if (nodes_list->value(i-1) == depot){
                    return false;
                }
            }
        }
        return true;
    }
    
    
    
    bool BasicMtspSolution::is_full(){  // a solução está completa?
        if (nodes_list->is_full()) return true;
        else return false;
    }
    bool BasicMtspSolution::is_empty(){  // a solução está vazia?
        if (nodes_list->is_empty()) return true;
        else return false;
    }
    
    
    void BasicMtspSolution::set_iteration(int c) { iteration = c; }
    void BasicMtspSolution::set_time(double t){ time_exec = t; }
    void BasicMtspSolution::set_random_seed(long s){
        random_seed = s;
    }
   

    
    int BasicMtspSolution::get_iteration_sol(){ return iteration; }
    int BasicMtspSolution::get_time_sol() { return time_exec; }
    long BasicMtspSolution::get_seed_rand() { return random_seed; }

    
    int BasicMtspSolution::next_node(int current_node){
        return nodes_list->next_value(current_node);
    }
    
    void BasicMtspSolution::save_how_list(ofstream& file_out){
        file_out << "maior rota: " << longest_route_cost<< "\tcusto total: " << total_cost << "  \tsolução: ";
        nodes_list->save(file_out);
        file_out << "\r\n";  
    }
    
    
    void BasicMtspSolution::save_to_plot(ofstream& file_out, Node*& nodes){
        file_out << setiosflags (ios::fixed) << setprecision(0);
        if (random_seed != -1) file_out << "Semente randômica: " << random_seed << "\r\n";
        for (int i=0; i<nodes_list->n_items(); i++){
            file_out << nodes_list->value(i) << "\t";
            file_out << nodes[nodes_list->value(i)].x() << "\t";
            file_out << nodes[nodes_list->value(i)].y();
            file_out << "\r\n";
        }
        file_out << setiosflags (ios::fixed) << setprecision(FLOAT_PRECISION);
    }
    
    void BasicMtspSolution::save_longest_cost(ofstream& file_out){
        file_out << longest_route_cost << "\t";
    }
    void BasicMtspSolution::save_total_cost(ofstream& file_out){
        file_out << total_cost << "\t";
    }
*/
