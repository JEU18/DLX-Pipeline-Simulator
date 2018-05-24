#pragma once
#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iomanip>
#include <string>
#include "StateMachine.hpp"
using namespace std;

class DataPath {
public:
	StateMachine stateMachine;
	void setS1Bus();		void setS2Bus();
	void setDest();			void setAddr();
	int checkS2op(int num);
	void printResults();
	void setRegisterFile(int instrType);
	int ALU(int S1, int S2, int opcode);
	void setRegisterBuffers(int instrType);
	void start(unsigned char MainMemory[]);
	void setData(unsigned char MainMemory[]);
	int getOpcode(unsigned char MainMemory[]);
	int instrRType(unsigned char MainMemory[]);
	void setHardwareRegister(unsigned char MainMemory[]);
	void execute(unsigned char MainMemory[], int instrType);
	void setAllData(unsigned char MainMemory[], int instrType);
	void setMemoryRegister(unsigned char MainMemory[], int instrType);
	int LoadProgramCode(char* fileName, unsigned char MainMemory[]);
private:
	int IRimm, MemoryOpcode, ALUopcode, memLength = 0;
	int S2Bus = 0;
	unsigned int RegFile[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
		22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
	int IR = 0, Data = 0, AReg = 0, BReg = 0, CReg = 0,
		PC = 0, MAR = 0, MDR = 0, S1Bus = 0, Dest = 0, Addr = 0;
};