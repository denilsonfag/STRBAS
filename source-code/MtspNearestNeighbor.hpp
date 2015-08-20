/* 
 * File:   MtspNearestNeighbor.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__MTSPNEAREST_HPP__)
    #define  __MTSPNEAREST_HPP__

    #include "MtspSolution.hpp"
    #include "FloatMatrix.hpp"

    class MtspNearestNeighbor{
        int depot;
        int n;
        int m;
        IntList* visited_list;
        MtspSolution* nn_solution;
    public:
        MtspNearestNeighbor(int n_nodes, int m_salesmen);
        ~MtspNearestNeighbor();
       
        void create_solution_workload_balance(FloatMatrix* &cost_matrix, IntList* &valid_nodes_instance);
        void create_solution_one_by_one(FloatMatrix* &cost_matrix, IntList* &valid_nodes_instance);

        float get_total_cost();
    };

#endif
