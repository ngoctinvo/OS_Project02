#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "sysdep.h"

char *User2System(int virtAddr, int limit) {
    int i;
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit+1];
    if (kernelBuf==NULL) return kernelBuf;
    memset(kernelBuf,0,limit+1);
    for (i=0;i<limit;i++) {
        machine->ReadMem(virtAddr+i,1,&oneChar);
        kernelBuf[i]=(char)oneChar;
        if (oneChar==0) break;
    }
    return kernelBuf;
}


int System2User(int  virtAddr,int len,char *buffer) {
    if (len < 0) return -1;
    if (len == 0 ) return 0;
    int i=0;
    int oneChar=0;
    do {
        oneChar=(int)buffer[i];
        machine->WriteMem(virtAddr+i,1,oneChar);
        i++;
    } while(i < len && oneChar!=0);
    return i;
}

void IncreasePC(){
	machine->registers[PrevPCReg]=machine->registers[PCReg];
	machine->registers[PCReg]=machine->registers[NextPCReg];
	machine->registers[NextPCReg]+=4;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which) {
        case NoException:
            printf("Everything ok!\n");
            return;
        case SyscallException:
            switch (type) {
                case SC_Halt:
                    DEBUG('a', "Shutdown, initiated by user program.\n");
                    printf("Shutdown, initiated by user program.\n");
                    interrupt->Halt();
                    break;
                case SC_RandomNum: {
                    int ranNum=Random();
		    machine->WriteRegister(2,ranNum);
                    break;
                }
        
                case SC_ReadNum: {
                    char *num = new char[10]; 
                    int leng = gSynchConsole->Read(num, 9); 
                    int ans = 0; 
                    if (leng==-1) { 
                        interrupt->Halt();
                        return;
                    }
                    if (leng==0) break; 
                    int i = (num[0]=='-')?1:0; 
                    for (; i < leng; i++) 
                        if (num[i]>='0' && num[i]<='9')
                            ans = ans * 10 + (int)(num[i] - '0');
                        else { 
                            printf("Input must be number\n");
                            printf("Shutdown, initiated by user program.\n");
                            interrupt->Halt();
                            return;
                        }
                    if (num[0]=='-') ans = -ans;
                    machine->WriteRegister(2, ans); 
                    delete[] num;
                    break;
                }
                case SC_PrintNum: {
                    int num = machine->ReadRegister(4); 
                    char *out = new char[10]; 
                    memset(out,0,10);
                    if (num == 0) { 
                        out[0]='0';
                        gSynchConsole->Write(out,1); 
                    }
                    else {
                        int leng = 0;
                        int sign = (num>0)?1:-1; 
                        num = num * sign;
                        while (num > 0) { 
                            out[leng++]=(char)(num%10+'0');
                            num/=10;
                        }
                        if (sign == -1) out[leng++]='-'; 
                        char swap; 
                        for (int i = 0; i < leng / 2; i++) {
                            swap = out[i];
                            out[i]=out[leng-i-1];
                            out[leng-i-1]=swap;
                        }
                        gSynchConsole->Write(out,leng);
                    }
                    delete[] out;
                    break;
                }
                case SC_ReadChar: {
                    char character; 
                    int ok = gSynchConsole->Read(&character, 1); 
                    if (ok==-1) { 
                        interrupt->Halt();
                        return;
                    }
                    if (ok==0) break; 
                    machine->WriteRegister(2, (int)character); 
                    break;
                }
                case SC_PrintChar: {
                    char character = (char)machine->ReadRegister(4); 
                    gSynchConsole->Write(&character, 1); 
                    break;
                }
                case SC_ReadString: {
                    int virtAddr = machine->ReadRegister(4); 
                    int leng = machine->ReadRegister(5); 
                    if (leng <= 0) { 
                        interrupt->Halt();
                        return;
                    }
                    char *string = new char[leng+1]; 
                    int ok = gSynchConsole->Read(string, leng); 
                    if (ok==-1) { 
                        interrupt->Halt();
                        return;
                    }
                    if (ok==0) break; 
                    string[ok]='\0'; 
                    System2User(virtAddr, leng+1, string); 
                    delete[] string;
                    break;
                }
                case SC_PrintString: {
                    int virtAddr = machine->ReadRegister(4); 
                    char *string = User2System(virtAddr, 255);
                    gSynchConsole->Write(string, 255); 
                    delete[] string;
                    break;
                }

                case SC_CreateFile: {
		//Doc tham so name tu thanh ghi r4
                    int addr = machine->ReadRegister(4);
                    char *name = new char[256];
                    name = User2System(addr, 255);
		//Neu ten file khong hop le -> bao loi, tra ve -1
		    if (name==NULL || strlen(name)==0)
                    {
                       printf("File name is incorrect! \n");
                       machine->WriteRegister(2,-1);
			delete[] name;
			break;
                     
                    }
		// Neu tao file that bai -> bao loi va tra ve -1
                    if (fileSystem->Create(name, 0) == false) {
			printf("Create file failed! \n");
                        machine->WriteRegister(2, -1);
			
                    }
			// Tao file thanhh cong -> tra ve 0
                    else {
                        machine->WriteRegister(2, 0);
                    }
                    delete []name;
                    break;
                }
                case SC_Open: {
		//Doc tham so name tu r4, chuyen tu user space-> system space
                    int addr = machine->ReadRegister(4);
                    char *name = User2System(addr, 255);
		//Neu name nhap khong hop le -> bao loi, tra ve -1
                    if (name==NULL)
                    {
                        printf("Name of file is incorrect! \n");
                        machine->WriteRegister(2,-1);
			delete[] name;
                        break;
                    }
		//Doc tham so type tu thanh ghi r5
                    int type_file = machine->ReadRegister(5);
		//nhap loai file khong hop le -> bao loi va tra ve -1
		//type=0 doc&ghi, type=1 chi doc
                    if (type_file != 0 && type_file != 1) {
                        printf("Type file error, only type=0 for read&write or type=1 for read! \n");
                        machine->WriteRegister(2, - 1);
			//delete[] name;
                        break;
                    }

		// o 0 de danh cho console input
                    if (strcmp(name, "stdin") == 0) {
                        machine->WriteRegister(2, 0);
                        delete[] name;
                        break;
                    }
		// o 1 de danh cho console output
                    if (strcmp(name, "stdout") == 0) {
                        machine->WriteRegister(2, 1);
                        delete[] name;
                        break;
                    }
		//Kiem tra xem tai index nao file la NULL, neu ko tim duoc -> bao loi, tra ve -1
                    int index = -1;
                    for (int i = 0; i < 10; i++)
                        if (fileSystem->file[i] == NULL) {
                            index = i;
                            break;
                        }
                    if (index == -1) {
                        machine->WriteRegister(2, - 1);
                        delete[] name;
                        break;
                    }
		//Dung fileSystem de mo file
                    fileSystem->file[index] = fileSystem->Open(name, type_file);
                    
		// Neu file da mo khac Null -> thanh cong
	 	    if (fileSystem->file[index] != NULL)
                        machine->WriteRegister(2, index);
		//Neu file da mo ==null -> that bai, bao loi - tra ve -1
                    else {
			printf("Open file failed! \n");
			machine->WriteRegister(2, - 1);
			}
                    delete[] name;
                    break;
                }
      	        case SC_Close: {
		//Doc tham so id tu thanh ghi r4
                    int id = machine->ReadRegister(4);
		//Kiem tra id file hop le hay ko -> bao loi, tra ve -1
                    if (id < 2 || id > 9) {
			printf("File ID is incorrect! \n");
                        machine->WriteRegister(2, - 1);
                    }
                    else {
		//Neu dong file thanh cong -> tra ve 0
                        if (fileSystem->file[id] != NULL) {
                            delete fileSystem->file[id];
                            fileSystem->file[id] = NULL;
                            machine->WriteRegister(2, 0);
                        } 
		//neu dong file  that bai -> bao loi, tra ve -1
                        else {
			    printf("CLose file failed! \n");
			    machine->WriteRegister(2, - 1);
}
                    }
                    break;
                }

           case SC_Read: {
		//Doc tham so id tu thanh ghi r6
                    int id = machine->ReadRegister(6);
		//Kiem tra id file co hop le khong -> neu khong bao loi tra ve -1
                    if (id < 0 || id == 1 || id > 9 || fileSystem->file[id] == NULL) {
			printf("ID file to read is incorrect! \n");
                        machine->WriteRegister(2, - 1);
                        break;
                    }
                    int addr = machine->ReadRegister(4);
                    int charcount = machine->ReadRegister(5);
		//Neu charcount nhap vao ko hop le -> bao loi, tra ve -1
                    if (charcount <= 0) {
			printf("Char count of file is incorrect! \n");
                        machine->WriteRegister(2, - 1);
                        break;
                    }
                    char *buff = new char[charcount + 1];
                    int result;
                    if (id == 0) 
			result = gSynchConsole->Read(buff, charcount);
                    else 
			result = fileSystem->file[id]->Read(buff, charcount);
                //Neu doc file that bai -> bao loi, tra ve -1
		    if (result < 0) {
			printf("Read file failed! \n");
                        machine->WriteRegister(2, - 1);
                        delete[] buff;
                        break;
                    }
		//Neu doc toi cuoi file -> tra ve -2
                    else if (result == 0) {
                        machine->WriteRegister(2, - 2);
                        delete[] buff;
                        break;
                    }
		//Ket thuc chuoi bang null
                    buff[result]='\0';
                    System2User(addr, charcount+1, buff); 
                    machine->WriteRegister(2, result);
                    delete[] buff;
                    break;
                }
                case SC_Write: {
                    int id = machine->ReadRegister(6);
		//Kiem tra id file co hop le khong -> neu khong bao loi tra ve -1
                    if (id < 0 || id == 0 || id > 9 || fileSystem->file[id] == NULL || fileSystem->file[id]->type == 1) {
			printf("ID file to read is incorrect! \n");
                        machine->WriteRegister(2, - 1);
                        break;
                    }
                    int addr = machine->ReadRegister(4);
                    int charcount = machine->ReadRegister(5);
		//Neu charcount nhap vao ko hop le -> bao loi, tra ve -1
                    if (charcount <= 0) {
			printf("Char count of file is incorrect! \n");
                        machine->WriteRegister(2, - 1);
                        break;
                    }
                    char *buff = User2System(addr, charcount);
                    int result;
                    if (id == 1) 
			result = gSynchConsole->Write(buff, charcount);
                    else 
			result = fileSystem->file[id]->Write(buff, charcount);
                //Neu doc file that bai -> bao loi, tra ve -1
		    if (result < 0) {
			printf("Read file failed! \n");
                        machine->WriteRegister(2, - 1);
                        delete[] buff;
                        break;
                    }
		//Neu ghi toi cuoi file -> tra ve -2
                    else if (result == 0) {
                        machine->WriteRegister(2, - 2);
                        delete[] buff;
                        break;
                    }
                    machine->WriteRegister(2, result);
                    delete[] buff;
                    break;
                }
       
              
                case SC_Exec: {
			//Doc tham so name tu thanh ghi r4
			// tu user space -> system space
                    int addr = machine->ReadRegister(4);
                    char *name = new char[256];
                    name = User2System(addr, 255);

		  //Neu name == NULL -> bao loi, tra ve -1
		    if (name == NULL) 
		    {
		    printf("Name is incorrect! \n");
		    machine->WriteRegister(2,-1);
		       break;	
		    }

			OpenFile* openfile=fileSystem->Open(name);
		 //Neu mo file khong duoc -> bao loi, tra ve -1		    
		    if (openfile==NULL)
		    {
		    printf("Cannot open this file! \n");
		    machine->WriteRegister(2,-1);
		       break;
		    }
		    delete openfile;
		//Neu tat ca dieu kien thoa man, goi ham execUpdate
		//THanh cong -> tra ve id
                    int id = pTab->ExecUpdate(name);
                    machine->WriteRegister(2, id);
                    //delete[] name;
		    break;
                }
                case SC_Join: {
		//Lay tham so id tu thanh ghi r4
                    int id = machine->ReadRegister(4);
		// SU dung pthuc JOinUpdate -> tra ve ket qua 
                    int result = pTab->JoinUpdate(id);
                    machine->WriteRegister(2, result);
		    break;

                }

  		case SC_Exit: {
		//Lay tham so exitCode tu thanh ghi r4
                    int exitcode = machine->ReadRegister(4);
		//Neu exitcode = 0 la chuowng trinh thanh cong, nguoc lai thi co loi

		//Sdung phthuc ExitUpdate va tra ket qua ve thanh ghi r2
                    int result = pTab->ExitUpdate(exitcode);
                    machine->WriteRegister(2, result);
		    break;
                }
                case SC_CreateSemaphore: {
		//Doc tham so name
                    int addr = machine->ReadRegister(4);
                    char *name = new char[256];
                    name = User2System(addr, 255);
		//Doc tham so semval
                    int semval = machine->ReadRegister(5);

		//Kiem tra neu name khong hop le -> bao loi, tra ve -1
                    if (name == NULL)
                    {
                       printf("Name is incorrect! \n");
                       machine->WriteRegister(2,-1);
                       delete[] name;
		       break;
                    }
		//Sdung phuong thuc semTab->Create
                    int result = semTab->Create(name, semval);
		//Neu tao khong thanh cong -> bao loi , tra ve -1
                    if (result==-1)
                    {
                       machine->WriteRegister(2,-1);
                       delete[] name;
		       break;
                    }
		//Neu thanh cong, tra ve result
		    delete[] name;
                    machine->WriteRegister(2, result);
                     break;
                }
                case SC_Wait: {
		//Doc tham so name, chuyen tu user space sang system space
                    int addr = machine->ReadRegister(4);
                    char *name = new char[256];
                    name = User2System(addr, 255);
		//Kiem tra name co hop le hay khong, neu khong -> bao loi , tra ve -1
		if (name == NULL){
                       printf("Name is incorrect! \n");
                       machine->WriteRegister(2,-1);
                       delete[] name;
                       break;
                    }
		//Su dung phuong thuc semTab->Wait
       
                    int result = semTab->Wait(name);
		//Neu khong ton tai semaphore -> bao loi, tra ve -1
                    if (result == -1) {
                       machine->WriteRegister(2,-1);
                       delete[] name;
		     break;
                    }
                    delete[] name;
		//Neu tat ca deu hop le, tra ve result
                    machine->WriteRegister(2, result);
                     break;
                }
                case SC_Signal: {
		//Doc tham so name tu thanh ghi r4
                    int addr = machine->ReadRegister(4);
                    char *name = new char[256];
                    name = User2System(addr, 255);

		//Kiem tra name co hop le hay khong, neu khong -> bao loi , tra ve -1
		if (name == NULL){
                       printf("Name is incorrect! \n");
                       machine->WriteRegister(2,-1);
                       delete[] name;
                       break;
                    }
		//Su dung phuong thuc semTab->Signal
       
                    int result = semTab->Signal(name);
		//Neu khong ton tai semaphore -> bao loi, tra ve -1
                    if (result == -1) {
                       machine->WriteRegister(2,-1);
                       delete[] name;
                       break;  
                    }
		    delete[] name;
		//Thanh cong, tra ve result
                    machine->WriteRegister(2, result);
                     break;
                }
                default:
                    printf("Unexpected user mode exception %d %d\n", which, type);
	            ASSERT(FALSE);
            }
            IncreasePC();
            break;
        case PageFaultException:
            printf("No valid translation found\n");
            ASSERT(FALSE);
            interrupt->Halt();
            break;
        case ReadOnlyException:
            printf("Write attempted to page marked  \"read-only\"\n");
            ASSERT(FALSE);
            interrupt->Halt();
            break;
        case  BusErrorException:
            printf("Translation resulted in an invalid physical address\n");
            ASSERT(FALSE);
            interrupt->Halt();
            break;
        case AddressErrorException:
            printf("Unaligned reference or one that was beyond the end of the address space\n");
            ASSERT(FALSE);
            interrupt->Halt();
            break;
        case OverflowException:
            printf("Integer overflow in add or sub.\n");
            ASSERT(FALSE);
            interrupt->Halt();
            break;
        case IllegalInstrException:
            printf("Unimplemented or reserved instr.\n");
            ASSERT(FALSE);
            interrupt->Halt();
            break;
        case NumExceptionTypes:
            printf("NumExceptionTypes\n");
            ASSERT(FALSE);
            interrupt->Halt();
            break;
        default:
            printf("Unexpected user mode exception %d %d\n", which, type);
            ASSERT(FALSE);
    }
};
