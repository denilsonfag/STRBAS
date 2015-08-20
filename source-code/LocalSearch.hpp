/* 
 * File:   LocalSearch.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__LOCALSEARCH_HPP__)
    #define  __LOCALSEARCH_HPP__

    #include "MtspInstance.hpp"
    #include "MtspSolution.hpp"
    #include "BasicMtspSolution.hpp"

    class LocalSearch{
        int index_first;
        int index_last;
    public:
        LocalSearch();
        ~LocalSearch();

        // realiza a busca local 2-opt e atualiza solution com a melhor encontrada
        bool two_opt_route(Route* &route, FloatMatrix* &cost_matrix);
        // considera uma matrix de custos assimétrica, o que aumenta consideravalemente o processamento
        float gain_change_two_nodes_route(int index_a, int index_b, Route* &route, FloatMatrix* &cost_matrix);
        void change_nodes_route(int index_a, int index_b, Route* &route, FloatMatrix* &cost_matrix);
        
        // matriz de custos simétrica, também tenta trocar os depósitos intermediários da solução
        bool two_opt_basic_mtsp_solution(int mtsp_objective, MtspSolution* &solution, FloatMatrix* &cost_matrix);
        float symmetric_gain_change_two_nodes(int index_a, int index_b, BasicMtspSolution* &basic_solution,FloatMatrix* &cost_matrix);
        float symmetric_partial_cost(int before, int current, int after, BasicMtspSolution* &basic_solution, FloatMatrix* &cost_matrix);
        
        // realiza a busca local 3-opt e atualiza solution com a melhor encontrada
        bool three_opt(MtspSolution* &solution, int mtsp_objective, bool saving, ofstream& f_out);
    };

#endif
