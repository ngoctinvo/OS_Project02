#include "syscall.h"
#include "copyright.h"

int main(){
   int check;
   char* filename;
   PrintString("Please enter your filename to create: \n");
   ReadString(filename,100);
   check=CreateFile(filename);
   if (check==0)
	PrintString("Create file successfully! \n");
   else
	PrintString("Create file failed! \n");
   Halt();
}
