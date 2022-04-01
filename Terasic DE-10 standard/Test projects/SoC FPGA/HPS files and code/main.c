#include <stdio.h>  // Included for printf and scanf
#include <stdlib.h> // Included for exit(0);
#include <stdint.h> // Included for std data types
#include <fcntl.h>  // Included to open files
#include <signal.h> // Included to handle the ctrl+c (exit)
#include <sys/mman.h>   // Included for mmap and munmap (Linux)
#include "hps_0.h"  // Included because it contains our FPGA peripheral addresses

#define REG_BASE 0xFF200000 // Lightweight HPS-to-FPGA base address
#define REG_SPAN 0x00200000 // Lightweight HPS-to-FPGA span (size of the memory space)

void* h2f_bridge_base;   // Void pointer to be defined as the Lightweight HPS-to-FPGA bridge
uint32_t *ledReg;
int fileDescriptor;

void handler(int signo);

int main(){

    // Initialize an iterattor for your loop
    uint8_t iterator = 0;
    
    // Access the device memory
    fileDescriptor = open("/dev/mem", (O_RDWR | O_SYNC));
    
    if(fileDescriptor < 0){
        printf("Cannot access the device memory.\n");
        return -1;
    }
    
    // Map these settings to the void pointer (NULL , Size, read/write, fileOpened, BaseAddressValue)
    h2f_bridge_base = mmap(NULL, REG_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fileDescriptor, REG_BASE);

    if(h2f_bridge_base == MAP_FAILED){
        printf("Cannot map the memory");
        return -1;
    }

    // Define the address of the ledReg by casting the void pointer to uint32_t and adding the LED_BASE offset from Qsys hps_0.h
    ledReg = (uint32_t*)(h2f_bridge_base + LEDREG_BASE);    
    
    // Handle closing the program
    signal(SIGINT, handler);

    // Light up led 0
    *ledReg = 0x1;
    while(1){

        // Sleep for 250 ms
        usleep(250000);

        // If our iterator is less than 7 than increment
        // Else make it = 0
        if(iterator < 7){
            iterator++;
        }
        else{
            iterator = 0;
        }
        // Light up the next led and turn off all
        *ledReg = 1 << iterator;

    }
    return 0;
}

void handler(int signo){
    
    // Turn off the leds
    *ledReg = 0;

    // Release the memory allocated
    munmap(h2f_bridge_base, REG_SPAN);

    // Close the memory file
    close(fileDescriptor);

    // Exit the program
    exit(0);
}
