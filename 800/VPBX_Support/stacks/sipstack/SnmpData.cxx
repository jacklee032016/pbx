/*
* $Log: SnmpData.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SnmpData.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include <iostream>
#include "SnmpData.hxx"

using namespace Assist;

snmpData::snmpData(Data data, bool write = false, bool change = false)
{
    value = data;
    writable = write;
    changed = change;
}


snmpData::snmpData( bool write = false, bool change = false)
{
    value = "";
    writable = write;
    changed = change;
}

snmpData& snmpData::operator = (const snmpData& data)
{
    if (&data != this)
    {
        value = data.value;
        writable = data.writable;
        changed = data.changed;
    }
    return (*this);
}

snmpData::snmpData(const snmpData& src)
        :
        value(src.value),
        writable(src.writable),
        changed(src.changed)
{}


snmpData::~snmpData()
{
}


void snmpData::setData(Data data)
{
    value = data;
}

Data snmpData::getData()
{
    return value;
}

void snmpData::setChanged(bool cond)
{
    changed = cond;
}

bool snmpData::getChanged()
{
    return changed;
}

bool snmpData::isWritable()
{
    return writable;
}
