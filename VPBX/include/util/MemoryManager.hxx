#ifndef MEMORYMANAGER_HXX_
#define MEMORYMANAGER_HXX_
/*
* $Id: MemoryManager.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
    
#include <cstddef>
#include <iostream>
#include <cstdlib>
    
#define ALLOC_BLOCK 128

typedef struct Header
{
        Header* next;
        int size;
}HEADER;    

    
class MemoryManager
{
    public:
        /// Default constructor
        static MemoryManager* getInstance();
        static void*  mynew(size_t size);
        static void*  mynew(size_t size, void* p);
        //static void*  mynew[](size_t size, void* p);
        static void   mydelete(void* p);
        static void   mydelete(void* p, void*);
        //static void   mydelete[](void* p, void*);
        
        static void displayAllocList();
        static void displayFreeList();
    protected:
        MemoryManager();
    private:
        static MemoryManager* mInstance;
        
       
    
        HEADER* mAllocList;
        HEADER* mFreeList;
        bool mAvail; //if the first block is available or not.
    
    
        ///memory management functions
        void* alloc(size_t size);
        bool dealloc(void* ptr);
    
        HEADER* moreCore(int nBytes);
        HEADER* getFromFreeList(int nBlocks);
};
    
    
#if 0
void* MemoryManager::mynew(size_t size)
{
    
    MemoryManager* manager = MemoryManager::getInstance();
    void* p = manager->alloc(size);
    //void* p = alloc(size);
    cout << "in my new" << endl;
    return p;
    
    
    
#if 0
    void* p = malloc(size);
    cout << "in my new" << endl;
    return p;
#endif
    
    
}
    

void MemoryManager::mydelete(void* p) 
{

    MemoryManager* manager = MemoryManager::getInstance();
    if (p != NULL)
    {
        manager->dealloc(p);
     //dealloc(p);
    }
    
    
#if 0
    if (p != NULL)
    {
        free (p);
    }
#endif
}
#endif

#endif
