/* 
 * File:   ServiceOrder.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#include "ServiceOrder.hpp"

ServiceOrder::ServiceOrder(){}
ServiceOrder::~ServiceOrder(){}

// é definido um novo tempo de despacho para os serviços quando calculados os custos das rotas reais utilizando os dados considerados.
void ServiceOrder::new_service(int utx, int uty, int type, int tdispatch, int texec, int deadexec, int kmi, int kme){ 
    utm_x = utx;
    utm_y = uty;
    type_service = type;
    time_dispatch = tdispatch*60;  // estão em minutos nos dados mas o relógio da simulação é em segundos
    time_execution = texec*60;
    deadline_execution = deadexec;
    km_ini = kmi;
    km_end = kme;
    
    executed = false;
    total_time_execution_real_solution = 0.0;
    total_time_execution_proposed_solution = 0.0;
}

int ServiceOrder::get_time_execution(){
    return time_execution;
}
int ServiceOrder::get_time_dispatch(){
    return time_dispatch;
}
void ServiceOrder::set_executed_service(float partial_route_temporal_cost){
    executed = true;
    total_time_execution_proposed_solution = partial_route_temporal_cost - time_dispatch;
}
bool ServiceOrder::get_executed_service(){
    return executed;
}
// o código zero foi estipulado para serviços emergenciais
bool ServiceOrder::is_emergency(){
    if (type_service == 0){ return true; }
    else{ return false; }
}

void ServiceOrder::set_total_time_execution_real_solution(float partial_route_cost){
    total_time_execution_real_solution = partial_route_cost - time_dispatch;
}

void ServiceOrder::set_time_despatch(int new_time){
   time_dispatch =  new_time;
}

void ServiceOrder::print_total_time_execution(int s){
    cout << "  service: "  << s;
    cout << "   \treal time exec: " << total_time_execution_real_solution;
    cout << "\tproposed time exec: " << total_time_execution_proposed_solution;
    
    float diff_time = total_time_execution_real_solution - total_time_execution_proposed_solution;
    float improvement_time = (diff_time / total_time_execution_real_solution) * 100;
    cout << "   \timprovement: " << improvement_time << " %\r\n";
}
void ServiceOrder::print_service_order(){
    cout << "\ttipo: " << type_service << "\t\thorario de despacho: " << time_dispatch << "  \ttempo de execução: " << time_execution << "\r\n";
}
void ServiceOrder::save_service_order(ofstream& file_out){
    file_out << "\ttype: ";
    if (type_service == 0) file_out << "E";
    else file_out << "C";
    file_out << "\t\ttime dispatch: " << time_dispatch << " \ttime execution of order: " << time_execution << " seconds\r\n";
}
