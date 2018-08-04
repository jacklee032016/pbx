/*
* $Id: MemoryManager.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "MemoryManager.hxx"
MemoryManager* MemoryManager::mInstance = 0;    
    
    
MemoryManager::MemoryManager()
{
    mAllocList = NULL;
    mFreeList = NULL;
    mAvail = false;
}
    

MemoryManager* MemoryManager::getInstance()
{
    if (mInstance == 0)
    {
        mInstance = new MemoryManager();
       
        
    }
    return mInstance;
}
    
void* MemoryManager::alloc(size_t size)
{
    HEADER* p;
 //calculate the number of HEADER sizes we need to fit size bytes.
    //one more for the HEADER itself.
    int nUnits = 1+ (size + sizeof(HEADER) )/ sizeof(HEADER);
    
    if (mAllocList == NULL)
    {
        //no allocation done yet.
        mAllocList = (HEADER*)(moreCore(nUnits));
    }
    p = mAllocList;
    
    //allocate size blocks from mAllocList.
    if (p->size > nUnits)
    {
        p->size -= nUnits;

        HEADER* q = p+p->size;
        q->size = nUnits;
        q->next = p->next;

        p->next = q;
        p = q;
        
    }
    else if (p->size == nUnits)
    {
        if (p == mAllocList)
        {
            mAvail = false;

        }
    }
    else  //p.size < nUnits
    {
        //check out free list , or allocate more.
        p = getFromFreeList(nUnits);

    }
    return (void*) (p+1);
}
    
    
HEADER* MemoryManager::getFromFreeList(int nBlocks)
{
    //detach from free list and attach to alloc list.
    if (mFreeList != NULL)
    {
       HEADER* prev = NULL;
       HEADER* current = mFreeList;
       while (current != NULL)
       {
           //find one that fits nBlocks.
           if (current->size >= nBlocks)
           {
	       if (current->size > nBlocks)
               {
		   //form the correct size
                    current->size -= nBlocks;

		    HEADER* q = current+current->size;
		    q->size = nBlocks;
		    q->next = current->next;
		    
		    current->next = q;
		    current = q;
               }
               //detach current from freeList and attach to allocList
               if (prev != NULL)
               {
                   prev->next = current->next;
               }
               else //this is the first item.
               {
                   mFreeList = current->next;
               }
	       //attach to allocList.
	       current->next = mAllocList->next;
               mAllocList->next = current;
               return ( (HEADER*)(current) );
           }
           prev = current;
           current = current->next;
       }
       //came here. So, not found a big enough block.
       //allocate more.
       current = moreCore(nBlocks);
       return ((HEADER*) (current));
    }
    else // no free list.
    {
        HEADER* p;
        //allocate more.
        p = moreCore(nBlocks);
        return ( (HEADER*) (p) );
    }
}
    
    
HEADER* MemoryManager::moreCore(int nBlocks)
{
    //get a block of memory.
    int numChunks = ALLOC_BLOCK * ( (ALLOC_BLOCK + nBlocks -1)/ALLOC_BLOCK);
    int nBytes = numChunks * sizeof(HEADER);
    
    HEADER* p = (HEADER*)(malloc(nBytes));  //calling system new.
    
    //attach to mAllocList.
    if (mAllocList != NULL)
    {
        if (mAvail) //available, but small, then coalesce them.
        {
            p->next = mAllocList->next;
            p->size = numChunks+mAllocList->size;
        }
        else
        {
            p->next = mAllocList;
            p->size = numChunks;
        }
    }
    else
    {
        p->next = NULL;
        p->size = numChunks;
    }
    mAvail = true;
    mAllocList = p;

    //return the correct block size.
    if (p->size > nBlocks)
    {

        p->size -= nBlocks;

        HEADER* q = p+p->size;
        q->size = nBlocks;
        q->next = p->next;

        p->next = q;


        return q;
    }
    else if (p->size == nBlocks)
    {
        return p;
    }
    else
    {
        //should not come here.
        return NULL;
    }

}


bool MemoryManager::dealloc(void* ptr)
{
    //detach from allocList, and attach to freeList.
   
    //get the HEADER ptr.
    HEADER* removeItem = (HEADER*)(ptr)-1; 
    
    if (removeItem == NULL)
    {
        return false;
    }
    HEADER* prev = NULL;
    HEADER* current = mAllocList;

    while ( (current != NULL) && (current != removeItem) )
    {
        prev = current;
        current = current->next;
    }
    if (current == NULL)
    {
	return false;  //item not a valid ptr.
    }
    if (current != mAllocList)
    {
        //have some value in prev.
        prev->next = current->next;
    }
    else
    {
        mAllocList = mAllocList->next;
    }

    current->next = mFreeList;
    mFreeList = current;
    
    return true;
}
    
void* MemoryManager::mynew(size_t size)
{
    MemoryManager* manager = MemoryManager::getInstance();
    void* p = manager->alloc(size);
    //cout << "in my new" << endl;
   // cout << "size:" << size << endl;
    return p;
}
    
void* MemoryManager::mynew(size_t size, void* type)
{
    MemoryManager* manager = MemoryManager::getInstance();
    void* p = manager->alloc(sizeof(type)*size);
    //cout << "in my new" << endl;
    //cout << "size:" << size << endl;
    
    return p;
}    


void MemoryManager::mydelete(void* p, void* type)
{
    mydelete(p);
}

#if 0
void* MemoryManager::mynew[](size_t size, void* type)
{
    MemoryManager* manager = MemoryManager::getInstance();
    void* p = manager->alloc(sizeof(type)*size);
    //cout << "in my new" << endl;
    return p;     
}


void MemoryManager::mydelete[](void* p, void* type)
{
    mydelete(p);
}
#endif
    
void MemoryManager::mydelete(void* p)
{
    //cout << "in my delete" << endl;

    MemoryManager* manager = MemoryManager::getInstance();
    if (p != NULL)
    {
        manager->dealloc(p);
     //dealloc(p);
    }
}


void MemoryManager::displayAllocList()
{
    cout << "Displaying size of ALLOC LIST: "<< endl;
    MemoryManager* manager = MemoryManager::getInstance();

    HEADER* p = manager->mAllocList;
    while (p != NULL)
    {
        cout << "size:" << p->size << endl;
        p = p->next; 
    }
}
 


void MemoryManager::displayFreeList()
{
    cout << "Displaying size of FREE LIST:" << endl;
    MemoryManager* manager = MemoryManager::getInstance();

    HEADER* p = manager->mFreeList;
    while (p != NULL)
    {
        cout << "size:" << p->size << endl;
        p = p->next;
    }
}


