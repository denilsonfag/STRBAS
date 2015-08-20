/* 
 * File:   ServiceOrder.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#if !defined(__SERVICEORDER_HPP__)
    #define  __SERVICEORDER_HPP__

    // classe que representa as ordens de serviço
    #include <iomanip>
    #include <iostream>
    #include <fstream>
    using namespace std;

    class ServiceOrder {
        int utm_x;
        int utm_y;
        int type_service;
        int time_dispatch;      // momento de despacho do serviço, em minutos a partir das 08:00 do dia
        int time_execution;     // tempo necessário para execução do serviço
        int deadline_execution; // instante de término do prazo de execução do seviço 
        int km_ini;
        int km_end;
        
        bool executed;
        float total_time_execution_real_solution;
        float total_time_execution_proposed_solution;
        
    public:
        
        ServiceOrder();
        ~ServiceOrder();

        void new_service(int utx, int uty, int type, int tdispatch, int texec, int deadexec, int kmi, int kme); 
        
        int get_time_execution();
        int get_time_dispatch();
        
        void set_executed_service(float partial_route_temporal_cost);
        bool get_executed_service();  // retorna true se o serviço foi executado
        bool is_emergency();      // retorna true se o serviço for uma emregencia
        
        void set_total_time_execution_real_solution(float partial_route_cost);
        
        void set_time_despatch(int current_route_cost);
        
        void print_total_time_execution(int s);
        void print_service_order();
        void save_service_order(ofstream& file_out);
    };
    
#endif 
