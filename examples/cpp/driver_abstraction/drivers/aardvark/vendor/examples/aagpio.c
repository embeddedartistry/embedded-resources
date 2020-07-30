/*=========================================================================
| (c) 2003-2007  Total Phase, Inc.
|--------------------------------------------------------------------------
| Project : Aardvark Sample Code
| File    : aagpio.c
|--------------------------------------------------------------------------
| Perform some simple GPIO operations with a single Aardvark adapter.
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

//=========================================================================
// MAIN PROGRAM
//=========================================================================
int main(int argc, char* argv[])
{
	int port;
	u08 val;
	Aardvark handle;
	AardvarkExt aaext;

	if(argc < 2)
	{
		printf("usage: aagpio PORT\n");
		return 1;
	}

	port = atoi(argv[1]);

	// Open the device
	handle = aa_open_ext(port, &aaext);

	if(handle <= 0)
	{
		printf("Unable to open Aardvark device on port %d\n", port);
		printf("Error code = %d\n", handle);
		return 1;
	}

	printf("Opened Aardvark adapter; features = 0x%02x\n", aaext.features);

	// Configure the Aardvark adapter so all pins
	// are now controlled by the GPIO subsystem
	aa_configure(handle, AA_CONFIG_GPIO_ONLY);

	// Turn off the external I2C line pullups
	aa_i2c_pullup(handle, AA_I2C_PULLUP_NONE);

	// Make sure the charge has dissipated on those lines
	aa_gpio_set(handle, 0x00);
	aa_gpio_direction(handle, 0xff);

	// By default all GPIO pins are inputs.  Writing 1 to the
	// bit position corresponding to the appropriate line will
	// configure that line as an output
	aa_gpio_direction(handle, AA_GPIO_SS | AA_GPIO_SCL);

	// By default all GPIO outputs are logic low.  Writing a 1
	// to the appropriate bit position will force that line
	// high provided it is configured as an output.  If it is
	// not configured as an output the line state will be
	// cached such that if the direction later changed, the
	// latest output value for the line will be enforced.
	aa_gpio_set(handle, AA_GPIO_SCL);
	printf("Setting SCL to logic low\n");

	// The get method will return the line states of all inputs.
	// If a line is not configured as an input the value of
	// that particular bit position in the mask will be 0.
	val = aa_gpio_get(handle);

	// Check the state of SCK
	if(val & AA_GPIO_SCK)
		printf("Read the SCK line as logic high\n");
	else
		printf("Read the SCK line as logic low\n");

	// Optionally we can set passive pullups on certain lines.
	// This can prevent input lines from floating.  The pullup
	// configuration is only valid for lines configured as inputs.
	// If the line is not currently input the requested pullup
	// state will take effect only if the line is later changed
	// to be an input line.
	aa_gpio_pullup(handle, AA_GPIO_MISO | AA_GPIO_MOSI);

	// Now reading the MISO line should give a logic high,
	// provided there is nothing attached to the Aardvark
	// adapter that is driving the pin low.
	val = aa_gpio_get(handle);
	if(val & AA_GPIO_MISO)
		printf("Read the MISO line as logic high (passive pullup)\n");
	else
		printf("Read the MISO line as logic low (is pin driven low?)\n");

	// Now do a 1000 gets from the GPIO to test performance
	{
		int i;
		for(i = 0; i < 1000; ++i)
		{
			aa_gpio_get(handle);
		}
	}

	{
		u08 oldval, newval;

		// Demonstrate use of aa_gpio_change
		aa_gpio_direction(handle, 0x00);
		oldval = aa_gpio_get(handle);

		printf("Calling aa_gpio_change for 2 seconds...\n");
		newval = aa_gpio_change(handle, 2000);

		if(newval != oldval)
			printf("  GPIO inputs changed.\n");
		else
			printf("  GPIO inputs did not change.\n");
	}

	// Turn on the I2C line pullups since we are done
	aa_i2c_pullup(handle, AA_I2C_PULLUP_BOTH);

	// Configure the Aardvark adapter back to SPI/I2C mode.
	aa_configure(handle, AA_CONFIG_SPI_I2C);

	// Close the device
	aa_close(handle);

	return 0;
}
