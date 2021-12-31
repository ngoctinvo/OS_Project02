#include "stable.h"

STable::STable() 
{
    bm = new BitMap(MAX_SEMAPHORE);

    for (int i = 0; i < MAX_SEMAPHORE; ++i)
        semTab[i] = NULL;
}

STable::~STable() 
{
    for (int i = 0; i < MAX_SEMAPHORE; ++i)
        if (semTab[i] != NULL) 
            delete semTab[i];
    
    if (bm != NULL) 
    delete bm;
}

int STable::Create(char *name, int init) 
{
    for (int i = 0; i < MAX_SEMAPHORE; ++i)
        if (semTab[i] != NULL && 
            strcmp(semTab[i]->GetName(), name) == 0)        
                 return -1;

    int i = bm->Find();     // Tìm slot trống
    if (i == -1) 
    {
        return -1;
    }

    bm->Mark(i);
    semTab[i] = new Sem(name, init);
    return i;
}

int STable::Wait(char *name) 
{
    for (int i = 0; i < MAX_SEMAPHORE; ++i)
        if (semTab[i] != NULL && 
            strcmp(semTab[i]->GetName(), name) == 0) 
            {
                semTab[i]->wait();
                return 0;
            }
    return -1;
}

int STable::Signal(char *name) 
{
    for (int i = 0; i < MAX_SEMAPHORE; ++i)
        if (semTab[i] != NULL && strcmp(semTab[i]->GetName(), name) == 0) 
        {
            semTab[i]->signal();
            return 0;
        }
    return -1;
}