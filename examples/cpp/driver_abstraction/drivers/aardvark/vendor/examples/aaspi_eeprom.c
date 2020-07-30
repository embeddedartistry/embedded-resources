/*=========================================================================
| (c) 2004-2007  Total Phase, Inc.
|--------------------------------------------------------------------------
| Project : Aardvark Sample Code
| File    : aaspi_eeprom.c
|--------------------------------------------------------------------------
| Perform simple read and write operations to an SPI EEPROM device.
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
#include "aardvark.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=========================================================================
// CONSTANTS
//=========================================================================
#define PAGE_SIZE 32

//=========================================================================
// FUNCTIONS
//=========================================================================
static void _writeMemory(Aardvark handle, u16 addr, u16 length, int zero)
{
	u16 i, n;

	u08 data_out[3 + PAGE_SIZE];
	u08 data_in[3 + PAGE_SIZE];

	// Write to the SPI EEPROM
	//
	// The AT25080A EEPROM has 32 byte pages.  Data can written
	// in pages, to reduce the number of overall SPI transactions
	// executed through the Aardvark adapter.
	n = 0;
	while(n < length)
	{
		// Send write enable command
		data_out[0] = 0x06;
		aa_spi_write(handle, 1, data_out, 1, data_in);

		// Assemble write command and address
		data_out[0] = 0x02;
		data_out[1] = (addr >> 8) & 0xff;
		data_out[2] = (addr >> 0) & 0xff;

		// Assemble a page of data
		i = 3;
		do
		{
			data_out[i++] = zero ? 0 : (u08)n;
			++addr;
			++n;
		} while(n < length && (addr & (PAGE_SIZE - 1)));

		// Write the transaction
		aa_spi_write(handle, i, data_out, i, data_in);
		aa_sleep_ms(10);
	}
}

static void _readMemory(Aardvark handle, u16 addr, u16 length)
{
	int count;
	int i;

	u08* data_out = (u08*)malloc(length + 3);
	u08* data_in = (u08*)malloc(length + 3);

	memset(data_out, 0, length + 3);
	memset(data_in, 0, length + 3);

	// Assemble read command and address
	data_out[0] = 0x03;
	data_out[1] = (addr >> 8) & 0xff;
	data_out[2] = (addr >> 0) & 0xff;

	// Write length+3 bytes for data plus command and 2 address bytes
	count = aa_spi_write(handle, (u16)(length + 3), data_out, (u16)(length + 3), data_in);

	if(count < 0)
	{
		printf("error: %s\n", aa_status_string(count));
	}
	else if(count != length + 3)
	{
		printf("error: read %d bytes (expected %d)\n", count - 3, length);
	}

	// Dump the data to the screen
	printf("\nData read from device:");
	for(i = 0; i < length; ++i)
	{
		if((i & 0x0f) == 0)
			printf("\n%04x:  ", addr + i);
		printf("%02x ", data_in[i + 3] & 0xff);
		if(((i + 1) & 0x07) == 0)
			printf(" ");
	}
	printf("\n");

	// Free the packet
	free(data_out);
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
	int mode = 0;
	u16 addr;
	u16 length;

	const char* command;

	if(argc < 7)
	{
		printf("usage: aaspi_eeprom PORT BITRATE read  MODE ADDR LENGTH\n");
		printf("usage: aaspi_eeprom PORT BITRATE write MODE ADDR LENGTH\n");
		printf("usage: aaspi_eeprom PORT BITRATE zero  MODE ADDR LENGTH\n");
		printf("  mode 0 : pol = 0, phase = 0\n");
		printf("  mode 3 : pol = 1, phase = 1\n");
		printf("  modes 1 and 2 are not supported\n");
		return 1;
	}

	port = atoi(argv[1]);
	bitrate = atoi(argv[2]);
	command = argv[3];
	mode = atoi(argv[4]);
	addr = (unsigned short)strtol(argv[5], 0, 0);
	length = atoi(argv[6]);

	if(mode == 1 || mode == 2)
	{
		printf("error: spi modes 1 and 2 are not supported by the AT25080A\n");
		return 1;
	}

	// Open the device
	handle = aa_open(port);
	if(handle <= 0)
	{
		printf("Unable to open Aardvark device on port %d\n", port);
		printf("Error code = %d\n", handle);
		return 1;
	}

	// Ensure that the SPI subsystem is enabled.
	aa_configure(handle, AA_CONFIG_SPI_I2C);

	// Power the EEPROM using the Aardvark adapter's power supply.
	// This command is only effective on v2.0 hardware or greater.
	// The power pins on the v1.02 hardware are not enabled by default.
	// aa_target_power(handle, AA_TARGET_POWER_BOTH);
	aa_target_power(handle, AA_TARGET_POWER_BOTH);

	// Setup the clock phase
	aa_spi_configure(handle, mode >> 1, mode & 1, AA_SPI_BITORDER_MSB);

	// Set the bitrate
	bitrate = aa_spi_bitrate(handle, bitrate);
	printf("Bitrate set to %d kHz\n", bitrate);

	// Perform the operation
	if(strcmp(command, "write") == 0)
	{
		_writeMemory(handle, addr, length, 0);
		printf("Wrote to EEPROM\n");
	}
	else if(strcmp(command, "read") == 0)
	{
		_readMemory(handle, addr, length);
	}
	else if(strcmp(command, "zero") == 0)
	{
		_writeMemory(handle, addr, length, 1);
		printf("Zeroed EEPROM\n");
	}
	else
	{
		printf("unknown command: %s\n", command);
	}

	// Close the device
	aa_close(handle);

	return 0;
}
