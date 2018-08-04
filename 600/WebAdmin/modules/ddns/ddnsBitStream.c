/*
* $Id: ddnsBitStream.c,v 1.2 2007/05/30 19:23:02 lizhijie Exp $
*/

#include "_ddns.h"
#include "ddnsCodec.h"

enum
{
	Begin = 0,
	End = 0xffffffff,
};

int bitstreamCreate(bitstream_t *stream, char *byData, UINT nBufLen)
{
	if(stream->m_pBuf)
	{
		free(stream->m_pBuf);
	}

	stream->m_pBuf = (char *)malloc(sizeof(nBufLen));

	if(stream->m_pBuf)
	{
		memcpy(stream->m_pBuf, byData, nBufLen);
		stream->m_nSize = nBufLen;
		stream->m_dwIndex = 0;
		return TRUE;
	}
	else
	{
		//CDebugger::Instance()->Out("[CBitStream]Error. Not enough memory\xd\xa");
		return FALSE;
	}
}

int  bitstreamInitBuffer(bitstream_t *stream, UINT nBufLen)
{
	if(nBufLen > stream->m_nSize)
	{
		if( stream->m_pBuf)
			free( stream->m_pBuf);
		stream->m_pBuf = (char *)malloc(nBufLen);
		if(stream->m_pBuf)
		{
			memset(stream->m_pBuf, 0, nBufLen);
			stream->m_nSize = nBufLen;
			stream->m_dwIndex = 0;
			return TRUE;
		}
		else 
			return FALSE;
	}
	else
	{
		stream->m_nSize = nBufLen;
		return TRUE;
	}
}

int bitstreamSeek(bitstream_t *stream, long dwIndex)
{
	if(dwIndex > stream->m_nSize * BITSOFBYTE - 1)
		return FALSE;

	if(dwIndex == Begin)
		dwIndex = 0;
	else if(dwIndex == End)
		dwIndex = stream->m_nSize * BITSOFBYTE - 1;
	else
		dwIndex = dwIndex;
	return TRUE;
}

int bitstreamRead(bitstream_t *stream, char *pBuf, UINT nBitCount)
{
	int iRemainIndex = stream->m_dwIndex % BITSOFBYTE;
	int iBufIndicator = stream->m_dwIndex / BITSOFBYTE;
	int iByteCount = (nBitCount - 1) / BITSOFBYTE + 1;
        int i;

	for(i = 0; i < iByteCount && iBufIndicator < stream->m_nSize - 1; i++, iBufIndicator++)
	{
		pBuf[i] = (stream->m_pBuf[iBufIndicator] << iRemainIndex) | 
			(stream->m_pBuf[iBufIndicator + 1] >> (BITSOFBYTE - iRemainIndex));
	}
	if(i == iByteCount)
	{
		stream->m_dwIndex += nBitCount;
		return nBitCount;
	}
	else
	{
		pBuf[i] = stream->m_pBuf[iBufIndicator] << iRemainIndex;

		if(nBitCount <= BITSOFBYTE - iRemainIndex)	//Enough bits to read
		{
			stream->m_dwIndex += nBitCount;
			return nBitCount;
		}
		else
		{
			stream->m_dwIndex += BITSOFBYTE * (i + 1) - iRemainIndex;
			return BITSOFBYTE * (i + 1) - iRemainIndex;
		}
	}
}

int bitstreamWrite(bitstream_t *stream, char *pBuf, UINT nBitCount)
{
	int iRemainIndex = stream->m_dwIndex % BITSOFBYTE;
	int iBufIndicator = stream->m_dwIndex / BITSOFBYTE;
	int iByteCount = (nBitCount - 1) / BITSOFBYTE + 1;
	int i;

	printf("Byte:%c index:%ld m_pBuf:%s ", *pBuf, stream->m_dwIndex, stream->m_pBuf);
	for(i = 0; i < iByteCount && iBufIndicator < stream->m_nSize - 1; i++, iBufIndicator++)
	{
		stream->m_pBuf[iBufIndicator] |= pBuf[i] >> iRemainIndex;
		stream->m_pBuf[iBufIndicator + 1] |= pBuf[i] << (BITSOFBYTE - iRemainIndex);
	}
	if(i == iByteCount)
	{
		stream->m_dwIndex += nBitCount;
		return nBitCount;
	}
	else
	{
		stream->m_pBuf[iBufIndicator] |= pBuf[i] >> iRemainIndex;

		if(nBitCount <= BITSOFBYTE - iRemainIndex)	//Enough bits to read
		{
			stream->m_dwIndex += nBitCount;
			return nBitCount;
		}
		else
		{
			stream->m_dwIndex += BITSOFBYTE * (i + 1) - iRemainIndex;
			return BITSOFBYTE * (i + 1) - iRemainIndex;
		}
	}
}

int bitstreamCopyBuffer(bitstream_t *stream, char *pBuf, UINT nBufLen)
{
	printf("Bitstream %s.\n", stream->m_pBuf);
	if(nBufLen > stream->m_nSize)
	{
		memcpy(pBuf, stream->m_pBuf, stream->m_nSize);
		return stream->m_nSize;
	}
	else
	{
		memcpy(pBuf, stream->m_pBuf, nBufLen);
		return nBufLen;
	}
}

