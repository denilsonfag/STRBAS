/* 
 * File:   RealMatrix.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "RealMatrix.hpp"

RealMatrix::RealMatrix(){
    matrix_file_name = "inputs/real-data/costs.txt";
    matrix_file.open(matrix_file_name, ios::in);
}
RealMatrix::~RealMatrix(){
    matrix_file.close();
}

// retorna -1 se for o arquivo tiver acabado
void RealMatrix::read_next_register(){
    matrix_file >> id_register;
    matrix_file >> id_work_day;
    matrix_file >> id_point_a;
    matrix_file >> id_point_b;
    matrix_file >> lat_a;
    matrix_file >> lng_a;
    matrix_file >> lat_b;
    matrix_file >> lng_b;
    matrix_file >> real_distance;
    matrix_file >> real_time_cost;
}     

void RealMatrix::load_real_distance_matrix(int selected_id_work_day, FloatMatrix* &real_distance_matrix){
    matrix_file.close();
    matrix_file.open(matrix_file_name, ios::in);
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes
        this->read_next_register();
        if (id_register != -1){
            if (id_work_day == selected_id_work_day){
                real_distance_matrix->set_value(id_point_a, id_point_b, real_distance);
            }
        }
        else{
            flag = false;
        }
    }
}
void RealMatrix::load_real_time_matrix(int selected_id_work_day, FloatMatrix* &real_time_matrix){
    matrix_file.close();
    matrix_file.open(matrix_file_name, ios::in);
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes
        this->read_next_register();
        if (id_register != -1){
            if (id_work_day == selected_id_work_day){
                real_time_matrix->set_value(id_point_a, id_point_b, real_time_cost);
            }
        }
        else{
            flag = false;
        }
    }
}
 