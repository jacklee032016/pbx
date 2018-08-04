#ifndef SNMPDATA_HXX
#define SNMPDATA_HXX
/*
 * $Id: SnmpData.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include <iostream>
#include "Data.hxx"

namespace Vocal
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

} // namespace Vocal

#endif
