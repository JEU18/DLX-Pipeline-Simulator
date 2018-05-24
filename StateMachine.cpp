#include "stdafx.h"
#include "StateMachine.hpp"

void StateMachine::steps(int instrType)
{
		if (instrType == Add || instrType == And || instrType == Or
			|| instrType == Sll || instrType == Srl || instrType == Sub) {
			if (currentStep == 2) { nextStep = 3; }
			if (currentStep == 3) { nextStep = 13; }
			if (currentStep == 13) { nextStep = 0; }
		}
		else if (instrType == Addi || instrType == Ori) {
			if (currentStep == 2) { nextStep = 4; }
			if (currentStep == 4) { nextStep = 13; }
			if (currentStep == 13) { nextStep = 0; }
		}
		else if (instrType == Lb || instrType == Lh || instrType == Lw) {
			if (currentStep == 2) { nextStep = 7; }
			if (currentStep == 7) { nextStep = 10; }
			if (currentStep == 10) { nextStep = 12; }
			if (currentStep == 12) { nextStep = 13; }
			if (currentStep == 13) { nextStep = 0; }
		}
		else if (instrType == Sb || instrType == Sh || instrType == Sw) {
			if (currentStep == 2) { nextStep = 7; }
			if (currentStep == 7) { nextStep = 9; }
			if (currentStep == 9) { nextStep = 11; }
			if (currentStep == 11) { nextStep = 0; }
		}
		else if (instrType == Beqz || instrType == Bnez) {
			if (currentStep == 2) { nextStep = 5; }
			if (currentStep == 5 && ZFlag == 1) { nextStep = 8;}
			else if (ZFlag == 0 || currentStep == 8) { nextStep = 0; }
		}
		else if (instrType == J) {
			if (currentStep == 2) { nextStep = 6; }
			if (currentStep == 6) { nextStep = 0; }
		}
}
//---------------------------------------------------------------------------------------
void StateMachine::ReadTheInstruction()
{
	instruction = "Read the Instruction";
	currentStep = 1;
	PCMARselect = 0;	MemRead = 1;
	MemOp = 0;	IRload = 1;
}
//---------------------------------------------------------------------------------------
void StateMachine::UpdatePC(int instrType)
{
	instruction = "Update the PC";
	currentStep = 2;
	Aload = 1;	PCload = 1;	S2op = 7;
	Bload = 1;	PCoeS1 = 1;	ALUop = 0;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::PerformOperationRType(int instrType)
{
	instruction = "Perform the Operation";
	currentStep = 3;
	Aoe = 1;	S2op = 0;	Cload = 1;
	Boe = 1;	ALUop = instrType;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::PerformOperationIType(int instrType)
{
	instruction = "Perform the Operation";
	currentStep = 4;
	Aoe = 1;	S2op = 3;	Cload = 1;
	IRoeS2 = 1;	ALUop = instrType;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::CheckRS1(int reg, int instrType)
{
	instruction = "Check if RS1 Equals Zero";
	currentStep = 5;
	if (reg != 0) { ZFlag = 0; }
	if (reg == 0) { ZFlag = 1; }
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::UpdatePCOffset(int instrType)
{
	instruction = "Update the PC with the Offset";
	currentStep = 6;
	PCoeS1 = 1;	S2op = 5;	PCload = 1;
	IRoeS2 = 1;	ALUop = 0;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::FindMAR(int instrType)
{
	instruction = "Find MAR";
	currentStep = 7;
	MARload = 1;	ALUop = 0;	S2op = 3;
	Aoe = 1;	IRoeS2 = 1;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::UpdatePCIR(int instrType)
{
	instruction = "Update the PC with IR";
	currentStep = 8;
	PCoeS1 = 1;	ALUop = 0;	S2op = 3;
	PCload = 1; IRoeS2 = 1;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::FindMDRStore(int instrType)
{
	instruction = "Find MDR";
	currentStep = 9;
	Boe = 1;	MDRload = 1;
	ALUop = 3;	S2op = 0;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::FindMDRLoad(int instrType)
{
	instruction = "Find MDR";
	currentStep = 10;
	PCMARselect = 1;	MemRead = 1;
	MDRload = 1;	MemOp = getMemOp(instrType);
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::WriteToMem(int instrType)
{
	instruction = "Save to Memory";
	currentStep = 11;
	PCMARselect = 1;	MemOp = getMemOp(instrType);
	MemWrite = 1;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::FindC(int instrType)
{
	instruction = "Find C";
	currentStep = 12;
	MDRoeS2 = 1;	ALUop = 3;
	Cload = 1;	S2op = 1;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
void StateMachine::SaveToReg(int instrType)
{
	instruction = "Save to Register";
	currentStep = 13;
	REGselect = getREGselect(instrType);
	REGload = 1;
	steps(instrType);
}
//---------------------------------------------------------------------------------------
int StateMachine::getALUop(int instrType)
{
	switch (instrType) {
	case Add: ALUop = 0; break;
	case Addi: ALUop = 0; break;
	case And: ALUop = 4; break;
	case Or: ALUop = 5; break;
	case Ori: ALUop = 5; break;
	case Sll: ALUop = 8; break;
	case Srl: ALUop = 10; break;
	case Sub: ALUop = 1; break;
	}
	return ALUop;
}
//---------------------------------------------------------------------------------------
int StateMachine::getMemOp(int instrType)
{
	switch (instrType) {
	case Lb: MemOp = 2; break;
	case Lh: MemOp = 1; break;
	case Lw: MemOp = 0; break;
	case Sb: MemOp = 2; break;
	case Sh: MemOp = 1; break;
	case Sw: MemOp = 0; break;
	}
	return MemOp;
}
//---------------------------------------------------------------------------------------
int StateMachine::getREGselect(int instrType)
{
	if (instrType > 100) {
		REGselect = 0;
	}
	else {
		REGselect = 1;
	}
	return REGselect;
}