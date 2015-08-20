/* 
 * File:   Utilities.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

# include "Utilities.hpp"

void Utilities::sort_float_vector(float* &float_vector, IntList& sorted_list){
        
    int n_items = sorted_list.size_list();
    
    bool first_insert = true;  // será a primeira inserção em sorted_list?
    float last_float_inserted;     // último float do qual foi inserido o índice em sorted_list
    
    int i_next_insert;         // índice do float que vai ser inserido
    float next_float_insert;       // float que vai ser inserido

    while (!sorted_list.is_full()){

        if (first_insert){ // primeira inserção: localizar menor float e inserir seu índice
            first_insert = false;
            for(int i=0; i < n_items; i++){
                if (i==0){
                    i_next_insert = i;
                    next_float_insert = float_vector[i];
                }
                else{
                    if(float_vector[i] < next_float_insert){
                        i_next_insert = i;
                        next_float_insert = float_vector[i];
                    }
                }
            }
        }

        else{ // não é a primeira inserção
            
            bool first_comp = true;  // primeira comparação 
            for(int i=0; i < n_items; i++){
                
                if (float_vector[i] == last_float_inserted){
                    if (!sorted_list.in_the_list(i)){ // o ínidice i ainda não foi inserido
                        i_next_insert = i;
                        next_float_insert = float_vector[i];
                        i = n_items;
                    }
                }
                
                else{
                    if (float_vector[i] > last_float_inserted){  // valores menores já estão ordenados
                        if (first_comp){
                            first_comp = false;
                            i_next_insert = i;
                            next_float_insert = float_vector[i];
                        }
                        else{  // não é a primeira comparação
                            if(float_vector[i] < next_float_insert){
                                i_next_insert = i;
                                next_float_insert = float_vector[i];
                            }
                        }
                    }
                }
            }    
        }
        
        last_float_inserted = next_float_insert;
        sorted_list.add_value(i_next_insert);
    } // end while
}
    
// retorna o índice do maior valor de um vetor de floats
int Utilities::argmax(int n_cands, float* &float_vector){
    int i_max;
    float max;
    for(int i=0; i<n_cands; i++){
        if (i==0){
            i_max = i;
            max = float_vector[i];
        }
        else{
            if(float_vector[i] > max){
                i_max = i;
                max = float_vector[i];
            }
        }
    }
    return i_max;
}

// retorna a média de um vetor de floats
float Utilities::average (float* &values, int n_values){
    float sum_values = 0;
    float av = 0;
    for (int i=0; i<n_values; i++){
        sum_values = sum_values + values[i];
    }
    av = sum_values / n_values;
    return av;
}

// retorna o desvio padrão de um vetor de floats
float Utilities::std_dev (float* &values, int n_values){
    float st_dv;
    if (n_values == 1){ st_dv = 0; }
    else{
        float av = this->average(values, n_values);
        float sum_parcs = 0;
        for (int i=0; i<n_values; i++){
            sum_parcs = sum_parcs + pow((values[i]-av),2);
        }
        st_dv = sqrt(sum_parcs/(n_values-1));
    }
    return st_dv;
}
