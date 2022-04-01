#ifndef _ALTERA_HPS_0_H_
#define _ALTERA_HPS_0_H_

/*
 * This file was automatically generated by the swinfo2header utility.
 * 
 * Created from SOPC Builder system 'hps' in
 * file './hps.sopcinfo'.
 */

/*
 * This file contains macros for module 'hps_0' and devices
 * connected to the following master:
 *   h2f_lw_axi_master
 * 
 * Do not include this header file and another header file created for a
 * different module or master group at the same time.
 * Doing so may result in duplicate macro names.
 * Instead, use the system header file which has macros with unique names.
 */

/*
 * Macros for device 'ledReg', class 'altera_avalon_pio'
 * The macros are prefixed with 'LEDREG_'.
 * The prefix is the slave descriptor.
 */
#define LEDREG_COMPONENT_TYPE altera_avalon_pio
#define LEDREG_COMPONENT_NAME ledReg
#define LEDREG_BASE 0x0
#define LEDREG_SPAN 16
#define LEDREG_END 0xf
#define LEDREG_BIT_CLEARING_EDGE_REGISTER 0
#define LEDREG_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LEDREG_CAPTURE 0
#define LEDREG_DATA_WIDTH 8
#define LEDREG_DO_TEST_BENCH_WIRING 0
#define LEDREG_DRIVEN_SIM_VALUE 0
#define LEDREG_EDGE_TYPE NONE
#define LEDREG_FREQ 50000000
#define LEDREG_HAS_IN 0
#define LEDREG_HAS_OUT 1
#define LEDREG_HAS_TRI 0
#define LEDREG_IRQ_TYPE NONE
#define LEDREG_RESET_VALUE 0


#endif /* _ALTERA_HPS_0_H_ */
