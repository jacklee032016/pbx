/*
* $Id: VEnvVar.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "VEnvVar.hxx"
#include "vpbx_globals.h"

using std::mstring;


VEnvVar VEnvVar::VPS_PORT("VPS_PORT", "6005");
VEnvVar VEnvVar::VPS_SERVER("VPS_SERVER", "216.102.92.37");
VEnvVar VEnvVar::VCONFIG_DIR("VCONFIG_DIR", ASSIST_INSTALL_PATH "/provisioning_data/Config");
VEnvVar VEnvVar::VLISTENER_PORT("VLISTENER_PORT", "6006");
VEnvVar VEnvVar::VLOG_FILE_NAME("VLOG_FILE_NAME", ASSIST_INSTALL_PATH "/provisioning_data/Logs/pserver_log");
VEnvVar VEnvVar::VPS_FILESYSTEM("VPS_FILESYSTEM", ASSIST_INSTALL_PATH "/provisioning_data");
VEnvVar VEnvVar::VLOG_LEVEL("VLOG_LEVEL", "6");  //INFO from cplog.h


VEnvVar::VEnvVar(const mstring& variable, const mstring& defvalue)
        : _variable(variable), _value(defvalue), _set(true)
{
    char *c;
    c = getenv(_variable.c_str());
    if (c)
    {
        _value = c;
    }
}

VEnvVar::VEnvVar(const mstring& variable)
        : _variable(variable), _set(false)
{
    char *c;
    c = getenv(variable.c_str());
    if (c)
    {
        _value = c;
        _set = true;
    }
}
