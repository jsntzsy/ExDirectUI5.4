/**
 * @file thunk_window.h
 * @brief 窗口与对象消息挂接定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-25
 * @copyright
 */
#pragma once
#include <Windows.h>

namespace ExDirectUI
{

#pragma region ExThunkWindowData定义

	//ThunkWindow消息回调函数
	typedef LRESULT(CALLBACK* ExThunkWindowProc)(LPVOID ptr, UINT msg, WPARAM wparam, LPARAM lparam);

#if defined(_M_IX86)
	//x86编译
	
	// 按一字节对齐
#pragma pack(push, 1)
	struct ExThunkWindowData
	{
		// mov [esp+4], ptr
		DWORD asm_mov;
		DWORD asm_ptr;

		// jmp pfn_relative
		BYTE  asm_jmp;
		DWORD asm_proc_relative;

		//初始化
		void* Init(ExThunkWindowProc wnd_proc, LPVOID ptr)
		{
			asm_mov = 0x042444C7;
			asm_ptr = (DWORD)(ULONG_PTR)ptr;  // 回调函数第一个参数(hWnd)改放ptr
			asm_jmp = 0xE9;

			// 跳转到wnd_proc指定的入口函数
			asm_proc_relative = (DWORD)((INT_PTR)wnd_proc - ((INT_PTR)this + sizeof(ExThunkWindowData)));

			FlushInstructionCache(GetCurrentProcess(), this, sizeof(ExThunkWindowData));
			
			//将指针返回
			return this;
		}

		void* operator *() { return this; }
	};
#pragma pack(pop)

#elif defined(_M_AMD64)
	//AMD64编译

	//按2字节对齐
#pragma pack(push,2)
	struct ExThunkWindowData
	{
		// mov rcx, ptr
		USHORT  asm_RcxMov;
		ULONG64 asm_RcxImm;

		// mov rax, proc
		USHORT  asm_RaxMov;
		ULONG64 asm_RaxImm;

		// jmp rax
		USHORT  asm_RaxJmp;


		//初始化
		void* Init(ExThunkWindowProc wnd_proc, LPVOID ptr)
		{
			// mov rcx, ptr
			asm_RcxMov = 0xB948;
			asm_RcxImm = (ULONG64)ptr;

			// mov rax, proc
			asm_RaxMov = 0xB848;
			asm_RaxImm = (ULONG64)wnd_proc;

			//jump rax
			asm_RaxJmp = 0xE0FF;

			FlushInstructionCache(GetCurrentProcess(), this, sizeof(ExThunkWindowData));
			return this;
		}

		void* operator *() { return this; }
	};
#pragma pack(pop)

#elif defined(_ARM_)
	//ARM编译

	//以4字节对齐
#pragma pack(push,4)

	struct ExThunkWindowData
	{
		// mov    r0, ptr
		DWORD    asm_mov_r0;

		// mov    pc, proc
		DWORD    asm_mov_pc;

		DWORD    asm_ptr;
		DWORD    asm_proc;

		//初始化
		void* Init(ExThunkWindowProc wnd_proc, LPVOID ptr)
		{
			asm_mov_r0 = 0xE59F0000;
			asm_mov_pc = 0xE59FF000;
			asm_ptr = (DWORD)ptr;
			asm_proc = (DWORD)wnd_proc;

			FlushInstructionCache(GetCurrentProcess(), this, sizeof(ExThunkWindowData));
			return this;
		}

		void* operator *() { return this; }
	};
#pragma pack(pop)

#else

//不支持其他编译形式
#error 仅支持x86/AMD64/ARM编译形式
#endif

#pragma endregion

	inline void* ExThunkWindow(ExThunkWindowProc wnd_proc, LPVOID ptr)
	{
		ExThunkWindowData* thunk_data = (ExThunkWindowData*)VirtualAlloc(
			nullptr, sizeof(ExThunkWindowData), MEM_COMMIT,
			PAGE_EXECUTE_READWRITE
		);

		return thunk_data ? thunk_data->Init(wnd_proc, ptr) : nullptr;
	}

}