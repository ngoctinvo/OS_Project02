#include "syscall.h"
//chuong trinh in ra duoi dang "ky tu - ma ascii tuong ung"
int
main()
{	
	unsigned char c = (unsigned char)(0);
	while (c<= (unsigned char)255){
		PrintChar(c);
		PrintString(" - ");
		PrintNum((int)c);
		PrintString("\n");
		if (c==(unsigned char)(255)) 
			break;
		c++;
	}

	Halt();
}

	
