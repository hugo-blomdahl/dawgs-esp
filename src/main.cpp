#include <iostream>
#include "communication.hpp"
extern "C" void app_main() {
    Comms comms;
    while(true){
        std::cout << "Hello world!!!" << std::endl;
        sleep(1);
    }
}
