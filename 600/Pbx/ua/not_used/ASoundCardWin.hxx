// ASoundCardWin.h: interface for the ASoundCardWin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASOUNDCARDWIN_H__40EDB79D_D875_4619_AE94_DE9CAA028E58__INCLUDED_)
#define AFX_ASOUNDCARDWIN_H__40EDB79D_D875_4619_AE94_DE9CAA028E58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <mmsystem.h>

class ASoundCardWin;

class ASoundCardAudioFormat
{
	ASoundCardAudioFormat();
	virtual ~ASoundCardAudioFormat();

protected:
	WAVEFORMATEX m_waveFormat;

friend ASoundCardWin;
};


/// Audio Stream direction
enum SoundCardDirection {
	SoundIn, //record sound (from microphone - as sample)
	SoundOut, //play sound
	MaxDirection
};


class ASoundCardWin
{
public:
	ASoundCardWin();
	virtual ~ASoundCardWin();

	///
	bool Open(
		SoundCardDirection eDirection,
		const ASoundCardAudioFormat &aAudioFormat
		);
	///open default device with default stream format
	bool Open(SoundCardDirection eDirection) {
		ASoundCardAudioFormat aFormat;

		return	Open(eDirection, aFormat);
	}
	bool IsOpen();
	bool SetBuffers(int nCount, int nSize);
	int Read(char *pBuff, int nBytes);
	int Write(char *pBuff, int nBytes);
	Status();
	SoundCardDirection GetDirection() {
		return m_eDirection;
	}
	void Close();

protected:
	SoundCardDirection m_eDirection;
	ASoundCardAudioFormat m_aAudioFormat;

	//AND:Win32 specific members (handles, etc...)
	HWAVEIN	m_hWaveIn;
	HWAVEOUT m_hWaveOut;
	HANDLE	m_hEventDone;
	class	Buffer {
	public:
		Buffer() {
			m_pData = 0;
			m_nDataLength = 0;
			Reset();
		}
		~Buffer() {
			delete m_pData;
		}

		void SetSize(int n) {
			delete m_pData;
			m_pData = 0;
			waveHeader.dwBufferLength = 0;
			if (n > 0 && n < 0xFFFF)
				m_pData = new char[n];
			if (m_pData)
				m_nDataLength = n;
			Reset();
		}
		bool Reset() {
			memset(&waveHeader, 0, sizeof(WAVEHDR));
			waveHeader.lpData = m_pData;
			waveHeader.dwBufferLength = m_nDataLength;
			waveHeader.dwFlags = WHDR_DONE;
			return m_pData && m_nDataLength;
		}
		bool IsOk() {
			return waveHeader.dwBufferLength != 0;
		}

		WAVEHDR	waveHeader;
	private:
		char *m_pData;
		int m_nDataLength;
	} *m_pBuffers;
/*	WAVEHDR	m_WAVEHDR[5];
	char m_lpData[5][512];
*/	int m_nBuffersCount;
	int m_nBuffersSize;
	int m_nWHIndex;
	int m_nBuffByteOffset;
};

#endif // !defined(AFX_ASOUNDCARDWIN_H__40EDB79D_D875_4619_AE94_DE9CAA028E58__INCLUDED_)
