/* 
 * File:   IntList.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 18 de julho de 2015, 16:11
 */

#if !defined(__INTLIST_HPP__)
    #define  __INTLIST_HPP__

    #include <fstream>    
    #include <iostream>
    using namespace std;

    class IntList{   // lista com valores inteiros positivos
    private:
        int size;    // número máximo de valores que a lista pode conter
        int *values; // valores da lista
        int index;   // aponta a próxima posição vazia da lista. Se zero, a lista está vazia
    public:    
        IntList();
        ~IntList();
        void create(int size_list);

        void clear();                   // esvazia a lista.
        bool add_value(int value);      // adiciona value no final da lista. Retorna falso se a lista estiver cheia.
        bool change_values(int index_a, int index_b);  // troca os valores entre os indices informados. Retorna falso se algum dos indices estiver fora do número de itens da lista.
        bool reverse(int index_start, int index_end);  // inverte os valores da lista de index_start a index_end.  Retorna falso se index_end for <= index_start ou se algum dos indices estiver fora do número de itens da lista.
       
        int size_list();                // retorna a capacidade da lista
        int n_items();                  // retorna o número de itens na lista
        int value(int index_value);     // retorna um valor da lista pelo índice. Retorna -1 se algum dos indices estiver fora do número de itens da lista.
        int last_value();               // retorna o último valor da lista
        bool in_the_list(int value);    // retorna verdadeiro se o valor estiver na lista
        bool is_full();                 // retorna verdadeiro se a lista estiver cheia
        bool is_empty();                // retorna verdadeiro se a lista estiver vazia
        
        void save(ofstream& file_out);  // salva a lista em arquivo
        void print();                   // imprime a lista
        void print_adding_1();          // imprime a lista adicionando 1 a todos os valores
    };

#endif
