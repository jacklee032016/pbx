#ifndef _Sip_Transaction_List__hxx
#define _Sip_Transaction_List__hxx
/*
* $Log: SipTransactionList.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTransactionList.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

namespace Assist
{
    
template <class NodeType> class SipTransactionList
{
    public:
	struct SipTransListNode;

	SipTransactionList()
	    : first(), last()
	{
	    first.next = &last;
	    last.prev  = &first;
	}

	~SipTransactionList()
	{
	    SipTransListNode *curr=first.next;
	    
	    while(curr != &last)
	    {
		SipTransListNode* tmp = curr;
		curr = curr->next;
		delete tmp;
	    }
	}

	SipTransListNode* getFirst()
	{
	    if(first.next == &last)
		return 0;
	    else
		return first.next;
	}

	SipTransListNode* getLast()
	{
	    if(last.prev == &first)
		return 0;
	    else
		return last.prev;
	}

	SipTransListNode* getNext(SipTransListNode* curr)
	{
	    if(!curr || curr == last.prev)
		return 0;
	    else
		return curr->next;
	}

	SipTransListNode* getPrev(SipTransListNode* curr)
	{
	    if(!curr || curr == first.next)
		return 0;
	    else
		return curr->prev;
	}

	void erase(SipTransListNode* curr)
	{
	    if(curr)
	    {
		curr->prev->next = curr->next;
		curr->next->prev = curr->prev;
		delete curr;
	    }
	}
    
	SipTransListNode* insert(const NodeType& val)
	{
	    SipTransListNode * newNode = new SipTransListNode(val);
	    
	    newNode->next = first.next;
	    newNode->prev = newNode->next->prev;
	    first.next = newNode;
	    newNode->next->prev = newNode;
	    return newNode;
	}

	struct SipTransListNode
	{
		NodeType val;
		NodeType& operator()()
		{ return val;}

		SipTransListNode()
		    : val(), prev(0), next(0)
		{}
		SipTransListNode(const NodeType& val)
		    : val(val), prev(0), next(0)
		{}
		~SipTransListNode()
		{}
    
		SipTransListNode& operator=(const SipTransListNode& other);
		SipTransListNode * prev;
		SipTransListNode * next;
	};
    
    private:
	SipTransListNode first;
	SipTransListNode last;

};

 
}

#endif
