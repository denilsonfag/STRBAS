/* 
 * File:   RealExperiment.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

// classe para realização do experimento com dados reais
#if !defined(__REALEXPERIMENT_HPP__)
    #define  __REALEXPERIMENT_HPP__

    #include "Simulator.hpp"    

    #define N_SIMULATIONS_BY_DAY      100  // número de simulações para cada dia de trabalho
    #define TYPE_SIMULATION             1  // 1: instâncias estáticas; 2: dinâmico acionando STACS a cada nova ordem; 3: dinâmico acionando STACS a cada liberação de uma equipe ou surgimento de uma emergência
    #define NEAREST_TEAM                0  // (apenas se TYPE_SIMULATION == 3) não usa o STACS: despacha o serviço que estiver mais próximo da equipe
    #define DAY_TEST                   -1  // index do dia de trabalho para testes. Se -1, todos os dias serão simulados
    #define SEED_RANDOM                -1  // se -1, é criada uma semente aleatória

    class RealExperiment{
        IntList* list_id_work_days;      // lista com os códigos dos dias de trabalho da base de dados
        Simulation* current_simulation;  // dia de trabalho que esta sendo simulado
        ofstream f_log_exper;            // arquivo de log do experimento
        ofstream f_time_simuls;          // armazena os tempos de execução das simulações
    public:
        RealExperiment();
        ~RealExperiment();
        void run_real_experiment(int param);  // seleção de parâmetros apenas para experimento real estático
    };

#endif
