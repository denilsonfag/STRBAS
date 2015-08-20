/* 
 * File:   STACS.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "STACS.hpp"

STACS::STACS(MtspInstance* &instance, int param){
    // variáveis da instância:
    n_salesmen = instance->get_n_salesmen();
    cost_matrix = instance->get_cost_matrix();
    n_total_nodes = instance->get_n_nodes();
    valid_nodes_instance = instance->get_valid_nodes_instance();
    n_valid_nodes = valid_nodes_instance->n_items();
    start_nodes_ants = instance->get_initial_nodes_routes();
    final_nodes_ants = instance->get_final_nodes_routes();
    
    beta = param;
    
    #if AS_MODE
        // parâmetros (tau0 é calculado na inicialização do feromônio):
        N = n_valid_nodes;  // dorigo 04 pg 71: n_salesmen = n
//        beta = 2;
        rho = 0.5;
    
        #if LOG_STACS
            f_log << "\r\nAnt System mode\r\n";
        #endif
    #endif
    
    #if EAS_MODE
        // parâmetros (tau0 é calculado na inicialização do feromônio):
        N = n_valid_nodes;  // dorigo 04 pg 71: n_salesmen = n
//        beta = 2;
        rho = 0.5;
        e = n_valid_nodes;
        
        #if LOG_STACS
            f_log << "\r\nElitist Ant System mode\r\n";
        #endif
    #endif

    #if RBAS_MODE
        // parâmetros (tau0 é calculado na inicialização do feromônio):
        N = n_valid_nodes;  // dorigo 04 pg 71: n_salesmen = n
//        beta = 2;
        rho = 0.1;
        w = 6;
        
        #if LOG_STACS
            f_log << "\r\nRank-Based Ant System mode\r\n";
        #endif
    #endif

    #if ACS_MODE
        // parâmetros (tau0 é calculado na inicialização do feromônio):
        N = 10;  // dorigo 04 pg 71: N = 10
//        beta = 2;
        rho = 0.1;
        q0 = 0.9;
        ksy = 0.1;
        
        #if LOG_STACS
            f_log << "\r\nAnt Colony System mode\r\n";
        #endif
    #endif
}
STACS::~STACS(){}

MtspSolution* STACS::run(long seed_random){
    
    time_ini_execution = (float)clock();  // instante do início da execução
    time_execution = 0;
            
    this->inicialization(seed_random);
    this->initialize_pheromone_matrix();
    
    cycle_counter = 0;   // contador de ciclos 
    non_improved_cycles_counter = 0;   // contador de ciclos sem melhora da best_so_far_solution da execução
    
    bool improved_last_cycle = false;  // sinaliza se foi encontrada uma soluçao melhor no último ciclo
    
    // Cada loop corresponde a um ciclo, no qual são criadas N soluções:
    while ((cycle_counter != SC_CYCLES) && (non_improved_cycles_counter != SC_NOIMPROV) && ((time_execution <= SC_SECONDS) || (SC_SECONDS == -1))){ // os três critérios de parada

        #if LOG_STACS
            f_log << "\r\n\r\n---------> Starting cycle " << cycle_counter+1 << "\r\n";
        #endif

        MtspSolution* best_solution_cycle = new MtspSolution(); // melhor solução do ciclo
        best_solution_cycle->create(n_valid_nodes, n_salesmen);
        
        for (created_sols_cycle_counter = 0; created_sols_cycle_counter < N; created_sols_cycle_counter++){ // em cada loop é criada uma solução
            
            MtspSolution* last_created_sol = &solutions_cycle[created_sols_cycle_counter];
            
            this->solution_construction(last_created_sol);

            this->local_search_all_created_solutions(last_created_sol);
            
            this->update_best_solution(best_solution_cycle, last_created_sol);
        }
        
        // atualizando a melhor solução da execução:
        improved_last_cycle = this->update_best_solution(best_solution_run, best_solution_cycle);
        
        if (improved_last_cycle){   // a solução foi atualizada
            non_improved_cycles_counter = 0;
            
            // imprimindo na tela todas as soluções melhoradas:
            if (cycle_counter == 0){ cout << " --> Starting run. Improved solutions:\r\n"; }
            cout << "cycle: " << cycle_counter+1 << "  \t"; best_solution_run->print();
        }
        else{  // a solução não foi atualizada
            non_improved_cycles_counter++;
        }

        this->global_pheromone_update(solutions_cycle, best_solution_cycle, best_solution_run);
        
        delete best_solution_cycle;
 
        #if LOG_STACS
            f_log << "\r\n---------> Finalized cycle " << cycle_counter+1 << "\r\n\r\n";
        #endif
        
        // atualizando contador de ciclos e de tempo:
        cycle_counter++;
        time_execution = ((float)clock() - time_ini_execution)/CLOCKS_PER_SEC;
    }
    
    this->finalization();
    
    return best_solution_run;
}

void STACS::inicialization(long seed_random){
    
    #if LOG_STACS
        f_log.open("outputs/log_stacs.txt", ios::app); f_log << setiosflags (ios::fixed) << setprecision(2);
        f_log << "\r\n----------------------------------------\r\nStarting run \tseed random = " << seed_random << "\r\n"; 
    #endif

    // rotinas específicas:
    random_generator = new Random(seed_random); // criando gerador de randomicos a partir da semente
    util = new Utilities();                     // funções auxliares 
    aco = new ACOAlgorithms();                  // rotinas ACO 
    local_search = new LocalSearch();           // buscas locais

    // varíáveis locais:
    tau0 = 0.0;
    pheromone_matrix = new FloatMatrix(n_total_nodes);      // criando matriz de feromônio
    ants = new Ant[n_salesmen];                             // criando as formigas
    sorted_lists = new IntList[n_total_nodes];              // criando as listas com os nós ordenados pelas distâncias
    for (int i=0; i < n_total_nodes; i++){ sorted_lists[i].create(n_total_nodes); }
    this->make_sorted_lists();
    
    candidate_list = new IntList();   
    candidate_list->create(n_valid_nodes);    // apenas nós válidos poderão ser incluídos
    prohibited_list = new IntList();     
    prohibited_list->create(n_total_nodes);   // todos os nós são incluídos
    
    solutions_cycle = new MtspSolution[N];    // soluções criadas no ciclo
    for (int i=0; i < N; i++){ 
        solutions_cycle[i].create(n_valid_nodes, n_salesmen); 
    }
    
    best_solution_run = new MtspSolution();   // não é deletada porque é o retorno da execução
    best_solution_run->create(n_valid_nodes, n_salesmen);
    
    #if LOG_STACS
        f_log << "\r\nInstance:\r\n"; 
        f_log << "   Number of nodes:\t " << n_valid_nodes << "\r\n"; 
        f_log << "   Number of salesmen:\t " << n_salesmen << "\r\n"; 
        f_log << "   Start nodes routes:\t";
        start_nodes_ants->save(f_log); f_log << "\r\n"; 
        f_log << "   Final nodes routes:\t";
        final_nodes_ants->save(f_log); f_log << "\r\n";
        f_log << "   Cost matrix:" << "\r\n";
        cost_matrix->save(f_log);
        f_log << "   Sorted nodes lists:" << "\r\n";
        this->save_sorted_lists(f_log);
        f_log << "\r\nACO Parameters:\r\n"; 
        this->save_setup(f_log);
    #endif
}
void STACS::initialize_pheromone_matrix(){

    // criando a solução do vizinho mais próximo:    
    MtspNearestNeighbor *nn_app = new MtspNearestNeighbor(n_valid_nodes, n_salesmen);
    nn_app->create_solution_workload_balance(cost_matrix, valid_nodes_instance);
    float cost_nearest_neighbor_sol = nn_app->get_total_cost();
    delete nn_app;
    
    #if AS_PHER_INIT  // Ant System
        tau0 = aco->AS_initial_pheromone_value(N, cost_nearest_neighbor_sol);
        #if LOG_STACS
            f_log << "\r\nAnt System pheromone inicialization\r\n";
        #endif
    #endif

    #if EAS_PHER_INIT  // Elitis Ant System
        tau0 = aco->EAS_initial_pheromone_value(N, rho, e, cost_nearest_neighbor_sol);
        #if LOG_STACS
            f_log << "\r\nElitis Ant System pheromone inicialization\r\n";
        #endif
    #endif

    #if RBAS_PHER_INIT  // Rank-Based Ant System
        tau0 = aco->RBAS_initial_pheromone_value(rho, w, cost_nearest_neighbor_sol);
        #if LOG_STACS
            f_log << "\r\nRank-Based Ant System pheromone inicialization\r\n";
        #endif
    #endif
            
    #if ACS_PHER_INIT  // Ant Colony System
        tau0 = aco->ACS_initial_pheromone_value(n_valid_nodes, cost_nearest_neighbor_sol);
        #if LOG_STACS
            f_log << "\r\nAnt Colony System pheromone inicialization\r\n";
        #endif
    #endif

    // preenchendo a matriz de feromônio com o tau0 calculado:
    for (int i=0; i < n_total_nodes; i++){
        for (int j=0; j < n_total_nodes; j++){
            pheromone_matrix->set_value(i,j,tau0);
        }
    }
            
    #if LOG_STACS
        f_log << "Nearest neighbor solution cost = " << cost_nearest_neighbor_sol << "\t\ttau0 = " << setprecision(10) << tau0 << setprecision(2) << "\r\n";
        this->save_pheromone_matrix(f_log);
    #endif
}

void STACS::solution_construction(MtspSolution* &solution){
    
    #if LOG_STACS
        f_log << "\r\n------> Starting solution construction\t  cycle: " << cycle_counter + 1 << "\tsolution: " << created_sols_cycle_counter + 1 << "\r\n";
    #endif

    prohibited_list->clear();                   // esvaziando a lista de nós proibidos
    this->add_prohibited_list_invalid_nodes();  // adicionando os nós que não são válidos à visited_list

    this->initialize_colony(solution);  // resetando as formigas (so nos iniciais e finais são adicionados à prohibited_list)

    // cada loop representa um movimento de uma formiga:
    while (!prohibited_list->is_full()){   // enquanto a lista de nós proibidos não estiver cheia

        #if LOG_STACS
            f_log << "\r\n---> Starting colony movement\r\n";
        #endif

        selected_ant = this->select_ant(solution);  // selecionando a formiga para se mover

        bool end_cbm_attempts = false;  // flag da repetição pela busca de um movimento melhor
        int attempts_counter = -1;  // contagem das tentativas
        while(!end_cbm_attempts){  // enquanto não atingir o número maximo de tentativas
            attempts_counter ++;

            selected_node = this->select_node();  // selecionando um nó para a formiga escolhida se mover
            
            if (attempts_counter < CBM_ATTEMPTS) {
                
                int better_ant = this->check_movement(solution);
                
                if (selected_ant == better_ant) {  end_cbm_attempts = true; }  // é o melhor movimento
                else{ // atualizando para a melhor formiga
                    selected_ant = better_ant; 
                    #if LOG_STACS
                        f_log << "-> New ant selected:\t\t\t\t\t " << selected_ant << "\tcurrent node: " << ants[selected_ant].current_node() << "\r\n";
                    #endif
                }  
            }
            else{ end_cbm_attempts = true;}  // alcançou limite de tentativas
        }

        this->move_ant(selected_ant, selected_node, solution);  // movimenta a formiga escolhida para o nó escolhido
        
        prohibited_list->add_value(selected_node);  // adiciona o nó visitado na lista de proibidos
    }
    
    this->finalize_solution(solution);
}

void STACS::initialize_colony(MtspSolution* &solution){
    solution->reset();  // resetando a solução
    // inicializando as formigas:
    for (int k=0; k < n_salesmen; k++){
        int start_node = start_nodes_ants->value(k);
        int end_node = final_nodes_ants->value(k);
        ants[k].reset(start_node, end_node);                // reiniciando formiga
        solution->add_node(k, start_node, cost_matrix);     // adicionando nós iniciais à solução
        if (!prohibited_list->in_the_list(start_node)){
            prohibited_list->add_value(start_node);         // adicionando nós iniciais à lista de visitados
        }
        if (!prohibited_list->in_the_list(end_node)){
            prohibited_list->add_value(end_node);           // adicionando nós finais à lista de visitados
        }
    }
}

int STACS::select_ant(MtspSolution* &solution){
    int selected_ant = -1;
    
    #if TACO_ANT_SELECTION
        selected_ant = aco->TACO_ant_selection(solution);
        #if LOG_STACS
            f_log << "Selected ant with shortest partial route (TACO):\t " << selected_ant << "\r\n"; 
        #endif
    #endif

    #if LOG_STACS
        f_log << "   Current node selected ant:\t\t\t\t";
        if (ants[selected_ant].current_node() < 10)  f_log << " ";
        f_log << ants[selected_ant].current_node(); f_log << "\r\n";
        f_log << "   Executed route:\t\t\t\t\t"; solution->save_route(selected_ant, f_log); f_log << "\r\n";
        f_log << "Prohibited list:\t\t\t\t\t"; prohibited_list->save(f_log); f_log << "\r\n";
    #endif
    return selected_ant;
}
int STACS::select_node(){
    int current_node = ants[selected_ant].current_node();
    int selected_node = -1;
    
    this->update_candidate_list();  // atualizando a lista de nós candidatos para o nó no qual a formiga escolhida se encontra:

    #if AS_STR
        selected_node = aco->AS_state_transition_rule(current_node, candidate_list, random_generator, cost_matrix, pheromone_matrix, ALPHA, beta);
        #if LOG_STACS
            f_log << "Selected node by AS STR  = " << selected_node << "\r\n"; 
        #endif
    #endif

    #if ACS_STR
        selected_node = aco->ACS_state_transition_rule(current_node, candidate_list, random_generator, cost_matrix, pheromone_matrix, util, ALPHA, beta, q0);
        #if LOG_STACS
            f_log << "Selected node by ACS state transition rule:\t\t " << selected_node << "\r\n"; 
        #endif
    #endif

    return selected_node;
}
int STACS::check_movement(MtspSolution* &solution){
    return aco->TACO_check_better_movement(selected_node, n_salesmen, ants, cost_matrix, solution); // retorna a formiga que resulta no melhor movimento ao nó escolhido
}
void STACS::move_ant(int moving_ant, int next_node, MtspSolution* &solution){
    int old_node = ants[moving_ant].current_node();
    ants[moving_ant].move(next_node);
    solution->add_node(moving_ant, next_node, cost_matrix);
    
    #if LOG_STACS
        f_log << "Performed movement:\t\t\t\t\t ant " << moving_ant << " from node " << old_node <<" to node " << next_node <<  "\r\n";
    #endif
        
    this->local_pheromone_update(old_node, next_node);    
}
void STACS::local_pheromone_update(int node_a, int node_b){
    #if ACS_LPU
        aco->ACS_local_pheromone_update(node_a, node_b, pheromone_matrix, ksy);  // atualização local de feromônio como no ACS
        #if LOG_STACS
            f_log << "ACS local pheromone update performed in edge\t\t " << node_a << " -> " << node_b <<  "\r\n";
        #endif
    #endif
}

void STACS::finalize_solution(MtspSolution* &solution){
    #if LOG_STACS
        f_log << "\r\n---> Returning ants to their initial nodes\r\n";
    #endif

    // deslocando as formigas aos seus nos finais:
    for (int k=0; k < n_salesmen; k++){  
        int final_node = final_nodes_ants->value(k);
        this->move_ant(k, final_node, solution);
    }

    #if LOG_STACS
        f_log << "Criated routes:\r\n";
        for (int k=0; k<n_salesmen; k++){
            f_log << "  ant " << k << ":\t\t\t\t\t\t";
            solution->save_route(k, f_log);
            f_log << "\r\n";
        }
    #endif
    
    // calculando e incluindo dados finais na solução:
    int cycle = cycle_counter+1;
    float time = (((float)clock() - time_ini_execution)/CLOCKS_PER_SEC)*1000;
    long seed = random_generator->get_first_seed();
    solution->finalize_solution(cycle, time, seed);
    
    #if LOG_STACS
        f_log << "\r\n---> Complete solution created:\t\t\t\t";
        solution->save(f_log);
    #endif
}
void STACS::local_search_all_created_solutions(MtspSolution* &solution){
    #if LS_2_OPT_ALL
        if (TYPE_COST_MATRIX == 1){  // matriz simétrica 
            local_search->two_opt_basic_mtsp_solution(MTSP_OBJECTIVE, solution, cost_matrix);
        }
        else{
            Route* routes = solution->pointer_routes();
            for (int k=0; k < n_salesmen; k++){
                Route* route = &routes[k];
                local_search->two_opt_route(route, cost_matrix);
            }
        }
    #endif
}

void STACS::global_pheromone_update(MtspSolution* &solutions_cycle, MtspSolution* &best_solution_cycle, MtspSolution* &best_solution_run){
    #if AS_GPU
        aco->AS_global_pheromone_update(MTSP_OBJECTIVE, pheromone_matrix, N, solutions_cycle, rho);  // atualização global de feromônio como no AS
        
        #if LOG_STACS
            f_log << "\r\nAS global pheromone update performed.\t";
            f_log << "\ttau0 = " << setprecision(10) << tau0 << setprecision(2) << "\r\n";
            this->save_pheromone_matrix(f_log);
        #endif
    #endif
    
    #if EAS_GPU
        aco->EAS_global_pheromone_update(MTSP_OBJECTIVE, pheromone_matrix, N, solutions_cycle, best_solution_run, rho, e);
        
        #if LOG_STACS
            f_log << "\r\nEAS global pheromone update performed.\t";
            f_log << "\ttau0 = " << setprecision(10) << tau0 << setprecision(2) << "\r\n";
            this->save_pheromone_matrix(f_log);
        #endif
    #endif

    #if RBAS_GPU
        aco->RBAS_global_pheromone_update(MTSP_OBJECTIVE, pheromone_matrix, N, solutions_cycle, best_solution_run, rho, w);
        
        #if LOG_STACS
            f_log << "\r\nRank-Based global pheromone update performed.\t";
            f_log << "\ttau0 = " << setprecision(10) << tau0 << setprecision(2) << "\r\n";
            this->save_pheromone_matrix(f_log);
        #endif
    #endif
            
    #if ACS_GPU
        aco->ACS_global_pheromone_update(MTSP_OBJECTIVE, n_salesmen, best_solution_run, pheromone_matrix, rho);  // atualização global de feromônio como no ACS
        
        #if LOG_STACS
            f_log << "\r\n---> ACS global pheromone update performed\t";
            f_log << "tau0 = " << setprecision(10) << tau0 << setprecision(2) << "\r\n";
            this->save_pheromone_matrix(f_log);
        #endif
    #endif
}

void STACS::finalization(){
    delete local_search;
    delete aco;
    delete util;
    delete random_generator;
    delete [] solutions_cycle;
    delete prohibited_list;
    delete candidate_list;
    delete [] sorted_lists;
    delete [] ants;
    delete pheromone_matrix;
    #if LOG_STACS
        f_log << "\r\n\r\nCompleted run. Time execution =  " << setprecision(0) << time_execution << setprecision(2) << " seconds.\r\n"; 
        f_log.close();
    #endif
}

// funcões auxiliares:

// cria listas com os índices ordenados pelos valores da matriz de custos:
void STACS::make_sorted_lists(){  
    for (int i=0; i < n_total_nodes; i++){
        sorted_lists[i].clear();
        float* col_cost_matrix = cost_matrix->get_col_matrix(i);
        util->sort_float_vector(col_cost_matrix, sorted_lists[i]);
    }
}
void STACS::add_prohibited_list_invalid_nodes(){
    for (int i=0; i < n_total_nodes; i++){
        if (!valid_nodes_instance->in_the_list(i)){  // se o nó não for válido, inserir na tabu_list
            if (!prohibited_list->in_the_list(i)){
                prohibited_list->add_value(i);
            }
        }
    }
}
void STACS::update_candidate_list(){
    int current_node = ants[selected_ant].current_node();
    int candidate_node = -1;
    candidate_list->clear();
    for (int i=0; i < n_total_nodes; i++){
        candidate_node = sorted_lists[current_node].value(i);
        if (!prohibited_list->in_the_list(candidate_node)){
            candidate_list->add_value(candidate_node);
        }
        // limitando a candidate_list pelo parâmetro CL:
        if (candidate_list->n_items() == SIZE_CAND_LIST) i = n_total_nodes;
    }
    #if LOG_STACS
        f_log << "Candidate list :\t\t\t\t\t"; candidate_list->save(f_log); f_log << "\r\n";
    #endif
}
bool STACS::update_best_solution(MtspSolution* &current_best_solution, MtspSolution* &new_solution){
    float cost_current_best_solution = current_best_solution->cost_by_objective(MTSP_OBJECTIVE);
    float cost_new_solution = new_solution->cost_by_objective(MTSP_OBJECTIVE);
    
    if ((cost_current_best_solution == 0.0) || (cost_new_solution < cost_current_best_solution)){
        this->copy_solution(new_solution, current_best_solution);
        return true;
    }
    else{
        return false;
    }
}
void STACS::copy_solution(MtspSolution* &source_solution, MtspSolution* &target_solution){
    target_solution->reset();
    // copiando as rotas:
    int n_routes = source_solution->n_routes_sol();
    for (int k=0;k < n_routes; k++){
        int n_nodes_route = source_solution->n_nodes_route(k);
        for (int i=0;i < n_nodes_route; i++){
            int curr_node = source_solution->node_route(k, i);
            target_solution->add_node(k, curr_node, cost_matrix);
        }
    }
    // incluindo dados finais e calculando solução copiada:
    int cycle = source_solution->cycle_sol();
    int time = source_solution->time_exec_sol();
    long seed = source_solution->random_seed_sol();
    target_solution->finalize_solution(cycle, time, seed);
}    

void STACS::print_sorted_lists(){
    for (int i=0; i < n_total_nodes; i++){ 
        cout << i << ": ";
        sorted_lists[i].print();
        cout << "\r\n";
    }
}
void STACS::print_detail_cands_list(int moving_ant){  // imprime em detalhes a lista de nós candidatos:
    if (!candidate_list->is_empty()){
        int current_node = ants[moving_ant].current_node();
        cout << "\r\nnodes\t";
        for (int i=0; i<candidate_list->n_items(); i++){
            cout << candidate_list->value(i) << "\t";
        }
        cout << "\r\ndist\t";
        for (int i=0; i<candidate_list->n_items(); i++){
            float cost = cost_matrix->get_value(current_node, candidate_list->value(i));
            cout << cost << "\t";
        }
        cout << "\r\npher\t";
        for (int i=0; i<candidate_list->n_items(); i++){
            cout << pheromone_matrix->get_value(current_node,candidate_list->value(i)) << "\t";
        }
        cout << "\r\n";
    }
    else{
        cout << "--> empty candidate list\r\n";
    }
}
void STACS::save_sorted_lists(ofstream& f_log){
    for (int i=0; i < n_total_nodes; i++){ 
        f_log << i << ": ";
        sorted_lists[i].save(f_log);
        f_log << "\r\n";
    }
}
void STACS::save_setup(ofstream& f_log){
    f_log << "STACS setup:\r\n";
    f_log << "   Algorithm based:\t\t\t\t";
    #if AS_MODE
        f_log << "Ant System mode\r\n";
    #endif
    #if EAS_MODE
        f_log << "Elitist Ant System mode\r\n";
    #endif
    #if RBAS_MODE
        f_log << "Rank_based Ant System mode\r\n";
    #endif
    #if ACS_MODE
        f_log << "Ant Colony System mode\r\n";
    #endif

    f_log << "   MTSP objective:\t\t\t\t";
    if (MTSP_OBJECTIVE == 1) f_log << "minimize longest route (workload balance)\r\n";
    if (MTSP_OBJECTIVE == 2) f_log << "minimize total cost\r\n";
    f_log << "   Stopping criterion:\t\t\t\t";
    if (SC_NOIMPROV > 0) f_log << SC_NOIMPROV << " cycles without improved solution\r\n";
    if (SC_SECONDS > 0) f_log << SC_SECONDS << " seconds per execution\r\n";
    if (SC_CYCLES > 0) f_log << SC_CYCLES << " cycles per execution\r\n";
    f_log << "   Candidate list size:\t\t\t\t" << SIZE_CAND_LIST << "\r\n";  
    f_log << "   Check best movement attempts:\t\t" << CBM_ATTEMPTS << "\r\n"; 
    f_log << "   N (solutions generated in each cycle):\t" << N << "\r\n";
    f_log << "   alfa (pheromone weight):\t\t\t" << ALPHA << "\r\n";
    f_log << "   beta (visibility weight):\t\t\t" << beta << "\r\n";
    f_log << "   rho (pheromone evaporation rate):\t\t" << rho << "\r\n";
    
    #if EAS_MODE
        f_log << "   e (weight of best so far solution on GPU):\t" << e << "\r\n";
    #endif

    #if ACS_MODE
        f_log << "   q0 (determinism level):\t\t\t" << q0 << "\r\n";
        f_log << "   ksi (pheromone evaporation rate in LPU):\t" << ksy << "\r\n";
    #endif

    #if LS_2_OPT_CYC
        f_log << "   Local search:\t\t\t\t2-opt inter routes in best solutions cycles\r\n";
    #endif
    
    #if LS_2_OPT_ALL
        f_log << "   Local search:\t\t\t\t2-opt intra and inter routes in all created solutions\r\n";
    #endif

}
void STACS::save_pheromone_matrix(ofstream& f_log){
    f_log << setprecision(10);
    f_log << "Pheromone matrix (in relation to tau0)\t\ttau0 = " << tau0 << "\r\n"; 
    f_log << setprecision(0);
    for (int i=0; i < n_total_nodes; i++){
        f_log << "\t" << i; 
    }
    for (int i=0; i < n_total_nodes; i++){
        f_log << "\r\n" << i;
        for (int j=0; j < n_total_nodes; j++){
            float current_pher = pheromone_matrix->get_value(i,j); 
            if(current_pher == tau0){
                f_log << "\t=";
            }    
            else{
                f_log << "\t" << (current_pher / tau0) * 100 << "%";    
            }
        }
    }
    f_log << "\r\n";
    f_log << setprecision(2);
}
