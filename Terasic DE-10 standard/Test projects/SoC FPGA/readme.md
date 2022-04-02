# My first SoC FPGA project

The goal of this project is to be able to use the **SoC FPGA** capabilities of **Terasic's DE-10 Standard board** in order to create a quick **Platform designer** (formerly known as Qsys) component to program an 8 bit register **(Parallel I/O IP)** on the FPGA fabric as an **Avalon memory mapped slave**, then we will compile and build it using Quartus to extract the **.sof** and turn it into a compressed binary **.rbf** file to program the FPGA part of the board using the script **reconfigureFPGA** shell file that I have provided. Additionally, we will go over how to write a **C** code that would run on your **Hard processor system** (HPS) and how to compile the program successfully using the ARM GCC GNU compiler as well as how to transmit the files over to the DE-10.

Special thanks to [Mr. Bo Gao](https://www.youtube.com/user/bskull3232 "Mr. Bo Gao youtube channel") for his amazing tutorial series for the DE-10.

Without further ado, let's get started! 


## Dependencies

1. [Quartus 18.1 lite (Windows)](https://www.intel.com/content/www/us/en/software-kit/664781/intel-quartus-prime-pro-edition-design-software-version-18-1-for-windows.html "Quartus 18.1 lite \(Windows\)")
2. [SoC EDS shell 18.1 lite (Windows)](https://www.intel.com/content/www/us/en/software-kit/665461/intel-soc-fpga-embedded-development-suite-standard-edition-software-version-18-1-for-windows.html "SoC EDS shell 18.1 lite \(Windows\)") 

3. [Terasic DE10-Standard console linux](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=205&No=1081&PartNo=4 "Terasic DE10-Standard console linux")

4. [Terasic DE10-Standard System Builder](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=205&No=1081&PartNo=4 "Terasic DE10-Standard System Builder")

5. Windows subsystem linux (Optional)

6. Putty or any other terminal app


## Running the Linux kernel

1. Download the linux img from the website in the dependencies and flash it onto a **class 4+** sd card using **rufus** or **win32diskimager**

2. change the MSEL[4:0] dipswitch pins
10010 means that the FPGA is configured from the EPCS (default) 
01010  means that the FPGA is configured from HPS software: Linux  
Refer to the Quick start guide if you are still stuck here.

3. install the necessary FTDI drivers if needed from:
https://ftdichip.com/drivers/

4. Plug in your USB Type mini-B cable cable and power cable

5. UART settings for your terminal
Baud rate: 115200
Data bits: 8 
Parity: None 
Stop Bits: 1 
Flow Control: no

6. Username: root and no password

## System builder verilog template

We can use the **system builder** provided by **Terasic** to generate a Quartus project file **.qpf** file of the wires and connections that we will be using. Simply, open the program and select the components that you want and then generate then we will import to this file to Quartus.

To make this led test program we need to enable the CLK, LEDS and HPS then we can generate the **.qpf** file

Open the generated **.qpf** file with Quartus

## Minor edits

You can remove the other clocks from the **.v** file in Quartus, as we only need "input CLOCK_50," and we can edit our leds from [9:0] 10 leds to only [7:0] 8 leds.

Things that we don't need:
1. We can remove the SPI interface if we don't need it by deleting:
```
output		          		HPS_SPIM_CLK,
input 		          		HPS_SPIM_MISO,
output		          		HPS_SPIM_MOSI,
output		          		HPS_SPIM_SS,
```

2. We also don't need the external reset, the LTC_GPIO, HPS_LED, LCM (LCD), I2C or GSENSOR.
```	
inout 		          		HPS_GSENSOR_INT,
inout 		          		HPS_I2C1_SCLK,
inout 		          		HPS_I2C1_SDAT,
inout 		          		HPS_I2C2_SCLK,
inout 		          		HPS_I2C2_SDAT,
inout 		          		HPS_I2C_CONTROL,
inout 		          		HPS_KEY,
inout 		          		HPS_LCM_BK,
inout 		          		HPS_LCM_D_C,
inout 		          		HPS_LCM_RST_N,
output		          		HPS_LCM_SPIM_CLK,
input 		          		HPS_LCM_SPIM_MISO,
output		          		HPS_LCM_SPIM_MOSI,
output		          		HPS_LCM_SPIM_SS,
inout 		          		HPS_LED,
inout 		          		HPS_LTC_GPIO,
```
3. we don't need the flash interface, since we already have the sd one:
```	
inout 		     [3:0]		HPS_FLASH_DATA,
output		          		HPS_FLASH_DCLK,
output		          		HPS_FLASH_NCSO,
```
4. USB-UART pin is reserved so you can remove or leave this line:
```
inout 		          		HPS_CONV_USB_N,
```
## Platform designer (Qsys)

1. open up the Platform designer aka (Qsys) from tools in Quartus.
2. create a new Qsys file and let's name it for example "hps".
3. From the IP catalog add the Arria V/Cyclone V HPS found under Hard Processor Systems under Processors and Peripherals.
4. configure the HPS with the following configuration (copied from the GHRD reference file by terasic).

This part is going to be a little bit tedious, because Quartus validates every single value that put in and blocks you from doing anything until it is done.

#### HPS configuration:
	
	In the FPGA Interfaces tab:

		* Disable MPU standby and signal events
		* Disable the FPGA-to-HPS interface width
		* Disable the HPS-to-FPGA interface width
		* Lightweight HPS-to-FPGA (32 bit)
		* remove the SDRAM, since we already have the DDR3RAM.
	
	In the Peripheral Interfaces tab:

		*EMAC1 pin: HPS I/O Set 0
		*EMAC1 mode: RGMII

		*NAND Flash controller was already removed 
		
		*Quad SPI Flash controller was already removed

		* SDIO pin: HPS I/O set 0
		* SDIO mode: 4-bit data parallel

		* USB1 pin: HPS I/O set 0
		* USB1 PHY interface mode: SDR with PHY clock output mode

		* SPI was already removed

		* UART0 pin: HPS I/O set 0
		* UART0 mode: No Flow control

		* I2C was already removed

		* We don't need the CAN bus interface or the Trace Port interface
		
		*configure the 1 open-drain output from the reset chip to the ethernet controller by exporting GPIO pin 35 (go click on it in the MUX Table)

	In the HPS clock, Input Clocks tab:
		
		* External clock sources should be 25.0 MHz

	In the HPS clock, Output Clocks tab:
		
		* PLL clock source: EOSC1 clock
		* SDMMC clock source: Peripheral NAND SDMMC clock
		* L4MP clock source: Peripheral base clock
		* LM4SP clock source: Peripheral base clock
		* L3MP clock frequency: 185.0 MHz
		* L3SP clock frequency: 92.5 MHz
		* Debug AT clock frequency: 25.0 MHz
		* Debug CLK frequency: 12.5 MHz
		* Debug trace clock frequency: 25 MHz
		* L4MP clock frequency: 100 MHz
		* L4SP clock freqency: 100 MHz
		* HPS-to-FPGA user 0 clock freqency: 100 MHz
		* SDMMC CLK freq: 200 MHz
		* USB CLK freq: 200 MHz
		* GPIO debounce CLK freq: 32k Hz
		* HPS-to-FPGA user 0 CLK freq: 100 MHz
	
	In the SDRAM, PHY settings tab:
		
		* Memory CLK frequency: 400 MHz
		* PLL reference CLK frequency: 25 MHz

	In the SDRAM, Memory Parameters tab:
		
		* Memory vendor: Micron
		* Total interface width: 32-bit (x2 16-bit devices)
		* Number of DQS groups: 4
		* Row address width: 15
		* Column address width: 10
		* Bank-address width: 3
		* Memory CAS latency settings: 11
		* ODT Rtt nominal value: RZQ/6
		* Memory write CAS latency setting: 7
	
	In the SDRAM, Memory timing tab:
		
		* tIS (base): 190 ps
		* tIH (base): 140 ps
		* tDS (base): 30 ps
		* tDH (base): 65 ps
		* tDQSQ: 125 ps
		* tQH: 0.38 cycles
		* tDQSCK: 255 ps
		* tDQSS: 0.25 cycles
		* tQSH: 0.4 cycles
		* tDSH: 0.2 cycles
		* tDSS: 0.2 cycles
		* tINT: 500 us
		* tMRD: 4 cycles
		* tRAS: 35 ns
		* tRCD: 13.75 ns
		* tRP: 13.75 ns
		* tREFI: 7.8 ns
		* tRFC: 260.0 ns
		* tWR: 15 ns
		* tWTR: 4 cycles
		* tFAW: 45.0 ns
		* tRRD: 7.5 ns
		* tRTP 7.5 ns
	
	In the SDRAM, Board settings tab:
		
		* Maximum CK delay to DIMM/device: 0.03 ns
		* Maximum DQS delay to DIMM/device: 0.02 ns
		* Minimum delay difference between CK and DQS: 0.06 ns
		* Maximum delay difference between CK and DQS: 0.12 ns
		* Maximum skew within the DQS group: 0.01 ns
		* Maximum sekw between the DQS groups: 0.06 ns
		* Average delay difference between DQ and DQS: 0.05 ns
		* Maximum skew within address and command bus: 0.02 ns
		* Average delay difference between address and command and CK: 0.01 ns
		
5. Click on finish and exit the HPS configuration menu.

6. Qsys wiring:
 * We want the HPS to be the master reset output (it resets the main bus clk and the bus clk resets all) so we click on "h2f_reset" and we connect it to "clk_in_reset" by clickign on the small circle in the System Contents view.
* Also the clock output will be the clock input for the Avalon interconnect (AXI bus) by connecting the "clk" to the "h2f_lw_axi_clock".
* Now we can add peripherals such as a Parallel I/O (register) with **output** direction to power up the 8 leds.
**Note: a c code line such as  \*led <<=1;**
 would require the direction to be **in/out not output**, So you should use something like \*led = 1<<i;
* Connect the CLK reset to the reset of the ledReg.
* Connect the CLK to the peripheral.
* Connect the Slave port "s1" to the master port of the main processor "h2f_lw_axi_master".
**Note: the AXI bus and the Avalon memory mapped bus are connected through the pipelined Avalon interconnect bridge, so we can add it or we can just compile and Qsys will add the bridge converters needed and handle the rest.**
* Export the external_connection by double clicking on export (Conduit signal)
7. Generate the HDL files
**Note: notice that the avalon_st_adapter: "mn_interconnect_0" has been added**

8. We want to add the generated HDL block into our top level design. 
go to Generate -> show instantiation template and copy it.

9. Go back to **Quartus** and paste it under "structural coding" in Quartus in your top level Verilog design created when we first started the project by the System Builder
10. Now add the **.qsys** file from project -> Add/remove files
11. Now start connecting your modules with their respective wires and connections.
It should look like this
```
hps u0 (
        .clk_clk                           (CLOCK_50),                                      //clk.clk
        .memory_mem_a                      (HPS_DDR3_ADDR),                   					  	//memory.mem_a
        .memory_mem_ba                     (HPS_DDR3_BA),                     					  	//      .mem_ba
        .memory_mem_ck                     (HPS_DDR3_CK_P),                   				  		//      .mem_ck
        .memory_mem_ck_n                   (HPS_DDR3_CK_N),                   					  	//      .mem_ck_n
        .memory_mem_cke                    (HPS_DDR3_CKE),                    					  	//      .mem_cke
        .memory_mem_cs_n                   (HPS_DDR3_CS_N),                   					  	//      .mem_cs_n
        .memory_mem_ras_n                  (HPS_DDR3_RAS_N),                  					  	//      .mem_ras_n
        .memory_mem_cas_n                  (HPS_DDR3_CAS_N),                  					  	//      .mem_cas_n
        .memory_mem_we_n                   (HPS_DDR3_WE_N),                   					  	//      .mem_we_n
        .memory_mem_reset_n                (HPS_DDR3_RESET_N),                						  //      .mem_reset_n
        .memory_mem_dq                     (HPS_DDR3_DQ),                     					  	//      .mem_dq
        .memory_mem_dqs                    (HPS_DDR3_DQS_P),                  					  	//      .mem_dqs
        .memory_mem_dqs_n                  (HPS_DDR3_DQS_N),                  					  	//      .mem_dqs_n
        .memory_mem_odt                    (HPS_DDR3_ODT),                    					  	//      .mem_odt
        .memory_mem_dm                     (HPS_DDR3_DM),                     						  //      .mem_dm
        .memory_oct_rzqin                  (HPS_DDR3_RZQ),                    						  //      .oct_rzqin
        .hps_io_hps_io_emac1_inst_TX_CLK   (HPS_ENET_GTX_CLK),   											      // hps_io.hps_io_emac1_inst_TX_CLK
        .hps_io_hps_io_emac1_inst_TXD0     (HPS_ENET_TX_DATA[0]),     									    //       .hps_io_emac1_inst_TXD0
        .hps_io_hps_io_emac1_inst_TXD1     (HPS_ENET_TX_DATA[1]),     									    //       .hps_io_emac1_inst_TXD1
        .hps_io_hps_io_emac1_inst_TXD2     (HPS_ENET_TX_DATA[2]),    		 							      //       .hps_io_emac1_inst_TXD2
        .hps_io_hps_io_emac1_inst_TXD3     (HPS_ENET_TX_DATA[3]),     									    //       .hps_io_emac1_inst_TXD3
        .hps_io_hps_io_emac1_inst_RXD0     (HPS_ENET_RX_DATA[0]),     									    //       .hps_io_emac1_inst_RXD0
        .hps_io_hps_io_emac1_inst_MDIO     (HPS_ENET_MDIO),     											      //       .hps_io_emac1_inst_MDIO
        .hps_io_hps_io_emac1_inst_MDC      (HPS_ENET_MDC),      											      //       .hps_io_emac1_inst_MDC
        .hps_io_hps_io_emac1_inst_RX_CTL   (HPS_ENET_RX_DV),   											        //       .hps_io_emac1_inst_RX_CTL
        .hps_io_hps_io_emac1_inst_TX_CTL   (HPS_ENET_TX_EN),   										        	//       .hps_io_emac1_inst_TX_CTL
        .hps_io_hps_io_emac1_inst_RX_CLK   (HPS_ENET_RX_CLK),   									  	    	//       .hps_io_emac1_inst_RX_CLK
        .hps_io_hps_io_emac1_inst_RXD1     (HPS_ENET_RX_DATA[1]),     					    			  //       .hps_io_emac1_inst_RXD1
        .hps_io_hps_io_emac1_inst_RXD2     (HPS_ENET_RX_DATA[2]),    						    		  	//       .hps_io_emac1_inst_RXD2
        .hps_io_hps_io_emac1_inst_RXD3     (HPS_ENET_RX_DATA[3]),     								  	  //       .hps_io_emac1_inst_RXD3
        .hps_io_hps_io_sdio_inst_CMD       (HPS_SD_CMD),       										       	  //       .hps_io_sdio_inst_CMD
        .hps_io_hps_io_sdio_inst_D0        (HPS_SD_DATA[0]),        							   			  //       .hps_io_sdio_inst_D0
        .hps_io_hps_io_sdio_inst_D1        (HPS_SD_DATA[1]),        						  				  //       .hps_io_sdio_inst_D1
        .hps_io_hps_io_sdio_inst_CLK       (HPS_SD_CLK),       								      			  //       .hps_io_sdio_inst_CLK
        .hps_io_hps_io_sdio_inst_D2        (HPS_SD_DATA[2]),        						  				  //       .hps_io_sdio_inst_D2
        .hps_io_hps_io_sdio_inst_D3        (HPS_SD_DATA[3]),        								  		  //       .hps_io_sdio_inst_D3
        .hps_io_hps_io_usb1_inst_D0        (HPS_USB_DATA[0]),        								     	  //       .hps_io_usb1_inst_D0
        .hps_io_hps_io_usb1_inst_D1        (HPS_USB_DATA[1]),        								     	  //       .hps_io_usb1_inst_D1
        .hps_io_hps_io_usb1_inst_D2        (HPS_USB_DATA[2]),        								    	  //       .hps_io_usb1_inst_D2
        .hps_io_hps_io_usb1_inst_D3        (HPS_USB_DATA[3]),        								    	  //       .hps_io_usb1_inst_D3
        .hps_io_hps_io_usb1_inst_D4        (HPS_USB_DATA[4]),        							    		  //       .hps_io_usb1_inst_D4
        .hps_io_hps_io_usb1_inst_D5        (HPS_USB_DATA[5]),        							    		  //       .hps_io_usb1_inst_D5
        .hps_io_hps_io_usb1_inst_D6        (HPS_USB_DATA[6]),        							    		  //       .hps_io_usb1_inst_D6
        .hps_io_hps_io_usb1_inst_D7        (HPS_USB_DATA[7]),        							     		  //       .hps_io_usb1_inst_D7
        .hps_io_hps_io_usb1_inst_CLK       (HPS_USB_CLKOUT),       								    		  //       .hps_io_usb1_inst_CLK
        .hps_io_hps_io_usb1_inst_STP       (HPS_USB_STP),       									    		  //       .hps_io_usb1_inst_STP
        .hps_io_hps_io_usb1_inst_DIR       (HPS_USB_DIR),       										     	  //       .hps_io_usb1_inst_DIR
        .hps_io_hps_io_usb1_inst_NXT       (HPS_USB_NXT),       										    	  //       .hps_io_usb1_inst_NXT
        .hps_io_hps_io_uart0_inst_RX       (HPS_UART_RX),       										     	  //       .hps_io_uart0_inst_RX
        .hps_io_hps_io_uart0_inst_TX       (HPS_UART_TX),      											        //       .hps_io_uart0_inst_TX
        .hps_io_hps_io_gpio_inst_GPIO35    (<connected-to-hps_io_hps_io_gpio_inst_GPIO35>), //       .hps_io_gpio_inst_GPIO35
	.ledreg_external_connection_export (LEDR) 	                //        edreg_external_connection.export
    );
```
   12. Don't forget to replace "<connected-to-hps_io_hps_io_gpio_inst_GPIO35>" with HPS_ENET_INT_N.
   13. Then from Quartus, Project -> Add/Remove files to project -> add the qsys file -> press OK.
   14.  Save and then Start Analysis & Elaboration (letting Quartus know about our HPS module and pin setups).
   Notice how Quartus inserts the interconnect that we left out:
```
Info (12250): Interconnect is inserted between master hps_0.h2f_lw_axi_master and slave ledReg.s1 because the master is of type axi and the slave is of type avalon.
```
   15. Qsys also generates .Tcl files that help us to do the pin constraints found in tools -> tcl scripts -> hps -> submodules -> parameters.tcl (run) and pin_assignment (run).
   16. Compile and Synthesize the design. (Should take around 3~5 mins).
   17. Your design won't fit in the device and the Synthesis process will fail, so you should go back and do step 15 again, but now Quartus knows what you are trying to do.
   18. Before compiling your design again go to assignments -> device.
   ### Important

Don't accept Quartu's recommendation and press NO, and make sure to pick the right SoC FPGA device again because Quartus changes it back to default and that could ruin your entire project.

Show available devices: Any, Any, Any.

Device Family: Cyclone V
Device: Cyclone V SX Extended Features

NAME: 5CSXFC6D6F31C6

   19. device and pin options:
   * Unused pins -> Reserve all unsued pins: As input tri-stated 

**NOTE: (The weak pull up will make the leds toggled but on dim brightness (DEFAULT))**

   20. If you check your pin planner you should find the CLK and the LedReg planned already
   
   **NOTE: You can delete the unused signals (Unknown) or just ignore them**
   
   21.  Synthesize again.
   22. add intelFPGA_lite bin folder to your PATH environment variable 			
   23. "C:\intelFPGA_lite\18.1\quartus\sopc_builder\bin" or whatever path you have.
   24. Open your SoC EDS command shell environment and cd into your working project directory: 
```
$ cd Desktop/SoC\ FPGA\ project\ 1\ Files/SoC\ FPGA/Qsys_led_test_system_builder/
```
**NOTE: if you have spaces you write your path like my example**
   
   25. Edit the generate_hps_qsys_header.sh with notepad and change "./soc_system.sopcinfo" \ to "./yourSopcFileName.sopcinfo" \ 
```
#!/bin/sh
sopc-create-header-files \
"./hps.sopcinfo" \
--single hps_0.h \
--module hps_0
```
   26. You need to run this .sh file from the Nios II shell or the SoC EDS shell if you are on windows.
```
$ ./generate_hps_qsys_header.sh
```
   27. If it worked you should see: swinfo2header: Creating macro file 'hps_0.h' for module 'hps_0' and you should be able to find the .h file in the project directory.

## HPS code
**NOTE: You will find these files inside the HPS files and code folder.**
1. Write your program.c file 
2. Make a makefile for it (Optional)
3. Copy your hps_0.h file to your c code directory, since it contains the base addresses of the Qsys components.
4. Compile your program using the make file by writing "make" in the c code directory from the EDS shell.

5. create a **.rbf** by copying the **sof_to_rbf.bat** script or by creating one.
Don't forget to edit the **.sof** name in the batch file.
%QUARTUS_ROOTDIR%\\bin64\\quartus_cpf -c YourProjectName.sof -o bitstream_compression=on soc_system.rbf
pause
* Copy the .sof file into your c code directory and run the .bat script.
* Your .rbf file should be read and generated.
**NOTE: the .rbf file must always be named *soc_system.rbf* because the Linux kernel is programmed to look for that file on HPS start up (You can change it if you want)**
 
## Transfering files to DE-10

Now that we have both the **.rbf** file and **compiled main.c** file, we can transfer them to the DE-10 using Secure Copy Protocol (scp).

To enable DHCP on your DE-10 run:

```
root@socfpga:~ udhcpc
```
The IP leased to me by the network was **192.168.1.9**

Transfering the **soc_system.rbf** file from your windows linux shell to the DE-10:
```
Barbary@LAPTOP-95R9DUMQ ~/Desktop/SoC FPGA project 1 Files/SoC FPGA/Qsys_led_test_system_builder/HPS files and code
$ scp soc_system.rbf root@192.168.1.9:~/SoC_FPGA_test/Qsys_test

Are you sure you want to continue connecting (yes/no)? yes
soc_system.rbf                                                                                100% 1913KB   6.2MB/s   00:00
```
Transfering the **main** file from your windows linux shell to the DE-10:
```
Barbary@LAPTOP-95R9DUMQ ~/Desktop/SoC FPGA project 1 Files/SoC FPGA/Qsys_led_test_system_builder/HPS files and code
$ scp main root@192.168.1.9:~/SoC_FPGA_test/Qsys_test

Are you sure you want to continue connecting (yes/no)? yes
main                                                                                          100%   23KB 500.8KB/s   00:00
```
Your DE-10 console should look like this after the 2 transfers:
```
root@socfpga:~/SoC_FPGA_test/Qsys_test# ls
main soc_system.rbf
```
Now you can use VIM to make the shell file or you can transfer it using scp.
```
root@socfpga:~/SoC_FPGA_test/Qsys_test# vim reconfigureFPGA.sh
```
Press **i** to start inserting in VIM then write the following commands:

```
mkdir -p fat
mount /dev/mmcblk0p1 fat
cp soc_system.rbf fat/soc_system.rbf
umount fat
reboot
```
Then press **esc** to enter command mode and write **:wq** to exit VIM and save your file.

**NOTE: before running the reconfigureFPGA shell file remember to make it executable or you will get permission denied error and it needs to be in the same directory as the soc_system.rbf file.**
```
root@socfpga:~/SoC_FPGA_test/Qsys_test# chmod u+r+x reconfigureFPGA.sh
```	

Now execute the shell file:
```
root@socfpga:~/SoC_FPGA_test/Qsys_test# ./reconfigureFPGA.sh
```

Your system should reboot and your FPGA should be reconfigured.

Finally, you can go and run your main file using:
```
root@socfpga:~/SoC_FPGA_test/Qsys_test# ./main
```

And you should see your leds blinking in order one by one with a 250 ms delay.

**NOTE: To break out of the program press on CTRL+C**
