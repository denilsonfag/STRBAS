/* 
 * File:   Route.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__ROUTE_HPP__)
    #define  __ROUTE_HPP__

    #include "IntList.hpp"
    #include "FloatMatrix.hpp"

    class Route{
        IntList *route;             // lista com os nos da rota
        float total_cost_route;     // custo da rota
        float temporal_cost_route;  // custo temporal da rota, utilizada em experimentos com dados reais
    public:
        Route();
        ~Route();
        void create(int size_route);

        void reset();           // esvazia e lista de nós e zera os custos
        bool add_node(int new_node, FloatMatrix* &cost_matrix);  // adiciona new_node no final da rota. Retorna falso se a rota estiver cheia.
        bool add_node_with_temp_cost(int new_node, FloatMatrix* &cost_matrix, float temporal_cost);

        int n_nodes();
        int node(int ind);
        int last_node();
        bool is_empty();
        float total_cost();
        float temporal_cost();
        
        void save(ofstream& file_out);
        void save_nodes_route(ofstream& file_out);
        void print();
        void print_nodes_route();
    };

#endif
