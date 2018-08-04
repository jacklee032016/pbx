#include "layer1.h"
#include "helper.h"

#define ASHFC_COEFF_NUMBER			8     /* number of frequencies to be analyzed */
#define ASHFC_DTMF_TRESH     			200000L /* above this is dtmf (square of)*/

/* For DTMF recognition:
 * 2 * cos(2 * PI * k / N) precalculated for all k
 */
static u64 ashfc_cos2k_coeffs[ASHFC_COEFF_NUMBER] =
{
	/* k << 15 (source: hfc-4s/8s documentation (www.colognechip.de)) */
	55960, 53912, 51402, 48438, 38146, 32650, 26170, 18630
};

/* digit matrix */
static char ashfc_dtmf_matrix[4][4] =
{
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

void ashfc_dtmf_decode(unsigned char *data, int len)
{
	unsigned char what;
	signed int *hfccoeff;
	signed int sk, sk1, sk2;
	int k, n, i;
	signed int result[ASHFC_COEFF_NUMBER], threshold, threshold_low;
	int lowgroup, highgroup;

//	printk("\nASHFC : DTMF is detecting\n");
	if (len < 64)
	{
		if (len > 0)
			printk(KERN_ERR "%s: coefficients have invalid size. (is=%d < must=%d)\n",	__FUNCTION__, len, 64);
		return ;
	}
	
	hfccoeff = (signed int *)data;
	for (k = 0; k < ASHFC_COEFF_NUMBER; k++)
	{/* power amplitude for 8 frequencies */
		sk2 = (*hfccoeff++)>>4;
		sk = (*hfccoeff++)>>4;
		
		if (sk>32767 || sk<-32767 || sk2>32767 || sk2<-32767)
			printk(KERN_WARNING "DTMF-Detection overflow\n");

		/* compute |X(k)|**2 */
		result[k] = (sk * sk) - (((ashfc_cos2k_coeffs[k] * sk) >> 15) * sk2) + (sk2 * sk2);
	}
	
	threshold = 0;
	for (i = 0; i < ASHFC_COEFF_NUMBER; i++)
	{
		if (result[i] < 0)
			result[i] = 0;
		if (result[i] > ASHFC_DTMF_TRESH)
		{
			if (result[i] > threshold)
				threshold = result[i];
		}
	}

	if (threshold == 0)
	{
		what = 0;
		return ;
	}

#if 0
//	if (dsp_debug & DEBUG_DSP_DTMFCOEFF)
		printk(KERN_ERR  "a %3d %3d %3d %3d %3d %3d %3d %3d"
			" tr:%3d r %3d %3d %3d %3d %3d %3d %3d %3d\n",
			result[0]/10000, result[1]/10000, result[2]/10000,
			result[3]/10000, result[4]/10000, result[5]/10000,
			result[6]/10000, result[7]/10000, threshold/10000,
			result[0]/(threshold/100), result[1]/(threshold/100),
			result[2]/(threshold/100), result[3]/(threshold/100),
			result[4]/(threshold/100), result[5]/(threshold/100),
			result[6]/(threshold/100), result[7]/(threshold/100));
#endif

	/* calc digit (lowgroup/highgroup) */
	lowgroup = highgroup = -1;
	
	threshold_low = threshold/8;// >> 3;  /* tones which are not on, must be below 9 dB */
	threshold = threshold/4;// >> 2;  /* touchtones must match within 6 dB */
	for (i = 0; i < ASHFC_COEFF_NUMBER; i++)
	{
		if (result[i] < threshold_low )
			continue;  /* ignore */
		if (result[i] < threshold )
		{
			lowgroup = highgroup = -1;
			break;  /* noise inbetween */
		}

		/* good level found. This is allowed only one time per group */
		if (i < ASHFC_COEFF_NUMBER/2)
		{
			/* lowgroup*/
			if (lowgroup >= 0)
			{
				// Bad. Another tone found. */
				lowgroup = -1;
				break;
			}
			else
				lowgroup = i;
		}
		else
		{
			/* higroup */
			if (highgroup >= 0)
			{
				// Bad. Another tone found. */
				highgroup = -1;
				break;
			}
			else
				highgroup = i-(ASHFC_COEFF_NUMBER/2);
		}
	}

	/* get digit or null */
	what = 0;
	if (lowgroup>=0 && lowgroup<4&& highgroup>=0 && highgroup< 4)
	{
		what = ashfc_dtmf_matrix[lowgroup][highgroup];
		printk("\n\nASHFC : DTMF digit '%c' is detected\n\n", what);
	}
	return ;
}

