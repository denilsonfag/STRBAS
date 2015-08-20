/* 
 * File:   ACOAlgorithms.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 14 de Junho de 2015, 12:20
 */

#include "ACOAlgorithms.hpp"

ACOAlgorithms::ACOAlgorithms(){
    tau0 = 0.0;
}
ACOAlgorithms::~ACOAlgorithms(){}

int ACOAlgorithms::TACO_ant_selection(MtspSolution* &solution){
    int n_salesmen = solution->n_routes_sol();
    int selected;
    float less_cost;
    for (int k=0; k < n_salesmen; k++){
        if (k==0) {
            selected = k;
            less_cost = solution->cost_route(k);
        }
        else{
            if (solution->cost_route(k) < less_cost){
                selected = k;
                less_cost = solution->cost_route(k);
            }
        }
    }
    return selected;
}
int ACOAlgorithms::TACO_check_better_movement(int checking_node, int m, Ant* &ants, FloatMatrix* &cost_matrix, MtspSolution* &solution){  // retorna o índice da formiga que resulta no melhor movimento
    int better_ant;
    float better_predict_route;
    float predict_route;
    for (int k=0; k < m; k++){
        predict_route = this->predict_route(k, checking_node, ants, cost_matrix, solution);
        if (k == 0){
            better_ant = k;
            better_predict_route = predict_route;
        }
        else{
            if (predict_route < better_predict_route){
                better_ant = k;
                better_predict_route = predict_route;
            }
        }
    }
    return better_ant;
}

float ACOAlgorithms::AS_initial_pheromone_value(int N, float cost_nearest_neighbo_sol){

    tau0 = N / cost_nearest_neighbo_sol;  // dorigo04, pg 70

    return tau0;
}
int ACOAlgorithms::AS_state_transition_rule(int current_node, IntList* &candidate_list, Random* &random_generator, FloatMatrix* &cost_matrix, FloatMatrix* &pheromone_matrix, int alpha, int beta){
    int choosed_node = -1;  // nó escolhido
    int n_candidates = candidate_list->n_items(); // qtde de nós candidatos
    
    float* probability = this->create_probability_vector(current_node, candidate_list, cost_matrix, pheromone_matrix, alpha, beta); // calculando a probabilidade para todos os nós candidatos

    int raffled_node = random_generator->raffle_float_vector(n_candidates, probability);  // nó sorteado, de acordo com as probabilidades
    
    if (raffled_node == -1){  
        cout << "\r\n\r\nErro no sorteio\r\n\r\n";
        cout << "current node: " << current_node << "\r\n";
        cout << "candidate list: "; candidate_list->print(); cout << "\r\n";
        cout << "probability vector: ";
        for (int i=0; i < n_candidates; i++){ cout << probability[i] << "\t"; } cout << "\r\n";
        
        choosed_node = -1;  // erro no sorteio: retorna -1
        
    }
    else {  // sorteio OK, procedimento normal
        choosed_node = candidate_list->value(raffled_node);
    }
    delete [] probability;
    return choosed_node;
}
void ACOAlgorithms::AS_global_pheromone_update(int mtsp_objective, FloatMatrix* &pheromone_matrix, int n_sols_cycle, MtspSolution* &solutions_cycle, float rho){
    
    this->AS_pheromone_evaporation(pheromone_matrix, rho); // evaporação
    
    // depósito:
    for (int sc_count=0; sc_count < n_sols_cycle; sc_count++){
        int n_routes = solutions_cycle[sc_count].n_routes_sol();
        float cost_route = solutions_cycle[sc_count].cost_by_objective(mtsp_objective);
        
        for (int k=0; k < n_routes; k++){
            
            int n_nodes_route = solutions_cycle[sc_count].n_nodes_route(k);
            
            for (int i=0; i < (n_nodes_route-1); i++){
                int node_i = solutions_cycle[sc_count].node_route(k, i);
                int node_j = solutions_cycle[sc_count].node_route(k, (i+1));
                
                float current_tau = pheromone_matrix->get_value(node_i, node_j);
                
                float new_tau = current_tau + (1 / cost_route);  // Dorigo, Stützle 2004, equação 3.4
                
                pheromone_matrix->set_value(node_i, node_j, new_tau);
            }
        }
    }
}
void ACOAlgorithms::AS_pheromone_evaporation(FloatMatrix* &pheromone_matrix, float rho){
    int matrix_dimension = pheromone_matrix->get_dimension();
    for (int i=0; i < matrix_dimension; i++){
        for (int j=0; j < matrix_dimension; j++){
            float current_tau = pheromone_matrix->get_value(i,j);
            float new_tau = (1-rho) * current_tau;
            pheromone_matrix->set_value(i,j,new_tau);
        }
    }
}

float ACOAlgorithms::EAS_initial_pheromone_value(int N, float rho, int e, float cost_nearest_neighbo_sol){

    tau0 = (e + N) / (rho * cost_nearest_neighbo_sol);  // dorigo04, pg 70

    return tau0;
}
void ACOAlgorithms::EAS_global_pheromone_update(int mtsp_objective, FloatMatrix* &pheromone_matrix, int n_sols_cycle, MtspSolution* &solutions_cycle, MtspSolution* &best_solution_run, float rho, int e){
    
    // a atualização ocorre inicialmente como no AS:
    this->AS_global_pheromone_update(mtsp_objective, pheromone_matrix, n_sols_cycle, solutions_cycle, rho);
    
    // as arestas da best so far solution recebem mais feromônio:
    int n_routes = best_solution_run->n_routes_sol();
    float cost_route = best_solution_run->cost_by_objective(mtsp_objective);
    for (int k=0; k < n_routes; k++){

        int n_nodes_route = best_solution_run->n_nodes_route(k);

        for (int i=0; i < (n_nodes_route-1); i++){
            int node_i = best_solution_run->node_route(k, i);
            int node_j = best_solution_run->node_route(k, (i+1));

            float current_tau = pheromone_matrix->get_value(node_i, node_j);

            float new_tau = current_tau + (e * (1 / cost_route));  // Dorigo, Stützle 2004, equação 3.6

            pheromone_matrix->set_value(node_i, node_j, new_tau);
        }
    }
}

float ACOAlgorithms::RBAS_initial_pheromone_value(float rho, int w, float cost_nearest_neighbo_sol){

    tau0 = ((0.5 * w) * (w - 1)) / (rho * cost_nearest_neighbo_sol);  // dorigo04, pg 70

    return tau0;
}
void ACOAlgorithms::RBAS_global_pheromone_update(int mtsp_objective, FloatMatrix* &pheromone_matrix, int n_sols_cycle, MtspSolution* &solutions_cycle, MtspSolution* &best_solution_run, float rho, int w){
    
    this->AS_pheromone_evaporation(pheromone_matrix, rho); // evaporação
    
    // localizar as w-1 melhores soluções do ciclo e atualizar a matriz de feromônio:
    int r = 0;                   // rank da formiga atual
    int index_r = -1;            // índice de r em solutions_cycle 
    float cost_route_r = 0.0;    // custo da solução da formiga r
    while (r < w){
        r++;
        int new_index_r = this->next_best_solution(mtsp_objective, cost_route_r, n_sols_cycle, solutions_cycle);
        if (new_index_r != -1){ 
            index_r = new_index_r;
            cost_route_r = solutions_cycle[index_r].cost_by_objective(mtsp_objective);
            // atualizar a matriz de feromômio com a solução localizada:
            int n_routes = solutions_cycle[index_r].n_routes_sol();
            for (int k=0; k < n_routes; k++){
                int n_nodes_route = solutions_cycle[index_r].n_nodes_route(k);
                for (int i=0; i < (n_nodes_route-1); i++){
                    int node_i = solutions_cycle[index_r].node_route(k, i);
                    int node_j = solutions_cycle[index_r].node_route(k, (i+1));
                    float current_tau = pheromone_matrix->get_value(node_i, node_j);
                    float new_tau = current_tau + ((w - r) * (1 / cost_route_r));  // Dorigo, Stützle 2004, equação 3.8
                    pheromone_matrix->set_value(node_i, node_j, new_tau);
                }
            }
        }
        else{  // não foram geradas mais que w-1 soluções com custos diferentes
            r = w;  // parando a atualização
        }
    }
    
    // as arestas da best so far solution recebem mais feromônio:
    int n_routes = best_solution_run->n_routes_sol();
    for (int k=0; k < n_routes; k++){
        float cost_route = best_solution_run->cost_by_objective(mtsp_objective);
        int n_nodes_route = best_solution_run->n_nodes_route(k);
        for (int i=0; i < (n_nodes_route-1); i++){
            int node_i = best_solution_run->node_route(k, i);
            int node_j = best_solution_run->node_route(k, (i+1));
            float current_tau = pheromone_matrix->get_value(node_i, node_j);
            float new_tau = current_tau + (w * (1 / cost_route));  // Dorigo, Stützle 2004, equação 3.8
            pheromone_matrix->set_value(node_i, node_j, new_tau);
        }
    }
}

float ACOAlgorithms::ACS_initial_pheromone_value(int n, float cost_nearest_neighbo_sol){

    tau0 = 1 / (n * cost_nearest_neighbo_sol);  // dorigo04, pg 71
        
    return tau0;
}
int ACOAlgorithms::ACS_state_transition_rule(int current_node, IntList* &candidate_list, Random* &random_generator, FloatMatrix* &cost_matrix, FloatMatrix* &pheromone_matrix, Utilities* &util, int alpha, int beta, float q0){
    int choosed_node = -1;  // nó escolhido
    int n_candidates = candidate_list->n_items(); // qtde de nós candidatos
    
    float* probability = this->create_probability_vector(current_node, candidate_list, cost_matrix, pheromone_matrix, alpha, beta); // calculando a probabilidade para todos os nós candidatos

    // psedorandom proportional rule:
    double q = random_generator->random_number();
    
    if (q <= q0){

        int larger_prob = util->argmax(n_candidates, probability);   // nó com maior probabilidade
        choosed_node = candidate_list->value(larger_prob);
    }
    else{

        int raffled_node = random_generator->raffle_float_vector(n_candidates, probability);  // nó sorteado, de acordo com as probabilidades
        choosed_node = candidate_list->value(raffled_node);
    }
    
    delete [] probability;
    
    return choosed_node;
}
void ACOAlgorithms::ACS_local_pheromone_update(int i, int j, FloatMatrix* &pheromone_matrix, float ksi){
    float current_tau = pheromone_matrix->get_value(i,j);
    float new_tau = ((1-ksi) * current_tau) + (ksi * tau0);  // fórmula de atualização local do ACS
    pheromone_matrix->set_value(i,j,new_tau);
}
void ACOAlgorithms::ACS_global_pheromone_update(int mtsp_objective, int m, MtspSolution* &best_so_far_solution, FloatMatrix* &pheromone_matrix, float rho){

    float best_cost = best_so_far_solution->cost_by_objective(mtsp_objective);
    
    float delta_tau = 1 / best_cost;  // fórmula da atualização global do ACS (1)

    for(int k=0; k < m; k++){
        int n_nodes = best_so_far_solution->n_nodes_route(k);
        for (int i=1; i < n_nodes; i++){
            int a = best_so_far_solution->node_route(k, i-1);
            int b = best_so_far_solution->node_route(k, i);
            float current_tau = pheromone_matrix->get_value(a,b);
            float new_tau = ((1-rho)*current_tau) + (rho*delta_tau); // fórmula da atualização global do ACS (2)
            pheromone_matrix->set_value(a,b,new_tau);
        }
    }
}        
        
float* ACOAlgorithms::create_probability_vector(int current_node, IntList* &candidate_list, FloatMatrix* &cost_matrix, FloatMatrix* &pheromone_matrix, int alpha, int beta){
    int n_candidates = candidate_list->n_items(); // qtde de nós candidatos
    float* probability = new float[n_candidates];  // vetor com os numeradores: tau^alfa * eta^beta
    for (int i=0; i < n_candidates; i++){  // índice da candidate_list e probability

        int candidate_node = candidate_list->value(i); // nó candidato
        float cost = cost_matrix->get_value(current_node,candidate_node);
        
        float trail = pheromone_matrix->get_value(current_node,candidate_node);
        float visibility = 1 / cost;
        
        if (isinf(trail) || isinf(visibility)){  // tratamento de infinitos
            if (signbit(trail) || signbit(visibility)){  // retorna verdadeiro se for infinito negativo
                cout << "\r\n\r\nNegative infinite probability\r\n\r\n" ;
                trail = 0.0;
                visibility = 0.0;
            }
            else{ // probabilidade inifinita positiva
//                cout << "\r\n\r\nPositive infinite probability\r\n\r\n" ;
                // atribuindo a trail e visibility o maior valor posssível elevado a 10:
                float larger_trail = pheromone_matrix->larger_value();
                trail = pow(larger_trail,10);
                float larger_visib = 1 / cost_matrix->smaller_value_diff_zero();
                visibility = pow(larger_visib,10);
            }
        }

        float prob = pow(trail, alpha) * pow(visibility, beta);
        if (isnan(prob)){ cout << "\r\n\r\nNot a number probability\r\n\r\n" ; prob = 1.0; }

        probability[i] = prob;
    }
    return probability;
}
float ACOAlgorithms::predict_route(int k, int checking_node, Ant* &ants, FloatMatrix* &cost_matrix, MtspSolution* &solution){  // calcula a previsão da rota de uma formiga caso adicione o nó escolhido
    // rota parcial da formiga em análise:
    float part_route = solution->cost_route(k);
    // distância entre o nó atual da formiga em análise e o próximo nó escolhido:
    float dist_choosed = cost_matrix->get_value(ants[k].current_node(), checking_node);
    // distância entre o próximo nó e o nó final da formiga em análise:
    float dist_starting = cost_matrix->get_value(checking_node, ants[k].end_node());
    return part_route + dist_choosed + dist_starting;
}
// retorna o índice de solutions cycle da solução cujo custo seja o próximo maior que o parâmetro
int ACOAlgorithms::next_best_solution(int mtps_objetive, float minimum_cost, int n_sols_cycle, MtspSolution* &solutions_cycle){
    int index_best = -1;
    float curr_best_cost = 0.0;
    for (int sc_count=0; sc_count < n_sols_cycle; sc_count++){
        float curr_cost = solutions_cycle[sc_count].cost_by_objective(mtps_objetive);
        if (curr_cost > minimum_cost){
            if (curr_best_cost == 0.0){
                curr_best_cost = curr_cost;
                index_best = sc_count;
            }
            else{
                if (curr_cost < curr_best_cost){
                    curr_best_cost = curr_cost;
                    index_best = sc_count;
                }
            }
        }
    }
    return index_best;
}
