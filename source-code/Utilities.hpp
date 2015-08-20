/* 
 * File:   Utilities.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__UTILITIES_HPP__)
    #define  __UTILITIES_HPP__

    #include <cmath>
    #include "IntList.hpp"

    class Utilities{
    public:
        // recebe um vetor de float e ordena os índices numa lista
        // o vetor e a lista devem ter o mesmo número de itens
        void sort_float_vector(float* &float_vector, IntList &sorted_list);
       
        // retorna o índice do maior valor de um vetor de floats:
        int argmax(int n_cands, float* &float_vector);
        
        // retorna a média de um vetor de floats
        float average (float* &values, int n_values);
        
        // retorna o desvio padrão de um vetor de floats
        float std_dev (float* &values, int n_values);
    };

#endif
