/* 
 * File:   ControlRealExperiment.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__CONTROLEXPERIMENT_HPP__)
    #define  __CONTROLEXPERIMENT_HPP__

    // Configurações gerais do protótipo:
    #define TYPE_EXPERIMENT   2  // tipo do experimento: 1: experimento com instâncias benchmark; 2: experimento com dados reais
    #define DEPOT             0  // índice do vértice depósito em todos os experimentos

    // Configurações para experimentos com instâncias benchmark:
    #define N_EXECUTIONS      1000  // número de execuções independentes do STACS para cada instância
    #define N_SALESMEN        2  // número de caixeiros da instância
    #define INTEGER_EUC_MAT   1  // tipo de dados da matriz de distâncias euclidianas: (1) valores inteiros, (0) valores reais
    #define TYPE_MTSP_SOLS    1  // 1: único depósito e finais fechados; 2: m nós inicias e finais abertos e fixos
    #define CODE_INSTANCE     1  // 1: eil51, 2: eil76, 3: eil101, 4: pr76, 5: pr1002, 6: sgb128, 0: grids para testes
    #define TEST             -1  // para diminuir o número de nós da instância em testes. -1 = todos os nós

    // Configurações para experimentos com dados reais:
    #define N_SIMULATIONS_BY_DAY        1  // número de simulações para cada dia de trabalho
    #define TYPE_SIMULATION             2  // 1: instâncias estáticas; 2: dinâmico acionando STACS a cada nova ordem; 3: dinâmico acionando STACS a cada liberação de uma equipe ou surgimento de uma emergência
    #define NEAREST_TEAM                0  // (apenas se TYPE_SIMULATION == 3) não usa ACO: despacha o serviço que estiver mais próximo da equipe
    #define TYPE_COST_MATRIX            2  // 1:euclidiana, 2: distâncias reais, 3: tempos reais, 4:tempo + deslocamento
    #define DESPATCH_REAL_SOL           1  // definindo o horário de despacho do serviço a partir do momento que uma equipe inicia o deslocamento para ele na solução real calculada
    #define INDEX_DAY_TEST              1  // index do dia de trabalho para testes. Se -1, todos os dias serão simulados
    #define SAVE_DAY_STATE_CHANGES      0  // salva todas as mudanças de estado dos dias de trabalho no log do experimento  
    #define SAVE_PARTIAL_DAY_SOLS_PLOT  0  // salva todas as soluções ACO geradas durante as simulações
    #define PRINT_COSTS_MATRIX          0  // imprimir as três matrizes logo após sua construção
    #define PRINT_DAY_SERVICES          0  // imprimir os serviços do dia de trabalho
    #define PRINT_EMERGENCY_CARE        0  // imprimir a melhora dos atendimentos de emergência

#endif
