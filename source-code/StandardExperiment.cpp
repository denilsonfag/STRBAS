/* 
 * File:   StandardExperiment.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "StandardExperiment.hpp"

StandardExperiment::StandardExperiment(){
    f_log_experiment.open("outputs/log_stand_experiment.txt", ios::app);
    f_log_experiment << setiosflags (ios::fixed) << setprecision(2); 
    f_bests_sols_executions.open("outputs/plot_bests_sols_execs_stand_experiment.txt", ios::app);
    f_bests_sols_executions << setiosflags (ios::fixed) << setprecision(2);
}
StandardExperiment::~StandardExperiment(){
    f_bests_sols_executions.close();
    f_log_experiment.close();
}

void StandardExperiment::save_experiment_setup(){
    f_log_experiment << "\r\n-----------------------------------------------------\r\n";
    f_log_experiment << "Type solutions:\t\t\t";
    if (TYPE_MTSP_SOLS == 1) f_log_experiment << "basic MTSP: single depot and closed routes\r\n";
    if (TYPE_MTSP_SOLS == 2) f_log_experiment << "MTSP with multiple starting depots and single end depot (open routes)\r\n";
    f_log_experiment << "Model instance:\t\t\t"; 
    switch (CODE_INSTANCE){
        case 0: f_log_experiment << "Test grids\r\n"; break;
        case 1: f_log_experiment << "TSPLIB Eil51 (51 nodes)\r\n"; break;
        case 2: f_log_experiment << "TSPLIB Eil76 (76 nodes)\r\n"; break;
        case 3: f_log_experiment << "TSPLIB Eil101 (101 nodes)\r\n"; break;
        case 4: f_log_experiment << "TSPLIB Pr76 (76 nodes)\r\n"; break;
        case 5: f_log_experiment << "TSPLIB Pr1002 (1002 nodes)\r\n"; break;
        case 6: f_log_experiment << "sgb128 (128 nodes)\r\n"; break;
    }
    f_log_experiment << "Salesmen number:\t\t" << N_SALESMEN << "\r\n";
    f_log_experiment << "Independent executions:\t\t" << N_EXECUTIONS << "\r\n";    
    f_log_experiment << "Type cost matrix: \t\t";
    if (INTEGER_EUC_MAT == 0) f_log_experiment << "float\r\n";
    if (INTEGER_EUC_MAT == 1) f_log_experiment << "integer\r\n";
}

void StandardExperiment::run_standard_experiment(){

    float time_ini_experiment = (float)clock();  // instante do início do experimento
    
    // melhores resultados do experimento:
    float best_longest_exper;  // minimizar rota mais longa
    float best_totsol_exper;   // minimizar soma das rotas
    // dados da melhor solução de cada execução:
    float* longest_best_sols = new float[N_EXECUTIONS];
    float* totsol_best_sols = new float[N_EXECUTIONS];
    float* cycle_best_sols = new float[N_EXECUTIONS];
    float* time_best_sols = new float[N_EXECUTIONS];

    // carregando a instância
    MtspInstance* benchmark_instance = new MtspInstance(); 
    benchmark_instance->load_benchmark_instance(N_SALESMEN, CODE_INSTANCE, INTEGER_EUC_MAT, TYPE_MTSP_SOLS, STAND_DEPOT, TEST);

    // cada loop é uma execuçao independente:
    for (int exec_counter=0; exec_counter < N_EXECUTIONS; exec_counter++){  // execuções do algoritmo

        float time_ini_execution = (float)clock();  // instante do início da execução

        long seed_random;
        if (SEED_RANDOM_STAND == -1){
            seed_random = (long)time(NULL);  // gerando semente aleatória
        }
        else{
            seed_random = SEED_RANDOM_STAND;
        }
        
        int beta = 2;
        STACS* stacs_app = new STACS(benchmark_instance, beta);  // carregando aplicação
        
        if (exec_counter == 0){ // salvando e imprimindo
            cout << "\r\n-----------------------------------------------------\r\n";
            cout << "Start of the experiment\r\n";
            benchmark_instance->print_instance();
            this->save_experiment_setup();      // salvando os parâmetros do experimento
            stacs_app->save_setup(f_log_experiment); // salvando os parâmetros do STACS
            f_log_experiment << "\r\nBest solution of each execution:";
            f_log_experiment << "\r\nexec\tlongest\t\ttsolut\t\tcycle\t\ttime(ms)\tseed execution\r\n";
        }
        cout << "\r\nExecution " << exec_counter + 1 << "\r\n";
        
        MtspSolution* aco_solution = stacs_app->run(seed_random);  // executando a aplicação. A melhor solução é armazenada em best_sol_exe
        delete stacs_app;

        // salvando as melhores soluções de cada execução, de acordo com o objetivo
        Node* plan_nodes_vector = benchmark_instance->get_plan_nodes_vector();
        aco_solution->save_to_plot(f_bests_sols_executions, plan_nodes_vector);

        // copiando custos da melhor solução da execução:
        longest_best_sols[exec_counter] = aco_solution->longest_route_cost_sol();
        totsol_best_sols[exec_counter] = aco_solution->total_cost_sol();
        cycle_best_sols[exec_counter] = aco_solution->cycle_sol();
        time_best_sols[exec_counter] = aco_solution->time_exec_sol();

        // salvando as melhores soluções de cada execução:
        float longest = aco_solution->longest_route_cost_sol();
        float tsolut = aco_solution->total_cost_sol();
        int cycle = aco_solution->cycle_sol();
        int time = aco_solution->time_exec_sol();
        long seed_r = aco_solution->random_seed_sol();
        
        // atualizando melhores resultados do experimento:
        if ((exec_counter == 0) || (longest < best_longest_exper)){
            best_longest_exper = longest;
        }        
        if ((exec_counter == 0) || (tsolut < best_totsol_exper)){
            best_totsol_exper = tsolut;
        }        
        f_log_experiment << exec_counter+1 << "\t" << longest << "\t\t" << tsolut << "\t\t" << cycle << "\t\t" << time << "\t\t" << seed_r << "\r\n";

        delete aco_solution;

        float time_execution = (((float)clock() - time_ini_execution) / CLOCKS_PER_SEC);
        cout << "\r\nTotal time execution: " << (int) time_execution << " seconds\r\n";
    }

    Utilities u;
    float avglongests = u.average(longest_best_sols, N_EXECUTIONS);
    float avgtsoluts = u.average(totsol_best_sols, N_EXECUTIONS);
    float avgcycles = u.average(cycle_best_sols, N_EXECUTIONS);
    float avgttimes = u.average(time_best_sols, N_EXECUTIONS);
    float sdlongests = u.std_dev(longest_best_sols, N_EXECUTIONS);
    float sdtsoluts = u.std_dev(totsol_best_sols, N_EXECUTIONS);
    float sdcycles = u.std_dev(cycle_best_sols, N_EXECUTIONS);
    float sdttimes = u.std_dev(time_best_sols, N_EXECUTIONS);

    f_log_experiment << "\r\nAvgs\t" << avglongests << "\t\t" << avgtsoluts << "\t\t" << avgcycles << "\t\t" << avgttimes << "\r\n";
    f_log_experiment << "SDs\t" << sdlongests << "\t\t" << sdtsoluts << "\t\t" << sdcycles << "\t\t" << sdttimes << "\r\n";

    f_log_experiment << "\r\nBest costs:\t\ttotal: "  << best_totsol_exper << "\t\tlongest: " << best_longest_exper << "\r\n";

    float time_experiment = (((float)clock() - time_ini_experiment) / CLOCKS_PER_SEC);
    cout << "\r\nTotal time experiment: " << (int)time_experiment << " seconds\r\n";
    f_log_experiment << "\r\nTotal time experiment: " << (int)time_experiment << " seconds\r\n";

    delete [] time_best_sols;
    delete [] cycle_best_sols;
    delete [] totsol_best_sols;
    delete [] longest_best_sols;

    delete benchmark_instance;
}
