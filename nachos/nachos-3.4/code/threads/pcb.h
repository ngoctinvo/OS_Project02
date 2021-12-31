#ifndef PCB_H
#define PCB_H
#include "thread.h"
#include "synch.h"

class PCB
{
private:
    Semaphore *joinsem;         // semaphore cho quá trình join
    Semaphore *exitsem;         // semaphore cho quá trình exit
    Semaphore *multex;          // semaphore cho quá trình truy xuất độc quyền

    int exitcode;
    int numwait;                // số tiến trình đã join

    Thread *thread;

    char PName[50];             // lưu tên của tiến trình
    int pID;                    // lưu ID của tiến trình

public:
    int parentID;               // ID của tiến trình cha

    PCB(int id); 
    ~PCB(); 


    int Exec(char *filename, int pid); // Tạo 1 thread mới có tên là filename và process là pid

    int GetID();                        // Trả về ProcessID của tiến trình gọi thực hiện
    int GetNumWait();                   // Trả về số lượng tiến trình chờ
    void JoinWait();                    // Tiến trình cha đợi tiến trình con kết thúc
    void ExitWait();                    // Tiến trình con kết thúc
    void JoinRelease();                 // Báo cho tiến trình cha thực thi tiếp
    void ExitRelease();                 // Cho phép tiến trình con kết thúc
    void IncNumWait();                  // Tăng số tiến trình chờ
    void DecNumWait();                  // Giảm số tiến trình chờ
    void SetExitCode(int ec);           // Đặt exitcode của tiến trình
    int GetExitCode();                  // Trả về exitcode 
    void SetFileName(char* fn);         // Đặt tên của tiến trình
    char* GetFileName();                // Trả về tên của tiến trình
};

#endif