/* 
 * File:   Random.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

// geracao de pseudo randomicos adaptado do livro "Numerical Recipes in C"
#if !defined(__RANDOM_HPP__)
    #define  __RANDOM_HPP__

    #define IA 16807
    #define IM 2147483647
    #define AM (1.0/IM)
    #define IQ 127773
    #define IR 2836

    class Random{
        long seed;        // se altera à medida que numeros randomicos são retornados
        long first_seed;  // semente enviada como parâmetro na criação do objeto
    public:
        
        Random(long seed_random);
        ~Random();

        // retorna a semente utilizada na criação do objeto:
        long get_first_seed();
        
        // retorna valores uniformemente distribuidos em [0,1]
        double random_number();
        
        // recebe um inteiro e retorna outro (aleatório) entre 1 e o inteiro recebido
        int raffle_int (int n_itens);
        
        // recebe um vetor de floats e retorna um índice sorteado
        // os floats representam a probabilidade de seu índice ser sorteado
        int raffle_float_vector (int n_itens, float* &float_vector);
    };

#endif
