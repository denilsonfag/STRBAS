/* 
 * File:   main.cpp
 * Author: Denilson Fagundes Barbosa
 * E-mail: denilsonfag@yahoo.com.br
 * Created on 22 de Novembro de 2014, 12:20
 */

#define TYPE_EXPERIMENT   2  // tipo do experimento: 1: experimento com instâncias benchmark; 2: experimento com dados reais

#if TYPE_EXPERIMENT == 1
    #include "StandardExperiment.hpp"
#endif
#if TYPE_EXPERIMENT == 2
    #include "RealExperiment.hpp"
#endif

int main() {
    
    cout << setiosflags (ios::fixed) << setprecision(2);  // impressões com duas casas decimais
    
    #if (TYPE_EXPERIMENT == 1)  // experimento com instâncias benchmark
        StandardExperiment *se = new StandardExperiment();
        se->run_standard_experiment();
        delete se;
    #endif

    #if (TYPE_EXPERIMENT == 2)  // experimento com dados reais
        
        for (int param = 1; param <= 1; param++){  // beta
            
//            cout << "\r\n\r\n==================>>>>>>>> BETA = " << param << "\r\n\r\n";
            RealExperiment *re = new RealExperiment();
            re->run_real_experiment(param);
            delete re;
            
        }
    #endif
}
