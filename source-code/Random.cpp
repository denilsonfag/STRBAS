/* 
 * File:   Random.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include <fstream>

#include "Random.hpp"

Random::Random(long seed_random){ 
    seed = seed_random;
    first_seed = seed_random;
}
Random::~Random(){}

long Random::get_first_seed(){ 
    return first_seed; 
}

// retorna valores uniformemente distribuidos em [0,1]
double Random::random_number(){
    long *idum = &seed;
    long k;
    double ans;
    k =(*idum)/IQ;
    *idum = IA * (*idum - k * IQ) - IR * k;  // esta linha altera a semente original
    if (*idum < 0 ) *idum += IM;
    ans = AM * (*idum);
    return ans;
}

// recebe um inteiro e retorna outro (aleatório) entre 1 e o inteiro recebido
int Random::raffle_int (int n_itens){
    double rn = this->random_number();
    double aux_rn = rn * n_itens;
    for(int i=1; i<=n_itens; i++){
        if (aux_rn < i){
            return i;
        }
    }
}

// recebe um vetor de floats e retorna um índice sorteado
// os floats representam a probabilidade de seu índice ser sorteado
int Random::raffle_float_vector (int n_itens, float*& float_vector){
    // criar vetor com as probabilidades acumuladas:
    float *acc_probs = new float[n_itens];
    for(int i=0; i<n_itens; i++){
        if(i==0){
            acc_probs[i] = float_vector[i];
        }
        else{
            acc_probs[i] = acc_probs[i-1] + float_vector[i];
        }
    }
    // criando o randômico no intervalo das probabilidades:
    double rn = this->random_number();
    double aux_rn = rn * acc_probs[n_itens-1];
    // realizando o sorteio:
    int raffled = -1;  // retorno caso haja erro no sorteio
    for(int i=0; i<n_itens; i++){
        if (acc_probs[i] >= aux_rn){
            raffled = i;
            i = n_itens;
        }
    }
    delete [] acc_probs;
    return raffled;
}
