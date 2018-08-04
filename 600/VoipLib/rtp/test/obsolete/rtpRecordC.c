/*
* $Id: rtpRecordC.c,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "rtpTypes.h"
#include "rtpApi.h"


int main (int argc, char *argv[])
{
    int file;
    RtpSessionPtr stack;
    RtpPacketPtr packet;
    int i;
    int ret;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: rtpRecordC file\n");
        return -1;
    }
    file = open("FILErecv", O_CREAT | O_APPEND | O_WRONLY);

    if (file == -1)
    {
        fprintf(stderr, "Could not open file: FILErecv\n");
        return -1;
    }

    // receiving on localhost on port 5000/5001
    RtpSession_create1( &stack, "localhost", 0, 5000, 5002, 5001);
    RtpPacket_create1( &packet, 256, 0, 0);

    // stop loop after 400 packet attempts
    for (i = 0; i < 999; i++)
    {
        RtpSession_receive( stack, &packet, &ret);
        if (ret > 0)
        {
            char* location;
            int length;

            RtpPacket_getPayloadLoc( packet , &location );
            RtpPacket_getPayloadUsage( packet, &length );

            /* loop until packet is written to file */
            while ( length > 0 )
            {
                int written = write( file, location, length );
                if ( written == -1 )
                {
                    fprintf(stderr, "\nProblem writing to file\n");
                }
                location += written;
                length -= written;
            }

            fprintf(stderr, "*");

            RtpSession_intervalRTCP( stack, &ret);
            if (ret) RtpSession_transmitRTCP ( stack, &ret);
            RtpSession_receiveRTCP( stack, &ret);
        }
        else
        {
            // no packet received
            fprintf(stderr, "x");
        }

        // slow program down
        usleep (2 * 1000);
    }

    return 0;
}
