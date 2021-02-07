#include "Graph.h"
#include <climits>

int main(){
    int in = 0;
    bool first = true;
    while(in != 1 && in != 2 && in != 3){
        if(first){
            first = false;
        }
        else{
            std::cout << "Invalid input. Enter option 1, 2 or 3\n";
        }

        std::cout << "=========================================\n"
                    "                   MENU                   \n"
                    "                  ------                  \n"                
                    "   Type 1 for Route info                  \n"
                    "   Type 2 for info on a specific airport  \n"
                    "   Type 3 for info on a specific airline  \n"
                    "==========================================\n";
        std::cin >> in;
    }
    switch(in){
        case 1:
            
            break;
        
        case 2:

            break;
        
        case 3:

            break;
        
        default:
            break;
    }
    return 0;
}