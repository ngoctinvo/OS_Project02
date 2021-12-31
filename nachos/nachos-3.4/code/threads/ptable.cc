#include "ptable.h"
#include "system.h"

PTable::PTable(int size)
{
    if (size < 0) return;
    
    bm = new BitMap(MAXPROCESS);
    bmsem = new Semaphore("Bitmap semaphore", 1);
    psize = size;

    for (int i = 0; i < MAXPROCESS; ++i)
        pcb[i] = NULL;

    pcb[0] = new PCB(-1);
    pcb[0]->parentID = -1;

    bm->Mark(0);
}

PTable::~PTable() {
    for (int i = 0; i < MAXPROCESS; i++)
        if (pcb[i] != NULL) 
            delete pcb[i];

    if (bm != NULL) 
        delete bm;

    if (bmsem != NULL)
        delete bmsem;
}

int PTable::ExecUpdate(char *name) 
{
    bmsem->P();

    if (name == NULL)
    {
        bmsem->V();
        return -1;
    }

    //Kiểm tra sự tồn tại bằng phương thức Open của class fileSystem
    OpenFile *exec = fileSystem->Open(name);
    
    if (exec == NULL) {
        bmsem->V();
        return -1;
    }

    delete exec;

    //Kiểm tra xem nó có gọi chính nó hay không
    if (strcmp(currentThread->getName(), name) == NULL) 
    {
        bmsem->V();
        return -1;
    }
    
    // Tìm vị trí chưa được dùng
    int index = GetFreeSlot();

    if (index == -1) {
        bmsem->V();
        return -1;
    }

    int currentID = -1;
    for (int i = 0; i < MAXPROCESS; i++)
        if (pcb[i] != NULL && strcmp(currentThread->getName(), pcb[i]->GetFileName()) == 0)
            currentID = pcb[i]->GetID();
    
    if (currentID == -1) 
    {
        bmsem->V();
        return -1;
    }

    pcb[index] = new PCB(currentID);

    bm->Mark(index);
    int status = pcb[index]->Exec(name, index);

    // Kiểm tra việc thực thi
    if (status == -1) 
    {
        bm->Clear(index);
        delete pcb[index];
        bmsem->V();
        return -1;
    }
        
    bmsem->V();

    return index;
}

int PTable::ExitUpdate(int ec) {
    int index = -1;

    // Lấy ra id của tiến trình
    for (int i = 0; i < MAXPROCESS; i++)
        if (pcb[i] != NULL && strcmp(currentThread->getName(), pcb[i]->GetFileName()) == 0)
            index = i;

    if (index == -1) return -1;

    int pid = pcb[index]->GetID();
    
    if (pid == 0)               // Nếu tiến trình là main process thì gọi Halt()
    {
        interrupt->Halt();
        return 0;
    }

    int parentid = pcb[index]->parentID;

    pcb[index]->SetExitCode(ec);    // Đặt exitcode cho tiến trình gọi

    pcb[parentid]->JoinRelease();   // Giải phóng tiến trình cha đang đợi
    pcb[index]->ExitWait();

    Remove(pid);

    currentThread->Finish();
    return pid;
}

int PTable::JoinUpdate(int id) 
{
    // Kiểm tra tính hợp lệ của process id
    if (id < 0 || id >= MAXPROCESS)
        return -1;

    int currentID = -1;
    for (int i = 0; i < MAXPROCESS; i++)
        if (pcb[i] != NULL && strcmp(currentThread->getName(), pcb[i]->GetFileName()) == 0)
            currentID = pcb[i]->GetID();

    // Kiểm tra tiến trình gọi Join có phải là cha của tiến trình có processID là id
    if (pcb[id] == NULL || pcb[id]->parentID != currentID)
        return -1;
    
    pcb[currentID]->IncNumWait();   //Chờ tiến trình con thực hiện
    pcb[currentID]->JoinWait();

    int ec = pcb[id]->GetExitCode();    // Xử lý exitcode
    pcb[currentID]->DecNumWait();

    pcb[id]->ExitRelease();
    return ec;
}

int PTable::GetFreeSlot() 
{
    return bm->Find();
}

bool PTable::IsExist(int pid)
{
    if (pid < 0 || pid >= MAXPROCESS || pcb[pid] == NULL)
        return false;
    return true;
}

void PTable::Remove(int pid) 
{
    if (pid < 0 || pid >= MAXPROCESS || pcb[pid] == NULL)
        return;

    delete pcb[pid];
    pcb[pid] = NULL;
    
    bm->Clear(pid);
}

char *PTable::GetFileName(int id)
{
    if (id < 0 || id >= MAXPROCESS || pcb[id] == NULL)
        return NULL;
    return pcb[id]->GetFileName();
}