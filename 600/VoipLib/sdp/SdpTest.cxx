/*
 * $Id: SdpTest.cxx,v 1.3 2007/06/05 11:33:25 lizhijie Exp $
 */

#include"global.h"
#include "SdpSession.hxx"
#include <string>
#include <fstream.h>
#include <VTest.hxx>
#include <unistd.h>

#define OPTIONS "f:l:v"

bool
testEncode (SdpSession& sd)
{
    cpLog (LOG_DEBUG, "------- Test encode() begin -------");

    // Session Description
    // v=0 by default - no other value as of now
    // o= owner/creator and session identifier
    // s= session name
    // i=* session information
    // u=* URI of description
    // e=* email address
    // p=* phone number
    // c=* connection information (not required if included in all media
    // b=* bandwidth information
        // Time Description
        // t= time the session is active
        // r=* zero or more repeated times
    // z=* time zone adjustment
    // k=* encryption key
    // Verify
    cout << sd.encode();

    cpLog (LOG_DEBUG, "------- Test encode() end -------");
    return true;
}    // testEncode

bool
testDump(split_t& tc_files, bool verbose)
{
    cpLog (LOG_DEBUG, "------- Test Dump() begin -------");

    split_t::iterator linecount = tc_files.begin();
    while (linecount != tc_files.end())
    {
        // Get test case file name and throw away the description
        string tc_in;
        string record;

        record = (*linecount);
        split_t lines(split(record, ":"));    
        split_t::iterator filename;
        filename = lines.begin();

        // Generate test case input & outp file names
        tc_in = (*filename) + ".in";

        // Open and read the test case input file
        ifstream ifs(tc_in.c_str(), ios::in);
        char buffer[4096];
        string sdp;
        sdp.erase();

        if (!ifs)
        {
            cpLog(LOG_ERR, "Cannot open input file: %s ", tc_in.c_str());
            return false;
        }
    
        // Read in the SDP packet from file
        while (!ifs.eof())
        {
               ifs.getline(buffer, sizeof(buffer), '\n');
               sdp += buffer;
               sdp += "\n";
           }

        if (verbose)
            cpDebug(LOG_DEBUG, "String constructed from input file:\n[%s]",
                sdp.c_str());

        // Decode input file into SDP object 
           SdpSession sdpDesc;
           sdpDesc.decode(sdp);
/*
           if (!sdpDesc.dump(sdpDesc))
            return false;
*/
        ++linecount;
    }

    cpLog (LOG_DEBUG, "------- Test Dump() end -------");
    return true;
}    // testDump

bool
testDecode(split_t& tc_files, bool verbose)
{
    cpLog (LOG_DEBUG, "------- Test decode() begin -------");
    bool same = true;

    split_t::iterator linecount = tc_files.begin();
    while (linecount != tc_files.end())
    {
        string tc_in, tc_out;
        string record;

        // Get test case file name and throw away the description
        record = (*linecount);
        split_t lines(split(record, ":"));    
        split_t::iterator filename;
        filename = lines.begin();

        // Generate test case input & outp file names
        tc_in = (*filename) + ".in";
        tc_out = (*filename) + "_encode" + ".out";

        // Open and read the test case input file
        ifstream ifs(tc_in.c_str(), ios::in);
        fstream ofs(tc_out.c_str(), ios::in | ios::out);
        char buffer[4096];
        string sdp;
        sdp.erase();

        if (!ifs)
        {
            cpLog(LOG_ERR, "Cannot open input file: %s ", tc_in.c_str());
            return false;
        }
        if (!ofs)
        {
            cpLog(LOG_ERR, "Cannot open out file: %s ", tc_out.c_str());
            return false;
        }
    
        // Read in the SDP packet from file
        while (!ifs.eof())
        {
               ifs.getline(buffer, sizeof(buffer), '\n');
               sdp += buffer;
               sdp += "\n";
           }

        if (verbose)
        	{
            cpDebug(LOG_DEBUG, "String constructed from input file:\n[%s]",
                sdp.c_str());
        	}

        // Decode input file into SDP object 
           SdpSession sdpDesc;
           sdpDesc.decode(sdp);

        // Encode the object into SDP string and write it to output file 
        // for verification
           ofs << sdpDesc.encode();
    
        // Compare the input and output files
        // Reset stream pointers
        ifs.clear();
        ofs.clear();

        char in_char, out_char;

           while (ifs.get(in_char))
           {
               ofs.get(out_char);
               if (out_char == '\r')
               {
                // throw waay the '\r' character
                   ofs.get(out_char);
               }

               if (in_char != out_char)
               {
                   same = false;
                   cpLog(LOG_ERR, "In & out are different: in=\"%c\", out=\"%c\"", in_char, out_char);
                   break;
               }
           }

           if (same)
           	{
               cpDebug(LOG_DEBUG, "In & out are identical");
           	}
           else
           	{
               cpDebug(LOG_DEBUG, "In & out are different");
           	}

        ++linecount;
    }

    cpLog (LOG_DEBUG, "------- Test decode() end -------");
    return same;
}    // testDecode


bool
testAssignOperator(split_t& tc_files, bool verbose)
{
    cpLog (LOG_DEBUG, "------- Test operator= begin -------");
    bool same = true;

    split_t::iterator linecount = tc_files.begin();
    while (linecount != tc_files.end())
    {
        string tc_in, tc_out;
        string record;

        // Get test case file name and throw away the description
        record = (*linecount);
        split_t lines(split(record, ":"));    
        split_t::iterator filename;
        filename = lines.begin();

        // Generate test case input & outp file names
        tc_in = (*filename) + ".in";
        tc_out = (*filename) +"_assign" + ".out";

        // Open and read the test case input file
        ifstream ifs(tc_in.c_str(), ios::in);
        fstream ofs(tc_out.c_str(), ios::in | ios::out);
        char buffer[4096];
        string sdp;
        sdp.erase();

        if (!ifs)
        {
            cpLog(LOG_ERR, "Cannot open input file: %s ", tc_in.c_str());
            return false;
        }
        if (!ofs)
        {
            cpLog(LOG_ERR, "Cannot open out file: %s ", tc_out.c_str());
            return false;
        }
    
        // Read in the SDP packet from file
        while (!ifs.eof())
        {
               ifs.getline(buffer, sizeof(buffer), '\n');
               sdp += buffer;
               sdp += "\n";
           }

        if (verbose)
        	{
            cpDebug(LOG_DEBUG, "String constructed from input file:\n[%s]", sdp.c_str());
        	}

        // Decode input file into SDP object 
           SdpSession sdpDesc, sdpAssign;
           sdpDesc.decode(sdp);

        sdpAssign = sdpDesc;
        // Encode the assign object into SDP string and write 
        // it to output file for verification
           ofs << sdpAssign.encode();
    
        // Compare the input and output files
        // Reset stream pointers
        ifs.clear();
        ofs.clear();

        char in_char, out_char;

           while (ifs.get(in_char))
           {
               ofs.get(out_char);
               if (out_char == '\r')
               {
                // throw waay the '\r' character
                   ofs.get(out_char);
               }

               if (in_char != out_char)
               {
                   same = false;
                   cpLog(LOG_ERR, "In & out are different: in=\"%c\", out=\"%c\"", in_char, out_char);
                   break;
               }
           }

           if (same)
           	{
               cpDebug(LOG_DEBUG, "In & out are identical");
           	}
           else
           	{
               cpDebug(LOG_DEBUG, "In & out are different");
           	}

        ++linecount;
    }

    cpLog (LOG_DEBUG, "------- Test operator= end -------");
    return same;
}    // testAssignOperator


bool
testCopyConstructor(split_t& tc_files, bool verbose)
{
    cpLog (LOG_DEBUG, "------- Test copy constructor begin -------");
    bool same = true;

    split_t::iterator linecount = tc_files.begin();
    while (linecount != tc_files.end())
    {
        string tc_in, tc_out;
        string record;

        // Get test case file name and throw away the description
        record = (*linecount);
        split_t lines(split(record, ":"));    
        split_t::iterator filename;
        filename = lines.begin();

        // Generate test case input & outp file names
        tc_in = (*filename) + ".in";
        tc_out = (*filename) +"_copy" + ".out";

        // Open and read the test case input file
        ifstream ifs(tc_in.c_str(), ios::in);
        fstream ofs(tc_out.c_str(), ios::in | ios::out);
        char buffer[4096];
        string sdp;
        sdp.erase();

        if (!ifs)
        {
            cpLog(LOG_ERR, "Cannot open input file: %s ", tc_in.c_str());
            return false;
        }
        if (!ofs)
        {
            cpLog(LOG_ERR, "Cannot open out file: %s ", tc_out.c_str());
            return false;
        }
    
        // Read in the SDP packet from file
        while (!ifs.eof())
        {
               ifs.getline(buffer, sizeof(buffer), '\n');
               sdp += buffer;
               sdp += "\n";
           }

        if (verbose)
        	{
            cpDebug(LOG_DEBUG, "String constructed from input file:\n[%s]",sdp.c_str());
        	}

           SdpSession sdpDesc;
           sdpDesc.decode(sdp);
           SdpSession sdpCopy(sdpDesc);

        // Encode the object into SDP string and write it to output file 
        // for verification
           ofs << sdpCopy.encode();

        // Compare the input and output files
        // Reset stream pointers
        ifs.clear();
        ofs.clear();

        char in_char, out_char;

           while (ifs.get(in_char))
           {
               ofs.get(out_char);
               if (out_char == '\r')
               {
                // throw waay the '\r' character
                   ofs.get(out_char);
               }

               if (in_char != out_char)
               {
                   same = false;
                   cpLog(LOG_ERR, "In & out are different: in=\"%c\", out=\"%c\"", in_char, out_char);
                   break;
               }
           }

           if (same)
           	{
               cpDebug(LOG_DEBUG, "In & out are identical");
           	}
           else
           	{
               cpDebug(LOG_DEBUG, "In & out are different");
           	}

        ++linecount;
    }

    cpLog (LOG_DEBUG, "------- Test copy constructor end -------");
    return same;
}    // testCopyConstructor


bool
testNcsSdp ()
{
    cpLog (LOG_DEBUG, "------- Test NCS SDP begin -------");
    try
    {
        SdpSession ncs;

        cpLog (LOG_DEBUG, "Check NCS conformance from default constructor");
        cpLog (LOG_DEBUG, "The verdict should be FAILED");
        if (ncs.verify (SdpProtocolTypeNCS))
        {
            cpLog (LOG_ERR, "Results: PASSED");
        }

        cpLog (LOG_DEBUG, "Check NCS conformance after setting Protocol Type to NCS");
        ncs.setProtocolType (SdpProtocolTypeNCS);
        if (ncs.verify (SdpProtocolTypeNCS))
        {
            cpLog (LOG_DEBUG, "Results: PASSED");
        }
        else
            return false;

        ncs.setAddress ("gauss.private.vovida.com");
        (ncs.getConnection())->setUnicast ("192.168.5.2");
        (ncs.getMediaList())[0]->setPort (28888);

        cpLog (LOG_DEBUG, "Check NCS conformance filling in the rest");
        if (ncs.verify (SdpProtocolTypeNCS))
        {
            cpLog (LOG_DEBUG, "Results: PASSED");
        }
        else
            return false;
    }
    catch (SdpExceptions& err)
    {
        cpLog (LOG_ERR, "Got Exception: %d", err.value);
        return false;
    }
    cpLog (LOG_DEBUG, "------- Test NCS SDP end -------");
    return true;
}    // testNcsSdp


int
main(int argc, char *argv[])
{
    string tc_file = "../testCases/SDP_tc";    // Default test case file name
    string log_file = "SdpTest.log";    // Default log file name
    bool verbose = false;

    if (argc > 1)
    {
        char c;

        while ((c = getopt(argc, argv, OPTIONS)) != -1)
        {
            switch (c)
            {
                case 'f':
                    tc_file = optarg;
                    break;
                case 'l':
                    log_file = optarg ;
                    log_file += ".log";
                    break;
                case 'v':
                    verbose = true;
                    break;
                default:
                    cerr << "Unknow option: \"" << c << "\"" << endl;
            }
        }    
    }

    ifstream ifs(tc_file.c_str(), ios::in);
    char buffer[256];
    string s;
    s.erase();

    if (!ifs)
    {
        cpLog(LOG_ERR, "Cannot open input file: %s ", tc_file.c_str());
        return false;
    }
    while (!ifs.eof())
    {
        ifs.getline(buffer, sizeof(buffer), '\n');
        s += buffer;
        s += "\n";
    }

    // read in all test case file names & store by sections
    split_t lines(split(s, "\n"));
    split_t tc_decode;
    split_t tc_assign_op;
    split_t tc_copy_const;
    split_t tc_NCS;
    split_t tc_dump;

    split_t::iterator linecount = lines.begin();
    while (linecount != lines.end())
    {
        if ((*linecount)[0] == '/')
        {
            /// Throw away comment lines    
            ++linecount;
            continue;
        }

        string line = (*linecount);
        int pos = line.find(']');
        string sec_label = line.substr(0,pos+1);
        if (sec_label == "[DECODE]")
        {
            ++linecount;
            while ((linecount != lines.end()) && ((*linecount)[0] != '['))
            {
                if ((*linecount)[0] == '/')
                {
                    ++linecount;
                    continue;
                }    
                (tc_decode).push_back(*linecount);    
                ++linecount;
            }
        }    
        else if (sec_label == "[ASSIGN_OP]")
        {
            ++linecount;
            while ((linecount != lines.end()) && ((*linecount)[0] != '['))
            {
                if ((*linecount)[0] == '/')
                {
                    ++linecount;
                    continue;
                }    
                (tc_assign_op).push_back(*linecount);    
                ++linecount;
            }
        }
        else if (sec_label == "[COPY_CONST]")
        {
            ++linecount;
            while ((linecount != lines.end()) && ((*linecount)[0] != '['))
            {
                if ((*linecount)[0] == '/')
                {
                    ++linecount;
                    continue;
                }    
                (tc_copy_const).push_back(*linecount);    
                ++linecount;
            }
        }
        else if (sec_label == "[NCS]")
        {
            ++linecount;
            while ((linecount != lines.end()) && ((*linecount)[0] != '['))
            {
                if ((*linecount)[0] == '/')
                {
                    ++linecount;
                    continue;
                }    
                (tc_NCS).push_back(*linecount);    
                ++linecount;
            }
        }
        else if (sec_label == "[DUMP]")
        {
            ++linecount;
            while ((linecount != lines.end()) && ((*linecount)[0] != '['))
            {
                if ((*linecount)[0] == '/')
                {
                    ++linecount;
                    continue;
                }    
                (tc_dump).push_back(*linecount);    
                ++linecount;
            }
        }
        else
        {
            cpLog(LOG_ERR, "Unknown section label: %s ", (*linecount).c_str());
            return false;
        }
    }

    VTest sdpTest(__FILE__);
    cpLogOpen(log_file.c_str());
    cpLogSetLabel ("SdpTest");
    cpLogSetPriority (LOG_DEBUG_STACK);

    if (verbose)
    {
        cpDebug(LOG_DEBUG, "Test Case File = \"%s\"", tc_file.c_str());
        cpDebug(LOG_DEBUG, "Log File = \"%s\"", tc_file.c_str());
        cpDebug(LOG_DEBUG, "Verbose = \"%s\"", verbose ? "ON" : "OFF");
    }    

    sdpTest.test(1, testDecode(tc_decode, verbose), "Testing encode functionality");
    sdpTest.test(2, testAssignOperator(tc_assign_op, verbose), "Testing assign opertor");
    sdpTest.test(3, testCopyConstructor(tc_copy_const, verbose), "Testing copy constructor");
    sdpTest.test(4, testNcsSdp(), "Testing NCS conformance");
//    sdpTest.test(5, testDump(tc_dump, verbose), "Testing dump functionality");
    return true;
}

