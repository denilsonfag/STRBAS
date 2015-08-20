/* 
 * File:   Node.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "Node.hpp"

Node::Node(){}
Node::~Node(){}
void Node::create(float x, float y){
        cx = x;
        cy = y;
}
float Node::x(){ return cx; }
float Node::y(){ return cy; }
