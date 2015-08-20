/* 
 * File:   RealExperiment.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "RealExperiment.hpp"

RealExperiment::RealExperiment(){
    f_log_exper.open("outputs/log_real_experiment.txt", ios::app);
    f_log_exper << setiosflags (ios::fixed) << setprecision(2);
    f_time_simuls.open("outputs/time_simulations.txt", ios::app);
    f_time_simuls << setiosflags (ios::fixed) << setprecision(2);
}

RealExperiment::~RealExperiment(){
    f_time_simuls.close();
    f_log_exper.close();
}

void RealExperiment::run_real_experiment(int param){
    
    float time_ini_experiment = (float)clock();  // instante do início do experimento
    f_log_exper << "\r\n---------------------------------------------\r\nStarting experiment\r\n";
    f_log_exper << "Number of simulations by workday: " << N_SIMULATIONS_BY_DAY << "\r\n";
    switch (TYPE_SIMULATION){ 
        case 1: { // 1: instâncias estáticas
            f_log_exper << "Simulations type: static simulation\r\n";
            break;
        }
        case 2: { // 2: dinâmico acionando STACS a cada nova ordem
            f_log_exper << "Simulations type: activating STACS for each new order\r\n";
            break;
        }
        case 3: { // 3: dinâmico acionando STACS a cada liberação de uma equipe ou surgimento de uma emergência
            if (NEAREST_TEAM){
                f_log_exper << "Simulations type: dispatching nearest order to available team\r\n";
            }
            else{
                f_log_exper << "Simulations type: activating STACS for each available team\r\n";
            }
            break;
        }
    }
    f_log_exper << "Cost matrix: ";
    switch (TYPE_COST_MATRIX){  // matriz de custos a ser utiliada na aplicação
        case 1: {   // distâncias euclidianas
            f_log_exper << "euclidean distances\r\n";
            break;
        }
        case 2: {   // distâncias reais
            f_log_exper << "real distances\r\n";
            break;
        }
        case 3: {   // tempos reais
            f_log_exper << "real time displacement\r\n";
            break;
        }
        case 4: {   // tempos reais de deslocamento + tempo de execução dos serviços
            f_log_exper << "real time displacement + time execution orders\r\n";
            break;
        }
    }
    
    // salvando configuração do stacs
    MtspInstance* inst = new MtspInstance();
    inst->create_empty_instance(1, 1);
    STACS* stacs = new STACS(inst, param);
    stacs->save_setup(f_log_exper);
    delete stacs;
    delete inst;
    
    // carregando informações sobre os dias de trabalho:
    RealData* real_data = new RealData(REAL_DEPOT);
    int n_work_days = real_data->count_work_days();  // contando número de dias de trabalho na base de dados
    list_id_work_days = new IntList();
    list_id_work_days->create(n_work_days);
    real_data->load_ids_work_days(list_id_work_days);  // criado a lista com as ids dos dias de trabalho
    delete real_data;
    
    for (int index_day = 0; index_day < n_work_days; index_day++){
        
        int current_id_work_day = list_id_work_days->value(index_day);
        if (DAY_TEST != -1){ current_id_work_day = DAY_TEST; }
        
        // excluindo dias de trabalho inválidos:
        if ((current_id_work_day != 1) 
         && (current_id_work_day != 2) 
         && (current_id_work_day != 8) 
         && (current_id_work_day != 9) 
         && (current_id_work_day != 23) 
         && (current_id_work_day != 16)
         ){
        
            for (int counter_simulations = 1; counter_simulations <= N_SIMULATIONS_BY_DAY; counter_simulations++){

                float time_ini_simulation_day = (float)clock();  // instante do início do experimento
                
                // se SEED_RANDOM == -1, a semente é alterada a cada simulação de um dia de trabalho
                long seed_random;
                if (SEED_RANDOM == -1){
                    seed_random = (long)time(NULL);  // gerando semente aleatória
                }
                else{
                    seed_random = SEED_RANDOM;
                }
                
                current_simulation = new Simulation(current_id_work_day, seed_random);
                current_simulation->load_data_work_day();

                if (counter_simulations == 1){ current_simulation->save_work_day(f_log_exper); }
                cout << "\r\n------------------------------\r\nWork day: " << current_id_work_day << "\r\n";
                
                f_log_exper << "\r\nSimulation: " << counter_simulations << "\t";
                f_log_exper << "Random seed: " << seed_random << "\r\n";

                // 1: instâncias estáticas; 
                // 2: dinâmico acionando STACS a cada nova ordem; 
                // 3: dinâmico acionando STACS a cada liberação de uma equipe ou surgimento de uma emergência
                    // NEAREST_TEAM: despacha o serviço que estiver mais próximo da equipe que ficou disponível (não usa STACS)
                switch (TYPE_SIMULATION){ 

                    case 1: { // 1: instâncias estáticas
                        current_simulation->execute_static_simulation(counter_simulations, f_log_exper, param);
                        break;
                    }
                    case 2: { // 2: dinâmico acionando STACS a cada nova ordem
                        current_simulation->run_din_simul_new_order(counter_simulations, f_log_exper);
                        break;
                    }
                    case 3: { // 3: dinâmico acionando STACS a cada liberação de uma equipe ou surgimento de uma emergência
                        if (NEAREST_TEAM){
                            current_simulation->run_din_simul_available_team(counter_simulations, f_log_exper, true);
                        }
                        else{
                            current_simulation->run_din_simul_available_team(counter_simulations, f_log_exper, false);
                        }
                        break;
                    }
                }
                
                float time_simulation = (((float)clock() - time_ini_simulation_day) / CLOCKS_PER_SEC);
                
                delete current_simulation;

                // gravando o tempo total da simulação:
                if (counter_simulations == 1){ f_time_simuls << "\r\n" << current_id_work_day << "\t"; }
                f_time_simuls << (int)time_simulation  << "\t";
                f_log_exper << "Total time of simulation: " << (int)time_simulation << " seconds\r\n";
            }
        }
        
        if (DAY_TEST != -1){ index_day = n_work_days; }
    }    
    
    delete list_id_work_days;
    
    float time_experiment = (((float)clock() - time_ini_experiment) / CLOCKS_PER_SEC);
    f_log_exper << "\r\n\r\nTotal time of experiment: " << (int)time_experiment << " seconds\r\n";
}
