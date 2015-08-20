/* 
 * File:   MtspNearestNeighbor.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "MtspNearestNeighbor.hpp"

    MtspNearestNeighbor::MtspNearestNeighbor(int n_nodes, int m_salesmen){
        depot = 0;  // o depósito é definido como o primeiro nó da instância
        n = n_nodes;
        m = m_salesmen;
        visited_list = new IntList();
        visited_list->create(n);
        nn_solution = new MtspSolution();
        nn_solution->create(n, m);
    }
    MtspNearestNeighbor::~MtspNearestNeighbor(){
        delete nn_solution;
        delete visited_list;
    }
    void MtspNearestNeighbor::create_solution_workload_balance(FloatMatrix* &cost_matrix, IntList* &valid_nodes_instance){
        // todos os caixeiros partem do depósito:
        for (int k=0; k<m; k++){
            nn_solution->add_node(k, depot, cost_matrix);
        }
        visited_list->clear();
        visited_list->add_value(depot);

        // adicionando nós inválidos à visited list:
        for (int i=0; i<n; i++){
            if (!valid_nodes_instance->in_the_list(i)){  // se o nó não for válido, inserir na tabu_list
                if (i != depot){
                    if (!visited_list->in_the_list(i)){
                        visited_list->add_value(i);
                    }
                }
            }
        }
     
        // criar as rotas dos caixeiros:
        while (!visited_list->is_full()){
            // selecionar o caixeiro que tem o menor custo parcial:
            int selected_salesman;
            float less_cost;
            for (int k=0; k<m; k++){
                if (k==0) {
                    selected_salesman = k;
                    less_cost = nn_solution->cost_route(k);
                }
                else{
                    if (nn_solution->cost_route(k) < less_cost){
                        selected_salesman = k;
                        less_cost = nn_solution->cost_route(k);
                    }
                }
            }
            
            // localizar o nó mais próximo do nó atual do caixeiro selecionado, 
            // que ainda não tenha sido visitado
            int current = nn_solution->last_node_route(selected_salesman);
            int selected_node;
            float less_dist;
            bool first = true;
            for (int i=0; i<n; i++){
                if (!visited_list->in_the_list(i)){
                    float curr_cost = cost_matrix->get_value(current,i);
                    if(first){
                        selected_node = i;
                        less_dist = curr_cost;
                        first = false;
                    }
                    else{
                        if (curr_cost < less_dist){
                            selected_node = i;
                            less_dist = curr_cost;
                        }
                    }
                }
            }
            nn_solution->add_node(selected_salesman, selected_node, cost_matrix);
            visited_list->add_value(selected_node);
        }
        
        // adicionando o retorno ao depósito às rotas
        for(int k=0; k<m; k++){
            nn_solution->add_node(k, depot, cost_matrix);
        }
        
        // finalizando a solução:
        nn_solution->finalize_solution(-1, -1, -1);
    }
  
    void MtspNearestNeighbor::create_solution_one_by_one(FloatMatrix* &cost_matrix, IntList* &valid_nodes_instance){
        // todos os caixeiros partem do depósito:
        for (int k=0; k<m; k++){
            nn_solution->add_node(k, depot, cost_matrix);
        }
        visited_list->clear();
        visited_list->add_value(depot);
        
        // adicionando nós inválidos à visited list:
        for (int i=0; i<n; i++){
            if (!valid_nodes_instance->in_the_list(i)){  // se o nó não for válido, inserir na tabu_list
                if (i != depot){
                    if (!visited_list->in_the_list(i)){
                        visited_list->add_value(i);
                    }
                }
            }
        }
     
        int points_by_salesman = valid_nodes_instance->n_items() / m;
        int counter_points = 0;
        int selected_salesman = 0;
        
        // criar as rotas dos caixeiros:
        while (!visited_list->is_full()){
            
            if (counter_points > points_by_salesman){
                selected_salesman++;
                if (selected_salesman >= m){
                    selected_salesman--;
                }
                else {
                    counter_points = 0;
                }
            }
            counter_points++;

            // localizar o nó mais próximo do nó atual do caixeiro selecionado, 
            // que ainda não tenha sido visitado
            int current = nn_solution->last_node_route(selected_salesman);
            int selected_node;
            float less_dist;
            bool first = true;
            for (int i=0; i<n; i++){
                if ((!visited_list->in_the_list(i)) && (i!=depot)){
                    float curr_cost = cost_matrix->get_value(current,i);
                    if(first){
                        selected_node = i;
                        less_dist = curr_cost;
                        first = false;
                    }
                    else{
                        if (curr_cost < less_dist){
                            selected_node = i;
                            less_dist = curr_cost;
                        }
                    }
                }
            }
            nn_solution->add_node(selected_salesman, selected_node, cost_matrix);
            visited_list->add_value(selected_node);
        }
        
        // adicionando o retorno ao depósito às rotas
        for(int k=0; k<m; k++){
            nn_solution->add_node(k, depot, cost_matrix);
        }
        
        // finalizando a solução:
        nn_solution->finalize_solution(-1, -1, -1);
    }
    
    float MtspNearestNeighbor::get_total_cost(){
        return nn_solution->total_cost_sol();
    }
