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
/*****************************************************************************
*
* @file sleep.c
*
* This function provides a second delay using the Global Timer register in
* the ARM Cortex R5 MP core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.00 	pkp  	 02/20/14 First release
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "sleep.h"
#include "xtime_l.h"
#include "xparameters.h"

/*****************************************************************************/
/*
*
* This API is used to provide delays in seconds. Maximum value of seconds
* attained with sleep routine is 10995
*
* @param	seconds requested
*
* @return	0 always
*
* @note		None.
*
****************************************************************************/

s32 sleep(u32 seconds)
{

	XTime tEnd, tCur;

	/* Disable ttc Timer */
	Xil_Out32(TTC3_BASEADDR + 0x0000000CU,0x00000001U);
	/*set prescale value*/
	Xil_Out32(TTC3_BASEADDR + 0x00000000U,0x0000000DU);
	/*write interval value to register*/
	Xil_Out32(TTC3_BASEADDR + 0x00000024U,0xFFFFFFFFU);
	/*write match value to register*/
	Xil_Out32(TTC3_BASEADDR + 0x00000030U,0xFFFFFFFFU);
	/* Enable ttc Timer */
	Xil_Out32(TTC3_BASEADDR + 0x0000000CU,0x0000001AU);

	XTime_GetTime(&tCur);

	tEnd  = tCur + (((XTime) seconds) * COUNTS_PER_SECOND);
	do
	{
	    XTime_GetTime(&tCur);

	} while (tCur < tEnd);

	/* Disable ttc Timer */
	Xil_Out32(TTC3_BASEADDR + 0x0000000CU, Xil_In32(TTC3_BASEADDR + 0x0000000CU) | 0x00000001U);

	return 0;
}
