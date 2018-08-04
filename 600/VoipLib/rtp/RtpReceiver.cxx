/*
 * $Id: RtpReceiver.cxx,v 1.3 2007/06/05 11:33:25 lizhijie Exp $
 */

#include "global.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include "vtypes.h"
#include <unistd.h>
#include <string.h>

#include "cpLog.h"
#include "vsock.hxx"

#include "NetworkAddress.h"
#include "NtpTime.hxx"
#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "rtpCodec.h"


const int RtpReceiver::IN_BUFFER_SIZE = 24576;
const int RtpReceiver::RECV_BUF = 1012;
const int RtpReceiver::MISORDER = 6;
const int RtpReceiver::PROB_MAX = 100;


static NtpTime nowTime, pastTime;
/* ----------------------------------------------------------------- */
/* --- RtpReceiver Constructor ------------------------------------- */
/* ----------------------------------------------------------------- */

RtpReceiver::RtpReceiver (int localMinPort, int localMaxPort,
                          RtpPayloadType newApiFormat,
                          RtpPayloadType newNetworkFormat, int jitterNew)
{
	/// udp stack is a sendrecv stack
	myStack = new UdpStack (NULL, localMinPort, localMaxPort) ;
	freeStack = true;

	constructRtpReceiver (newApiFormat, newNetworkFormat, jitterNew);
}

RtpReceiver::RtpReceiver (int localPort, RtpPayloadType newApiFormat,
                          RtpPayloadType newNetworkFormat,
                          int jitterNew)
{
	/// udp stack is a sendrecv stack
	myStack = new UdpStack (NULL, localPort) ;
	freeStack = true;

	constructRtpReceiver (newApiFormat, newNetworkFormat, jitterNew);
}

RtpReceiver::RtpReceiver (UdpStack* udp, RtpPayloadType newApiFormat,
                          RtpPayloadType newNetworkFormat, int jitterNew)
{
	/// udp stack is a sendrecv stack
	myStack = udp;
	freeStack = false;

	constructRtpReceiver (newApiFormat, newNetworkFormat, jitterNew);
}


void RtpReceiver::constructRtpReceiver (RtpPayloadType newApiFormat,
                                        RtpPayloadType newNetworkFormat,
                                        int jitterNew)
{
	inPos = 0;
	playPos = 0;
	memset (inBuff, 0, IN_BUFFER_SIZE);

	// set format and baseSampleRate
	setApiFormat(newApiFormat, 160, 0, NULL, false);
	setNetworkFormat(newNetworkFormat, 160, 0, NULL, false);

	// set private variables
	jitterSeed = jitterNew;

	// no transmitter
	sourceSet = false;
	ssrc = 0;
	probationSet = false;
	srcProbation = 0;
	probation = -2;
	prevPacket = NULL;
	rtcpRecv = NULL;
	silenceCodec = 0;
	codecString[0] = '\0';

	cpDebug(LOG_DEBUG_STACK, "Constructed receiver");
}


RtpReceiver::~RtpReceiver ()
{
	if (freeStack)
	{
		delete myStack;
		myStack = NULL;
	}
	rtcpRecv = NULL;

	cpDebug(LOG_DEBUG_STACK, "Close receiver");
}


/* --- receive packet functions ------------------------------------ */

RtpPacket* RtpReceiver::receive ()
{
	RtpPacket* p = NULL;
	int len = 0;
	int len1 = 0;
	int silencePatched = 0;
	bool faking = 0;


	// empty network que
	NtpTime arrival (0, 0);
	while (1) // network empty or time to play return packet
	{
		p = getPacket();
		if (p == NULL) 
			break;

		// only play packets for valid sources
		if (probation < 0)
		{
			cpLog(LOG_ERR, "****Packet from invalid source");
			delete p;
			p = NULL;
			continue;
		}

		arrival = getNtpTime();
		int packetTransit = 0;
		int delay = 0;

		rtp_ntohl(p);

		// convert codec
		RtpPayloadType lType = p->getPayloadType();
		/**/

#if 0
		if (lType == rtpPayloadDTMF_RFC2833)
		{
		rtp_htonl(p);
		return p;
		}
#endif
		//printf("in receive rtp %d\n",lType);
		/**/
		if ((lType != rtpPayloadDTMF_RFC2833) && (lType != rtpPayloadCiscoRtp) && lType != apiFormat)
		{
#ifndef __sparc
			// replace p with a new packet
			RtpPacket* oldp = p;
			p = convertRtpPacketCodec (apiFormat, oldp);
			assert (oldp->getSequence() == p->getSequence());
			delete oldp;
#endif
		}

		len = p->getPayloadUsage();
		if (len <= 0 || len > 1012)
		{
			cpLog(LOG_ERR, "Got an invalid packet size");
			delete p;
			p = NULL;
			continue;
		}

		/*
		// drop SID packets
		if (len < ( networkFormat_payloadSize / 3 ) )
		{
		cpDebug(LOG_DEBUG_STACK, "Dropping SID packet" );
		prevSeqRecv = p->getSequence();
		delete p;
		p = NULL;
		continue;
		}
		*/

		// fix frame boundry
		if (len > networkFormat_payloadSize )
		{
			int lenold = len;
			len = ( len / networkFormat_payloadSize ) * networkFormat_payloadSize;
			p->setPayloadUsage( len );
			cpDebug(LOG_DEBUG_STACK, "Fixing frame boundry to %d from %d", len, lenold );
			network_pktSampleSize = (lenold / networkFormat_payloadSize) * network_pktSampleSize;
		}

		// bypass jitterBuffer
		if (jitterTime == -1)
		{
			cpDebug(LOG_DEBUG_STACK, "Skipping jitter buffer" );
			cpDebug(LOG_DEBUG, "Skipping jitter buffer" );
			// update counters
			prevSeqRecv = p->getSequence();
			prevSeqPlay = p->getSequence();

			// update packet received
			packetReceived++;
			payloadReceived += p->getPayloadUsage();

			// update jitter calculation
			packetTransit = arrival - rtp2ntp(p->getRtpTime());
			delay = packetTransit - transit;
			transit = packetTransit;
			if (delay < 0)
				delay = -delay;
			jitter += delay - ((jitter + 8) >> 4);
			return p;
		}


		// reordering the packets according to the seq no
		// leave gaps and copy the next packet in all the gap
		// when the late packets come copy it into the correct pos

		if (RtpSeqGreater(p->getSequence(), prevSeqRecv))
		{
			// insert packet at end, repeat filling buffer if sequence skipping
			//bool cycleEnd = false;
			//            int w=0;

			while (RtpSeqGreater(p->getSequence(), prevSeqRecv))
			{
				// silence patching
				//                prevPacketRtpTime += network_pktSampleSize;
				silencePatched = 0;
				faking = 0;
				
				while( RtpTimeGreater( p->getRtpTime() - network_pktSampleSize, prevPacketRtpTime ) && ((p->getSequence() - 1) == prevSeqRecv))
				{
					if( cpLogGetPriority() >= LOG_DEBUG_HB )
					{
						cerr<<"s"<<network_pktSampleSize;
					}
					
					if( silenceCodec == 0 )
					{
						cpDebug(LOG_DEBUG_STACK, "Patching silence" );
						if ((p->getPayloadType() >= rtpPayloadDynMin) && (p->getPayloadType() <= rtpPayloadDynMax) && (codecString[0] != '\0'))
						{
							silenceCodec = findSilenceCodecString(codecString, len);
						}
						else
						{
							silenceCodec = findSilenceCodec( p->getPayloadType(), len );
						}
						
						if( silenceCodec == 0 )
						{
							if( len > rtpCodecInfo[ numRtpCodecInfo - 1 ].length )
							{
								cpLog( LOG_ERR, "Requested codec too big to fake %d", len );
								assert( 0 );
							}
							
							cpDebug(LOG_DEBUG_STACK, "Faking silence packet with 0x00" );
							silenceCodec = (char*)&rtpCodecInfo[ numRtpCodecInfo - 1 ].silence;
							faking = 1;
						}
					}
					
					assert( silenceCodec );

					if ((inPos + len) < IN_BUFFER_SIZE)
					{
						memcpy (inBuff + inPos, silenceCodec, len);
						inPos += len;
						//printf("inPos S=%d\n", inPos);
						silencePatched++;
					}
					else
					{
						// circular memory copy 
						len1 = IN_BUFFER_SIZE - inPos;
						memcpy (inBuff + inPos, silenceCodec, len1);
						memcpy (inBuff, silenceCodec + len1, len - len1);
						inPos = len - len1;
						//printf("inPos S=%d\n", inPos);
						silencePatched++;
					}
					prevPacketRtpTime += network_pktSampleSize;
				}
				
				if( prevPacketRtpTime != p->getRtpTime() - network_pktSampleSize)
				{
					cpDebug(LOG_DEBUG_STACK, "Silent patching fail to correct rtptime" );
					cpDebug(LOG_DEBUG_STACK, "prevPacketRtpTime(%u), p->getRtpTime()(%u), networkPacketSize(%d)", prevPacketRtpTime, p->getRtpTime(), network_pktSampleSize );
					prevPacketRtpTime = p->getRtpTime() - network_pktSampleSize;
				}

				if ((inPos + len) < IN_BUFFER_SIZE)
				{
					memcpy (inBuff + inPos, p->getPayloadLoc(), len);
					inPos += len;
					//printf("inPos =%d\n", inPos);
				}
				else
				{
					// circular memory copy
					len1 = IN_BUFFER_SIZE - inPos;
					memcpy (inBuff + inPos, p->getPayloadLoc(), len1);
					memcpy (inBuff, p->getPayloadLoc() + len1, len - len1);
					inPos = len - len1;
				//printf("inPos =%d\n", inPos);
				}

				// update counters
				RtpSeqNumber tSeq = prevSeqRecv;
				prevSeqRecv++;
				if(prevSeqRecv > RTP_SEQ_MOD)
				{
					prevSeqRecv = 0;
				}
				if (prevSeqRecv < tSeq)
				{
					cpDebug(LOG_DEBUG_STACK, "Recv cycle");
					assert(prevSeqRecv == 0);
					recvCycles += RTP_SEQ_MOD;
				}
			}
			
			prevPacketRtpTime = p->getRtpTime();
			if (silencePatched > 0)
			{
				cpDebug(LOG_DEBUG_STACK, "silencePatched = %d", silencePatched);
			}
			
			if (faking) 
				silenceCodec = 0;
			
			if (p->getSequence() != prevSeqRecv)
			{
				cpDebug(LOG_DEBUG_STACK, "Unequal packet:%d stack:%d",  prevSeqRecv, p->getSequence());
			}
		}
		else
		{
			// insert packet in middle
			cpDebug(LOG_DEBUG_STACK, "insert middle packet: %d prevSeq %d", p->getSequence(), prevSeqRecv);
			RtpSeqNumber base_prevSeqRecv = prevSeqRecv;
			int inSeqRecv = 1;
			
			while (RtpSeqGreater(base_prevSeqRecv, p->getSequence()))
			{
				inSeqRecv++;
				base_prevSeqRecv--;
			}
			
			int inPosTemp = inPos - inSeqRecv * len;
			if (inPosTemp < 0)
				inPosTemp = IN_BUFFER_SIZE + inPosTemp;

			if ((inPosTemp + len) < IN_BUFFER_SIZE)
			{
				memcpy (inBuff + inPosTemp, p->getPayloadLoc(), len);
			}
			else
			{
				// circular memory copy
				len1 = IN_BUFFER_SIZE - inPosTemp;
				memcpy (inBuff + inPosTemp, p->getPayloadLoc(), len1);
				memcpy (inBuff, (p->getPayloadLoc()) + len1, len - len1);
			}
		}

		// update packet received
		packetReceived++;
		payloadReceived += len;

		// update jitter calculation
		packetTransit = arrival - rtp2ntp(p->getRtpTime());
		delay = packetTransit - transit;
		transit = packetTransit;
		if (delay < 0)
			delay = -delay;
		
		jitter += delay - ((jitter + 8) >> 4);

		//printf("jitter is %d delay %d\n",jitter,delay);
		//    fractional
		// s->jitterTime += (1./16.) * ((double)deley - s->jitterTime);
		//    integer
		//jitterTime += delay - ((jitterTime+8) >> 4);

		if (p)
		{
			delete p;
			p = NULL;
			break;
		}
	}

	/*
	// check if time to play packet
	if (getNtpTime() < gotime)
	{
	//        cpLog(LOG_ERR,"wait");
	//cout <<"w";
	return NULL;
	}
	*/

	int packetSize = apiFormat_payloadSize;


	// Only do catchup for the beginning packets  (is this needed? - kle)
	/*
	if (! doneCatchup )
	{
	if ( ( inPos > packetSize * 2 )
	&& ( playPos == 0 ) )
	{
	playPos = inPos - packetSize * 2;
	cpLog(LOG_ERR, "catch up");
	}
	doneCatchup = true;
	}
	*/


	// deque next packet
	if ( (inPos == 0) && (playPos == 0) )
	{
		cpLog (LOG_DEBUG_STACK, "Recv buffer is empty");
		receiverError = recv_bufferEmpty;
		return NULL;
	}

	if (((inPos + IN_BUFFER_SIZE - playPos) % IN_BUFFER_SIZE) < packetSize)
	{
		//        cpLog (LOG_ERR,"Not enough data for a api packet size %d", packetSize);
		receiverError = recv_bufferEmpty;
		return NULL;
	}

	// create next packect
	assert (!p);
	p = new RtpPacket (packetSize);
	if ( (playPos + packetSize) < IN_BUFFER_SIZE)
	{
		memcpy (p->getPayloadLoc(), inBuff + playPos, packetSize);
		playPos += packetSize;
		//printf("playPos =%d\n", playPos);
	}
	else
	{
		// circular memory copy
		len1 = IN_BUFFER_SIZE - playPos;
		memcpy (p->getPayloadLoc(), inBuff + playPos, len1);
		memcpy (p->getPayloadLoc() + len1, inBuff, packetSize - len1);
		playPos = packetSize - len1;
		//printf("playPos =%d\n", playPos);
	}

	// finish packet
	p->setSSRC (ssrc);
	p->setPayloadType (apiFormat);
	//printf("finish type %d\n",apiFormat);
	p->setPayloadUsage (packetSize);
	p->setRtpTime (prevRtpTime + api_pktSampleSize);
	p->setSequence (prevSeqPlay + 1);

	if (probation > 0)
		probation --;
	receiverError = recv_success;

	/*
	if( ( prevRtpTime + network_pktSampleSize ) != p->getRtpTime() )
	{
	network_pktSampleSize = p->getRtpTime() - prevRtpTime;
	cpDebug(LOG_DEBUG_STACK, "Fixing network_pktSampelSize to %d", network_pktSampleSize );
	}
	*/
	//    prevRtpTime += network_pktSampleSize;
	prevRtpTime = p->getRtpTime();
	prevNtpTime = getNtpTime();
	gotime = rtp2ntp (p->getRtpTime() + api_pktSampleSize) + jitterTime;


	// update counters
	RtpSeqNumber sSeq = prevSeqPlay;
	prevSeqPlay++;
	if (prevSeqPlay < sSeq)
	{
		cpDebug(LOG_DEBUG_STACK, "Play cycle");
		assert (prevSeqPlay == 0);
		playCycles += RTP_SEQ_MOD;
	}

	return p;
}

RtpPacket* RtpReceiver::getPacket ()
{
    // check for network activity
    fd_set netFD;
    FD_ZERO (&netFD);
    FD_SET (myStack->getSocketFD(), &netFD);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    int selret = select (myStack->getSocketFD() + 1, &netFD, NULL, NULL, &timeout);
    if (selret <= 0)
    {
        // select error or no network activity
        if (selret < 0)
		cpLog(LOG_ERR, "Select loop error");
        else 
        {
        	cpDebug(LOG_DEBUG_STACK, "RtpReceiver receives %d pkts", packetReceived);
		cpDebug(LOG_DEBUG_STACK, "UdpStack receives %d pkts", myStack->getPacketsReceived());
		return NULL;
        }
    }


    // create packet
    RtpPacket* p = new RtpPacket (RECV_BUF);
    assert (p);


    // receive packet
    int len;
    len = myStack->receiveFrom ((char*)p->getHeader(), p->getPacketAlloc(), 0);
    p->setTotalUsage (len);
//   cpDebug(LOG_DEBUG, "RTP get packet len = %d", len);
//    cpDebug(LOG_DEBUG_STACK, "RTP seq = %d", p->getSequence());
//    cpDebug(LOG_DEBUG_STACK, "RTP rtptime = %d", p->getRtpTime());

    // check packet
	if( !p->isValid() )
	{
		cpLog(LOG_ERR, "****Packet is not valid");
		delete p; p = NULL;
		return NULL;
	}


    // check if rtp event
    if (p->getPayloadType() == rtpPayloadDTMF_RFC2833 || p->getPayloadType() == rtpPayloadCiscoRtp)
    {
   //printf("rfc 2833\n");
   //return p;

        if(getDTMFInterface() != 0)
        {
            //If a call-back is set, let the callback handle
            //the DTMF event
            //printf("rfc 2833\n");

            recvEvent( p );
            delete p; p = NULL;
            return NULL;

        }
        else
        {
            //Treat it as any other packet
            return p;
        }

    }

    // update receiver info
    //   function may return 1, meaning packet
    //   out of seq tolr or source not valid
    if (updateSource(p))
    {
        cpLog(LOG_ERR, "****Packet is discarded or source not valid");
        delete p; p = NULL;
        return NULL;
    }

    return p;
}


int RtpReceiver::updateSource (RtpPacket* p)
{
    // check if ssrc in probation list
    if (sourceSet && p->getSSRC() == srcProbation && probationSet)
        // old probation packets still in que
        return 1;

    // drop SID packets when SID packets are beginning packets
    if ((p->getPayloadUsage() < ( networkFormat_payloadSize / 3 )) ||
        (p->getPayloadUsage() < 20) )
    {
        cpDebug(LOG_DEBUG_STACK, "Dropping SID packet" );
        prevSeqRecv = p->getSequence();
        return 1;
    }

    // new source found or resync old source
    if (!sourceSet || p->getSSRC() != ssrc)
    {
        if (addSource(p))
            return 1;
    }


    // no vaild source yet
    assert (probation >= 0);


    // check if receiving new payload format
    /*
    if (p->getPayloadType() != networkFormat ||
            p->getPayloadUsage() != networkFormat_payloadSize)
    {
        cpDebug(LOG_DEBUG, "Transmitter changing payload parameters(%d/%d) (%d/%d)",
            p->getPayloadType(), networkFormat,
            p->getPayloadUsage(), networkFormat_payloadSize );
        p->printPacket();
        initSource(p);
    }
    */

    // drop CN packets
    if ((p->getPayloadType() == rtpPayloadCN) ||
        (p->getPayloadType() == 13)) {
        cpDebug(LOG_DEBUG_STACK, "drop 1 rtpPayloadCN packet");
        prevSeqRecv = p->getSequence();  // drop comfort noise packet
        return 1;
    }

    if (p->getPayloadType() != networkFormat )
    {
        cpDebug(LOG_DEBUG, "Transmitter changing payload parameters(%d/%d) (%d/%d)",
            p->getPayloadType(), networkFormat,
            p->getPayloadUsage(), networkFormat_payloadSize );
        p->printPacket();
        initSource(p);
    }

/*
    cpDebug(LOG_DEBUG_STACK, "prevPacketRtpTime = %d", prevPacketRtpTime);
    cpDebug(LOG_DEBUG_STACK, "network_pktSampleSize = %d", network_pktSampleSize);
    cpDebug(LOG_DEBUG_STACK, "p->getRtpTime() = %d", p->getRtpTime());
    cpDebug(LOG_DEBUG_STACK, "it is %f", (( p->getRtpTime() - prevPacketRtpTime) * 1.0) / network_pktSampleSize);
*/
    // fix rtp time stamp boundry
    if ((( prevPacketRtpTime + network_pktSampleSize ) != p->getRtpTime()) &&
        ((( p->getRtpTime() - prevPacketRtpTime) * 1.0) / network_pktSampleSize < 1.5) &&
        ((( p->getRtpTime() - prevPacketRtpTime) * 1.0) / network_pktSampleSize
> 0.5))
    {
        network_pktSampleSize = p->getRtpTime() - prevPacketRtpTime;
        cpDebug(LOG_DEBUG_STACK, "Fixing network_pktSampleSize to %d", network_pktSampleSize );
    }

    // check if valid sequence window
    RtpSeqNumber seq = p->getSequence();
//    cpDebug(LOG_DEBUG, "seq = %d, prevSeqRecv = %d", seq, prevSeqRecv);
    if ( RtpSeqGreater(seq, prevSeqRecv) )  // future packet
    {
        if (seq > prevSeqRecv)
        {
            if ( (seq - prevSeqRecv) > MISORDER )
            {
                // large sequence jump forward, skip over packets less then seq
                cpDebug(LOG_DEBUG_STACK, "jump to %d (%d jumps)", seq, seq - prevSeqRecv);
                cpDebug(LOG_DEBUG_STACK, "rtpTime %u, prevPacketRtpTime %u", p->getRtpTime(), prevPacketRtpTime);
                prevSeqRecv = seq - 1;
                prevPacketRtpTime = p->getRtpTime() - network_pktSampleSize;
            }
        }
        else if ( (seq + RTP_SEQ_MOD - prevSeqRecv) > MISORDER)
        {
            cpDebug(LOG_DEBUG_STACK, "jump cycle to %d (%d jumps)", seq, seq + RTP_SEQ_MOD - prevSeqRecv );
            cpDebug(LOG_DEBUG_STACK, "rtpTime %u, prevPacketRtpTime %u", p->getRtpTime(), prevPacketRtpTime);
            prevSeqRecv = seq - 1;
            prevPacketRtpTime = p->getRtpTime() - network_pktSampleSize;
        }
    }
    else if ( RtpSeqGreater(prevSeqRecv, seq) )  // past packet
    {
        // check if the pkt is too late comparing to the pkt already
        // played. Since the prevSeqPlay is irrelavant due to diff
        // api_payload Size, use inPos and playPos to calculate.
        int size = networkFormat_payloadSize;
        int backNoOfSeq = (prevSeqRecv > seq) ?
                          (prevSeqRecv - seq) : (prevSeqRecv + RTP_SEQ_MOD - seq );
        if (inPos > playPos)
        {
            if ((inPos - (backNoOfSeq + 1) * size) < playPos)
            {
                cpDebug(LOG_DEBUG_STACK, "too late now: discard seq %d", seq);
                return 1;
            }
        }
        else
        {
            if ((inPos + IN_BUFFER_SIZE - (backNoOfSeq + 1) * size) < playPos)
            {
                cpDebug(LOG_DEBUG_STACK, "too late now: discard seq %d", seq);
                return 1;
            }
        }
    }
    else // (seq == prevSeqRecv)  // present packet
    {
        // duplicate with previous packet
        //cpDebug(LOG_DEBUG_STACK,"duplicate packet %d", seq);
        //return 1;
    }


    /*
        // check late packets
        if (RtpSeqGreater(prevSeqPlay, p->getSequence()))
        {
            // packet arrived late
            cpDebug(LOG_DEBUG_STACK,"late now:%d packet:%d", prevSeqPlay, p->getSequence());
            //return 1;
        }
    */
    return 0;
}

int RtpReceiver::addSource(RtpPacket* p)
{
    // don't allow ssrc changes without removing first
    if (sourceSet)
    {
        if (probation < 4)
        {
            probation ++;
            cpLog(LOG_ERR, "Rejecting new transmitter %u, keeping %u", p->getSSRC(), ssrc);
            return 1;
        }
        else removeSource(ssrc);
    }

    // check if ssrc in probation list
    if (sourceSet && p->getSSRC() == srcProbation && probationSet)
        return 1;

    sourceSet = true;
    ssrc = p->getSSRC();
    cpDebug(LOG_DEBUG_STACK, "Received ssrc = %u", ssrc);
    probation = 0;
    packetReceived = 0;
    payloadReceived = 0;


    // init SDES and RTCP fields
    if (rtcpRecv) rtcpRecv->addTranInfo(ssrc, this);

    initSource (p);

    return 0;
}

void RtpReceiver::initSource (RtpPacket* p)
{
    assert (ssrc == p->getSSRC());

    cpDebug(LOG_DEBUG_STACK, "InitSource %u with sequence %d and rtp time %u", ssrc, p->getSequence(), p->getRtpTime());
    seedSeq = p->getSequence();
    seedNtpTime = getNtpTime();
    seedRtpTime = p->getRtpTime();

    // set receiving codec
    //If codec is G711, paylaod size cannot change, if it does it is
    //an error, just drop the packet
    if((p->getPayloadType() == 0) && 
       (p->getPayloadUsage() != networkFormat_payloadSize))
    {
        cpLog(LOG_ERR, "Incorrect payload size (%d) for payload type (%d), expected (%d) ignoring packet",
                               p->getPayloadUsage(), p->getPayloadType(), networkFormat_payloadSize);
    }
    else if ((p->getPayloadType() != networkFormat) ||
        (p->getPayloadUsage() != networkFormat_payloadSize))
    {
        int networkFormat_org = networkFormat;
        int network_pktSampleSize_org = network_pktSampleSize;
        int networkFormat_payloadSize_org = networkFormat_payloadSize;
        float multiplier = (p->getPayloadUsage() * 1.0) / (networkFormat_payloadSize * 1.0);
        float new_samples = multiplier * (network_pktSampleSize * 1.0);

        setNetworkFormat( p->getPayloadType(),
                          (int) new_samples,
                          0, p);

        if (networkFormat_perSampleSize)
            network_pktSampleSize = network_pktSampleSize / networkFormat_perSampleSize;
        else
            setNetworkFormatCodec ();

        if (networkFormat_org != networkFormat)
        {
            cpLog (LOG_DEBUG, "  networkFormat corrected to %d", networkFormat);
        }
		
	if (network_pktSampleSize_org != network_pktSampleSize)
		{
            cpLog (LOG_DEBUG, "  Number of network_pktSampleSize corrected to %d", network_pktSampleSize);
		}
        if (networkFormat_payloadSize_org != networkFormat_payloadSize)
        	{
            cpLog (LOG_DEBUG, "  Number of networkFormat_payloadSize corrected to %d", networkFormat_payloadSize);
        	}
    }

    //For dynamic payload type, setApiFormat the same as NetworkFormat
    if ((apiFormat != p->getPayloadType()) &&
        (p->getPayloadType() >= rtpPayloadDynMin) &&
        (p->getPayloadType() <= rtpPayloadDynMax))
    {
        setApiFormat( p->getPayloadType(),
                      network_pktSampleSize,
                      0, p);
    }

    inPos = 0;
    playPos = 0;

    // set timing information
    prevRtpTime = p->getRtpTime() - api_pktSampleSize;
    prevPacketRtpTime = p->getRtpTime() - network_pktSampleSize;
    prevNtpTime = rtp2ntp(p->getRtpTime()) - network_pktSampleSize * 1000 / networkFormat_clockRate;
    prevSeqRecv = p->getSequence() - 1;
    prevSeqPlay = p->getSequence() - 1;
    recvCycles = 0;
    playCycles = 0;

    transit = 0;
    jitter = 0;
    jitterTime = jitterSeed;

    // set up next gotime
    gotime = rtp2ntp (p->getRtpTime()) + jitterTime;
}

void RtpReceiver::removeSource (RtpSrc s, int flag)
{
	if (s != ssrc)
	{
		cpDebug(LOG_DEBUG_STACK, "Removing non active source: %u", s);
	}

	// no longer listen to this source
	probationSet = true;
	srcProbation = s;

	// no transmitter
	sourceSet = false;
	ssrc = 0;
	probation = -2;

	// remove from RTCP receiver
	if (rtcpRecv && !flag)
		rtcpRecv->removeTranInfo (s, 1);

	cpDebug(LOG_DEBUG_STACK, "Removing source: %u", s);
}

NtpTime RtpReceiver::rtp2ntp (RtpTime rtpTime)
{
	NtpTime ntptime = seedNtpTime +  ((rtpTime - seedRtpTime) * 1000 / apiFormat_clockRate);
	return ntptime;
}


/*
void RtpReceiver::clearBuffer()
{
    map<RtpSeqNumber, RtpPacket*>::iterator i = jitterBuffer.begin();
    while (i != jitterBuffer.end())
    {
      cout << "jitter deleting: " << (i->second) << endl;
        delete (i->second);
        jitterBuffer.erase(i);
        i = jitterBuffer.begin();
    }
    assert (jitterBuffer.empty());
}
 
 
 
void RtpReceiver::printBuffer ()
{
    map<RtpSeqNumber, RtpPacket*>::iterator i = jitterBuffer.begin();
    while (i != jitterBuffer.end())
    {
        cerr << (i->second)->getSequence() <<" ";
        i++;
    }
    cerr <<endl;
}
*/

/* --- Session state functions ------------------------------------- */
void RtpReceiver::emptyNetwork ()
{
    // set up network activit
    fd_set netFD;
    FD_ZERO (&netFD);
    FD_SET (myStack->getSocketFD(), &netFD);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    // create empty holder packet
    RtpPacket* p = new RtpPacket (RECV_BUF);
    assert (p);

    // receive packets until no more activity
    int len;
    int selret = select (myStack->getSocketFD() + 1,
                         &netFD, NULL, NULL, &timeout);
    while (selret > 0)
    {
        len = myStack->receive (p->getPacketData(), p->getPacketAlloc());
        if (len <= 0) break;
        FD_ZERO (&netFD);
        FD_SET (myStack->getSocketFD(), &netFD);
        selret = select (myStack->getSocketFD() + 1, &netFD, NULL, NULL, &timeout);
    }
    delete p; p = NULL;

    cpDebug(LOG_DEBUG_STACK, "RtpReceiver: Done empty network queue");
}


/* --- Private Information for RTCP -------------------------------- */

void RtpReceiver::setRTCPrecv (RtcpReceiver* s)
{
    rtcpRecv = s;
}


void RtpReceiver::setApiFormat (RtpPayloadType newtype, int no_samples, int packetSize,
                                RtpPacket* p, bool print)
{
	apiFormat = newtype;
	api_pktSampleSize = no_samples;
	apiFormat_perSampleSize = 1;

	switch (newtype)
	{
		case rtpPayloadPCMU:
		case rtpPayloadPCMA:
			if (print)
			{
				cpDebug(LOG_DEBUG, "Setting api format to: PCMU %d", no_samples);
			}
			apiFormat_clockRate = 8000;
			break;
		case rtpPayloadL16_mono:
			if (print)
			{
				cpDebug(LOG_DEBUG, "Setting api format to: L16 %d", no_samples);
			}
			apiFormat_clockRate = 44100;
			apiFormat_perSampleSize = 2;
			break;
		case rtpPayloadG729:
			if (print)
			{
				cpDebug(LOG_DEBUG, "Setting api format to: G729 %d", no_samples);
			}
			apiFormat_clockRate = 8000;
			break;
		default:
			cpLog(LOG_ERR, "apiFormat: codec(%d) at sampleSize(%d) packetSize(%d)",
				(int)newtype, no_samples, packetSize);
			apiFormat_clockRate = 8000;
	}
	
	if (p)
		apiFormat_payloadSize = p->getPayloadUsage();
	else
		apiFormat_payloadSize = api_pktSampleSize * apiFormat_perSampleSize;

	if( packetSize != 0 )
		apiFormat_payloadSize = packetSize;
}

void RtpReceiver::setNetworkFormat (RtpPayloadType newtype, int no_samples, int packetSize, 
                                    RtpPacket* p, bool print)
{
	networkFormat = newtype;
	network_pktSampleSize = no_samples;
	networkFormat_perSampleSize = 1;

	switch (newtype)
	{
		case rtpPayloadPCMU:
		case rtpPayloadPCMA:
			if (print)
			{
				cpDebug(LOG_DEBUG, "Setting network format to: PCMU %d", no_samples);
			}
			networkFormat_clockRate = 8000;
			break;
		case rtpPayloadL16_mono:
			if (print)
			{
				cpDebug(LOG_DEBUG, "Setting network format to: L16 %d", no_samples);
			}
			networkFormat_clockRate = 44100;
			networkFormat_perSampleSize = 2;
			break;
		default:
			cpLog(LOG_ERR, "networkFormat: codec(%d) at sampleSize(%d) packetSize(%d)",
				(int)newtype, no_samples, packetSize);
			networkFormat_clockRate = 8000;
	}
	
	if (p)
		networkFormat_payloadSize = p->getPayloadUsage();
	else
		networkFormat_payloadSize = network_pktSampleSize * networkFormat_perSampleSize;

	if( packetSize != 0 )
		networkFormat_payloadSize = packetSize;
}

void RtpReceiver::setCodecString(const char* codecStringInput)
{
	strncpy(codecString, codecStringInput, strlen(codecStringInput) + 1);
	cpDebug(LOG_DEBUG, "set CodecString %s", codecString);
}

