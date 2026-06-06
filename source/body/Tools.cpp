#include "Tools.h"

namespace Tools{
    void Out_System(std::string str){
        std::cout<<"[输出]"<<str<<std::endl;
    }
    
    void Out_System_Error(std::string str){
        std::cerr<<"[错误]"<<str<<std::endl;
    }

    int Input_System_int(){
        int a;
        std::cin>>a;
        return a;
    }

    std::string Input_System_string(){
        std::string a;
        std::cin>>a;
        return a;
    }
}