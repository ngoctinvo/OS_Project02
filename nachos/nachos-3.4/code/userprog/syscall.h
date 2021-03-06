/* syscalls.h 
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel. 
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation 
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */
#define SC_Halt	             1			
#define SC_ReadNum	     2		
#define SC_PrintNum	     3			
#define SC_ReadChar	     4		
#define SC_PrintChar	     5		
#define SC_ReadString	     6		
#define SC_PrintString	     7	
#define SC_RandomNum	     8
				
#define SC_CreateFile	     9			
#define SC_Open	             10
#define SC_Close	     11			
#define SC_Read		     12			
#define SC_Write	     13			
				
#define SC_Exit		     14			
#define SC_Exec		     15			
#define SC_Join	             16
#define SC_CreateSemaphore   17	
#define SC_Wait		     18			
#define SC_Signal	     19
			
#ifndef IN_ASM
#define ConsoleInput	0  
#define ConsoleOutput	1  

void Halt();
//Dinh nghia kieu dl SpaceID va OpenFIleID la kieu nguyen		
typedef int SpaceID;	
typedef int OpenFileID;	

//Cac system da cai dat trong project 1
int ReadNum();
void PrintNum(int number);
char ReadChar();
void PrintChar(char character);
void ReadString(char *buffer, int length);
void PrintString(char *buffer);
int RandomNum();

// Phan 1: syscall nhap xuat file
int CreateFile(char *name);
OpenFileID Open(char *name, int type);
void Close(OpenFileID id);
int Write(char *buffer, int charcount, OpenFileID id);
int Read(char *buffer, int charcount, OpenFileID id);

//Phan 2: da chuong, lap lich va dong bo hoa trong nachos
SpaceID Exec(char *name);
int Join(SpaceID id); 
void Exit(int status);	
int CreateSemaphore(char *name, int semval);
int Wait(char *name);
int Signal(char *name);

//
int Seek(int pos, OpenFileID id);
void Fork(void (*func)());
void Yield();	


#endif /* IN_ASM */

#endif /* SYSCALL_H */
