/* 
 * File:   Simulation.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "Simulator.hpp"

Simulation::Simulation(int current_id_work_day, long seed_random){
    randon_seed_simulation = seed_random;
    this->depot = REAL_DEPOT;
    id_work_day = current_id_work_day;
    real_data = new RealData(depot);
    real_matrix = new RealMatrix();
    random_generator = new Random(randon_seed_simulation);    
    
    f_longests.open("outputs/longests_costs.txt", ios::app);
    f_longests << setiosflags (ios::fixed) << setprecision(2); 
    f_total_costs.open("outputs/total_costs.txt", ios::app);
    f_total_costs << setiosflags (ios::fixed) << setprecision(2);
}
Simulation::~Simulation(){
    f_total_costs.close();
    f_longests.close();
    
    delete final_solution;
    delete real_solution;
    delete random_generator;
    delete current_instance;
    delete real_time_matrix;
    delete real_distance_matrix;
    delete [] day_services;
    delete real_matrix;
    delete real_data;
}

void Simulation::load_data_work_day(){

    int n_services = real_data->count_services(id_work_day);
    n_points = n_services + 1; // para incluir o depósito
    n_teams = real_data->count_teams(id_work_day); // definindo número de equipes do dia de trabalho
    
    // criando instância para preencher grafo euclidiano com coordenadas UTM
    current_instance = new MtspInstance();
    current_instance->create_empty_instance(n_points, n_teams);
    
    // inserindo o depósito como primeiro nó do grafo
    int depot_x = real_data->x_depot();
    int depot_y = real_data->y_depot();
    current_instance->add_plan_node(depot_x, depot_y);
    // criando o vetor de nós planos
    real_data->load_euclidean_graph(id_work_day, current_instance);
    
    // carregando matrizes com custos reais
    real_distance_matrix = new FloatMatrix(n_points);
    real_matrix->load_real_distance_matrix(id_work_day, real_distance_matrix);
    if (PRINT_COSTS_MATRIX) { 
        cout << "\r\nReal distance "; 
        real_distance_matrix->print(); 
    }
    real_time_matrix = new FloatMatrix(n_points);
    real_matrix->load_real_time_matrix(id_work_day, real_time_matrix);
    if (PRINT_COSTS_MATRIX) { 
        cout << setiosflags (ios::fixed) << setprecision(0);
        cout << "\r\nReal time "; 
        real_time_matrix->print();
        cout << setiosflags (ios::fixed) << setprecision(2);
    }

    // criando estruturas com os dados de todos os serviços do dia
    day_services = new ServiceOrder[n_points]; // para que o índice de day_services corresponda ao índice das matrizes. O índice do deposito é desconsiderado.
    real_data->load_service_orders(id_work_day, day_services);
    
    // preenchendo a matriz de custos que será usada na criação das soluções
    // utiliza os tempos de execução em day_services, se a matriz for de tempos de desloc + tempos de execução
    this->mount_cost_matrix(); // preenche a matriz de custos da instância, a que será usada na construção das soluções
    if (PRINT_COSTS_MATRIX) { 
        cout << "\r\nApplied cost "; current_instance->print_cost_matrix();
    }
    
    // carregando solução real realizada de acordo com a matriz de custos definida em current_instance
    // já aplica DESPATCH_REAL_SOL: atualiza em day_services o tempo de execução do serviço de acordo como a solução real calculada (time_dispatch - partial_route_cost)
    real_solution = new MtspSolution();
    real_solution->create(n_points, n_teams);
    real_data->load_real_solution(id_work_day, real_solution, current_instance, day_services, real_time_matrix,DESPATCH_REAL_SOL );
    
    ofstream f_real_sol;
    f_real_sol.open("outputs/real_longests_costs.txt", ios::app);
    real_solution->save_longest_cost(f_real_sol); f_real_sol << "\r\n";
    f_real_sol.close();
    
    if (PRINT_DAY_SERVICES) { cout << "\r\n"; this->print_day_services(); }
}
// preenche a matriz de custos de current_instance, que será utilizada na aplicação
void Simulation::mount_cost_matrix(){
    switch (TYPE_COST_MATRIX){  // matriz de custos a ser utilizada na aplicação
        case 1: {   // distâncias euclidianas
            current_instance->calcule_euclidean_matrix(false, 1000);  // para apresentação dos resultados em quilômetros
            break;
        }
        case 2: {   // distâncias reais
            for (int i=0; i < n_points; i++){
                for (int j=0; j < n_points; j++){
                    float cost;
                    cost = real_distance_matrix->get_value(i, j);
                    current_instance->set_value_cost_matrix(i, j, cost);
                }
            }
            break;
        }
        case 3: {   // tempos reais
            for (int i=0; i < n_points; i++){
                for (int j=0; j < n_points; j++){
                    float cost;
                    cost = real_time_matrix->get_value(i,j);
                    current_instance->set_value_cost_matrix(i, j, cost);
                }
            }
            break;
        }
        case 4: {   // tempos reais de deslocamento + tempo de execução dos serviços
            for (int i=0; i < n_points; i++){
                for (int j=0; j < n_points; j++){
                    float cost;
                    if (i == j){
                        cost = 0;
                    }
                    else{
                        int time_exec_serv;
                        if (j != depot) {
                            time_exec_serv = day_services[j].get_time_execution();
                        }
                        else {
                            time_exec_serv = 0;
                        }
                        float time_displacement = real_time_matrix->get_value(i, j);
                        cost = time_exec_serv + time_displacement;
                    }
                    current_instance->set_value_cost_matrix(i, j, cost);
                }
            }
            break;
        }
    }
}


void Simulation::execute_static_simulation(int counter_simulations, ofstream& f_log_exper, int param){
  
    // todas as ordens de serviço são válidas
    this->set_valid_all_nodes();
    
    // posicionando todas as equipes no depósito:
    this->position_teams_depot();
    
    // gerando uma solução com o algoritmo ACO para a instância completa e salvando em complete_final_solution
    STACS* stacs_app = new STACS(current_instance, param);
    
    double rand_number = random_generator->random_number();
    while (rand_number < 999999999){  // semente com 10 dígitos, como as geradas a partir da função time())
        rand_number = rand_number*10;
    }
    long seed_stacs = (long)rand_number;
            
    final_solution = stacs_app->run(seed_stacs);
    delete stacs_app;
    
    // sobrescrevendo a semente do STACS pela semente da simulação:
    final_solution->set_random_seed(randon_seed_simulation);

    // finalizando o experimento
    this->finalize_simulation(counter_simulations, f_log_exper);
}
void Simulation::set_valid_all_nodes(){
    current_instance->reset_valid_nodes_instance();
    // os indices de day_services correspondem ao id dos nodes. o índice do depósito é desconsiderado:
    for (int s=0; s < n_points; s++){ 
        if (s!=depot){
            current_instance->add_valid_node(s); // adicionando o serviço como um nó válido
        }
    }
}
void Simulation::position_teams_depot(){ // posicionando todas as equipes no depósito
    current_instance->reset_initial_nodes_routes();
    current_instance->reset_final_nodes_routes();
    for (int k = 0; k < n_teams; k++){
        current_instance->add_initial_node_route(depot);
        current_instance->add_final_node_route(depot);  // definindo os nós finais das rotas
    }
}


void Simulation::run_din_simul_new_order(int counter_simulations, ofstream& f_log_exper){
    
    current_time = 0.0; // relógio da simulação em segundos, definindo 0.0 como 08:00 hs do dia de trabalho atual 
    
    // define os nós finais das rotas como sendo o depósito e adiciona o depósito para todas as equipes em executed_routes
    this->initialize_dinamic_simulation();
    
    bool end_work_day = false;
    while (!end_work_day){  

        MtspSolution* current_stacs_solution;
        bool generated_aco_solution = false;
        
        // enquanto houver emergencias em valid_nodes
            // atualizar valid_nodes
            // despachar emergencias
        int n_valid_nodes = 0;
        bool emergency_in_valid_nodes = true;
        while (emergency_in_valid_nodes){
            this->update_valid_nodes_instance();
            n_valid_nodes = current_instance->get_n_valid_nodes();
            emergency_in_valid_nodes = false;
            for (int i=0; i < n_valid_nodes; i++){
                int valid_node = current_instance->get_valid_node(i);
                if (day_services[valid_node].is_emergency()){
                    emergency_in_valid_nodes = true;
                    this->emergency_dispatch(valid_node, f_log_exper);
                }
            }
        }
        
        // se valid_nodes <= n_teams
            // despachar às equipes mais próximas
        // senão
            // atualizar posição das equipes na instância
            // gerar solução ACO para valid_nodes
        if (n_valid_nodes <= n_teams){
            for (int i=0; i<n_valid_nodes; i++){
                int valid_node = current_instance->get_valid_node(i);
                this->emergency_dispatch(valid_node, f_log_exper);
            }
        }
        else{
            // atualizando a posição das equipes na instância a partir de executed_routes 
            this->update_positions_teams();       

            // gerando uma semente aleatória para o STACS:
            double rand_number = random_generator->random_number();
            while (rand_number < 999999999){  // semente com 10 dígitos, como as geradas a partir da função time())
                rand_number = rand_number*10;
            }
            long seed_stacs = (long)rand_number;
            
            // gerando uma solução com o algoritmo ACO para a instância atual e salvando em current_best_solution
            int beta = 2;
            STACS* aco_mtsp_app = new STACS(current_instance, beta);
            current_stacs_solution = aco_mtsp_app->run(seed_stacs);
            delete aco_mtsp_app;
            generated_aco_solution = true;
            
            #if DETAIL_LOG_EXPER
                f_log_exper << "\r\nCreated MTSP solution:\r\n";
                current_instance->save_valid_nodes(f_log_exper);
                current_instance->save_positions_teams(f_log_exper);
                f_log_exper << "Solution: ";
                current_stacs_solution->save_how_list(f_log_exper);
                f_log_exper << "\r\n";
            #endif

            // salvando a solução ACO gerada
            #if SAVE_PARTIAL_DAY_SOLS_PLOT
                // salvando em aco_partial_sols_day_to_plot.txt a solução final para o dia de trabalho
                ofstream f_day_aco_sols;
                f_day_aco_sols.open("outputs/plot_aco_partial_sols_day.txt", ios::app);
                Node* plan_nodes_vector = current_instance->get_plan_nodes_vector();
                f_day_aco_sols << "\r\nDia de trabalho: " << id_work_day << "\tSimulação: " << counter_day_simulations <<"\r\n";
                current_stacs_solution->save_to_plot(f_day_aco_sols, plan_nodes_vector);
                f_day_aco_sols.close();
            #endif
        }
        
        // verificar o horário de surgimento da nova ordem
        // se houver uma nova ordem:
            // atualizar current_time de acordo com o horário de surgimento da nova ordem + 1
        // senão
            // atualizar current_time para o momento final do dia de trabalho
            // end_work_day = true;
        float time_despatch_next_service = this->time_appearance_next_service_current_time();
        if (time_despatch_next_service == -1.0){ // não existem mais ordens a surgir
            end_work_day = true;   // finalizando a simulação: não existem serviços a surgir
            current_time = 360000.0; // final do dia de trabalho
        }
        else{
            current_time = time_despatch_next_service + 1.0;  // o próximo loop terá ao menos uma ordem válida
        }

        // despachar os serviços até current_time de acordo com a solução ACO
        if (generated_aco_solution){ // para confirmar que foi gerada ao menos uma solução ACO durante o dia de trabalho
            
            for (int k = 0; k < n_teams; k++){
                
                int cur_posit_k = final_solution->last_node_route(k);
                float temp_cost_k = final_solution->temporal_cost_route(k);
                int size_route_sol = current_stacs_solution->n_nodes_route(k);

                bool start_copy = false;
                for (int i=0; i < size_route_sol; i++){

                    int cur_node_sol = current_stacs_solution->node_route(k,i);

                    if (start_copy){
                        
                        if (cur_node_sol == depot){ // a cópia alcançou um depósito: parar de copiar a rota
                             i = size_route_sol;
                        }
                        else{  // copiar normalmente:
                            // calculando custo previsto:
                            float prev_travel_time = real_time_matrix->get_value(cur_posit_k, cur_node_sol);
                            float prev_exec_time = day_services[cur_node_sol].get_time_execution();
                            float previst_temporal_cost = temp_cost_k + prev_travel_time + prev_exec_time;

                            if (previst_temporal_cost < current_time){
                                this->dispatch_service_order(k, cur_node_sol, f_log_exper);
                            }
                            else{
                                i = size_route_sol;
                            }
                        }
                    }
                    if (cur_node_sol == cur_posit_k){
                        start_copy = true;
                    }
                }
            }
            delete current_stacs_solution;
        }
    }
    
    // montando a solução de executed_routes em proposed_final_solution
    this->finalize_final_solution();
    
    // finalizando o experimento:
    this->finalize_simulation(counter_simulations, f_log_exper);
}


void Simulation::run_din_simul_available_team(int counter_day_simulations, ofstream& f_log_exper, bool nearest_team){
  
    current_time = 0.0; // relógio da simulação em segundos, definindo 0.0 como 08:00 hs do dia de trabalho atual 
    
    // adiciona o depósito para todas as equipes em executed_routes
    this->initialize_dinamic_simulation();
    
    bool end_work_day = false;
    while (!end_work_day){  // em cada loop é despachado um serviço
        
        MtspSolution* current_stacs_solution;
    
        // atualizando next_emergency e time_dispatch_next_emergency (horário de despacho da próxima emergência)
        // se next_emergency == -1, não existem mais emergências a despachar
        this->update_next_emergency();
        
        // verifica qual a próxima equipe a ficar livre. Atualiza next_team_free e time_to_free_next_team
        this->update_next_team_free();
        
        // verificando se surgiu uma emergência até a próxima equipe ficar livre:
        if ((time_dispatch_next_emerg <= time_free_next_team) && (next_emergency != -1)){
            // despachando a emergência:
            this->emergency_dispatch(next_emergency, f_log_exper);
            // atualizando current_time (usado na atualização dos serviços válidos)
            // passa a ser o horário do serviço adicionado de 1 segundo:
            current_time = day_services[next_emergency].get_time_dispatch() + 1.0;
        }
        else{ // não surgiram emergências
            
            // atualizando os serviços válidos a partir de service_days e current_time
            // (comparando current_time com o tempo de despacho e excluindo os já executados)
            this->update_valid_nodes_instance();
            int n_valid_nodes = current_instance->get_n_valid_nodes();
            
            if (n_valid_nodes == 0){   // não existem serviços a executar em valid_nodes
                
                float time_despatch_next_service = this->time_appearance_next_service();
                if (time_despatch_next_service == -1.0){
                    end_work_day = true;   // finalizando os despachos: não existem serviço a executar
                }
                else{
                    current_time = time_despatch_next_service + 1.0;
                }
            }
    
            else {  // existem serviços a despachar
            
                int service_to_despatch; // serviço que será despachado
                int current_position = final_solution->last_node_route(next_team_free);  // posição atual da equipe (serviço em execução)
               
                if (n_valid_nodes < n_teams){  // se houver mais equipes que nós válidos, não gera solução ACO
                    
                    // despachar para a próxima equipe livre o serviço com melhor previsão de custo (atendimento + retorno ao depósito)
                    float best_estimated_cost;
                    for (int s=0; s < n_valid_nodes; s++){
                        
                        int service_point = current_instance->get_valid_node(s);
                        float cost_current_to_service = current_instance->get_value_cost_matrix(current_position, service_point);
                        float cost_service_to_depot = current_instance->get_value_cost_matrix(service_point, depot);
                        float estimated_cost = cost_current_to_service + cost_service_to_depot;
                        
                        if (s == 0){
                            service_to_despatch = service_point;
                            best_estimated_cost = estimated_cost;
                        }
                        else{
                            if (estimated_cost < best_estimated_cost){
                                service_to_despatch = service_point;
                                best_estimated_cost = estimated_cost;
                            }
                        }
                    }  
                }
                
                else{  // o número de serviços é igual ou maior ao número de equipes: gerar solução ACO

                    if (nearest_team){  // despachar o serviço que estiver mais próximo da equipe
                        
                        float best_cost;
                        for (int s=0; s < n_valid_nodes; s++){

                            int service_point = current_instance->get_valid_node(s);
                            float cost_current_to_service = current_instance->get_value_cost_matrix(current_position, service_point);

                            if (s == 0){
                                service_to_despatch = service_point;
                                best_cost = cost_current_to_service;
                            }
                            else{
                                if (cost_current_to_service < best_cost){
                                    service_to_despatch = service_point;
                                    best_cost = cost_current_to_service;
                                }
                            }
                        }  
                    }
                    else{  // gerar solução ACO
                        
                        // atualizando a posição das equipes na instância a partir de executed_routes 
                        this->update_positions_teams();       

                        // gerando uma solução com o algoritmo ACO para a instância atual e salvando em current_best_solution
                        int beta = 2;
                        STACS* aco_mtsp_app = new STACS(current_instance, beta);
                        
                        // gerando uma semente aleatória para o STACS:
                        double rand_number = random_generator->random_number();
                        while (rand_number < 999999999){  // semente com 10 dígitos, como as geradas a partir da função time())
                            rand_number = rand_number*10;
                        }
                        long seed_stacs = (long)rand_number;
                        
                        current_stacs_solution = aco_mtsp_app->run(seed_stacs);
                        delete aco_mtsp_app;
                        current_stacs_solution->set_random_seed(randon_seed_simulation);

                        #if DETAIL_LOG_EXPER
                            f_log_exper << "\r\nCreated MTSP solution:\r\n";
                            current_instance->save_valid_nodes(f_log_exper);
                            current_instance->save_positions_teams(f_log_exper);
                            f_log_exper << "Solution: ";
                            current_stacs_solution->save_how_list(f_log_exper);
                            f_log_exper << "\r\n";
                        #endif
                        
                        // salvando a solução ACO gerada
                        #if SAVE_PARTIAL_DAY_SOLS_PLOT
                            // salvando em aco_partial_sols_day_to_plot.txt a solução final para o dia de trabalho
                            ofstream f_day_aco_sols;
                            f_day_aco_sols.open("outputs/plot_aco_partial_sols_day.txt", ios::app);
                            Node* plan_nodes_vector = current_instance->get_plan_nodes_vector();
                            f_day_aco_sols << "\r\nDia de trabalho: " << id_work_day << "\tSimulação: " << counter_day_simulations <<"\r\n";
                            current_stacs_solution->save_to_plot(f_day_aco_sols, plan_nodes_vector);
                            f_day_aco_sols.close();
                        #endif
                        
                        // localizando o serviço a ser despachado à proxima equipe livre, de acordo com a solução ACO gerada
                        service_to_despatch = current_stacs_solution->next_node_route(next_team_free, current_position);

                        // a solução ACO afirma que o melhor é a equipe livre retornar ao depósito
                        // solução: despachar a solução com melhor custo previsto para a equipe
                        if (service_to_despatch == depot){

                            // despachar para a próxima equipe livre o serviço com melhor previsão de custo (atendimento + retorno ao depósito)
                            float best_estimated_cost;
                            for (int s=0; s < n_valid_nodes; s++){

                                int service_point = current_instance->get_valid_node(s);
                                float cost_current_to_service = current_instance->get_value_cost_matrix(current_position, service_point);
                                float cost_service_to_depot = current_instance->get_value_cost_matrix(service_point, depot);
                                float estimated_cost = cost_current_to_service + cost_service_to_depot;

                                if (s == 0){
                                    service_to_despatch = service_point;
                                    best_estimated_cost = estimated_cost;
                                }
                                else{
                                    if (estimated_cost < best_estimated_cost){
                                        service_to_despatch = service_point;
                                        best_estimated_cost = estimated_cost;
                                    }
                                }
                            }  
                        }
                        delete current_stacs_solution;
                    }
                }

                // atualizando current_time (usado na atualização dos serviços válidos)
                // passa a ser o horário que a equipe ficou livre + 1 segundo 
                current_time = time_free_next_team + 1.0;
                
                // realizando o despacho do serviço escolhido à equipe escolhida:
                this->dispatch_service_order(next_team_free, service_to_despatch, f_log_exper);
            }
        }
        
    }
    
    // montando a solução de executed_routes em proposed_final_solution
    this->finalize_final_solution();
    
    // finalizando o experimento:
    this->finalize_simulation(counter_day_simulations, f_log_exper);
}


void Simulation::initialize_dinamic_simulation(){
     // definindo os nós finais das rotas da instância
    current_instance->reset_final_nodes_routes();
    for (int k=0; k < n_teams; k++){
        current_instance->add_final_node_route(depot);    
    }
    
    // armazena as rotas finais em construção:
    final_solution = new MtspSolution();
    final_solution->create(n_points, n_teams);
    for (int k = 0; k < n_teams; k++){
        FloatMatrix* cost_matrix = current_instance->get_cost_matrix();
        final_solution->add_node(k, depot, cost_matrix);   // adicionando o depósito às rotas parciais executadas
    }
}
// verifica a próxima emergência que será despachada, a partir de day_services (não utiliza current_time)
void Simulation::update_next_emergency(){
    
    next_emergency = -1;
    time_dispatch_next_emerg = 0.0;
    
    // os indices de day_services correspondem ao id dos nodes. o índice do depósito é desconsiderado:
    for (int s=0; s < n_points; s++){ 
        if (s != depot){
            if (!day_services[s].get_executed_service()){ // o serviço ainda não foi executado
                float time_dispatch = day_services[s].get_time_dispatch();
                if (day_services[s].is_emergency()){ // o serviço a ser despachado é uma emergência
                    if (next_emergency == -1){
                        next_emergency = s;
                        time_dispatch_next_emerg = time_dispatch;
                    }
                    else{
                        if (time_dispatch < time_dispatch_next_emerg){
                            next_emergency = s;
                            time_dispatch_next_emerg = time_dispatch;
                        }
                    }
                }
            }
        }
    }
}
// verifica qual a primeira equipe que vai concluir o seu serviço atual, utilizando os custos temporais parciais das rotas (não utiliza current_time)
void Simulation::update_next_team_free(){

    float temporal_cost_route;  // custo atual da rota da equipe

    for (int k = 0; k < n_teams; k++){
        
        temporal_cost_route = final_solution->temporal_cost_route(k);
                
        // verificando o menor custo temporal (a primeira equipe a ficar livre):
        if (k == 0){
            next_team_free = k;
            time_free_next_team = temporal_cost_route;
        }
        else{
            if (temporal_cost_route < time_free_next_team){
                next_team_free = k;
                time_free_next_team = temporal_cost_route;
            }
        }
    }
}
// verifica qual equipe está mais próxima da emergência, de acordo com a matriz de custos aplicada:
void Simulation::emergency_dispatch(int emergency_order, ofstream& f_log_exper){
    
    int best_team;  // a melhor equipe para atender a emergência é que estiver mais próxima, de acordo com a matriz de custos aplicada
    float best_cost_to_care;  //custo entre a posição atual da equipe e a emergência
    
    // definindo a equipe mais próxima do local da emergência:
    for (int k = 0; k < n_teams; k++){
        int current_position = final_solution->last_node_route(k);  // posição atual da equipe (serviço em execução)
        float cost_to_care = current_instance->get_value_cost_matrix(current_position, emergency_order); // custo da matriz aplicada
        
        // verificando a melhor equipe para atendimento:
        if (k == 0){
            best_team = k;
            best_cost_to_care = cost_to_care;
        }
        else{
            if (cost_to_care < best_cost_to_care){
                best_team = k;
                best_cost_to_care = cost_to_care;
            }
        }
    }
    
    // realizando o despacho do serviço à equipe escolhida:
    this->dispatch_service_order(best_team, emergency_order, f_log_exper);
}
// atualizando os serviços válidos a partir de service_days (não consulta executed_routes)
// (comparando current_time com o tempo de despacho e excluindo os já executados)
void Simulation::update_valid_nodes_instance(){
    
    current_instance->reset_valid_nodes_instance();
    
    // os indices de day_services correspondem ao id dos nodes. o índice do depósito é desconsiderado:
    for (int s=0; s < n_points; s++){ 
        if (s!=depot){
            if (!day_services[s].get_executed_service()){ // o serviço ainda não foi executado
                if (day_services[s].get_time_dispatch() <= current_time){ // o tempo de despacho do serviço é menor ou igual ao tempo atual
                    
                    current_instance->add_valid_node(s); // adicionando o serviço como um nó válido
                
                }
            }
        }
    }
}
// retorna o horário de despacho da próxima ordem a surgir (apenas a partir de day_services):
float Simulation::time_appearance_next_service(){
    
    float time_dispatch_next_service = -1.0;  // indica que não há mais serviços a executar em day_services
    int next_service = -1;
    
    // os indices de day_services correspondem ao id dos nodes. o índice do depósito é desconsiderado:
    for (int s=0; s < n_points; s++){ 
        if (s != depot){
            if (!day_services[s].get_executed_service()){ // o serviço ainda não foi executado
                float time_dispatch = day_services[s].get_time_dispatch();
                if (next_service == -1){
                        next_service = s;
                        time_dispatch_next_service = time_dispatch;
                }
                else{
                    if (time_dispatch < time_dispatch_next_service){
                        next_service = s;
                        time_dispatch_next_service = time_dispatch;
                    }
                }
            }
        }
    }
    
    return time_dispatch_next_service;
}
// retorna o horário de despacho da próxima ordem a surgir (a partir de day_services e current_time)
float Simulation::time_appearance_next_service_current_time(){
    
    float time_dispatch_next_service = -1.0;  // indica que não há mais serviços a executar em day_services
    int next_service = -1;
    
    // os indices de day_services correspondem ao id dos nodes. o índice do depósito é desconsiderado:
    for (int s=0; s < n_points; s++){ 
        if (s != depot){
            if (!day_services[s].get_executed_service()){ // o serviço ainda não foi executado
                float time_dispatch = day_services[s].get_time_dispatch();
                
                if (time_dispatch > current_time){
                
                    if (next_service == -1){
                            next_service = s;
                            time_dispatch_next_service = time_dispatch;
                    }
                    else{
                        if (time_dispatch < time_dispatch_next_service){
                            next_service = s;
                            time_dispatch_next_service = time_dispatch;
                        }
                    }
                }
            }
        }
    }
    
    return time_dispatch_next_service;
}
// atualizando os nós iniciais da instância a partir de positions teams
void Simulation::update_positions_teams(){
    current_instance->reset_initial_nodes_routes();
    for (int k = 0; k < n_teams; k++){
        int current_node = final_solution->last_node_route(k);
        current_instance->add_initial_node_route(current_node);
    }
}
// realiza o despacho de um serviço à uma equipe
// atualiza cur_executed_routes e day_services
// grava o despacho no arquivo de log
void Simulation::dispatch_service_order(int team, int service_order, ofstream& f_log_exper){

    if (!day_services[service_order].get_executed_service()){    
        
        // calculando custo temporal:
        int current_position = final_solution->last_node_route(team);  // posição atual da equipe
        float travel_time = real_time_matrix->get_value(current_position, service_order);
        float exec_time = day_services[service_order].get_time_execution();
        float temporal_cost = travel_time + exec_time;

        // adicionando o serviço à rota da equipe:
        FloatMatrix* cost_matrix = current_instance->get_cost_matrix();
        final_solution->add_node_with_temp_cost(team, service_order, cost_matrix, temporal_cost);
        float partial_route_temporal_cost = final_solution->temporal_cost_route(team);
        day_services[service_order].set_executed_service(partial_route_temporal_cost);

        #if DETAIL_LOG_EXPER
            if (day_services[service_order].is_emergency()){
                f_log_exper << "\r\nEmergencial dispatch: simulation clock = " << (int)current_time << " seconds\r\n"; 
                f_log_exper << "Service order dispatched: " << service_order << "\t"; 
                day_services[service_order].save_service_order(f_log_exper);
            }
            else{
                f_log_exper << "\r\nCommercial dispatch: simulation clock = " << (int)current_time << " seconds\r\n"; 
                f_log_exper << "Service order dispatched: " << service_order << "\t"; 
                day_services[service_order].save_service_order(f_log_exper);
            }
        #endif
    }
}


void Simulation::finalize_final_solution(){
    // retornando as equipes à garagem:
    for(int k=0; k < n_teams; k++){
        FloatMatrix* cost_matrix = current_instance->get_cost_matrix();
        final_solution->add_node(k, depot, cost_matrix);
    }
    // incluindo dados finais:
    final_solution->finalize_solution(-1, -1, randon_seed_simulation);
}

void Simulation::finalize_simulation(int counter_day_simulations, ofstream& f_log_exper){
    
    f_log_exper << "Real solution:\t\t"; real_solution->save(f_log_exper);
    f_log_exper << "Best solution found:\t"; final_solution->save(f_log_exper);
    
    // salvando os custos obtidos em arquivos próprios:
    if (counter_day_simulations == 1){
        f_longests << "\r\n" << id_work_day << "\t";
        f_total_costs << "\r\n" << id_work_day << "\t";
    }
    final_solution->save_longest_cost(f_longests);    
    final_solution->save_total_cost(f_total_costs);
    
    // salvando em plot_final_created_sols_day.txt a solução final para o dia de trabalho
    ofstream f_day_aco_final_sols;
    f_day_aco_final_sols.open("outputs/plot_final_created_sols_day.txt", ios::app);
    Node* plan_nodes_vector = current_instance->get_plan_nodes_vector();
    f_day_aco_final_sols << "\r\nWorkday: " << id_work_day << "\tSimulation: " << counter_day_simulations <<"\r\n";
    final_solution->save_to_plot(f_day_aco_final_sols, plan_nodes_vector);
    f_day_aco_final_sols.close();
    
    float longest_real = real_solution->longest_route_cost_sol();
    float longest_proposed = final_solution->longest_route_cost_sol();
    float diff_longest = longest_real - longest_proposed;
    float improvement_longest = (diff_longest / longest_real) * 100;

    float total_real = real_solution->total_cost_sol();
    float total_proposed = final_solution->total_cost_sol();
    float diff_total = total_real - total_proposed;
    float improvement_total = (diff_total / total_real) * 100;
    
    f_log_exper << "Improvement\t\tlongest: " << improvement_longest << "%\t\ttotal cost: " << improvement_total << "%\r\n";
    
    if (PRINT_EMERGENCY_CARE){
        cout << "  Emergency care:\r\n";
        for (int s=0; s < n_points; s++){
            if (s != depot){
                if (day_services[s].is_emergency()){
                    cout << "  ";
                    day_services[s].print_total_time_execution(s);
                }
            }
        }
    }
    if (PRINT_DAY_SERVICES) { cout << "\r\n"; this->print_day_services(); }
}

void Simulation::save_work_day(ofstream& f_log_exper){
    f_log_exper << "\r\n------------------------------\r\nWork day: " << id_work_day << "\tteams: " << n_teams << "\tservices: " << n_points-1 << "\r\n";
}
void Simulation::save_day_services(ofstream& f_log_exper){
    f_log_exper << "Day service orders:\r\n";
    for (int i=0; i < n_points; i++){
        if (i != depot){
            f_log_exper << "  Service " << i << " : ";
            day_services[i].save_service_order(f_log_exper);
        }
    }
}
void Simulation::save_executed_routes(ofstream& f_log_exper){
    f_log_exper << "Partial routes of teams:\r\n";
    for (int k = 0; k < n_teams; k++){
        f_log_exper << "  Team " << k+1 << ": ";
        final_solution->save_route(k, f_log_exper);
    }
}
void Simulation::print_day_services(){
    cout << "Day service orders:\r\n";
    for (int i=0; i < n_points; i++){
        if (i != depot){
            cout << "  service " << i << " : ";
            day_services[i].print_service_order();
        }
    }
}
void Simulation::print_executed_routes(){
    cout << "Parcial routes performed:\r\n";
    for (int k = 0; k < n_teams; k++){
        cout << "  Team " << k << ": ";
        final_solution->print_route(k);
    }
}
