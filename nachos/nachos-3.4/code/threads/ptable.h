#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"

#define MAXPROCESS 10

class PTable
{
private:
    BitMap *bm;                 // đánh dấu vị trí được sử dụng
    PCB *pcb[MAXPROCESS];       
    int psize;
    Semaphore *bmsem;           // ngăn 2 tiến trình đồng thời

public:
    PTable(int size);

    ~PTable();

    int ExecUpdate(char *name);             // Xử lý cho system call SC_Exit

    int ExitUpdate(int ec);                 // Xử lý cho system call SC_Exit

    int JoinUpdate(int id);                 // Xử lý cho system call SC_Join

    int GetFreeSlot();                      // Tìm free slot để lưu thông tin cho tiến trình mới

    bool IsExist(int pid);                  // Kiểm tra tồn tại processID
        
    void Remove(int pid);                   // Khi tiến trình kết thúc, delete processID ra khỏi mảng quản lý nó

    char *GetFileName(int id);              // Trả về tên của tiến trình
};

#endif