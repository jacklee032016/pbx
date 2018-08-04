/*
 * $Id: SipTransDebugger.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipTransDebugger.hxx"
#include <stdio.h>

using namespace Vocal;

Data
SipTransDebugger::printDebug(SipTransHashTable* table)
{
    //// lock the whole table, just to make life easier...
    table->rwLock.ReadLock();

    int level2count = 0;
    int level3count = 0;
    int resCount = 0;
    int reqCount = 0;

    for (unsigned int i=0;i<table->size;i++)
    {
	table->buckets[i].rwLock.ReadLock();
	SipTransHashTable::Node* currNode;
	currNode = table->buckets[i].first;
	for(;currNode;)
	{
	    currNode->rwLock.ReadLock();
	    SipTransLevel1Node * topNode = currNode->myNode;
	    if(topNode)
	    {
		topNode->lock.lock();
		SipTransactionList<SipTransLevel2Node*>::SipTransListNode *
		level2node = topNode->level2.getFirst();
		while(level2node)
		{
		    level2count++;
		    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
		    level3node = level2node->val->level3.getFirst();
		    while(level3node)
		    {
			level3count++;
			if(level3node->val->msgs.request)
			    reqCount++;
			if(level3node->val->msgs.response)
			    resCount++;
			level3node =
			    level2node->val->level3.getNext(level3node);
		    }
		    level2node = topNode->level2.getNext(level2node);
		}
		topNode->lock.unlock();
	    }
	    currNode->rwLock.Unlock();
	    currNode = currNode->next;
	}
	table->buckets[i].rwLock.Unlock();
    }
    table->rwLock.Unlock();
    char details[200];
    sprintf(details,"%s[%ld] : %s[%d] : %s[%d]\nTotal: %s[%d], %s[%d] %s[%ld]",
	    "top node count", table->count,
	    "level 2 count", level2count,
	    "level 3 count", level3count,
	    "requests", reqCount,
	    "responses", resCount,
	    "total buckets", table->size);
    return Data(details);
}
