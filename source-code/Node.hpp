/* 
 * File:   Node.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__NODE_HPP__)
    #define  __NODE_HPP__

    class Node{    // vértice de uma grafo euclidiano
        float cx;  // coordenada plana x
        float cy;  // coordenada plana y
    public:
        Node();
        ~Node();
        void create(float x, float y);
        float x();
        float y();
    };

#endif
