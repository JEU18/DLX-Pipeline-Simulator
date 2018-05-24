#include "stdafx.h"
#include <iostream>
#include "DataPath.hpp"
#include "StateMachine.hpp"
using namespace std;

int main()
{
	DataPath dataPath;
	StateMachine stateMachine;
	unsigned char MainMemory[5000] = {};
	char fileName[] = "Input.txt";
	dataPath.LoadProgramCode(fileName, MainMemory);
	return 0;
}