#ifndef SNMPDATA_HXX
#define SNMPDATA_HXX
/*
* $Log: SnmpData.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SnmpData.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <iostream>
#include "Data.hxx"

namespace Assist
{
    
class snmpData
{

    public:
        snmpData() : writable(false), changed(false) { };
        snmpData(Data, bool write , bool changed );
        snmpData(bool write , bool changed );
        snmpData& operator = (const snmpData& data);
        snmpData(const snmpData& data);
        ~snmpData();
        void setData(Data);
        Data getData();
        void setChanged(bool);
        bool getChanged();
        bool isWritable();

    private:
        Data value;
        bool writable;
        bool changed;

};

} // namespace Assist

#endif
