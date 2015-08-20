/* 
 * File:   FloatMatrix.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__FLOAT_MATRIX_HPP__)
    #define  __FLOAT_MATRIX_HPP__

    #include <fstream>
    #include <iostream>
    using namespace std;
    
    class FloatMatrix{  
        int dimension;     // numero de valores da matriz = dimension x dimension
        float** matrix;    // matriz
    public:    
        FloatMatrix(int dimension_matrix);
        ~FloatMatrix();
        
        void fill_zero();  // preenche a matriz com zeros
        void set_value(int i, int j, float value);
        
        int get_dimension();
        float get_value(int i, int j);
        float* get_col_matrix (int col);
        float larger_value();  // retorna o maior valor da matriz;
        float smaller_value_diff_zero();  // retorna o menor valor na matriz diferente de zero
        
        void save(ofstream& f_out);
        void print();
    };

#endif
