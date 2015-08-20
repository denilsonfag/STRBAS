/* 
 * File:   FloatMatrix.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "FloatMatrix.hpp"

FloatMatrix::FloatMatrix(int dimension_matrix){
    dimension = dimension_matrix;
    matrix = new float*[dimension];
    for (int i=0; i < dimension; i++){
        matrix[i] = new float[dimension];
    }
}
FloatMatrix::~FloatMatrix(){
    for (int i=0; i < dimension; i++){
        delete [] matrix[i];
    }        
    delete [] matrix; 
}

void FloatMatrix::fill_zero(){
    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            matrix[i][j] = 0.0;
        }
    }
}
void FloatMatrix::set_value(int i, int j, float value){
    matrix[i][j] = value;
}

int FloatMatrix::get_dimension(){
    return dimension;
}
float FloatMatrix::get_value(int i, int j){
    if ((i >= dimension) || (i < 0) || (j >= dimension) || (j < 0)){ 
        return -1;  // erro
    }
    else{
        return matrix[i][j];
    }
}
float* FloatMatrix::get_col_matrix (int col){
    return matrix[col];
}
float FloatMatrix::larger_value(){
    float larger;
    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            if ((i==0) && (j==0)){
                larger = matrix[i][j];
            }
            else{
                if (matrix[i][j] > larger){
                    larger = matrix[i][j];
                }
            }
        }
    }
    return larger;
}
float FloatMatrix::smaller_value_diff_zero(){
    float smaller;
    bool first = true;
    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            if (matrix[i][j] != 0){
                if (first){
                    smaller = matrix[i][j];
                    first = false;
                }
                else{
                     if (matrix[i][j] < smaller){
                         smaller = matrix[i][j];
                     }
                }
            }
        }
    }
    return smaller;
}
 
void FloatMatrix::save(ofstream& f_out){
    for (int i=0; i < dimension; i++){
        f_out << "\t" << i; 
    }
    for (int i=0; i < dimension; i++){
        f_out << "\r\n" << i;
        for (int j=0; j < dimension; j++){
            f_out << "\t" << matrix[i][j];
        }
    }
    f_out << "\r\n";
}
void FloatMatrix::print(){
    for (int i=0; i < dimension; i++){
        cout << "\t" << i; 
    }
    for (int i=0; i < dimension; i++){
        cout << "\r\n" << i;
        for (int j=0; j < dimension; j++){
            cout << "\t" << matrix[i][j];
        }
    }
    cout << "\r\n";
}
