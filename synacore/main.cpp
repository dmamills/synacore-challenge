//
//  main.cpp
//  synacore
//
//  Created by Daniel Mills on 2018-08-03.
//  Copyright © 2018 Daniel Mills. All rights reserved.
//

#include <iostream>
#include "Cpu.cpp"


void disassemble(uint16_t* memory, int pc) {
    std::cout << "pc: " << pc << " value: " << memory[pc] << "\n";
}

int main(int argc, const char * argv[]) {

    if(argc == 0) {
        std::cout << "No filename provided.\n";
        return -1;
    }
    
    std::string filename = argv[1];
    Cpu cpu;
    
    cpu.loadFile(filename);
    
    try {
    cpu.run();
    } catch(std::exception e) {
        std::cout<<"caught exception\n";
    }
    //cpu.fulldisassemble();
    return 0;
}
