/***********************************************************************
 *
 * Copyright (C) 2004 by FS Forth-Systeme GmbH.
 * All rights reserved.
 *
 * $Id: ns9750_ser.h,v 1.1.1.1 2006/11/29 09:09:45 lizhijie Exp $
 * @Author: Markus Pietrek
 * @References: [1] NS9750 Hardware Reference, December 2003
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 ***********************************************************************/

#ifndef FS_NS9750_SER_H
#define FS_NS9750_SER_H

#define NS9750_SER_MODULE_BASE		(0x90200000)

#define get_ser_reg_addr(c) \
	((volatile unsigned int *)(NS9750_SER_MODULE_BASE+(unsigned int) (c)))

#define get_ser_reg_addr_channel(reg,chan) \
	get_ser_reg_addr((reg)+(((chan)<2)?0:0x00100000)+(((chan)&1)?0x40:0))

/* the register addresses */

#define NS9750_SER_CTRL_A		(0x00)
#define NS9750_SER_CTRL_B		(0x04)
#define NS9750_SER_STAT_A		(0x08)
#define NS9750_SER_BITRATE		(0x0C)
#define NS9750_SER_FIFO			(0x10)
#define NS9750_SER_RX_BUF_TIMER		(0x14)
#define NS9750_SER_RX_CHAR_TIMER	(0x18)
#define NS9750_SER_RX_MATCH		(0x1C)
#define NS9750_SER_RX_MATCH_MASK	(0x20)
#define NS9750_SER_FLOW_CTRL		(0x34)
#define NS9750_SER_FLOW_CTRL_FORCE	(0x38)

/* register bit fields */

/* control A register */

#define NS9750_SER_CTRL_A_CE		(0x80000000)
#define NS9750_SER_CTRL_A_BRK		(0x40000000)
#define NS9750_SER_CTRL_A_STICKP	(0x20000000)
#define NS9750_SER_CTRL_A_EPS		(0x10000000)
#define NS9750_SER_CTRL_A_PE		(0x08000000)
#define NS9750_SER_CTRL_A_STOP		(0x04000000)
#define NS9750_SER_CTRL_A_WLS_MA	(0x03000000)
#define NS9750_SER_CTRL_A_WLS_5		(0x00000000)
#define NS9750_SER_CTRL_A_WLS_6		(0x01000000)
#define NS9750_SER_CTRL_A_WLS_7		(0x02000000)
#define NS9750_SER_CTRL_A_WLS_8		(0x03000000)
#define NS9750_SER_CTRL_A_CTSTX		(0x00800000)
#define NS9750_SER_CTRL_A_RTSRX		(0x00400000)
#define NS9750_SER_CTRL_A_RL		(0x00200000)
#define NS9750_SER_CTRL_A_LL		(0x00100000)
#define NS9750_SER_CTRL_A_RES		(0x000CF000)
#define NS9750_SER_CTRL_A_DTR		(0x00020000)
#define NS9750_SER_CTRL_A_RTS		(0x00010000)
#define NS9750_SER_CTRL_A_RIE_MA	(0x00000E00)
#define NS9750_SER_CTRL_A_ERXDMA	(0x00000100)
#define NS9750_SER_CTRL_A_RIC_MA	(0x000000E0)
#define NS9750_SER_CTRL_A_TIC_MA	(0x0000001E)
#define NS9750_SER_CTRL_A_ETXDMA 	(0x00000001)

/* control B register */

#define NS9750_SER_CTRL_B_RDM1		(0x80000000)
#define NS9750_SER_CTRL_B_RDM2		(0x40000000)
#define NS9750_SER_CTRL_B_RDM3		(0x20000000)
#define NS9750_SER_CTRL_B_RDM4		(0x10000000)
#define NS9750_SER_CTRL_B_RBGT		(0x08000000)
#define NS9750_SER_CTRL_B_RCGT		(0x04000000)
#define NS9750_SER_CTRL_B_MODE_MA	(0x00300000)
#define NS9750_SER_CTRL_B_MODE_UART	(0x00000000)
#define NS9750_SER_CTRL_B_MODE_HDLC	(0x00100000)
#define NS9750_SER_CTRL_B_MODE_SPI_M	(0x00200000)
#define NS9750_SER_CTRL_B_MODE_SPI_S	(0x00300000)
#define NS9750_SER_CTRL_B_BITORDR	(0x00080000)
#define NS9750_SER_CTRL_B_RES		(0x0007703F)
#define NS9750_SER_CTRL_B_RTSTX		(0x00008000)
#define NS9750_SER_CTRL_B_ENDEC_MA	(0x00000FC0)

/* status A register */

#define NS9750_SER_STAT_A_MATCH1	(0x80000000)
#define NS9750_SER_STAT_A_MATCH2	(0x40000000)
#define NS9750_SER_STAT_A_MATCH3	(0x20000000)
#define NS9750_SER_STAT_A_MATCH4	(0x10000000)
#define NS9750_SER_STAT_A_BGAP		(0x08000000)
#define NS9750_SER_STAT_A_CGAP		(0x04000000)
#define NS9750_SER_STAT_A_RXFDB_MA	(0x00300000)
#define NS9750_SER_STAT_A_RXFDB_FULL	(0x00000000)
#define NS9750_SER_STAT_A_RXFDB_1	(0x00100000)
#define NS9750_SER_STAT_A_RXFDB_2	(0x00200000)
#define NS9750_SER_STAT_A_RXFDB_3	(0x00300000)
#define NS9750_SER_STAT_A_DCD		(0x00080000)
#define NS9750_SER_STAT_A_RI		(0x00040000)
#define NS9750_SER_STAT_A_DSR		(0x00020000)
#define NS9750_SER_STAT_A_CTS		(0x00010000)
#define NS9750_SER_STAT_A_RBRK		(0x00008000)
#define NS9750_SER_STAT_A_RFE		(0x00004000)
#define NS9750_SER_STAT_A_RPE		(0x00002000)
#define NS9750_SER_STAT_A_ROVER		(0x00001000)
#define NS9750_SER_STAT_A_RRDY		(0x00000800)
#define NS9750_SER_STAT_A_RHALF		(0x00000400)
#define NS9750_SER_STAT_A_RBC		(0x00000200)
#define NS9750_SER_STAT_A_RFULL		(0x00000100)
#define NS9750_SER_STAT_A_DCDI		(0x00000080)
#define NS9750_SER_STAT_A_RII		(0x00000040)
#define NS9750_SER_STAT_A_DSRI		(0x00000020)
#define NS9750_SER_STAT_A_CTSI		(0x00000010)
#define NS9750_SER_STAT_A_TRDY		(0x00000008)
#define NS9750_SER_STAT_A_THALF		(0x00000004)
#define NS9750_SER_STAT_A_TBC		(0x00000002)
#define NS9750_SER_STAT_A_TEMPTY	(0x00000001)

#define NS9750_SER_STAT_A_RX_COND_ERR ( NS9750_SER_STAT_A_RFE | \
		 		        NS9750_SER_STAT_A_ROVER | \
				        NS9750_SER_STAT_A_RPE )
#define NS9750_SER_STAT_A_RX_COND_ALL ( NS9750_SER_STAT_A_RX_COND_ERR | \
				        NS9750_SER_STAT_A_RBRK | \
				        NS9750_SER_STAT_A_RRDY | \
				    	NS9750_SER_STAT_A_RHALF | \
				    	NS9750_SER_STAT_A_RBC | \
				    	NS9750_SER_STAT_A_DCDI | \
				    	NS9750_SER_STAT_A_RII | \
				    	NS9750_SER_STAT_A_DSRI | \
				    	NS9750_SER_STAT_A_CTSI )
#define NS9750_SER_STAT_A_TX_COND_ALL ( NS9750_SER_STAT_A_TRDY | \
				        NS9750_SER_STAT_A_THALF | \
				        NS9750_SER_STAT_A_TBC | \
				    	NS9750_SER_STAT_A_TEMPTY )
/* bit rate register */

#define NS9750_SER_BITRATE_EBIT		 (0x80000000)
#define NS9750_SER_BITRATE_TMODE	 (0x40000000)
#define NS9750_SER_BITRATE_RXSRC	 (0x20000000)
#define NS9750_SER_BITRATE_TXSRC	 (0x10000000)
#define NS9750_SER_BITRATE_RXEXT	 (0x08000000)
#define NS9750_SER_BITRATE_TXEXT	 (0x04000000)
#define NS9750_SER_BITRATE_CLKMUX_MA	 (0x03000000)
#define NS9750_SER_BITRATE_CLKMUX_XTAL	 (0x00000000)
#define NS9750_SER_BITRATE_CLKMUX_BCLK	 (0x01000000)
#define NS9750_SER_BITRATE_CLKMUX_OUT1	 (0x02000000)
#define NS9750_SER_BITRATE_CLKMUX_OUT2	 (0x03000000)
#define NS9750_SER_BITRATE_TXCINV	 (0x00800000)
#define NS9750_SER_BITRATE_RXCINV	 (0x00400000)
#define NS9750_SER_BITRATE_TCDR_MA	 (0x00180000)
#define NS9750_SER_BITRATE_TCDR_1 	 (0x00000000)
#define NS9750_SER_BITRATE_TCDR_8 	 (0x00080000)
#define NS9750_SER_BITRATE_TCDR_16	 (0x00100000)
#define NS9750_SER_BITRATE_TCDR_32	 (0x00180000)
#define NS9750_SER_BITRATE_RCDR_MA	 (0x00070000)
#define NS9750_SER_BITRATE_RCDR_1 	 (0x00000000)
#define NS9750_SER_BITRATE_RCDR_8 	 (0x00020000)
#define NS9750_SER_BITRATE_RCDR_16	 (0x00040000)
#define NS9750_SER_BITRATE_RCDR_32	 (0x00060000)
#define NS9750_SER_BITRATE_TICS		 (0x00010000)
#define NS9750_SER_BITRATE_RICS		 (0x00008000)
#define NS9750_SER_BITRATE_N_MA		 (0x00007FFF)

/* receive buffer gap timer */

#define NS9750_SER_RX_BUF_TIMER_TRUN	 (0x80000000) /* UART and SPI */
#define NS9750_SER_RX_BUF_TIMER_BT_MA	 (0x0000FFFF) /* UART and SPI */
#define NS9750_SER_RX_BUF_TIMER_MAXLEN_MA (0x0000FFFF) /* HDLC only */

/* receive character gap timer */

#define NS9750_SER_RX_CHAR_TIMER_TRUN	 (0x80000000)
#define NS9750_SER_RX_CHAR_TIMER_CT_MA	 (0x000FFFFF)

/* receive match */

#define NS9750_SER_RX_MATCH_RDMB1_MA	 (0xFF000000)
#define NS9750_SER_RX_MATCH_RDMB2_MA	 (0x00FF0000)
#define NS9750_SER_RX_MATCH_RDMB3_MA	 (0x0000FF00)
#define NS9750_SER_RX_MATCH_RDMB4_MA	 (0x000000FF)

/* receive match mask */

#define NS9750_SER_RX_MATCH_MASK_RDMB1_MA (0xFF000000)
#define NS9750_SER_RX_MATCH_MASK_RDMB2_MA (0x00FF0000)
#define NS9750_SER_RX_MATCH_MASK_RDMB3_MA (0x0000FF00)
#define NS9750_SER_RX_MATCH_MASK_RDMB4_MA (0x000000FF)

#endif /* FS_NS9750_SER_H */
