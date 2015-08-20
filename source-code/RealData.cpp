/* 
 * File:   RealData.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "RealData.hpp"

RealData::RealData(int depot){
    this->depot = depot;
    data_file_name = "inputs/real-data/data.txt";
    data_file.open(data_file_name, ios::in);
    id_register = 0;  // o final do arquivo é -1
}
RealData::~RealData(){
    data_file.close();
}

void RealData::go_init_file(){
    data_file.close();
    data_file.open(data_file_name, ios::in);
}

// os tempos de execução são convertidos para segundos:
void RealData::read_next_register(){
    data_file >> id_register;
    data_file >> id_work_day;
    data_file >> id_point;
    data_file >> utm_x;
    data_file >> utm_y;
    if (id_point != depot){  // os registros dos depósitos não possuem os valores abaixo no arquivo
        data_file >> id_team;
        data_file >> type_service;
        data_file >> time_dispatch;
        data_file >> time_execution;
        time_execution = time_execution;
        data_file >> deadline_execution;
        data_file >> km_ini;
        data_file >> km_end;
    }
}     

int RealData::count_work_days(){
        
    bool first_register = true;
    int counter_days = 0;
    int current_day;

    data_file.close();
    data_file.open(data_file_name, ios::in);
    
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes

        this->read_next_register();

        if (id_register != -1){
            if (first_register){
                first_register = false;
                current_day = id_work_day;
                counter_days++;
            }
            if (current_day != id_work_day){
                current_day = id_work_day;
                counter_days++;
            }
        }
        else{
            flag = false;
        }
    }
    return counter_days;
}
void RealData::load_ids_work_days(IntList* &list_id_work_days){
    
    list_id_work_days->clear();
        
    bool first_register = true;
    int counter_days = 0;
    int current_day;
    
    data_file.close();
    data_file.open(data_file_name, ios::in);
    
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes

        this->read_next_register();
        
        if (id_register != -1){
            if (first_register){
                first_register = false;
                current_day = id_work_day;
                list_id_work_days->add_value(current_day);
                counter_days++;
            }
            if (current_day != id_work_day){
                current_day = id_work_day;
                list_id_work_days->add_value(current_day);
                counter_days++;
            }
        }
        else{
            flag = false;
        }
    }
}

int RealData::count_services(int selected_id_work_day){
    
    int counter_services = 0;

    data_file.close();
    data_file.open(data_file_name, ios::in);
    
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes

        this->read_next_register();

        if (id_register != -1){
            if (id_work_day == selected_id_work_day){
                if (id_point != depot){
                    counter_services++;
                }
            }
        }
        else{
            flag = false;
        }
    }
    return counter_services;
}
int RealData::count_teams(int selected_id_work_day){

    int counter_teams = 0;
    
    data_file.close();
    data_file.open(data_file_name, ios::in);
    
    int current_team;
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes

        this->read_next_register();

        if (id_register != -1){
            if (id_work_day == selected_id_work_day){
                if (id_point != depot){
                    if (counter_teams == 0){
                        counter_teams++;
                        current_team = id_team;
                    }
                    else{
                        if (current_team != id_team){
                            counter_teams++;
                            current_team = id_team;
                        }
                    }
                }        
            }
        }
        else{
            flag = false;
        }
    }
    return counter_teams;
}

void RealData::load_service_orders(int selected_id_work_day, ServiceOrder* &day_services){

    data_file.close();
    data_file.open(data_file_name, ios::in);
    
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes

        this->read_next_register();

        if (id_register != -1){
            if (id_work_day == selected_id_work_day){
                if (id_point != depot){  // o índice do depósito não é inicializado

                    day_services[id_point].new_service(utm_x, utm_y, type_service, time_dispatch, time_execution, deadline_execution, km_ini, km_end);

                }
            }
        }
        else{
            flag = false;
        }
    }
}
void RealData::load_euclidean_graph(int selected_id_work_day, MtspInstance* &instance){
    
    data_file.close();
    data_file.open(data_file_name, ios::in);
    
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes

        this->read_next_register();

        if (id_register != -1){
            if (id_work_day == selected_id_work_day){
                if (id_point != depot){

                    instance->add_plan_node(utm_x, utm_y);

                }
            }
        }
        else{
            flag = false;
        }
    }
}
int RealData::x_depot(){

    int coord_x = -1;
    
    data_file.close();
    data_file.open(data_file_name, ios::in);
    
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes

        this->read_next_register();

        if (id_register != -1){
            if (id_point == depot){
                coord_x = utm_x;
                flag = false;
            }
        }
        else{
            flag = false;
        }
    }
    return coord_x;
}
int RealData::y_depot(){
    
    int coord_y = -1;
    
    data_file.close();
    data_file.open(data_file_name, ios::in);
    
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes

        this->read_next_register();

        if (id_register != -1){
            if (id_point == depot){
                coord_y = utm_y;
                flag = false;
            }
        }
        else{
            flag = false;
        }
    }
    return coord_y;
}

// é definido um novo tempo de despacho para os serviços quando calculados os custos das rotas reais utilizando os dados considerados.
void RealData::load_real_solution(int selected_id_work_day, MtspSolution* &real_solution, MtspInstance* &instance, ServiceOrder* &day_services, FloatMatrix* real_time_matrix, int despatch_real_sol){
    
    FloatMatrix* cost_matrix = instance->get_cost_matrix();
    
    real_solution->reset();
    data_file.close();
    data_file.open(data_file_name, ios::in);
    
    int current_team = -1;
    int last_point_inserted = -1;
    float current_route_time_cost = 0.0;
    
    bool flag = true;
    while (flag) {  // lendo todo o arquivo das matrizes
        
        this->read_next_register();
        
        if (id_register != -1){ // fim do arquivo
            if (id_work_day == selected_id_work_day){  // este serviço faz parte da solução real
                
                if (last_point_inserted != -1){  // na primeira iteração não é adicionado nenhum no

                    if (last_point_inserted == depot){
                        current_team++;
                        real_solution->add_node(current_team, depot, cost_matrix);
                        current_route_time_cost = 0.0;
                    }

                    real_solution->add_node(current_team, id_point, cost_matrix);
                    
                    float real_time = real_time_matrix->get_value(last_point_inserted, id_point);
                    float time_exec = day_services[id_point].get_time_execution();
                    float adding_time_cost = real_time + time_exec;
                    current_route_time_cost = current_route_time_cost + adding_time_cost;
                }    
                    
                if (id_point != depot){
                    // definindo o horário de despacho do serviço a a partir do momento que uma equipe inicia o deslocamento para ele na solução real
                    if (despatch_real_sol){
                        int old_time = day_services[id_point].get_time_dispatch();
                        if (old_time > 0){ // o serviço não faz parte da instância inicial
                            day_services[id_point].set_time_despatch(current_route_time_cost); // atualizando o horário de despacho
                        }
                    } 
                }
                last_point_inserted = id_point;
            }
        }
        else{
            flag = false;
        }
    }
    real_solution->finalize_solution(-1, -1, -1);
}
