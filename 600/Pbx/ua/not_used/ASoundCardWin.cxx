// ASoundCardWin.cpp: implementation of the ASoundCardWin class.
//
//////////////////////////////////////////////////////////////////////

#include <atlbase.h>
#include <iostream.h>
#include <assert.h>

#include "ASoundCardWin.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ASoundCardAudioFormat::ASoundCardAudioFormat()
{
	unsigned numChannels = 1;
	unsigned sampleRate = 8000;
	unsigned bitsPerSample = 16;

	m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_waveFormat.nChannels = (WORD)numChannels;
	m_waveFormat.nSamplesPerSec = sampleRate;
	m_waveFormat.wBitsPerSample = (WORD)bitsPerSample;
//	m_waveFormat.nBlockAlign = (WORD)(numChannels*(bitsPerSample+7)/8);
	m_waveFormat.nBlockAlign = 2;
//	m_waveFormat.nAvgBytesPerSec = m_waveFormat.nSamplesPerSec * m_waveFormat.nBlockAlign;
	m_waveFormat.nAvgBytesPerSec = (sampleRate*bitsPerSample)/8;
	m_waveFormat.cbSize = 0;
}


ASoundCardAudioFormat::~ASoundCardAudioFormat()
{
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ASoundCardWin::ASoundCardWin()
{
	m_hWaveOut = NULL;
	m_hWaveIn = NULL;
	m_eDirection = SoundOut;
	m_hEventDone = CreateEvent(NULL, FALSE, FALSE, NULL); //AND:TODO:need destroy event ?
/*	for (int i = 0 ; i < 5 ; i++) {
		m_WAVEHDR[i].lpData = m_lpData[i];
		m_WAVEHDR[i].dwBufferLength = 512; //AND: Magic number :0\ 
		m_WAVEHDR[i].dwFlags = WHDR_DONE;
	}
*/	m_nWHIndex = 0;
	m_pBuffers = 0;
	SetBuffers(5, 512); //AND: Magic numbers :-/
}


ASoundCardWin::~ASoundCardWin()
{

}

//////////////////////////////////////////////////////////////////////

bool ASoundCardWin::Open(
	SoundCardDirection eDirection,
	const ASoundCardAudioFormat &aAudioFormat
	)
{
	//if (!aAudioFormat.IsOk())
	//	return	false;
	if (!m_pBuffers)
		SetBuffers(15, 512);
	if (!m_pBuffers)
		return	false;
	if (eDirection == SoundIn) {
		MMRESULT osError;

		osError = waveInOpen(
			&m_hWaveIn,
			WAVE_MAPPER,
			&m_aAudioFormat.m_waveFormat,
			(DWORD)m_hEventDone,
			0,
			CALLBACK_EVENT);
		if (osError != MMSYSERR_NOERROR) {
			m_hWaveIn = NULL; //AND:Bulletproof
			return false;
		}
	} else if (eDirection == SoundOut) {
		MMRESULT osError;

		osError = waveOutOpen(
			&m_hWaveOut,
			WAVE_MAPPER,
			&m_aAudioFormat.m_waveFormat,
			(DWORD)m_hEventDone,
			0,
			CALLBACK_EVENT);
		if (osError != MMSYSERR_NOERROR) {
			m_hWaveOut = NULL; //AND:Bulletproof
			return false;
		}
	} else
		return	false;
	m_aAudioFormat = aAudioFormat;
	m_nBuffByteOffset = -1;
	return	true;
}


bool ASoundCardWin::IsOpen()
{
	return	m_hWaveIn || m_hWaveOut;
}


bool ASoundCardWin::SetBuffers(int nCount, int nSize)
{
	if (IsOpen())
		return	false;
	if (nCount <= 0 || nCount >= 64)
		return	false;
	if (nSize <= 16 || nSize >= 0xFFFF)
		return	false;
	if (m_pBuffers) 
		delete m_pBuffers;
	m_pBuffers = new Buffer[nCount];  //AND:
	m_nBuffersCount = nCount;
	m_nBuffersSize = nSize;
	if (!m_pBuffers)
		return	false;
	for (int i = 0; i < m_nBuffersCount ; i++) {
		m_pBuffers[i].SetSize(nSize);
		assert(m_pBuffers[i].IsOk());
	}
	m_nWHIndex = 0;
	return	true;
}

	
int ASoundCardWin::Read(char *pBuff, int nBytes)
{
	MMRESULT osError;
	int nBytesRead = 0;

	if (!pBuff || nBytes <= 0 || m_hWaveIn == NULL || m_pBuffers == 0)
		return	0; //AND:Bugproof
	//    deviceMutex.lock();
	//
	//AND:first read after open -
	//AND:while have empty buffers - add him's into queve
	if (m_nBuffByteOffset < 0) {
		for (int i = 0; i < m_nBuffersCount ;i++) {
			assert(m_pBuffers[i].Reset());
			if ((osError = waveInPrepareHeader(m_hWaveIn,
				&m_pBuffers[i].waveHeader, sizeof(WAVEHDR)))
				!= MMSYSERR_NOERROR)
				return	0;
			
			if ((osError = waveInAddBuffer(m_hWaveIn,
				&m_pBuffers[i].waveHeader, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
				return	0; 
		}
		if ((osError = waveInStart(m_hWaveIn)) == MMSYSERR_NOERROR) // start recording
			m_nBuffByteOffset = 0;
		else
			return	0;

	}

	//Get data from buffers & add empty buffers into queve
	while (nBytes > 0) {
		Buffer & buffer = m_pBuffers[m_nWHIndex];

		// No free buffers, so wait for one
		while ((buffer.waveHeader.dwFlags & WHDR_DONE) == 0) {
			if (WaitForSingleObject(m_hEventDone, INFINITE) != WAIT_OBJECT_0) {
				//			    SetFormat(numChannels, sampleRate, bitsPerSample);
				return nBytesRead;
			}
		}
		int nCount = nBytes;
		if (nCount > (m_nBuffersSize - m_nBuffByteOffset)) //512 - buffer size
			nCount = m_nBuffersSize - m_nBuffByteOffset;

		memcpy(pBuff, buffer.waveHeader.lpData + m_nBuffByteOffset, nCount);

		m_nBuffByteOffset += nCount;
		pBuff += nCount;
		nBytes -= nCount;
		nBytesRead += nCount;

		//Buffer empty - set him into queve
		if (m_nBuffByteOffset >= m_nBuffersSize) {
			m_nBuffByteOffset = 0;
			m_nWHIndex = (m_nWHIndex+1) % m_nBuffersCount;
			assert(buffer.Reset());
			if ((osError = waveInPrepareHeader(m_hWaveIn, &buffer.waveHeader, sizeof(WAVEHDR)))
				!= MMSYSERR_NOERROR)
				return	nBytesRead;
			
			if ((osError = waveInAddBuffer(m_hWaveIn, &buffer.waveHeader, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
				return	nBytesRead; 
		}
	}
	return	nBytesRead;
}


int ASoundCardWin::Write(char *pBuff, int nBytes)
{
	MMRESULT osError;
	int nBytesWrite = 0;

	if (!pBuff || nBytes <= 0 || m_hWaveOut == NULL || m_pBuffers == 0)
		return	0; //AND:Bugproof
	while (nBytes > 0) {
		Buffer & buffer = m_pBuffers[m_nWHIndex];
		while ((buffer.waveHeader.dwFlags & WHDR_DONE) == 0) {
			// No free buffers, so wait for one
			if (WaitForSingleObject(m_hEventDone, INFINITE) != WAIT_OBJECT_0) {
				return nBytesWrite;
			}
		}
		
		// Can't write more than a buffer full
		int nCount = nBytes;
		
		if (nCount > m_nBuffersSize) //512 - buffer size
			nCount = m_nBuffersSize;
		assert(buffer.Reset());
		buffer.waveHeader.dwBufferLength = nCount;
		if ((osError = waveOutPrepareHeader(m_hWaveOut, &buffer.waveHeader, sizeof(WAVEHDR)))
			!= MMSYSERR_NOERROR)
			break;
		
		memcpy(buffer.waveHeader.lpData, pBuff, nCount);
		if ((osError = waveOutWrite(m_hWaveOut, &buffer.waveHeader, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
			break; 
		
		m_nWHIndex = (m_nWHIndex+1) % m_nBuffersCount;
		pBuff += nCount;
		nBytesWrite += nCount;
		nBytes -= nCount;
	}
	return	nBytesWrite;
}

/*
ASoundCardWin::Status()
{
}
*/

void ASoundCardWin::Close()
{
	MMRESULT osError;

	if (m_hWaveOut != NULL) {
		while ((osError = waveOutClose(m_hWaveOut)) == WAVERR_STILLPLAYING)
			waveOutReset(m_hWaveOut);
		m_hWaveOut = NULL;
	}
	if (m_hWaveIn != NULL) {
		while ((osError = waveInClose(m_hWaveIn)) == WAVERR_STILLPLAYING)
			waveInReset(m_hWaveIn);
		m_hWaveIn = NULL;
	}
}
