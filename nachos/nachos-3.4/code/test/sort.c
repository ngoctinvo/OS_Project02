#include "syscall.h"

int A[100];	/* size of physical memory; with code, we'll run out of space!*/

int
main()
{
	int i, j, tmp,z,n;
	PrintString("\nCHUONG TRINH SORT");
	PrintString("\n-------------------------------------------");
	PrintString("\nNhap so nguyen n - so phan tu cua mang (<= 100 ) : ");
	n=ReadNum();
//kiem tra n hop le	
	if (n>100)
	{
		PrintString("\nNhap qua gioi han cho phep \n");
		Halt();
	}
//kiem tra n hop le
	if (n<1)
	{
		PrintString("\nNhap nho hon gioi han cho phep \n");
		Halt();
	}

	for (i=0;i<n;i++)
	{
		PrintString("Nhap A[");
		PrintNum(i);
		PrintString("] = ");
		A[i]=ReadNum();
	}

	PrintString("\nSap xep mang theo thu tu tang dan (nhap 1) hay giam dan (nhap 0)?:  ");
	z=ReadNum();

	if (z==1){
		for (i=0;i<n-1;i++){
			for(j=0;j<n-i-1;j++){
				if (A[j] > A[j+1]){
					tmp=A[j];
					A[j]=A[j+1];
					A[j+1]=tmp;
				}
			}
		}
	}
	else{
		for (i=0;i<n-1;i++){
			for(j=i+1;j<n;j++){
				if (A[i] < A[j]){
					tmp=A[j];
					A[j]=A[i];
					A[i]=tmp;
				}
			}
		}

	}
	
	PrintString("\nMang sau khi sap xep: \n ");
	for (i=0;i<n;i++){
		PrintNum(A[i]);
		PrintString("  ");
	}	
	PrintString("\n");
	Halt();
}

	
