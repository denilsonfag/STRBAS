/* 
 * File:   Simulation.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__SIMULATION_HPP__)
    #define  __SIMULATION_HPP__

    #include "STACS.hpp"
    #include "RealMatrix.hpp"
    #include "RealData.hpp"    
    #include "ServiceOrder.hpp"

    #define DETAIL_LOG_EXPER            0  // grava detalhes das simulações no arquivo de log do experimento
    
    #define DESPATCH_REAL_SOL           1  // definindo o horário de despacho do serviço a partir do momento que uma equipe inicia o deslocamento para ele na solução real calculada
    #define REAL_DEPOT                  0  // índice do vértice depósito nos experimentos com dados reais
    
    #define SAVE_PARTIAL_DAY_SOLS_PLOT  0  // salva todas as soluções ACO geradas durante as simulações
    #define PRINT_COSTS_MATRIX          0  // imprimir as três matrizes logo após sua construção
    #define PRINT_DAY_SERVICES          0  // imprimir os serviços do dia de trabalho
    #define PRINT_EMERGENCY_CARE        0  // imprimir a melhora dos atendimentos de emergência

    class Simulation{
        
        long randon_seed_simulation;   // semente randomica da simulação
        float time_ini_simulation;     // momento de início do experimento deste dia de trabalho
        int id_work_day;               // código do dia de trabalho
        int depot;                     // garagem da agência de atendimento 
        int n_points;                  // total de pontos do grafo (serviços + depósito)
        int n_teams;                   // numero de equipes disponíveis no dia
        RealData* real_data;           // classe para acesso aos arquivos de dados reais
        RealMatrix* real_matrix;       // acesso às matrizes de custos reais
        ServiceOrder* day_services;    // todos os serviços do dia (não inclui o depósito)
        MtspInstance* current_instance;    // Instância que será montada e passada como parâmetro para o algoritmo proposto
        FloatMatrix* real_distance_matrix; // distâncias reais entre os pontos, em kms, a partir de MapQuest
        FloatMatrix* real_time_matrix;     // tempos reais de deslocamento, em segundos, a partir de MapQuest
        Random* random_generator; 
        
        MtspSolution* real_solution;           // soluçao com as rotas fechadas reais executadas pelas equipes
        MtspSolution* final_solution; // a solução para o dia todo obtida pela simulação  
        
        float current_time;             // tempo atual do dia de trabalho
        
        int next_emergency;              // índice da proóxima emergência
        float time_dispatch_next_emerg;  // horário de surgimento da próxima emrgência
        int next_team_free;              // próxima equipe a ficar livre
        float time_free_next_team;       // instante em que a equipe vai conluir o serviço atual
        
        ofstream f_longests;             // armazena os custos das maiores rotas das soluções finais da simulações
        ofstream f_total_costs;          // armazena os custos totais das soluções finais da simulações
        
    public:

        Simulation(int current_id_work_day, long seed_random);
        ~Simulation();
        
        void load_data_work_day();  // carrega todos os dados do dia de trabalho
        void mount_cost_matrix();             // monta a matriz de custos da instância, a que será usada na construção das soluções
        
        
        void execute_static_simulation(int counter_day_simulations, ofstream& f_log_exper, int param);
        void set_valid_all_nodes(); // todas as ordens de serviço são válidas
        void position_teams_depot(); // posicionando todas as equipes no depósito       
        
        
        void run_din_simul_new_order(int counter_day_simulations, ofstream& f_log_exper);
                
        
        void run_din_simul_available_team(int counter_day_simulations, ofstream& f_log_exper, bool nearest_team);

        
        void initialize_dinamic_simulation();
        void update_valid_nodes_instance();
        void finalize_final_solution();

        void finalize_simulation(int counter_day_simulations, ofstream& f_log_exper);
         
        void update_next_emergency();   // atualiza next_emergency e time_dispatch_next_emergency
        void update_next_team_free();   // atualiza next_team_free e time_to_free_next_team
        void emergency_dispatch(int id_service, ofstream& f_log_exper);  // realiza o despacho de um serviço emergencial        
        float time_appearance_next_service();
        void update_positions_teams();
        void dispatch_service_order(int team, int service_order, ofstream& f_log_exper);  // atribui um serviço à rota de uma equipe
        
        float time_appearance_next_service_current_time();
        
        void save_work_day(ofstream& f_log_exper);
        void save_day_services(ofstream& f_log_exper);
        void save_executed_routes(ofstream& f_log_exper);
        void print_day_services();
        void print_executed_routes();
    };

#endif    
