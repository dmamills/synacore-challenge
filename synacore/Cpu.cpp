//
//  Cpu.cpp
//  synacore
//
//  Created by Daniel Mills on 2018-08-03.
//  Copyright © 2018 Daniel Mills. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <stack>
#include <array>

typedef uint16_t val;

#define MAX_NUM 32767
#define START_REGISTER 32768
#define END_REGISTER 32775

const int NUM_REGISTERS = 8;

class Cpu {
  
public:
    Cpu() {

    }
    ~Cpu() {
        delete[] this->memory;
    }
    
    void loadFile(std::string filename) {
        this->pc = 0;
        for(auto i =0; i < 8;i++) {
            this->r[i] = 0;
        }
        FILE *fp = fopen(filename.c_str(), "rb");
        
        if(fp == NULL) {
            std::cout << "Unable to open file\n";
            return;
        }
        
        fseek(fp, 0L, SEEK_END);
        this->filesize = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        
        this->memory = new uint16_t[this->filesize];
        fread(this->memory, this->filesize, 1, fp);
        fclose(fp);
    }
    
    bool isRegister(val n) {
        return (n >= START_REGISTER && n <= END_REGISTER);
    }
    
    val getRegister(val n) {
        if(this->isRegister(n)) {
            return END_REGISTER - n;
        }
        
        //the int overflowth!
        return -1;
    }
    
    void disassemble(val opCode) {
        std::cout << "pc: " << (this->pc - 1) << " value: " << opCode << "\n";
    }
    
    /**
     gets the next value in memory,
     @param registerIdx: if the memory value represents a register, false returns that registers value, true returns in index of that register
    */
    val getNextValue(bool registerIdx = false) {
        val v = this->memory[this->pc];
        pc++;
        
        if(this->isRegister(v)) {
            if(registerIdx) {
                return this->getRegister(v);
            }
            
            return this->r[this->getRegister(v)];
        }
        
        return v;
    }
    
    void emulate() {
        val opCode = this->getNextValue();
        switch(opCode) {

            case 0x0: {
                throw (this->pc - 1);
                break;
            }
            case 0x1: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                this->r[a] = b;
                break;
            }
            case 0x2: {
                val a = this->getNextValue();
                this->s.push(a);
                break;
            }
            case 0x3: {
                if(this->s.size() == 0) {
                    throw;
                }
                
                val a = this->getNextValue(true);
                val v = this->s.top();
                this->s.pop();
                this->r[a] = v;
                break;
            }
            case 0x4: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                val c = this->getNextValue();
                
                if(b == c) {
                    this->r[a] = 1;
                } else {
                    this->r[a] = 0;
                }
                
                break;
            }
            case 0x5: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                val c = this->getNextValue();
                
                if(b > c) {
                    this->r[a] = 1;
                } else {
                    this->r[a] = 0;
                }
                
                break;
            }
            case 0x6: {
                val j = this->getNextValue();
                this->pc = j;
                break;
            }
            case 0x7: {
                val a = this->getNextValue();
                val b = this->getNextValue();
                if(a != 0) {
                    this->pc = b;
                }
                break;
            }
            case 0x8: {
                val a = this->getNextValue();
                val b = this->getNextValue();
                
                if(a == 0) {
                    this->pc = b;
                }
                break;
            }
            case 0x9: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                val c = this->getNextValue();
                
                this->r[a] = (b + c) % START_REGISTER;
                break;
            }
            case 0xA: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                val c = this->getNextValue();
                
                this->r[a] = (b * c) % START_REGISTER;
                break;
            }
            case 0xB: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                val c = this->getNextValue();
                
                this->r[a] = (b % c);
                break;
            }
            case 0xC: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                val c = this->getNextValue();
                
                this->r[a] = b & c;
                break;
            }
            case 0xD: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                val c = this->getNextValue();
                
                this->r[a] = b | c;
                break;
            }
            case 0xE: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                
                this->r[a] = ~ b & 32767;
                break;
            }
            case 0xF: {
                val a = this->getNextValue(true);
                val b = this->getNextValue();
                this->r[a] = this->memory[b];
                break;
            }
            case 0x10: {
                val a = this->getNextValue();
                val b = this->getNextValue();
                
                this->memory[a] = b;
                break;
            }
            case 0x11: {
                val a = this->getNextValue();
                this->s.push(this->pc);
                this->pc = a;
                break;
            }
            case 0x12: {
                
                if(this->s.size() == 0) {
                    throw;
                }
                val a = this->s.top();
                this->s.pop();
                this->pc = a;
                break;
            }
            case 0x13: {
                val c = this->getNextValue();
                std::cout << (char)c;
                break;
            }
            case 0x14: {
                val a = this->getNextValue(true);
                val c = getchar();
                this->r[a] = c;
                break;
            }
            case 0x15: {
                break;
            }
            default: {
                disassemble(opCode);
                throw this->pc;
                break;
            }
        }
    }
    
    void run() {
        while(this->pc < this->filesize) {
            this->emulate();
        }
    }
    
    void debug() {
        for(auto i =0; i< NUM_REGISTERS;i++) {
            std::cout << "Register " << i << " val: " << this->r[i] << "\n";
        }
        
        std::cout << "Stack size: " << this->s.size();
        
        if(this->s.size() > 0)
            std::cout<< " Top val: " << this->s.top() << "\n";
    }
    
private:
    
    int pc;
    long filesize;
    uint16_t* memory;
    std::array<val, NUM_REGISTERS> r;
    std::stack<val> s;
};
