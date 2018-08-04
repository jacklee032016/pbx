#ifndef VEnvVar_hxx
#define VEnvVar_hxx
/*
 * $Id: VEnvVar.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "global.h"
#include "mstring.hxx"


class VEnvVar
{
    public:
        /**
           Constructor to create EnvVariable object.If variable is 
           not set , defvalue is taken.
        */
        VEnvVar(const mstring& variable, const mstring& defvalue);

        /**
           Constructor to create EnvVariable object.If variable is 
           not set , marked as not set;
        */
        VEnvVar(const mstring& variable);

        bool isSet()
        {
            return _set;
        };

        const mstring& value()
        {
            return _value;
        }

        operator mstring() const
        {
            return _value;
        }
        operator const char*() const
        {
            return _value.c_str();
        }
        operator int() const
        {
            return _value;
        }

        VEnvVar& operator =(const VEnvVar& other)
        {
            if (this != &other)
            {
                _variable = other._variable;
                _value = other._value;
                _set = other._set;
            }
            return *this;
        }

        //Add the new Env Variables as wwe go along
        static VEnvVar VPS_PORT;
        static VEnvVar VCONFIG_DIR;
        static VEnvVar VLISTENER_PORT;
        static VEnvVar VLOG_FILE_NAME;
        static VEnvVar VPS_FILESYSTEM;
        static VEnvVar VPS_SERVER;
        static VEnvVar VLOG_LEVEL;

    private:
        std::mstring _variable;
        std::mstring _value;
        bool _set;
};

#endif
