/* 
 * File:   BasicMtspSolution.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__BASICMTSPSOLUTION_HPP__)
    #define  __BASICMTSPSOLUTION_HPP__

    #include "Node.hpp"
    #include "IntList.hpp"
    #include "FloatMatrix.hpp"

    class BasicMtspSolution{
        int depot;
        int size;                  // número de nós da solução
        IntList *nodes_list;       // lista com os nós da solução ordenados
        double total_cost;         // custo da soma das rotas
        double longest_route_cost; // custo da maior rota
        int iteration;             // ciclo em que a solução foi gerada 
        double time_exec;          // tempo de execução, em milissegundos
        long random_seed;          // semente dos randômicos utilizada para criar a solução
    public:
        BasicMtspSolution(int n, int m, int depot_node);
        ~BasicMtspSolution();

        void reset();
        void add(int node, double cost);

        int n_nodes();
        int node(int ind);
        int last_node();
        double get_total_cost();
        double get_longest_route();
        
        void set_total_cost(double t_cost);
        void set_longest_route(double cost);
        
        // troca os nós das posições enviadas e recalcula a solução
        void change_nodes(int index_i, int index_j, FloatMatrix* &cost_matrix);
        // atualiza total_cost e longest_route_cost
        void recalculate_solution(FloatMatrix* &cost_matrix);
        
        void print();
        
/*        
        // inverte a solução a partir de index_start (inclusive) a index_end (inclusive) e a reacalcula
        void reverse(int index_start, int index_end,  FloatMatrix* &cost_matrix);

        
        // retorna verdadeiro se a solução for válida:
        bool test_validate();

        
        
        
        bool is_full();
        bool is_empty();

        
        void set_iteration(int c);
        void set_time(double t);
        void set_random_seed(long s);
        

       
        int get_iteration_sol();
        int get_time_sol();
        long get_seed_rand();

        int next_node(int current_node);
        
        void print();
        void save_how_list(ofstream& file_out);
        void save_to_plot(ofstream& file_out, Node*& nodes);
        void save_longest_cost(ofstream& file_out);
        void save_total_cost(ofstream& file_out);
 */
    };

#endif
