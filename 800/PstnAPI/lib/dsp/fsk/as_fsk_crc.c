/*
*
*/

#include <string.h>
#include <stdio.h>
#include "as_fsk.h"
/*
 * The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard.
 */

#define	CRC_DEBUG		1

typedef unsigned short crc;

#define	WIDTH    	(8 * sizeof(crc) )
#define	TOPBIT   	(1 << (WIDTH - 1))

#if __ARM__
#define	NTT_FSK_POLYNOMIAL		0x1021
#else
#define	NTT_FSK_POLYNOMIAL		0x1021
#endif

crc  crcTable[256];

void as_fsk_crc_init(void)
{
	crc  remainder;
	int dividend;
	unsigned char bit;

	/* Compute the remainder of each possible dividend.  */
	for ( dividend = 0; dividend < 256; ++dividend)
	{
		/* Start with the dividend followed by zeros.*/
#if __ARM__
		remainder = dividend *256;//<< (WIDTH - 8);
#else
		remainder = dividend << (WIDTH - 8);
#endif
		/* Perform modulo-2 division, a bit at a time.  */
		for (bit = 8; bit > 0; --bit)
		{
			/** Try to divide the current data bit.  */			
			if (remainder & TOPBIT)
			{
#if __ARM__
				remainder = (remainder << 1) ^ NTT_FSK_POLYNOMIAL;
#else
				remainder = (remainder << 1) ^ NTT_FSK_POLYNOMIAL;
#endif
			}
			else
			{
#if __ARM__
				remainder = (remainder << 1);
#else
				remainder = (remainder << 1);
#endif
			}
		}

		crcTable[dividend] = remainder;
	}
}

unsigned short as_fsk_crc(unsigned char *msg, int nBytes)
{
	unsigned char  data;
	crc	         remainder = 0;
	int byte, bit;
	unsigned char message[NTT_FSK_BLOCK_LENGTH];
	unsigned char tmp, t2;
	int count;
	int parity = 0;
	unsigned char *hi, *low;

	memset(message, 0 , NTT_FSK_BLOCK_LENGTH);
	count = nBytes>NTT_FSK_BLOCK_LENGTH? NTT_FSK_BLOCK_LENGTH:nBytes;

#if 0
	memcpy(message, msg, count );
	for ( byte = 0; byte < count; byte++)
	{
		tmp = 0;
		parity = 0;
#if CRC_DEBUG	
		printf("0x%x", message[byte]);
#endif
		for(bit=0; bit<8; bit++)
		{
			if( (message[byte]&(1<<bit) ) )
			{
				parity++;
				tmp |= (1<<(7-bit) );
			}	
		}
		message[byte] = tmp;
		if(parity%2)
		{
			msg[byte] |= NTT_FSK_PARITY_BIT;
			message[byte] |=0x01;
		}	
#if CRC_DEBUG	
		printf("(0x%x) ", message[byte]);
#endif
	}
#if CRC_DEBUG	
	printf("\n");
#endif
#else
	for ( byte = 0; byte < count; byte++)
	{
		parity = 0;

		for(bit=0; bit<8; bit++)
		{
			if( (msg[byte]&(1<<bit) ) )
			{
				parity++;
			}	
		}
		if(parity%2)
		{
			msg[byte] |= NTT_FSK_PARITY_BIT;
		}	
	}

	memcpy(message, msg, count );

	for ( byte = 0; byte < count; byte++)
	{
		tmp = 0;

		for(bit=0; bit<8; bit++)
		{
			if( (message[byte]&(1<<bit) ) )
			{
				tmp |= (1<<(7-bit) );
			}	
		}
		message[byte] = tmp;
	}

#endif

	/* Divide the message by the polynomial, a byte at a time. */
	for ( byte = 0; byte < count; ++byte)
	{
#if __ARM__	
		data = message[byte] ^ (remainder/256);// >> (WIDTH - 8));
		remainder = crcTable[data] ^ (remainder*256);// << 8);
#else
		data = message[byte] ^ (remainder >> (WIDTH - 8));
		remainder = crcTable[data] ^ (remainder << 8);
#endif
	}
	hi = (unsigned char *)&remainder;
	low = hi+1;
	tmp = t2 = 0;

	for(bit=0; bit<8; bit++)
	{
		if( ((*hi)&(1<<bit) ) )
		{
			tmp |= (1<<(7-bit) );
		}
		if( ((*low)&(1<<bit) ) )
		{
			t2 |= (1<<(7-bit) );
		}
		
	}
	*hi = tmp;
	*low = t2;

	/* The final remainder is the CRC. */
	return (remainder);
	
}

