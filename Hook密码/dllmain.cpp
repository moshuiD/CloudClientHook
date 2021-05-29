// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include<Windows.h>
#include<iostream>
#include<fstream>
using namespace std;
void Hook(uintptr_t addr, void* FucAddr);
void Main();
int i{ 0 };
int lastI{ 0 };
int a{ 0 };
BYTE orgCode[5]{ 0 };
uintptr_t jmpBackAddr;
void _declspec(naked) MyFuc()
{
	_asm
	{
		mov[esi + 04], edx
		mov eax, [esp + 10]
		inc i
		mov a, edx
		jmp jmpBackAddr
	}
}
ofstream file;
void Main()
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	uintptr_t baseAddr = (uintptr_t)GetModuleHandleA("QtGui4.dll");
	Hook( baseAddr+ 0x87C6C, &MyFuc);
	jmpBackAddr = baseAddr + 0x87C73;
	file.open("output.txt", ios::out);
	while (1)
	{
		if (*(int*)&i != lastI)
		{
			lastI = *(int*)&i;
			cout << i << endl;
			cout << (char)a << endl;
			file << "第" << i << "次" << endl;
			if(a!=8)
			file << "写入的内容" << (char)a << endl;
			else
			{
				file << "写入了一个退格"<< endl;
			}
		}
		if (GetAsyncKeyState(VK_END) != 0)
			break;
	}
	memcpy((void*)(baseAddr+ 0x87C6C), orgCode, 5);
	file.close();
	FreeConsole();
	fclose(stdout);
}
void Hook(uintptr_t addr, void* FucAddr)
{
	DWORD dwOld = 0;
	VirtualProtect((LPVOID)addr, 5, PAGE_EXECUTE_READWRITE, &dwOld);
	memcpy(orgCode, (void*)addr, 5);
	BYTE code[5]{ 0xE9,0x00,0x00,0x00,0x00 };
	*(uintptr_t*)(code + 1) = (uintptr_t)(FucAddr)-addr - 5;
	memcpy((void*)addr, code, 5);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Main();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

