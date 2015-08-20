/* 
 * File:   ACO_Algorithms.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 14 de Junho de 2015, 12:20
 */

#if !defined (ACOALGORITHMS_HPP)
    #define   ACOALGORITHMS_HPP

    #include "LocalSearch.hpp"
    #include "Ant.hpp"
    #include "Utilities.hpp"
    #include "Random.hpp"

    class ACOAlgorithms{
        float tau0;  // qtde inicial de feromônio
    public:
        
        ACOAlgorithms();
        ~ACOAlgorithms();
        
        int TACO_ant_selection(MtspSolution* &solution);
        int TACO_check_better_movement(int checking_node, int m, Ant* &ants, FloatMatrix* &cost_matrix, MtspSolution* &solution);

        float AS_initial_pheromone_value(int n, float cost_nearest_neighbo_sol);
        int AS_state_transition_rule(int current_node, IntList* &candidate_list, Random* &random_generator, FloatMatrix* &cost_matrix, FloatMatrix* &pheromone_matrix, int alpha, int beta);
        void AS_global_pheromone_update(int mtsp_objective, FloatMatrix* &pheromone_matrix, int n_sols_cycle, MtspSolution* &solutions_cycle, float rho);
        void AS_pheromone_evaporation(FloatMatrix* &pheromone_matrix, float rho);
        
        float EAS_initial_pheromone_value(int N, float rho, int e, float cost_nearest_neighbo_sol);
        void EAS_global_pheromone_update(int mtsp_objective, FloatMatrix* &pheromone_matrix, int n_sols_cycle, MtspSolution* &solutions_cycle, MtspSolution* &best_solution_run, float rho, int e);

        float RBAS_initial_pheromone_value(float rho, int w, float cost_nearest_neighbo_sol);
        void RBAS_global_pheromone_update(int mtsp_objective, FloatMatrix* &pheromone_matrix, int n_sols_cycle, MtspSolution* &solutions_cycle, MtspSolution* &best_solution_run, float rho, int w);        

        float ACS_initial_pheromone_value(int n, float cost_nearest_neighbo_sol);
        int ACS_state_transition_rule(int current_node, IntList* &candidate_list, Random* &random_generator, FloatMatrix* &cost_matrix, FloatMatrix* &pheromone_matrix, Utilities* &util, int alpha, int beta, float q0);
        void ACS_local_pheromone_update(int i, int j, FloatMatrix* &pheromone_matrix, float ksi);
        void ACS_global_pheromone_update(int mtsp_objective, int m, MtspSolution* &solution, FloatMatrix* &pheromone_matrix, float rho);
        
        float* create_probability_vector(int current_node, IntList* &candidate_list, FloatMatrix* &cost_matrix, FloatMatrix* &pheromone_matrix, int alpha, int beta);
        float predict_route(int k, int checking_node, Ant* &ants, FloatMatrix* &cost_matrix, MtspSolution* &solution);
        int next_best_solution(int mtps_objetive, float minimum_cost, int n_sols_cycle, MtspSolution* &solutions_cycle);
    };
    
#endif
    