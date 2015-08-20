/* 
 * File:   RealData.hpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

// classe para acesso aos arquivos de dados reais
#if !defined(__REALDATA_HPP__)
    #define  __REALDATA_HPP__

    #include "MtspInstance.hpp"
    #include "MtspSolution.hpp"
    #include "ServiceOrder.hpp"

    class RealData{
        int depot;
        
        ifstream data_file; // para carregar a instância de arquivo
        const char* data_file_name; // nome do arquivo
        
        int id_register;
        int id_work_day;
        int id_point;
        int utm_x;
        int utm_y;
        int id_team;
        int type_service;
        int time_dispatch;
        int time_execution;
        int deadline_execution;
        int km_ini;
        int km_end;
        
    public:
        RealData(int depot);
        ~RealData();
        
        void go_init_file();  // retorna ao início do arquivo
        void read_next_register();  // lê o próximo registro (linha) do arquivo
        
        int count_work_days();  // retorna o número de dias de serviço no arquivo
        void load_ids_work_days(IntList* &list_id_work_days);  // carregas as ids dos dias no vetor
        
        int count_services(int id_work_day); // retorna a qtde de serviços de um dia
        int count_teams(int id_work_day); // retorna a qtde de equipes de um dia
        void load_service_orders(int id_work_day, ServiceOrder* &day_services);
        void load_euclidean_graph(int id_work_day, MtspInstance* &instance);
        int x_depot();
        int y_depot();
        void load_real_solution(int selected_id_work_day, MtspSolution* &real_solution, MtspInstance* &instance, ServiceOrder* &day_services, FloatMatrix* real_time_matrix, int despatch_real_sol);
    };

#endif
    