/* 
 * File:   STACS.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__STACS_HPP__)
    #define  __STACS_HPP__

    #include "ACOAlgorithms.hpp"
    #include "MtspNearestNeighbor.hpp"

    #define LOG_STACS           0  // grava um arquivo de log da execução

    #define MTSP_OBJECTIVE      1  // objetivo MTSP: (1)minimizar rota mais longa (workload balance), (2)minimizar total da solução (soma das rotas)
   
    #define SC_CYCLES        1000  // critério de parada, em ciclos
    #define SC_NOIMPROV        -1  // critério de parada, em ciclos sem encontrar uma solução melhor
    #define SC_SECONDS         -1  // critério de parada, em segundos
    
    #define TYPE_COST_MATRIX    1  // 1: euclidiana, 2: distâncias reais, 3: tempos reais, 4:tempo + deslocamento    
    #define SIZE_CAND_LIST     20  // tamanho máximo da lista de nós candidatos
    #define TACO_ANT_SELECTION  1  // escolha do próxima formiga como no TACO de val08 (apenas esta forma de seleção está implementada)
    #define CBM_ATTEMPTS        1  // número de tentativas em encontrar um melhor movimento para a colônia. se == 0, não checa nenhuma vez; se == 1, igual a val08
    
    #define ALPHA               1  // peso do feromônio nas escolhas (fixado em 1 na literatura)
    
    #define AS_MODE             0  // modo Ant System
    #define EAS_MODE            0  // modo Elitist Ant System 
    #define RBAS_MODE           1  // modo Rank Based Ant System  
    #define ACS_MODE            0  // modo Ant Colony System

    #define LS_2_OPT_ALL        1  // busca local 2-opt intra e inter routes em todas as soluções criadas

    #if AS_MODE
        #define AS_PHER_INIT    1  // inicialização da matriz de feromônio
        #define AS_STR          1  // regra de transição de estado do Ant System
        #define AS_GPU          1  // atualização global de feromônio do Ant System
    #endif
   
    #if EAS_MODE
        #define EAS_PHER_INIT   1  // inicialização da matriz de feromônio específica
        #define AS_STR          1  // regra de transição de estado do Ant System
        #define EAS_GPU         1  // atualização global de feromônio específica
    #endif

    #if RBAS_MODE
        #define RBAS_PHER_INIT  1  // inicialização da matriz de feromônio específica
        #define AS_STR          1  // regra de transição de estado do Ant System
        #define RBAS_GPU        1  // atualização global de feromônio específica
    #endif

    #if ACS_MODE
        #define ACS_PHER_INIT   1  // inicialização da matriz de feromônio como no ACS
        #define ACS_STR         1  // regra de transição de estado do Ant Colony System
        #define ACS_LPU         1  // atualização local de feromônio do ACS 
        #define ACS_GPU         1  // atualização global de feromônio do ACS     
    #endif

    class STACS{  

        // parâmetros:
        int N;          // soluções geradas em cada ciclo. Corresponde ao parâmetro m em Dorigo and Stützle 2004, pg 71.
        float tau0;     // qtde inicial de feromônio depositada em todas as arestas
        int beta;       //´peso da visibilidade nas escolhas das formigas
        float rho;      // coeficiente de evaporação de feromônio
        float q0;       // nível de determinismo do ACS
        float ksy;      // coeficiente de evaporação de feromônio nas atualizações de feromônio locais do ACS
        int e;          // peso da best so far solution na atualização de feromonio do EAS
        int w;          // número de formigas que depositam feromônio no RBAS
        
        // rotinas auxiliares:
        Random* random_generator;      // gerador de números pseudo randômicos
        Utilities* util;               // funções auxiliares
        ACOAlgorithms* aco;            // rotinas ACO
        LocalSearch* local_search;     // busca local aplicada às soluções geradas
        ofstream f_log;                // arquivo de log da execução

        // variáveis da instância de MTSP:
        int n_salesmen;                         // número de caixeiros
        FloatMatrix* cost_matrix;      // matriz de custos
        int n_total_nodes;             // total de nos na matriz de custos
        int n_valid_nodes;             // número de nós válidos da instância (para realização dos experimentos com dados reais)
        IntList* valid_nodes_instance; // índices das matrizes que representam nós que fazem parte da instância corrente
        IntList* start_nodes_ants;     // nós iniciais das rotas
        IntList* final_nodes_ants;     // nós finais das rotas
        
        // variávies locais:
        FloatMatrix* pheromone_matrix;    // matriz de feromônio
        Ant* ants;                        // formigas da colônia
        IntList* sorted_lists;            // listas para todos os nós com as distâncias ordenadas
        IntList* candidate_list;          // lista de nós candidatos
        IntList* prohibited_list;         // lista de nós proibidos à formiga num movimento

        float time_ini_execution;           // instante de início da execução (em milissegundos)
        float time_execution;               // armazena quanto tempo ja foi gasto na execução
        int cycle_counter;                // contador de ciclos  
        int non_improved_cycles_counter;  // contador de ciclos sem melhora da best_so_far_solution da execução
        int created_sols_cycle_counter;   // contador de soluções criadas em um ciclo
        
        int selected_ant;                 // formiga escolhida para se mover
        int selected_node;                // nó pra o qual a formiga escolhida se moverá
        
        MtspSolution* solutions_cycle;    // armazena todas as soluções criadas em cum ciclo do algoritmo
        MtspSolution* best_solution_run;  // melhor solução encontrada na execução: retorna 

    public:
        STACS(MtspInstance* &instance, int param);
        ~STACS();
        
        MtspSolution* run(long seed_random);
        void inicialization(long seed_random);

        void initialize_pheromone_matrix();
            void solution_construction(MtspSolution* &solution);
                void initialize_colony(MtspSolution* &solution);
                    int select_ant(MtspSolution* &solution);
                    int select_node();
                    int check_movement(MtspSolution* &solution);
                    void move_ant(int moving_ant, int next_node, MtspSolution* &solution);
                    void local_pheromone_update(int node_a, int node_b);
            void finalize_solution(MtspSolution* &solution);
            void local_search_all_created_solutions(MtspSolution* &solution);
        void global_pheromone_update(MtspSolution* &solutions_cycle, MtspSolution* &best_solution_cycle, MtspSolution* &best_solution_run);
        
        void finalization();
        
        // funcões auxiliares:
        void make_sorted_lists(); // cria listas de nós ordenados de acordo com a distância
        void add_prohibited_list_invalid_nodes();  // adiciona à lista de visitados os nós da instância que não são válidos
        void update_candidate_list();  // atualiza a candidate_list para a formiga em movimento
        bool update_best_solution(MtspSolution* &current_best, MtspSolution* &new_solution);  // retorna verdadeiro se a solução for atualizada
        void copy_solution(MtspSolution* &source_solution, MtspSolution* &target_solution);
        
        void print_sorted_lists();
        void print_detail_cands_list(int moving_ant);  // imprime em detalhes a lista de nós candidatos:
        void save_sorted_lists(ofstream& f_log);
        void save_setup(ofstream& f_log);
        void save_pheromone_matrix(ofstream& f_log);
    };

#endif
    