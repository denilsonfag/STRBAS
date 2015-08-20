/* 
 * File:   LocalSearch.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "LocalSearch.hpp"
#include "BasicMtspSolution.hpp"

LocalSearch::LocalSearch(){}
LocalSearch::~LocalSearch(){}

// realiza a busca local 2-opt e atualiza a rota se houver ganho em alguma troca
// retorna verdadeiro se atualizar a rota
bool LocalSearch::two_opt_route(Route* &route, FloatMatrix* &cost_matrix){

    int size_route = route->n_nodes();   // número de nós da rota
    int index_first = 0;                 // primeiro índice da rota
    int index_last = size_route - 1;     // último índice da rota

    bool updated_route = false;

    for (int index_a = index_first+1; index_a < index_last-1; index_a++){  // a começa do segundo e vai até o antepenúltimo nó

        for (int index_b = index_a+1; index_b < index_last; index_b++){  // b começa do próximo índice após a e vai até o penúltimo índice

            float gain = this->gain_change_two_nodes_route(index_a, index_b, route, cost_matrix);

            if (gain > 0.0){

//                cout << "Updated by 2-opt:\tgain: " << gain << "\r\nold route: "; route->print_with_costs(cost_matrix);  
                
                this->change_nodes_route(index_a, index_b, route, cost_matrix);  // troca a posição dos nós, invertendo a rota entre eles
                updated_route = true;
                
//                cout << "new route: "; route->print_with_costs(cost_matrix);

            }
        }
    }
    return updated_route;
}
float LocalSearch::gain_change_two_nodes_route(int index_a, int index_b, Route* &route, FloatMatrix* &cost_matrix){

    int index_before_a = index_a - 1;
    int index_after_b = index_b + 1;
    
    int node_a = route->node(index_a);
    int node_b = route->node(index_b);
    int node_before_a = route->node(index_before_a);
    int node_after_b = route->node(index_after_b);
    
//    cout << "\r\n\r\n"; route->print_with_costs(cost_matrix); cout << "\r\n";
    
    //calculando o custo do trecho a ser alterado na rota original:
    float cost_original = 0.0;
    for (int i = index_before_a; i < index_after_b; i++){
        int node_1 = route->node(i);
        int node_2 = route->node(i+1);
        float cost = cost_matrix->get_value(node_1, node_2);
        cost_original = cost_original + cost;
        
//        cout << "original\tnode A: " << node_1 << "\tnode B: " << node_2 << "\tcost: " << cost << "\r\n";
        
    }
    
//    cout << "cost original: " << cost_original << "\r\n\r\n";

    //calculando o custo do trecho já alterado:
    float cost_new = 0.0;
    cost_new = cost_new + cost_matrix->get_value(node_before_a, node_b);
//    cout << "new\tnode A: " << node_before_a << "\tnode B: " << node_b << "\tcost: " <<  cost_matrix->get_value(node_before_a, node_b) << "\r\n";
    
    cost_new = cost_new + cost_matrix->get_value(node_a, node_after_b);
//    cout << "new\tnode A: " << node_a << "\tnode B: " << node_after_b << "\tcost: " <<  cost_matrix->get_value(node_a, node_after_b) << "\r\n";
    
    for (int i = index_a; i < index_b; i++){
        int node_1 = route->node(i+1);
        int node_2 = route->node(i);
        float cost = cost_matrix->get_value(node_1, node_2);
        cost_new = cost_new + cost;  // calculando o trecho invertido
        
//        cout << "new\tnode A: " << node_1 << "\tnode B: " << node_2 << "\tcost: " << cost << "\r\n";
        
    }
    
//    cout << "cost new: " << cost_new << "\r\n\r\n";
    
    float gain = cost_original - cost_new;
    return gain;
}
void LocalSearch::change_nodes_route(int index_a, int index_b, Route* &route, FloatMatrix* &cost_matrix){
    int size_route = route->n_nodes();
    Route* new_route = new Route();
    new_route->create(size_route);
    
    int start_node = route->node(0);        // copiando nó inicial da rota
    new_route->add_node(start_node, cost_matrix);  
    
    int inversions_counter = 0;
    for (int i = 1; i < size_route; i++){
        int next_node;
        
        if ((i < index_a) || (i > index_b)){
            next_node = route->node(i);
        }
        else{
            next_node = route->node(index_b - inversions_counter);
            inversions_counter++;
        }
        new_route->add_node(next_node, cost_matrix);
    }
    
    route->reset();
    for (int i = 0; i < size_route; i++){
        int node = new_route->node(i);
        float cost = 0.0;
        if (i > 0) {
            int last_node = route->last_node();
            cost = cost_matrix->get_value(last_node, node);
        }
        route->add_node(node, cost_matrix);
    }
    
    delete new_route;
}

bool LocalSearch::two_opt_basic_mtsp_solution(int mtsp_objective, MtspSolution* &original_solution, FloatMatrix* &cost_matrix){

    // variáveis para criação da solução básica:
    int depot = original_solution->node_route(0,0); // primeiro nó da priemira rota
    int m = original_solution->n_routes_sol();
    int n = 0;  //aqui, numero de nos da solução de MTSP básico
    for (int k=0; k < m; k++){
        n = n + original_solution->n_nodes_route(k);
    }
    n = n - (m-1); // descontando depósitos duplicados
    
    // copiando para a solução básica:
    BasicMtspSolution* basic_sol = new BasicMtspSolution(n, m, depot);
    for (int k=0; k < m; k++){
        int n_nodes_route = original_solution->n_nodes_route(k);
        for (int i=0; i < n_nodes_route; i++){
            if (i==0){
                if (k==0){
                    basic_sol->add(depot, 0.0);
                }
            }
            else{
                int last_node = basic_sol->last_node();
                int next_node = original_solution->node_route(k, i);
                float cost = cost_matrix->get_value(last_node, next_node);
                basic_sol->add(next_node, cost);
            }
        }
        basic_sol->recalculate_solution(cost_matrix);
    }
    
    // código da busca local:
    int size_sol = basic_sol->n_nodes(); // número de nós da solução
    index_first = 0;                 // primeiro índice da solução
    index_last = size_sol - 2;       // o último nó da solução é o retorno ao nó inicial 
    
//    cout << "original solution: "; original_solution->print();

    bool updated_solution = false;
    
    for (int index_a = index_first+1; index_a < index_last; index_a++){  // a começa do segundo e não vai até o último índice

        for (int index_b = index_a+1; index_b <= index_last; index_b++){  // b começa do próximo índice após a e vai até o último índice

            bool change = true;  // a troca deve ser realizada

            // vizinhos dos nós atuais:
            int index_before_a = index_a-1;    // index_a nunca é igual a index_first
            int index_after_a = index_a+1;     // index_a nunca é igual a index_last

            int index_before_b = index_b - 1;                        // index_b nunca é igual a index_first
            int index_after_b;                                       // índice do nó posterior a j
            if (index_b == index_last) index_after_b = index_first;  // index_b pode ser igual a index_last
            else index_after_b = index_b + 1;                        // index_b não é igual a index_last

            // impedindo ponto de corte inválido:
            if ((index_a == index_first+1) && (index_b == index_last)) {
                change = false;
//                cout << "--> not changed: invalid cutoff point\r\n";
            }

            // impedindo que dois depositos sejam vizinhos:
            if (change){
                if (basic_sol->node(index_a) == depot){
                    if ((basic_sol->node(index_after_b) == depot) || (basic_sol->node(index_before_b)) == depot){
                        change = false;
//                        cout << "--> not changed: neighbor depots\r\n";
                    }
                }
                if (basic_sol->node(index_b) == depot){
                    if ((basic_sol->node(index_after_a) == depot) || (basic_sol->node(index_before_a) == depot)){
                        change = false;
//                        cout << "--> not changed: neighbor depots\r\n";
                    }   
                }
            }

            if (change){
                
                float gain = this->symmetric_gain_change_two_nodes(index_a, index_b, basic_sol, cost_matrix);

                if (gain > 0.0){  // realizando a troca apenas se o ganho total for positivo
                    // copiando a solução para uma temporária:
                    BasicMtspSolution* new_sol = new BasicMtspSolution(n, m, depot); 
                    // copiando nós:
                    for (int i=0; i < size_sol; i++){ new_sol->add(basic_sol->node(i), 0.0); }
                    // copiando outros dados:
                    new_sol->set_total_cost(basic_sol->get_total_cost());
                    new_sol->set_longest_route(basic_sol->get_longest_route());
                    
//                    cout << "\r\nsolution copied:   "; new_sol->print();
//                    cout << "changing index: " << index_a+1 << " -> " << index_b+1;
//                    cout << "\tnodes: " << new_sol->node(index_a) << " -> " << new_sol->node(index_b) << "\r\n";
                    
                    new_sol->change_nodes(index_a, index_b, cost_matrix);  // troca a posição dos nós e recalcula a solução temporária
                    
//                    cout << "solution changed:  "; new_sol->print();
                    
                    double new_best_cost;
                    double current_best_cost;
                    if (mtsp_objective == 1){  // minimizar a longest_route 
                        new_best_cost = new_sol->get_longest_route();
                        current_best_cost = basic_sol->get_longest_route();
                    }
                    else{  // minimizar o custo total
                        new_best_cost = new_sol->get_total_cost();
                        current_best_cost = basic_sol->get_total_cost();
                    }

                    if (new_best_cost < current_best_cost){    // a nova solução é melhor, de acordo com o objetivo
                        delete basic_sol;                       // liberando a memória de solution
                        basic_sol = new_sol;                    // solution passa a apontar para a nova solução
                        updated_solution = true;
                    }
                    else{
                        delete new_sol;  // liberando a memória de new_sol
//                        cout << "--> not changed: worst objective cost\r\n";
                    }
                }
            }
        }
    }    

    if (updated_solution){ // copiar basic_sol para a solução original:
        int n_nodes_sol = basic_sol->n_nodes();
        original_solution->reset();
        int k = -1;
        for (int i=0; i < n_nodes_sol; i++){
            if (basic_sol->node(i) == depot) {
                k++;
                if (k > 0){
                    original_solution->add_node((k-1), depot, cost_matrix);
                }
            }
            
            if (k < m){
                original_solution->add_node(k, basic_sol->node(i), cost_matrix);
            }
        }
        original_solution->calcule_total_costs_solution();
        original_solution->update_longest_route_costs();
     
//        cout << "======>>>>>>>> new 2-opt solution:\t\t"; original_solution->print();
        
    }
    delete basic_sol;
    return updated_solution;
}
float LocalSearch::symmetric_gain_change_two_nodes(int index_a, int index_b, BasicMtspSolution* &basic_solution, FloatMatrix* &cost_matrix){

    double gain;

    int index_before_a;                                      // índice do nó anterior a i
    if (index_a == index_first) index_before_a = index_last; // a é o primeiro nó da solução
    else index_before_a = index_a-1;                         // a é qualquer outro nó
    int index_after_a = index_a+1;                           // a não vai até o último índice

    int index_before_b = index_b - 1;                        // j nunca é o primeiro nó
    int index_after_b;                                       // índice do nó posterior a j
    if (index_b == index_last) index_after_b = index_first;  // j é o último nó da solução
    else index_after_b = index_b + 1;                        // j é qualquer outro nó

    // verificando se a troca gera uma solução com menor custo total:
    double partial_cost_a = this->symmetric_partial_cost(index_before_a, index_a, index_after_a, basic_solution, cost_matrix);
    double partial_cost_b = this->symmetric_partial_cost(index_before_b, index_b, index_after_b, basic_solution, cost_matrix);
    double cost_before_change = partial_cost_a + partial_cost_b;  // custo antes da troca
    double cost_after_change = 0.0;         // custo caso a troca seja feita:
    cost_after_change = cost_after_change + symmetric_partial_cost(index_before_a, index_b, index_after_a, basic_solution, cost_matrix);
    cost_after_change = cost_after_change + symmetric_partial_cost(index_before_b, index_a, index_after_b, basic_solution, cost_matrix);

    gain = cost_before_change - cost_after_change;

    return gain;
}
float LocalSearch::symmetric_partial_cost(int before, int current, int after, BasicMtspSolution* &basic_solution, FloatMatrix* &cost_matrix){
    double p_cost = 0.0; // retorno
    int before_node = basic_solution->node(before);
    int current_node = basic_solution->node(current);
    int after_node = basic_solution->node(after);
    p_cost = p_cost + cost_matrix->get_value(before_node,current_node);
    p_cost = p_cost + cost_matrix->get_value(current_node,after_node);
    return p_cost;
}


// realiza a busca local 3-opt e atualiza solution se houver ganho em alguma troca
// retorna verdadeiro de atualizar a solução
bool LocalSearch::three_opt(MtspSolution* &solution, int mtsp_objective, bool saving, ofstream& f_out){
    bool updated_solution = false;
/*   
    int depot = 0;

    updated_solution = this->two_opt(solution, mtsp_objective);  // atualiza solution para o ótimo local 2-opt

    size_sol = solution->n_nodes();  // número de nós da solução
    index_first = 0;                 // primeiro índice da solução
    index_last = size_sol - 2;       // o último nó da solução é o retorno ao nó inicial 

    if (saving) {
        f_out << "\r\noriginal solution:\t\t"; solution->print(); cout << "\r\n"; 
    }

    for (int index_a = index_first+1; index_a < index_last-2; index_a++){

        for (int index_b = index_a+1; index_b < index_last-1; index_b++){
            int index_c = index_b + 1;

            for (int index_d = index_c+1; index_d <= index_last; index_d++){

                bool apply = true;  // o conjunto de trocas deve ser realizado

                // impedindo ponto de corte inválido:
                if ((index_a == index_first+1) && (index_d == index_last)) {
                    apply = false;
                }

                if (apply){

                    int best_sol = 0;  // zero corresponde à solução original
                    float new_best_cost;
                    float current_best_cost;

                    // solução 1: a <-> b, c <-> d
                    MtspSolution* new_sol_1 = new MtspSolution(size_sol, depot); 
                    // copiando nós:
                    for (int i=0; i < size_sol; i++){ new_sol_1->add(solution->node(i), 0.0); }
                    // copiando outros dados:
                    new_sol_1->set_total_cost(solution->get_total_cost());
                    new_sol_1->set_longest_route(solution->get_longest_route());
                    new_sol_1->set_iteration(solution->get_iteration_sol());
                    new_sol_1->set_time(solution->get_time_sol());            
                    new_sol_1->set_random_seed(solution->get_seed_rand());
                    // trocando a <-> b:
                    new_sol_1->change_nodes(index_a, index_b, cost_matrix);  // troca a posição dos nós e recalcula a solução temporária
                    // trocando c <-> d:
                    new_sol_1->change_nodes(index_c, index_d, cost_matrix);  // troca a posição dos nós e recalcula a solução temporária

                    if (mtsp_objective == 1){  // minimizar a longest_route 
                        new_best_cost = new_sol_1->get_longest_route();
                        current_best_cost = solution->get_longest_route();
                    }
                    else{  // minimizar o custo total
                        new_best_cost = new_sol_1->get_total_cost();
                        current_best_cost = solution->get_total_cost();
                    }
                    if (new_best_cost < current_best_cost){    // a nova solução é melhor, de acordo com o objetivo
                        if (new_sol_1->test_validate()){  // retorna verdadeiro se a solução for válida
                            current_best_cost = new_best_cost;
                            best_sol = 1;  // código da solução 1
                        }
                    }

                    // solução 2: a <-> c, b <-> d
                    MtspSolution* new_sol_2 = new MtspSolution(size_sol, depot); 
                    // copiando nós:
                    for (int i=0; i < size_sol; i++){ new_sol_2->add(solution->node(i), 0.0); }
                    // copiando outros dados:
                    new_sol_2->set_total_cost(solution->get_total_cost());
                    new_sol_2->set_longest_route(solution->get_longest_route());
                    new_sol_2->set_iteration(solution->get_iteration_sol());
                    new_sol_2->set_time(solution->get_time_sol());            
                    new_sol_2->set_random_seed(solution->get_seed_rand());
                    // trocando a <-> c:
                    new_sol_2->change_nodes(index_a, index_c, cost_matrix);  // troca a posição dos nós e recalcula a solução temporária
                    // trocando b <-> d:
                    new_sol_2->change_nodes(index_b, index_d, cost_matrix);  // troca a posição dos nós e recalcula a solução temporária

                    if (mtsp_objective == 1){  // minimizar a longest_route 
                        new_best_cost = new_sol_2->get_longest_route();
                    }
                    else{  // minimizar o custo total
                        new_best_cost = new_sol_2->get_total_cost();
                    }
                    if (new_best_cost < current_best_cost){    // a nova solução é melhor, de acordo com o objetivo
                        if (new_sol_2->test_validate()){  // retorna verdadeiro se a solução for válida
                            current_best_cost = new_best_cost;
                            best_sol = 2;  // código da solução 2
                        }
                    }

                    // solução 3: a <-> c, b <-> d, a <-> b
                    MtspSolution* new_sol_3 = new MtspSolution(size_sol, depot); 
                    // copiando nós da solução 2:
                    for (int i=0; i < size_sol; i++){ new_sol_3->add(new_sol_2->node(i), 0.0); }
                    // copiando outros dados:
                    new_sol_3->set_total_cost(new_sol_2->get_total_cost());
                    new_sol_3->set_longest_route(new_sol_2->get_longest_route());
                    new_sol_3->set_iteration(new_sol_2->get_iteration_sol());
                    new_sol_3->set_time(new_sol_2->get_time_sol());            
                    new_sol_3->set_random_seed(new_sol_2->get_seed_rand());

                    // trocando a <-> b:
                    new_sol_3->change_nodes(index_a, index_b, cost_matrix);  // troca a posição dos nós e recalcula a solução temporária

                    if (mtsp_objective == 1){  // minimizar a longest_route 
                        new_best_cost = new_sol_3->get_longest_route();
                    }
                    else{  // minimizar o custo total
                        new_best_cost = new_sol_3->get_total_cost();
                    }
                    if (new_best_cost < current_best_cost){    // a nova solução é melhor, de acordo com o objetivo
                        if (new_sol_3->test_validate()){  // retorna verdadeiro se a solução for válida
                            current_best_cost = new_best_cost;
                            best_sol = 3;  // código da solução 3
                        }
                    }

                    // solução 4: a <-> c, b <-> d, c <-> d
                    MtspSolution* new_sol_4= new MtspSolution(size_sol, depot); 
                    // copiando nós da solução 2:
                    for (int i=0; i < size_sol; i++){ new_sol_4->add(new_sol_2->node(i), 0.0); }
                    // copiando outros dados:
                    new_sol_4->set_total_cost(new_sol_2->get_total_cost());
                    new_sol_4->set_longest_route(new_sol_2->get_longest_route());
                    new_sol_4->set_iteration(new_sol_2->get_iteration_sol());
                    new_sol_4->set_time(new_sol_2->get_time_sol());            
                    new_sol_4->set_random_seed(new_sol_2->get_seed_rand());

                    // trocando c <-> d:
                    new_sol_4->change_nodes(index_c, index_d, cost_matrix);  // troca a posição dos nós e recalcula a solução temporária

                    if (mtsp_objective == 1){  // minimizar a longest_route 
                        new_best_cost = new_sol_4->get_longest_route();
                    }
                    else{  // minimizar o custo total
                        new_best_cost = new_sol_4->get_total_cost();
                    }
                    if (new_best_cost < current_best_cost){    // a nova solução é melhor, de acordo com o objetivo
                        if (new_sol_4->test_validate()){  // retorna verdadeiro se a solução for válida
                            current_best_cost = new_best_cost;
                            best_sol = 4;  // código da solução 4
                        }
                    }

                    switch (best_sol){ 
                        case 0: {  // não faz nada: a solução atual é a melhor
                            delete new_sol_4;
                            delete new_sol_3;
                            delete new_sol_2;
                            delete new_sol_1;
                            break;
                        }
                        case 1: {
                            delete solution;
                            solution = new_sol_1;
                            solution->set_time((((float)clock() - time_ini_execution)/CLOCKS_PER_SEC)*1000);  // atualizando o tempo de criação da solução
                            delete new_sol_4;
                            delete new_sol_3;
                            delete new_sol_2;
                            break;
                        }
                        case 2: {
                            delete solution;
                            solution = new_sol_2;
                            solution->set_time((((float)clock() - time_ini_execution)/CLOCKS_PER_SEC)*1000);  // atualizando o tempo de criação da solução
                            delete new_sol_4;
                            delete new_sol_3;
                            delete new_sol_1;
                            break;
                        }
                        case 3: {
                            delete solution;
                            solution = new_sol_3;
                            solution->set_time((((float)clock() - time_ini_execution)/CLOCKS_PER_SEC)*1000);  // atualizando o tempo de criação da solução
                            delete new_sol_4;
                            delete new_sol_2;
                            delete new_sol_1;
                            break;
                        }
                        case 4: {
                            delete solution;
                            solution = new_sol_4;
                            solution->set_time((((float)clock() - time_ini_execution)/CLOCKS_PER_SEC)*1000);  // atualizando o tempo de criação da solução
                            delete new_sol_3;
                            delete new_sol_2;
                            delete new_sol_1;
                            break;
                        }
                    }
                    if (best_sol != 0){ // foi encontrada uma solução melhor
                        updated_solution = true;
                    }
                }
            }
        }
    }
    */
    return updated_solution;
}
