/* 
 * File:   MtspSolution.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__MTSPSOLUTION_HPP__)
    #define  __MTSPSOLUTION_HPP__

    #include <iomanip>
    #include "Node.hpp"
    #include "Route.hpp"

    class MtspSolution{
        int n_routes;             // número de rotas da solução
        Route* routes;            // rotas da solução
        float total_cost_solution;         // custo da soma das rotas
        float longest_route_cost_solution; // custo da maior rota
        float temp_total_cost_solution;
        float temp_longest_route_cost;
        
        int cycle;               // ciclo do algorimto ACO em que a solução foi gerada 
        int time_exec;           // tempo de execução do algorimto ACO até a solução ser gerada, em milissegundos
        long random_seed;        // semente dos randômicos da execução do algoritmo ACO
        bool complete_solution;  // informa se a solução já foi calculada e os dados finais inseridos
    public:
        MtspSolution();
        ~MtspSolution();
        void create(int size_routes, int number_routes);

        void reset();
        bool add_node(int k, int new_node, FloatMatrix* &cost_matrix);
        bool add_node_with_temp_cost(int k, int new_node, FloatMatrix* &cost_matrix, float temporal_cost);
        void finalize_solution(int cycle_sol, int time_exec_sol, long random_seed_exec);
            void calcule_total_costs_solution();
            void update_longest_route_costs();
        void set_random_seed (long new_seed);  // para alterar a semente nas simulações com dados reais
        
        int n_routes_sol();
        int n_nodes_route(int k);
        int node_route(int k, int index);
        int last_node_route(int k);
        int next_node_route(int k, int current_node);
        float total_cost_sol();
        float temporal_cost_route(int k);
        float longest_route_cost_sol();
        float cost_route(int k);
        Route* pointer_routes();    // utilizada nas buscas locais
        float cost_by_objective(int mtsp_objective);
        
        // utilizados na cópia de soluções:
        int cycle_sol();
        int time_exec_sol();
        long random_seed_sol();

        void save(ofstream& file_out);
        void save_route(int k, ofstream& file_out);
        void save_to_plot(ofstream& file_out, Node*& nodes);
        void save_longest_cost(ofstream& file_out);
        void save_total_cost(ofstream& file_out);
        void print();
        void print_route(int k);
    };

#endif
