/*
 * $Id: SipTransactionLevels.cxx,v 1.4 2007/06/05 11:33:25 lizhijie Exp $
 */

#include "global.h"
#include "SipTransactionLevels.hxx"

using namespace Vocal;

SipTransactionList<SipTransLevel2Node*>::SipTransListNode *SipTransLevel1Node::findOrInsert(const SipTransactionId& id)
{
	SipTransactionList<SipTransLevel2Node*>::SipTransListNode * curr = level2.getFirst();
	while(curr)
	{
		if(curr->val->myKey == id.getLevel2())
			break;
		curr = level2.getNext(curr);
	}
	
	if(!curr)
	{
		cpDebug(DEBUG_NEW_STACK,"inserting a New Level2 Node [%s]", id.getLevel2().logData());

		SipTransLevel2Node * newNode = new SipTransLevel2Node;
		newNode->myKey = id.getLevel2();
		curr = level2.insert(newNode);
		newNode->topNode = this;
		newNode->myPtr = curr;
	}
	else
	{
		cpDebug(DEBUG_NEW_STACK,"Level2 Node [%s] already exists as %s", id.getLevel2().logData(), curr->val->myKey.logData());
	}

	return curr;
}

SipTransactionList<SipTransLevel2Node*>::SipTransListNode *SipTransLevel1Node::find(const SipTransactionId& id)
{
	SipTransactionList<SipTransLevel2Node*>::SipTransListNode * curr = level2.getFirst();
	while(curr)
	{
		if(curr->val->myKey == id.getLevel2())
			break;
		curr = level2.getNext(curr);
	}
	if(curr)
		return curr;
	else
		return 0;
}

SipTransactionList<SipTransLevel3Node *>::SipTransListNode *SipTransLevel1Node::findLevel3AckInvite( const SipTransactionId& id )
{
	SipTransactionList<SipTransLevel2Node *>::SipTransListNode * level2Node =	level2.getLast();
	SipTransactionList<SipTransLevel3Node *>::SipTransListNode * level3Node =	0;
	while( level2Node )
	{
		if( level2Node->val->myKey.matchChar("V") == id.getLevel2().matchChar("V") )
		{
			cpDebug(LOG_DEBUG_STACK, "CSeq number matched" );
			level3Node = level2Node->val->findInvite();
			if( level3Node )
				break;
		}
		level2Node = level2.getPrev( level2Node );
	}
	return level3Node;
}

SipTransactionList<SipTransLevel3Node*>::SipTransListNode *SipTransLevel2Node::findOrInsert(const SipTransactionId& id)
{
	SipTransactionList<SipTransLevel3Node*>::SipTransListNode * curr =	level3.getFirst();
	while(curr)
	{
		if(curr->val->myKey == id.getLevel3())
			break;
		curr = level3.getNext(curr);
	}
	
	if(!curr)
	{
		cpDebug(DEBUG_NEW_STACK,"inserting a New Level3 Node [%s]",id.getLevel3().logData());
		SipTransLevel3Node * newNode = new SipTransLevel3Node;
		newNode->myKey = id.getLevel3();
		curr = level3.insert(newNode);
		newNode->level2Ptr = this;
		newNode->myPtr = curr;
	}
	return curr;
}

SipTransactionList<SipTransLevel3Node*>::SipTransListNode *SipTransLevel2Node::find(const SipTransactionId& id)
{
	SipTransactionList<SipTransLevel3Node*>::SipTransListNode * curr =
	level3.getFirst();
	while(curr)
	{
		if(curr->val->myKey == id.getLevel3())
			break;
		curr = level3.getNext(curr);
	}
	if(curr)
		return curr;
	else
		return 0;
}

SipTransactionList<SipTransLevel3Node*>::SipTransListNode *SipTransLevel2Node::findInvite()
{
	SipTransactionList<SipTransLevel3Node*>::SipTransListNode * curr = level3.getFirst();
	while(curr)
	{
		if(curr->val->myKey == INVITE_METHOD)
		{
			cpDebug(LOG_DEBUG_STACK, "Found INVITE" );
			break;
		}
		curr = level3.getNext(curr);
	}
	
	if(curr)
		return curr;
	else
		return 0;
}

