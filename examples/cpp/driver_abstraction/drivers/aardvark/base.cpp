#include "base.hpp"
#include "vendor/aardvark.h"
#include <cassert>

#if 0
//TODO?
    // Power the EEPROM using the Aardvark adapter's power supply.
    // This command is only effective on v2.0 hardware or greater.
    // The power pins on the v1.02 hardware are not enabled by default.
    aa_target_power(handle, AA_TARGET_POWER_BOTH);
#endif

using namespace embdrv;

void aardvarkAdapter::start() noexcept
{
	started_refcnt_++;

	if(!started())
	{
		handle_ = aa_open(port_);
		assert((handle_ > 0) && "Could not find Aardvark Device");

		mode(mode_);
	}
}

void aardvarkAdapter::stop() noexcept
{
	started_refcnt_--;

	if(started() && (started_refcnt_ == 0))
	{
		aa_close(handle_);
		handle_ = 0;
	}
}

aardvarkMode aardvarkAdapter::mode(aardvarkMode m) noexcept
{
	mode_ = m;

	lock();
	aa_configure(handle_, static_cast<AardvarkConfig>(m));
	unlock();

	return mode_;
}

bool aardvarkAdapter::i2cPullups() noexcept
{
	bool en = false;

	lock();
	int r = aa_i2c_pullup(handle_, AA_I2C_PULLUP_QUERY);
	unlock();

	if(r > 0)
	{
		en = true;
	}

	return en;
}

bool aardvarkAdapter::i2cPullups(bool en) noexcept
{
	lock();
	aa_i2c_pullup(handle_, en ? AA_I2C_PULLUP_BOTH : AA_I2C_PULLUP_NONE);
	unlock();

	return en;
}

bool aardvarkAdapter::targetPower() noexcept
{
	bool en = false;

	lock();
	int r = aa_target_power(handle_, AA_TARGET_POWER_QUERY);
	unlock();

	if(r > 0)
	{
		en = true;
	}

	return en;
}

bool aardvarkAdapter::targetPower(bool en) noexcept
{
	lock();
	aa_i2c_pullup(handle_, en ? AA_TARGET_POWER_BOTH : AA_TARGET_POWER_NONE);
	unlock();

	return en;
}

void aardvarkAdapter::pullup(uint8_t id, bool en) noexcept
{
	uint8_t value = 0;

	if(en)
	{
		value = aardvarkIO.at(id);
	}
	else
	{
		// ~ converts to an int, and we need uint8_t. Preventing an inadvertant conversion warning.
		value = static_cast<uint8_t>(~aardvarkIO.at(id));
	}

	pullup_mask_ &= value;

	lock();
	int r = aa_gpio_pullup(handle_, pullup_mask_);
	unlock();

	assert(r == AA_OK && "Failure to set pullup");
}
