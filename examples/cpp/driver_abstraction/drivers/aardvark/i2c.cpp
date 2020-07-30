#include "i2c.hpp"
#include "vendor/aardvark.h"

using namespace embdrv;

constexpr uint16_t bus_timeout_ms = UINT16_C(150);
constexpr int INPUT_BAUDRATE_TO_AARDVARK_CONV_FACTOR = 1000;

static embvm::i2c::status convertI2CTransactionErrorCode(int r) noexcept
{
	embvm::i2c::status status;

	if(r > 0)
	{
		status = embvm::i2c::status::ok;
	}
	else
	{
		switch(r)
		{
			case AA_OK:
				status = embvm::i2c::status::ok;
				break;
			case AA_I2C_WRITE_ERROR:
				status = embvm::i2c::status::addrNACK;
				break;
			case AA_I2C_NOT_AVAILABLE:
				status = embvm::i2c::status::busy;
				break;
			case AA_I2C_SLAVE_READ_ERROR:
			case AA_I2C_READ_ERROR:
				status = embvm::i2c::status::dataNACK;
				break;
			case AA_I2C_SLAVE_TIMEOUT:
			case AA_I2C_DROPPED_EXCESS_BYTES:
				status = embvm::i2c::status::error;
				break;
			case AA_I2C_BUS_ALREADY_FREE:
				status = embvm::i2c::status::bus;
				break;
			default:
				status = embvm::i2c::status::unknown;
		}
	}

	return status;
}

aardvarkI2CController::~aardvarkI2CController() noexcept = default;

void aardvarkI2CController::start() noexcept
{
	base_driver_.start();
}

void aardvarkI2CController::stop() noexcept
{
	base_driver_.stop();
}

embvm::i2c::pullups aardvarkI2CController::setPullups_(embvm::i2c::pullups pullups) noexcept
{
	assert(started_ && "Setting pullups before starting not supported\n");

	base_driver_.i2cPullups(pullups != embvm::i2c::pullups::external);

	return pullups;
}

void aardvarkI2CController::process_(const storagePair_t& pair) noexcept
{
	const auto& [op, cb] = pair;
	int r = AA_OK;
	uint16_t num_written = 0;
	uint16_t num_read = 0;
	embvm::i2c::status status;

	std::unique_lock<aardvarkAdapter> lock(base_driver_);

	switch(op.op)
	{
		case embvm::i2c::operation::continueWriteStop:
		case embvm::i2c::operation::write: {
			r = aa_i2c_write_ext(base_driver_.handle(), op.address, AA_I2C_NO_FLAGS,
								 static_cast<uint16_t>(op.tx_size), op.tx_buffer, &num_written);
			assert(op.tx_size == num_written);
			break;
		}
		case embvm::i2c::operation::writeNoStop:
		case embvm::i2c::operation::continueWriteNoStop: {
			r = aa_i2c_write_ext(base_driver_.handle(), op.address, AA_I2C_NO_STOP,
								 static_cast<uint16_t>(op.tx_size), op.tx_buffer, &num_written);
			assert(op.tx_size == num_written);
			break;
		}
		case embvm::i2c::operation::read: {
			r = aa_i2c_read_ext(base_driver_.handle(), op.address, AA_I2C_NO_FLAGS,
								static_cast<uint16_t>(op.rx_size), op.rx_buffer, &num_read);
			assert(op.rx_size == num_read);
			break;
		}
		case embvm::i2c::operation::writeRead: {
			r = aa_i2c_write_read(base_driver_.handle(), op.address, AA_I2C_NO_FLAGS,
								  static_cast<uint16_t>(op.tx_size), op.tx_buffer, &num_written,
								  static_cast<uint16_t>(op.rx_size), op.rx_buffer, &num_read);

			assert(num_written == op.tx_size);
			assert(num_read == op.rx_size);

			// We will use read status for our return value
			r = (r >> 8) & 0xff; // NOLINT
			// Unused, but here is how you parse: int write_status = (r & 0xff);
			break;
		}
		case embvm::i2c::operation::ping: {
			// Use num_written as a dummy read placeholder
			r = aa_i2c_read_ext(base_driver_.handle(), op.address, AA_I2C_NO_FLAGS, 1,
								reinterpret_cast<uint8_t*>(&num_written), &num_read);

			// If we don't read any data, we didn't get an ACK
			if(num_read == 0)
			{
				r = AA_I2C_WRITE_ERROR;
			}

			break;
		}
		case embvm::i2c::operation::stop:
			aa_i2c_free_bus(base_driver_.handle());
			break;
		case embvm::i2c::operation::restart:
			break; // Fallthrough - we set AA_OK above
	}

	status = convertI2CTransactionErrorCode(r);
	lock.unlock();

	callback(op, status, cb);
}

embvm::i2c::status aardvarkI2CController::transfer_(const embvm::i2c::op_t& op,
												const embvm::i2c::controller::cb_t& cb) noexcept
{
	enqueue({op, cb});

	return embvm::i2c::status::enqueued;
}

embvm::i2c::baud aardvarkI2CController::baudrate_(embvm::i2c::baud baud) noexcept
{
	assert(started_ && "Setting baudrate before starting not supported\n");

	base_driver_.lock();
	auto set_bitrate = aa_i2c_bitrate(
		base_driver_.handle(), static_cast<int>(baud) / INPUT_BAUDRATE_TO_AARDVARK_CONV_FACTOR);
	base_driver_.unlock();

	assert(set_bitrate == (static_cast<int>(baud) / INPUT_BAUDRATE_TO_AARDVARK_CONV_FACTOR));
	// TODO: tolerance checks

	return baud;
}

void aardvarkI2CController::configure_(embvm::i2c::pullups pullup) noexcept
{
	(void)pullup;
	assert(started_ && "Configuring before starting not supported\n");

	base_driver_.lock();
	auto timeout = aa_i2c_bus_timeout(base_driver_.handle(), bus_timeout_ms);
	base_driver_.unlock();

	assert(timeout == bus_timeout_ms);
}
