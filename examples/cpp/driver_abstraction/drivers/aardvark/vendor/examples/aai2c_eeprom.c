/*=========================================================================
| (c) 2003-2007  Total Phase, Inc.
|--------------------------------------------------------------------------
| Project : Aardvark Sample Code
| File    : aai2c_eeprom.c
|--------------------------------------------------------------------------
| Perform simple read and write operations to an I2C EEPROM device.
|--------------------------------------------------------------------------
| Redistribution and use of this file in source and binary forms, with
| or without modification, are permitted.
|
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
| LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
| FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
| COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
| INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
| BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
| CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
| LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
| ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
| POSSIBILITY OF SUCH DAMAGE.
 ========================================================================*/

//=========================================================================
// INCLUDES
//=========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aardvark.h"

//=========================================================================
// CONSTANTS
//=========================================================================
#define PAGE_SIZE 8
#define BUS_TIMEOUT 150 // ms

//=========================================================================
// FUNCTION
//=========================================================================
static void _writeMemory(Aardvark handle, u08 device, u08 addr, u16 length, int zero)
{
	u16 i, n;
	u08 data_out[1 + PAGE_SIZE];

	// Write to the I2C EEPROM
	//
	// The AT24C02 EEPROM has 8 byte pages.  Data can written
	// in pages, to reduce the number of overall I2C transactions
	// executed through the Aardvark adapter.
	n = 0;
	while(n < length)
	{
		// Fill the packet with data
		data_out[0] = addr;

		// Assemble a page of data
		i = 1;
		do
		{
			data_out[i++] = zero ? 0 : (u08)n;
			++addr;
			++n;
		} while(n < length && (addr & (PAGE_SIZE - 1)));

		// Write the address and data
		aa_i2c_write(handle, device, AA_I2C_NO_FLAGS, i, data_out);
		aa_sleep_ms(10);
	}
}

static void _readMemory(Aardvark handle, u08 device, u08 addr, u16 length)
{
	int count, i;
	u08* data_in = (u08*)malloc(length);

	// Write the address
	aa_i2c_write(handle, device, AA_I2C_NO_STOP, 1, &addr);

	count = aa_i2c_read(handle, device, AA_I2C_NO_FLAGS, length, data_in);
	if(count < 0)
	{
		printf("error: %s\n", aa_status_string(count));
		return;
	}
	if(count == 0)
	{
		printf("error: no bytes read\n");
		printf("  are you sure you have the right slave address?\n");
		return;
	}
	else if(count != length)
	{
		printf("error: read %d bytes (expected %d)\n", count, length);
	}

	// Dump the data to the screen
	printf("\nData read from device:");
	for(i = 0; i < count; ++i)
	{
		if((i & 0x0f) == 0)
			printf("\n%04x:  ", addr + i);
		printf("%02x ", data_in[i] & 0xff);
		if(((i + 1) & 0x07) == 0)
			printf(" ");
	}
	printf("\n");

	// Free the data_in pointer
	free(data_in);
}

//=========================================================================
// MAIN PROGRAM
//=========================================================================
int main(int argc, char* argv[])
{
	Aardvark handle;
	int port = 0;
	int bitrate = 100;
	u08 device;
	u08 addr;
	u16 length;
	int bus_timeout;

	const char* command;

	if(argc < 7)
	{
		printf("usage: aai2c_eeprom PORT BITRATE read  SLAVE_ADDR OFFSET LENGTH\n");
		printf("usage: aai2c_eeprom PORT BITRATE write SLAVE_ADDR OFFSET LENGTH\n");
		printf("usage: aai2c_eeprom PORT BITRATE zero  SLAVE_ADDR OFFSET LENGTH\n");
		return 1;
	}

	port = atoi(argv[1]);
	bitrate = atoi(argv[2]);
	command = argv[3];
	device = (u08)strtol(argv[4], 0, 0);
	addr = (u08)strtol(argv[5], 0, 0);
	length = atoi(argv[6]);

	// Open the device
	handle = aa_open(port);
	if(handle <= 0)
	{
		printf("Unable to open Aardvark device on port %d\n", port);
		printf("Error code = %d\n", handle);
		return 1;
	}

	// Ensure that the I2C subsystem is enabled
	aa_configure(handle, AA_CONFIG_SPI_I2C);

	// Enable the I2C bus pullup resistors (2.2k resistors).
	// This command is only effective on v2.0 hardware or greater.
	// The pullup resistors on the v1.02 hardware are enabled by default.
	aa_i2c_pullup(handle, AA_I2C_PULLUP_BOTH);

	// Power the EEPROM using the Aardvark adapter's power supply.
	// This command is only effective on v2.0 hardware or greater.
	// The power pins on the v1.02 hardware are not enabled by default.
	aa_target_power(handle, AA_TARGET_POWER_BOTH);

	// Set the bitrate
	bitrate = aa_i2c_bitrate(handle, bitrate);
	printf("Bitrate set to %d kHz\n", bitrate);

	// Set the bus lock timeout
	bus_timeout = aa_i2c_bus_timeout(handle, BUS_TIMEOUT);
	printf("Bus lock timeout set to %d ms\n", bus_timeout);

	// Perform the operation
	if(strcmp(command, "write") == 0)
	{
		_writeMemory(handle, device, addr, length, 0);
		printf("Wrote to EEPROM\n");
	}
	else if(strcmp(command, "read") == 0)
	{
		_readMemory(handle, device, addr, length);
	}
	else if(strcmp(command, "zero") == 0)
	{
		_writeMemory(handle, device, addr, length, 1);
		printf("Zeroed EEPROM\n");
	}
	else
	{
		printf("unknown command: %s\n", command);
	}

	// Close the device and exit
	aa_close(handle);
	return 0;
}
