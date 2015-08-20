/* 
 * File:   RealMatrix.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

// classe para acesso ao arquivo com as matrizes de custos reais
#if !defined(__REALMATRIX_HPP__)
    #define  __REALMATRIX_HPP__

    #include "IntList.hpp"
    #include "FloatMatrix.hpp" 

    class RealMatrix{
        
        ifstream matrix_file; // para carregar a instância de arquivo
        const char* matrix_file_name; // nome do arquivo
        
        int id_register;
        int id_work_day;
        int id_point_a;
        int id_point_b;
        float lat_a;
        float lng_a;
        float lat_b;
        float lng_b;
        float real_distance;
        int real_time_cost;
        
    public:
        RealMatrix();
        ~RealMatrix();
        
        void read_next_register();
        
        void load_real_distance_matrix(int selected_id_work_day, FloatMatrix* &real_distance_matrix);
        void load_real_time_matrix(int selected_id_work_day, FloatMatrix* &real_time_matrix);
    };

#endif
    