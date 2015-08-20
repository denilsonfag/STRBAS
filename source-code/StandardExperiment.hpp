/* 
 * File:   StandardExperiment.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__STANDARDEXPER_HPP__)
    #define  __STANDARDEXPER_HPP__

    #include "STACS.hpp"

    #define N_EXECUTIONS       10 // número de execuções independentes do STACS para cada instância
    #define N_SALESMEN          4  // número de caixeiros da instância
    #define CODE_INSTANCE       1  // 1: eil51, 2: eil76, 3: eil101, 4: pr76, 5: pr1002, 6: sgb128, 0: grids para testes
    #define STAND_DEPOT         0  // índice do vértice depósito nos experimentos com instâncias de benchmark    
    #define INTEGER_EUC_MAT     1  // tipo de dados da matriz de distâncias euclidianas: (1) valores inteiros, (0) valores reais
    #define TYPE_MTSP_SOLS      1  // 1: único depósito e finais fechados; 2: m nós inicias e finais abertos e fixos
    #define TEST               -1  // para diminuir o número de nós da instância em testes. -1 = todos os nós
    #define SEED_RANDOM_STAND  -1  // se -1, é criada uma semente aleatória

    class StandardExperiment{
        ofstream f_log_experiment;
        ofstream f_bests_sols_executions;
    public:
        StandardExperiment();
        ~StandardExperiment();
        void save_experiment_setup();  // salva o cabeçalho do experimento
        void run_standard_experiment();  // carrega a instância benchmark
    };

#endif
