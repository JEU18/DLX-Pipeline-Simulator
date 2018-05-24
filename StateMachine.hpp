#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
using namespace std;

class StateMachine {
public:
	void ReadTheInstruction();
	void FindC(int instrType);
	void steps(int instrType);
	void FindMAR(int instrType);
	void UpdatePC(int instrType);
	void SaveToReg(int instrType);
	void UpdatePCIR(int instrType);
	void WriteToMem(int instrType);
	void FindMDRLoad(int instrType);
	void FindMDRStore(int instrType);
	void UpdatePCOffset(int instrType);
	void CheckRS1(int reg, int instrType);
	void PerformOperationIType(int instrType);
	void PerformOperationRType( int instrType);		
	int getALUop(int instrType);
	int getMemOp(int instrType);
	int getAoe() { return Aoe; };
	int getBoe() { return Boe; };
	int getS2Op() { return S2op; };
	int getREGselect(int instrType);
	int getIRoeS2() { return IRoeS2; };		
	int getPCoeS1() { return PCoeS1; };			
	int getMemRead() { return MemRead; };	
	int getMDRoeS2() { return MDRoeS2; };
	int getMemWrite() { return MemWrite; };
	int getNextStep() { return nextStep; };
	int getPCMARselect() { return PCMARselect; };
	int getCurrentStep() { return currentStep; };
	string getInstruction() { return instruction; };
private:
	string instruction;
	int currentStep = 1, nextStep = 0;
	int S2op, ALUop, Cload, REGload, Aload, Aoe, Bload, Boe, 
		REGselect, IRload, IRoeS2, PCload, PCoeS1, PCMARselect, 
		MARload, MemRead, MemWrite, MemOp, MDRload, MDRoeS2, ZFlag;
	enum opcodes {
		Add = 132, Addi = 8, And = 136, Beqz = 4, Bnez = 5, J = 2,
		Lb = 32, Lh = 33, Lw = 35, Or = 137, Ori = 13, Sll = 104,
		Srl = 106, Sb = 40, Sh = 41, Sw = 43, Sub = 134
	};
};