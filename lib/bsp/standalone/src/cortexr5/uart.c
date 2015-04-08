/******************************************************************************
*
* Copyright (C) 2014 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information  of Xilinx, Inc.
* and is protected under U.S. and  international copyright and other
* intellectual property  laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any  rights to the
* materials distributed herewith. Except as  otherwise provided in a valid
* license issued to you by  Xilinx, and to the maximum extent permitted by
* applicable law:
* (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND  WITH ALL FAULTS, AND
* XILINX HEREBY DISCLAIMS ALL WARRANTIES  AND CONDITIONS, EXPRESS, IMPLIED,
* OR STATUTORY, INCLUDING  BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE
* and
* (2) Xilinx shall not be liable (whether in contract or tort,  including
* negligence, or under any other theory of liability) for any loss or damage of
* any kind or nature  related to, arising under or in connection with these
* materials, including for any direct, or any indirect,  special, incidental,
* or consequential loss or damage  (including loss of data, profits, goodwill,
* or any type of  loss or damage suffered as a result of any action brought
* by a third party) even if such damage or loss was  reasonably foreseeable
* or Xilinx had been advised of the  possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe  performance, such as life-support or
* safety devices or  systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any  other applications
* that could lead to death, personal  injury, or severe property or environmental
* damage  (individually and collectively, "Critical  Applications").
* Customer assumes the sole risk and liability of any use of Xilinx products in
* Critical  Applications, subject only to applicable laws and  regulations
* governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file uart.c
*
* This file contains APIs for configuring the UART.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 5.00 	pkp  02/20/14 First release
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

#include "xil_types.h"
#include "xparameters.h"
#include "xil_assert.h"
#include "xil_io.h"

/* Register offsets */
#define UART_CR_OFFSET		0x00000000U
#define UART_MR_OFFSET		0x00000004U
#define UART_BAUDGEN_OFFSET	0x00000018U
#define UART_BAUDDIV_OFFSET	0x00000034U

#define MAX_BAUD_ERROR_RATE	0x00000003U	/* max % error allowed */
#define UART_BAUDRATE	115200U
#define CSU_VERSION_REG     0xFFCA0044U

void Init_Uart(void);

void Init_Uart(void)
{
#ifdef STDOUT_BASEADDRESS
	u8 IterBAUDDIV;		/* Iterator for available baud divisor values */
	u32 BRGR_Value;		/* Calculated value for baud rate generator */
	u32 CalcBaudRate;	/* Calculated baud rate */
	u32 BaudError;		/* Diff between calculated and requested baud rate */
	u32 Best_BRGR = 0U;	/* Best value for baud rate generator */
	u8 Best_BAUDDIV = 0U;	/* Best value for baud divisor */
	u32 Best_Error = 0xFFFFFFFFU;
	u32 PercentError;
	u32 InputClk;
   u32 BaudRate = UART_BAUDRATE;

#if (STDOUT_BASEADDRESS == XPAR_XUARTPS_0_BASEADDR)
	InputClk = XPAR_XUARTPS_0_UART_CLK_FREQ_HZ;
#elif (STDOUT_BASEADDRESS == XPAR_XUARTPS_1_BASEADDR)
	InputClk = XPAR_XUARTPS_1_UART_CLK_FREQ_HZ;
#else
	/* STDIO is not set or axi_uart is being used for STDIO */
	return;
#endif
InputClk = 25000000U;
	/*
	 * Determine the Baud divider. It can be 4to 254.
	 * Loop through all possible combinations
	 */
	for (IterBAUDDIV = 4U; IterBAUDDIV < 255U; IterBAUDDIV++) {

		/*
		 * Calculate the value for BRGR register
		 */
		BRGR_Value = InputClk / (BaudRate * ((u32)IterBAUDDIV + 0x00000001U));

		/*
		 * Calculate the baud rate from the BRGR value
		 */
		CalcBaudRate = InputClk/ (BRGR_Value * ((u32)IterBAUDDIV + 0x00000001U));

		/*
		 * Avoid unsigned integer underflow
		 */
		if (BaudRate > CalcBaudRate) {
			BaudError = BaudRate - CalcBaudRate;
		} else {
			BaudError = CalcBaudRate - BaudRate;
		}

		/*
		 * Find the calculated baud rate closest to requested baud rate.
		 */
		if (Best_Error > BaudError) {

			Best_BRGR = BRGR_Value;
			Best_BAUDDIV = IterBAUDDIV;
			Best_Error = BaudError;

		}
	}

	/*
	 * Make sure the best error is not too large.
	 */
	PercentError = (Best_Error * 100U) / BaudRate;
	if (MAX_BAUD_ERROR_RATE < PercentError) {
		return;
	}

	/* set CD and BDIV */
	Xil_Out32(STDOUT_BASEADDRESS + UART_BAUDGEN_OFFSET, Best_BRGR);
	Xil_Out32(STDOUT_BASEADDRESS + UART_BAUDDIV_OFFSET, (u32)Best_BAUDDIV);

    /*
     * Veloce specific code
     */
    if((Xil_In32(CSU_VERSION_REG) & 0x0000F000U) == 0x00002000U ) {
	Xil_Out32(STDOUT_BASEADDRESS + UART_BAUDGEN_OFFSET, 0x00000002U);
	    Xil_Out32(STDOUT_BASEADDRESS + UART_BAUDDIV_OFFSET, 0x00000004U);
    }

	/*
	 * 8 data, 1 stop, 0 parity bits
	 * sel_clk=uart_clk=APB clock
	 */
	Xil_Out32(STDOUT_BASEADDRESS + UART_MR_OFFSET, 0x00000020U);

	/* enable Tx/Rx and reset Tx/Rx data path */
	Xil_Out32((STDOUT_BASEADDRESS + UART_CR_OFFSET), 0x00000017U);

	return;
#endif
}
