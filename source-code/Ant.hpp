/* 
 * File:   Ant.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__ANT_HPP__)
    #define  __ANT_HPP__

    #include <iostream>
    using namespace std;
    
    class Ant{
        int starting_node;  // nó de início da rota da formiga
        int ending_node;    // nó final da rota da formiga
        int curr_node;      // nó em que a formiga se encontra
    public:
        Ant();
        ~Ant();

        void reset(int start_node, int end_node);
        void move(int next_node);
        
        int start_node();
        int end_node();
        int current_node();

        void print_ant(int id_ant);
    };

#endif
