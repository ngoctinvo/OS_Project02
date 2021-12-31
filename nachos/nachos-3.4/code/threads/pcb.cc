#include "pcb.h"
#include "system.h"

PCB::PCB(int id) {
    joinsem = new Semaphore("join", 0);
    exitsem = new Semaphore("exit", 0);
    multex = new Semaphore("multex", 1);

    exitcode = numwait = 0;
    parentID = id;

    thread = NULL;

    if (id == -1)
    {
        pID = 0;
        thread = currentThread;
    }
    
}

PCB::~PCB() {
    if(joinsem != NULL)
        delete joinsem;
	if(exitsem != NULL)
        delete exitsem;
   	if(multex != NULL)
        delete multex;
    if (thread->space != NULL)
        delete thread->space;
}

void StartProcess_2(int pointer)
{
    Thread *thread = (Thread*) pointer;
    
    AddrSpace *space = new AddrSpace(thread->getName());

    if(space == NULL)
	{
		printf("Can't create AddSpace.");
		return;
	}

    thread->space = space;

    space->InitRegisters();		// Đặt giá trị thanh ghi ban đầu
    space->RestoreState();		// Tải page table register

    scheduler->Run(currentThread);
    scheduler->Run(thread);

    machine->Run();
}

int PCB::Exec(char *filename, int pid) 
{
    multex->P();                // ngăn 2 tiến trình vào cùng lúc

    if (thread != NULL || filename == NULL || pid < 0) 
    {
        multex->V();
        return -1;
    }

    // Kiểm tra khởi tạo
    thread = new Thread(filename);
    if (thread == NULL) 
    {
        printf("Not enough memory!!!\n");
        multex->V();
        return -1;
    }
    
	// Gọi thực thi Fork(StartProcess_2,id) => Ta cast thread thành kiểu int, sau đó khi xử ký hàm StartProcess ta cast Thread về đúng kiểu của nó.
    thread->Fork(StartProcess_2, (int)thread);

    this->pID = pid;

    multex->V();

    return pid;
}

int PCB::GetID() 
{
    return pID;
}

int PCB::GetNumWait() 
{
    return numwait;
}

void PCB::JoinWait()
{
    joinsem->P();       // mục đích để tiến trình ngừng lại, chờ gọi JoinRelease để thực hiện tiếp
}

void PCB::ExitWait() 
{ 
    exitsem->P();       // giải phóng tiến trình đang chờ
}

void PCB::JoinRelease()
{
    joinsem->V();       // giải phóng tiến trình gọi JoinWait
}

void PCB::ExitRelease() 
{
    exitsem->V();       // giải phóng tiến trình đang chờ
}

void PCB::IncNumWait()
{
    ++numwait;
}

void PCB::DecNumWait()
{
    --numwait;
}

void PCB::SetExitCode(int ec) 
{
    exitcode = ec;
}

int PCB::GetExitCode()
{
    return exitcode;
}

char *PCB::GetFileName() 
{
    return PName;
}

void PCB::SetFileName(char* fn)
{ 
    strcpy(PName,fn);
}
