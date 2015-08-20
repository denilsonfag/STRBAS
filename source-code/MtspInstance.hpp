/* 
 * File:   MtspInstance.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__MTSPINSTANCE_HPP__)
    #define  __MTSPINSTANCE_HPP__

    #include <cmath>    
    #include "Node.hpp"    
    #include "IntList.hpp"
    #include "FloatMatrix.hpp"

    class MtspInstance{
        int n_nodes;                      // número de nós da instância, incluindo depósitos
        int n_salesmen;                   // número de caixeiros da instância
        FloatMatrix* cost_matrix;         // matriz assimétrica de custos (os índices representam os nós)
        Node* plan_nodes_vector;          // vetor de vértices planos (representados por coordenadas)
        int index_last_nodes_vector;      // aponta o ultimo no do vetor de nós, nas instâncias não benchmark
        
        IntList* valid_nodes;             // lista com os índices de nós validos correntes da instância(apenas os existentes nesta lista farão parte das soluções)
        IntList* initial_nodes_routes;    // lista com os índices de nós iniciais das rotas
        IntList* final_nodes_routes;      // lista com os índices finais das rotas
    public:
        MtspInstance();
        ~MtspInstance();
        
        void load_benchmark_instance(int n_salesmen, int code_instance, int integer_euc_mat, int type_mtsp_sols, int depot_node, int test);
            void create_nodes_benchmark_instance(int code_instance, int test);
            void configure_salesmen(int n_salesmen, int type_mtsp_sols, int depot_node);
        
        void create_empty_instance(int n_nodes, int n_teams);
            bool add_plan_node(int coord_x, int coord_y);
            void set_value_cost_matrix(int i, int j, float cost);
            void reset_valid_nodes_instance();
            bool add_valid_node(int new_node);
            void reset_initial_nodes_routes();
            bool add_initial_node_route(int new_node);
            void reset_final_nodes_routes();
            bool add_final_node_route(int new_node);
        
        void calcule_euclidean_matrix(bool integer_euc_mat, int divisor);
        
        int get_n_nodes();
        int get_n_salesmen();
        Node* get_plan_nodes_vector();
        int get_n_valid_nodes();
        int get_valid_node(int index);
        IntList* get_valid_nodes_instance();
        float get_value_cost_matrix(int i, int j);
        FloatMatrix* get_cost_matrix();
        IntList* get_initial_nodes_routes();
        IntList* get_final_nodes_routes();
        
        void save_instance(ofstream& file_out);
        void save_valid_nodes(ofstream& file_out);
        void save_initial_nodes_routes(ofstream& file_out);
        
        void print_instance();
        void print_valid_nodes();
        void print_initial_nodes_routes();
        
        void print_cost_matrix();
    };
    
#endif
    