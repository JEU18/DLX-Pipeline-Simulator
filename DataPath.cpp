#include "stdafx.h"
#include "DataPath.hpp"

int DataPath::LoadProgramCode(char* fileName, unsigned char MainMemory[])
{
	string name(fileName);
	ifstream myfile(name);
	string ch1, ch2, line;
	if (!myfile) {
		cerr << "Unable to open file";
		exit(1);
	}
	while (myfile >> line) {
		for (int i = 0; i < line.length(); i+=2) {
			ch1 = line.at(i);	ch2 = line.at(i + 1);
			unsigned int i1 = stoi(ch1, nullptr, 16), i2 = stoi(ch2, nullptr, 16);
			unsigned char result = ((i1  << 4) | i2);
			MainMemory[memLength] = result;
			memLength++;
		}	
	}
	start(MainMemory);
	return 1;
}
//---------------------------------------------------------------------------------------
void DataPath::start(unsigned char MainMemory[])
{
	cout << "Press Enter to Go to Next Step\n\n" << endl << "First Instruction";
	cin.get();
	while (PC < memLength) {
			int instrType = getOpcode(MainMemory);
			MAR = 0; MDR = 0;
			stateMachine.ReadTheInstruction();
			execute(MainMemory, instrType);
			stateMachine.UpdatePC(instrType);
			execute(MainMemory, instrType);
			while (stateMachine.getNextStep() != 0) {
				switch (stateMachine.getNextStep()) {
				case 3:
					stateMachine.PerformOperationRType(instrType);
					execute(MainMemory, instrType); break;
				case 4:
					stateMachine.PerformOperationIType(instrType);
					execute(MainMemory, instrType); break;
				case 5:
					stateMachine.CheckRS1(AReg, instrType);
					execute(MainMemory, instrType); break;
				case 6:
					stateMachine.UpdatePCOffset(instrType);
					execute(MainMemory, instrType); break;
				case 7:
					stateMachine.FindMAR(instrType);
					execute(MainMemory, instrType); break;
				case 8:
					stateMachine.UpdatePCIR(instrType);
					execute(MainMemory, instrType); break;
				case 9:
					stateMachine.FindMDRStore(instrType);
					execute(MainMemory, instrType);
					MAR = 0; break;
				case 10:
					stateMachine.FindMDRLoad(instrType);
					execute(MainMemory, instrType);
					MAR = 0; break;
				case 11:
					stateMachine.WriteToMem(instrType);
					execute(MainMemory, instrType); break;
					MDR = 0;
				case 12:
					stateMachine.FindC(instrType);
					execute(MainMemory, instrType); break;
					MDR = 0;
				case 13:
					stateMachine.SaveToReg(instrType);
					execute(MainMemory, instrType); break;
					MDR = 0;
				} 
			}
			cout << "\n\nNext Instruction";
			cin.get();
	}
}
//---------------------------------------------------------------------------------------
void DataPath::execute(unsigned char MainMemory[], int instrType)
{
	setAllData(MainMemory, instrType);
	printResults();
}
//---------------------------------------------------------------------------------------
void DataPath::setAllData(unsigned char MainMemory[], int instrType)
{
	MemoryOpcode = stateMachine.getMemOp(instrType);
	setHardwareRegister(MainMemory);
	IRimm = IR & 0x0000FFFF;
	ALUopcode = stateMachine.getALUop(instrType);
	setMemoryRegister(MainMemory, instrType);
	setRegisterBuffers(instrType);
	setS1Bus();		setS2Bus();
	setDest();		setAddr();
	setData(MainMemory);
	setRegisterFile(instrType);
}
//---------------------------------------------------------------------------------------
void DataPath::setS1Bus()
{
	int c = stateMachine.getCurrentStep();
	if (c == 2 || c == 3 || c == 4 || c == 6 || c == 7 || c == 8) {
		if (stateMachine.getAoe() == 1) {
			S1Bus = AReg;
		}
		else if (stateMachine.getPCoeS1() == 1) {
			S1Bus = PC-4;
		}
	}
	else { S1Bus = 0; }
}
//---------------------------------------------------------------------------------------
void DataPath::setS2Bus()
{
	int c = stateMachine.getCurrentStep();
	if (c == 2 || c == 3 || c == 4 || c == 6 || c == 7 || c == 8 || c == 9 || c == 12) {
		if (stateMachine.getBoe() == 1) {
			S2Bus = checkS2op(BReg);
		}
		else if (stateMachine.getIRoeS2() == 1) {
			S2Bus = checkS2op(IR);
		}
		else if (stateMachine.getMDRoeS2() == 1) {
			S2Bus = checkS2op(MDR);
		}
		else if (checkS2op(0) == 7) {
			S2Bus = 4;
		}
	}
	else { S2Bus = 0; }
}
//---------------------------------------------------------------------------------------
void DataPath::setDest() 
{
	int c = stateMachine.getCurrentStep();
	if (c == 2 || c == 3 || c == 4 || c == 6 || c == 7 || c == 8 || c == 9 || c == 12) {
		Dest = ALU(S1Bus, S2Bus, ALUopcode);
	}
	else { Dest = 0; }
}
//---------------------------------------------------------------------------------------
void DataPath::setAddr()
{
	int c = stateMachine.getCurrentStep();
	if (c == 1 || c == 10 || c == 11) {
		if (stateMachine.getPCMARselect() == 0) {
			Addr = PC;
		}
		else if
			(stateMachine.getPCMARselect() == 1) {
			Addr = MAR;
		}
	}
	else { Addr = 0; }
}
//---------------------------------------------------------------------------------------
void DataPath::setData(unsigned char MainMemory[])
{
	if (stateMachine.getCurrentStep() == 1) { Data = IR; }
	else if (stateMachine.getCurrentStep() == 10 || stateMachine.getCurrentStep() == 11) { Data = MDR; }
	else { Data = 0; }
}
//---------------------------------------------------------------------------------------
void DataPath::setRegisterFile(int instrType)
{
	if (stateMachine.getCurrentStep() == 13) {
		switch (stateMachine.getREGselect(instrType)) {
		case 0: RegFile[(IR & 0x00F800) >> 11] = CReg; break;
		case 1: RegFile[(IR & 0x1F0000) >> 16] = CReg; break;
		}
	}
	else if (stateMachine.getCurrentStep() == 11) {
	}
}
//---------------------------------------------------------------------------------------
void DataPath::setMemoryRegister(unsigned char MainMemory[], int instrType)
{
	MemoryOpcode = stateMachine.getMemOp(instrType);
	if (stateMachine.getCurrentStep() == 7) {
		MAR = ALU(AReg, IRimm, 0);
	}
	if (stateMachine.getCurrentStep() == 9){
		MDR = BReg;
	}
	else if (stateMachine.getCurrentStep() == 10){
		if (MemoryOpcode == 2) {
			MDR = MainMemory[MAR]; //LB
		}
		else if (MemoryOpcode == 1) {
			MDR = MainMemory[MAR] << 12;
			for (int i = 0; i < 4; i++) {
				MDR |= ((MainMemory[MAR + i]) << (12 - (8 * i)));
			} //LH
		}
		else if (MemoryOpcode == 0) {
			MDR = MainMemory[MAR] << 24;
			for (int j = 0; j < 4; j++) {
				MDR |= ((MainMemory[MAR + j]) << (24 - (8 * j)));
			}//LW
		}		
	}
	if (stateMachine.getMemWrite() == 1) {
		if (MemoryOpcode == 2) { //SB
			MainMemory[MAR] = MDR;
		}
		else if (MemoryOpcode == 1) { //Sh
			MainMemory[MAR] = MDR >> 12;
			for (int k = 0; k < 4; k++) {
				MainMemory[MAR + k] = MDR >> (12 - (8 * k));
			}
		}
		else if (MemoryOpcode == 0) { //Sw
			MainMemory[MAR] = MDR >> 24;
			for (int k = 0; k < 4; k++) {
				MainMemory[MAR + k] = MDR >> (24 - (8 * k));
			}
		}
	}
}
//---------------------------------------------------------------------------------------
void DataPath::setHardwareRegister(unsigned char MainMemory[])
{
	if (stateMachine.getCurrentStep() == 1) {
		IR = MainMemory[PC] << 24;
		for (int i = 0; i < 4; i++) {
			IR |= ((MainMemory[PC + i]) << (24 - (8 * i)));
		}
	}
	if (stateMachine.getCurrentStep() == 2) {
		PC = PC + 4;
	}
	else if (stateMachine.getCurrentStep() == 6) {
		int offset = checkS2op(IR);
		PC += offset;
	}
	else if (stateMachine.getCurrentStep() == 8) {
		unsigned int offset = checkS2op(IR);
		PC -= checkS2op(IR); //FIND PC + IR 
	}
}
//---------------------------------------------------------------------------------------
void DataPath::setRegisterBuffers(int instrType)
{
	if (stateMachine.getCurrentStep() == 2) {
		AReg = (IR & 0x03E00000) >> 21;
		BReg = (IR & 0x001F0000) >> 16;
	}
	if (stateMachine.getCurrentStep() == 3){
		CReg = ALU(AReg, BReg, ALUopcode);
	}
	else if (stateMachine.getCurrentStep() == 4) {
		CReg = ALU(AReg, IRimm, ALUopcode);
	}
	else if (stateMachine.getCurrentStep() == 12) {
		CReg = ALU(MDR, 0, 0);
	}
}
//---------------------------------------------------------------------------------------
int DataPath::getOpcode(unsigned char MainMemory[])
{
	unsigned int op = MainMemory[PC] >> 2;
	if (op == 0) {
		op = instrRType(MainMemory);
	}
	return op;
}
//---------------------------------------------------------------------------------------
int DataPath::instrRType(unsigned char MainMemory[])
{
	unsigned int op = (MainMemory[PC + 3]) + 100;
	return op;
}
//---------------------------------------------------------------------------------------
int DataPath::ALU(int S1, int S2, int opcode)
{
	int result = 0;
	switch (opcode) {
	case 0: result = S1 + S2; break;
	case 4: result = S1 & S2; break;
	case 5: result = S1 | S2; break;
	case 8: result = S1 << S2; break;
	case 10: result = S1 >> S2; break;
	case 1:	result = S1 - S2; break;
	}
	return result;
}
//---------------------------------------------------------------------------------------
void DataPath::printResults() {							  
	cout << "\n\n" << stateMachine.getInstruction() << "\n===================================================="
	<< "===================================================================\n" << "Register File:" << endl;
	for (int i = 0; i < 32; i++) { 
	cout << "R" << dec << i << ": "  << RegFile[i] << setw(3);
	}
	cout << "\n\nMemory Registers:" << endl << "MAR: " << MAR << "\t" << "MDR: "  << dec << MDR << endl
		<< "\nHardware Registers:" << endl << "IR: " << dec << IR << "\t" << "PC: " << dec << PC << endl
		<< "\nData Bus:" << endl << "S1: " << dec << S1Bus << "\tS2: " << dec << S2Bus << "\tDest: " << Dest
		<< "\tAddr: " << Addr << "\tData: " << dec << Data << endl << "\nRegister Buffers" << endl << "A: "
		<< dec << AReg << "\t" << "B: " << dec << BReg << "\t" << "C: " << dec << CReg << endl;
	cin.get();
}
//---------------------------------------------------------------------------------------
int DataPath::checkS2op(int num)
{
	int S2 = 0;
		switch (stateMachine.getS2Op()) {
		case 0: S2 = num;
		case 1: S2 = num & 0x000000FF; break;
		case 3: S2 = num & 0x0000FFFF; break;
		case 5:	S2 = num & 0x03FFFFFF; break;
		case 7:	S2 = 4; break;
		}
	return S2;
}