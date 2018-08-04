/*
 * $Id: SipTransactionList.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

template <class T> class SipTransactionList::SipTransactionList()
    : first(), last()
{
    first.next = &last;
    last.prev  = &first;
}

template <class T> class SipTransactionList::
~SipTransactionList()
{
    SipTransListNode *curr=first->next;

    while(curr != &last)
    {
	SipTransListNode* tmp = curr;
	curr = curr->next;
	delete tmp;
    }
}

template <class T> SipTransListNode* class SipTransactionList::
getFirst()
{
    if(first.next == &last)
	return 0;
    else
	return first.next;
}

template <class T> SipTransListNode* class SipTransactionList::
getLast()
{
    if(last.prev == &first)
	return 0;
    else
	return last.prev;
}

template <class T> SipTransListNode* class SipTransactionList::
getNext(SipTransListNode* curr)
{
    if(!curr || curr == last.prev)
	return 0;
    else
	return curr->next;
}

template <class T> SipTransListNode* class SipTransactionList::
getPrev(SipTransListNode* curr)
{
    if(!curr || curr == first.next)
	return 0;
    else
	return curr->prev;
}

template <class T> class SipTransactionList::
delete(SipTransListNode* curr)
{
    if(curr)
    {
	curr->prev->next = curr->next;
	curr->next->prev = curr->prev;
	delete curr;
    }
}

template <class T> SipTransListNode* class SipTransactionList::
insert(const T& val)
{
    SipTransListNode * newNode = new SipTransListNode(val);

    newNode->next = first.next;
    newNode->prev = newNode->next->prev;
    first.next = newNode;
    return newNode;
}
