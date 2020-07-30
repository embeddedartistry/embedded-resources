#ifndef I2C_DRIVER_HPP_
#define I2C_DRIVER_HPP_

#include <cstdint>
#include <etl/vector.h>
#include <inplace_function/inplace_function.hpp>

namespace embvm
{
/// @addtogroup FrameworkDriver
/// @{

/// Definitions, functions, and classes related to I2C communication bus devices.
namespace i2c
{
/// Maximum size of the I2C controller callback functor object.
static constexpr size_t I2C_controller_REQD_STATIC_FUNCTION_SIZE = 96;

/// I2C address storage type.
using addr_t = uint8_t;
struct op_t;

/// Added to slave address to indicate a read command.
static constexpr uint8_t ReadBit = 0x01;

/** I2C operation types.
 *
 * The operation enum defines the type of transfer that the op_t structure
 * is to perform.
 */
enum class operation : uint8_t
{
	stop = 0, /// Generate a stop condition
	restart, /// Generate a bus restart

	/// Write to slave
	/// Performs the following sequence: start - address - write - stop
	write,

	/// Read from slave
	/// Performs the following sequence: start - address - read - stop
	read,

	/// write to slave and then read from slave
	/// Performs the following sequence: start - address - write - restart - address - read -
	/// stop
	writeRead,

	/// start a write to the slave and but do not issue a stop
	/// Performs the following sequence: start - address - write
	writeNoStop,

	/// continue a write to the slave and do not stop the transaction
	/// Performs the following sequence: write
	continueWriteNoStop,

	/// continue a write to the slave and then stop the transaction
	/// Performs the following sequence: write - stop
	continueWriteStop,

	/// Send only address to see if there is an ACK
	/// Performs the following sequence: start - address
	ping,
};

/// Represents the state of the I2C bus.
enum class state : uint8_t
{
	/// The bus is idle and ready to receive work.
	idle = 0,
	/// The bus is currently busy executing a transaction.
	busy,
	/// A bus error occurred and needs to be corrected.
	error,
};

/** I2C bus status.
 *
 * This enumeration represents the status of an I2C bus. This type extends the default
 * embvm::comm::status definition with additional I2C-specific statuses.
 */
enum class status
{
	/// The operation completed successfully.
	ok = 0,
	/// The operation was enqueued for later processing.
	enqueued,
	/// The device cannot handle the request at this time; try again.
	busy,
	/// An error occurred with this transaction.
	error,
	/// The success/failure status of the transaction could not be determined.
	unknown,

	// Additional i2c errors

	/// Addr was transmitted, NACK received.
	addrNACK,

	/// Data was transmitted, NACK received.
	dataNACK,

	/// Bus error, such as missing start/stop condition.
	bus,
};

/// Valid I2C bus baudrate options.
/// Numeric values are in Hz.
enum class baud
{
	lowSpeed = 10000,
	standard = 100000,
	fast = 400000
};

/// Pull-up configuration options for an I2C controller device.
enum class pullups : uint8_t
{
	/// External pullups exist for SDA and SCL.
	external = 0,
	/// Use internal pull-ups on SDA and SCL (if supported by the device).
	internal
};

/** I2C operation definition.
 *
 * The I2C bus operates in half-duplex mode, meaning that reads and writes do not occur
 * simultaneously. The operation type represents a variety of operations which can be
 * performed by an I2C device. The transmit options (tx_buffer and tx_size) are required by any
 * operations using "write". The receive options (rx_buffer and rx_size) are required by any
 * operations using "read".
 */
struct op_t
{
	/// Slave device address (7-bit).
	uint8_t address{0};

	/// Operation to perform.
	operation op{operation::stop};

	/// Pointer to the transmit buffer.
	/// Does not need to be specified for read-only commands.
	const uint8_t* tx_buffer{nullptr};

	/// Number of bytes to transmit.
	/// Does not need to be specified for read-only commands.
	size_t tx_size{0};

	/// Pointer to the receive buffer.
	/// Does not need to be specified for write-only commands.
	uint8_t* rx_buffer{nullptr};

	/// Number of bytes to receive.
	/// Does not need to be specified for write-only commands.
	size_t rx_size{0};
};

/** This class provides the I2C controller interface definition.
 *
 * This class defines common interfaces and types for I2C controller devices.
 *
 * ## Implementing a i2c::controller Driver
 *
 * To implement your own I2C controller device, simply inherit from this class type:
 *
 * @code
 * class aardvarkI2Ccontroller final : public i2c::controller
 * {...}
 * @endcode
 *
 * Derived classes must imlement the following pure virtual functions to complete I2C controller
 * behavior:
 * - configure_(), to configure the target hardware for operation
 *
 * Derived classes must also implement pure virtual functions required by the comm bus, such as:
 * - transfer_(), which initiates a bus transfer
 * - baudrate_(), which configures the baudrate
 *
 * ## Using a i2c::controller Driver
 *
 * i2c::controller usage follows this general pattern:
 *	1. Declare a i2c::controller device
 *  2. Configure the device
 *	3. Initialize a transfer
 *
 * ### Declare a i2c::controller device
 *
 * Derived classes will have their own specific constructor schemes. In all cases you will use
 * a derived class for the declaration.
 *
 * @code
 * aardvarkAdapter aardvark{aardvarkMode::I2C};
 * aardvarkI2Ccontroller d{aardvark, "i2c0"};
 * @endcode
 *
 * ### Configure a i2c::controller device
 *
 * After the device has been declared, the object can be used to configure
 * the I2C bus. Initial configuration will happen with the configure() function,
 * which sets up the I2C device with the target baudrate and pull-up option.
 *
 * ### Initialize a Transfer
 *
 * All transfers require an op structure instance with the proper settings:
 *
 * @code
 * i2c::op_t t;
 * t.op = i2c::operation::writeRead;
 * t.address = address_;
 * t.tx_size = sizeof(uint16_t);
 * t.tx_buffer = reinterpret_cast<const uint8_t*>(reg_buf);
 * t.rx_size = rx_size;
 * t.rx_buffer = rx_buffer;
 * @endcode
 *
 * You should also prepare a callback function to handle the transfer completion:
 *
 * @code
 * void i2c_callback(i2c::op_t input, i2c::status status)
 * {
 * ...
 * }
 * @endcode
 *
 * To initiate a transfer, call transfer() with the op and callback:
 *
 * @code
 * auto err = d.transfer(t, i2c_callback);
 * @endcode
 *
 * When the transfer completes, the callback function will be called with a copy
 * of the op struct and the result of the transfer.
 */
class controller
{
  public:
	/// Represents the type of the callback operation.
	using cb_t = stdext::inplace_function<void(i2c::op_t, i2c::status)>;
	using sweep_list_t = etl::vector<uint8_t, 128>;
	using sweep_cb_t = stdext::inplace_function<void(void)>;

  protected:
	/** Default constructor.
	 *
	 * Initializes the comm bus status.
	 */
	controller() noexcept = default;

	/** Construct the bus with a baudrate.
	 *
	 * Initializes the comm bus status and sets the baudrate to a default value.
	 *
	 * @param baud The default baudrate for the bus.
	 */
	explicit controller(i2c::baud baud) noexcept : baud_(baud)
	{
		// Empty body
	}

	/// Default destructor
	~controller() noexcept = default;

  public:
	/// Deleted copy constructor
	controller(const controller&) = delete;

	/// Deleted copy assignment operator
	const controller& operator=(const controller&) = delete;

	/// Deleted move constructor
	controller(controller&&) = delete;

	/// Deleted move assignment operator
	controller& operator=(controller&&) = delete;

  public:
  	/// Put the driver in an operational state.
	virtual void start() noexcept = 0;

	/// Put the driver in a non-operational state (e.g., powered off).
	virtual void stop() noexcept = 0;

	/** Configure the I2C bus.
	 *
	 * Configures the I2C bus for operation and sets the target baud rate and pull-ups.
	 *
	 * Derived classes must implement the configure_() function to set up the
	 * target hardware.
	 *
	 * @param baud The target baudrate (as an i2c::baud enumeration rather than integral Hz).
	 * @param pull The pullup setting, which defaults to external pull-ups (in hardware).
	 */
	virtual void configure(i2c::baud baud, i2c::pullups pull = i2c::pullups::external) noexcept
	{
		configure_(pull);
		baudrate(baud);
		pullups(pull);
	}

	/** Check the I2C bus status.
	 *
	 * @returns the status of the I2C bus as an i2c::state enumeration.
	 */
	i2c::state state() const noexcept
	{
		return state_;
	}

	/** Configure pull-ups.
	 *
	 * @param pullups The target pull-up setting.
	 * @returns The configured pull-up setting.
	 */
	i2c::pullups pullups(i2c::pullups pullups) noexcept
	{
		pullups_ = setPullups_(pullups);

		return pullups_;
	}

	/** Check the pull-up configuration.
	 *
	 * @returns The configured pull-up setting.
	 */
	i2c::pullups pullups() const noexcept
	{
		return pullups_;
	}

	/** Check whether the driver has started
	*
	* @returns true if the driver is operational.
	*/
	bool started() const noexcept
	{
		return started_;
	}

	/** Initiate a bus transfer.
	 *
	 * Initiates a transfer of information across a comm bus. The transfer function should handle
	 * read, write, and read-write operations. Other operations may also be supported by derived
	 * classes.
	 *
	 * The transfer request is forwarded the derived class, which must implement the pure virtual
	 * function transfer_().
	 *
	 * This call should not block. Transfer requests are asynchronous and are not guaranteed to
	 * return immediately with a result.
	 *
	 * If the derived driver class returns a status code other than i2c::status::enqueued or
	 * i2c::status::busy, the callback will be immediately invoked. If the operation was enqueued, the
	 * final status will be reported when the callback is called.
	 *
	 * @param op A bus transfer is defined by an operation (op). The derived class will use the
	 *	data supplied in the op to configure the bus and transfer the data.
	 * @param cb Optional callback parameter. When set, the callback will be invoked upon completion
	 *	of the transfer.
	 * @returns The status of the bus transfer.
	 */
	virtual i2c::status transfer(i2c::op_t& op, const cb_t& cb = nullptr) noexcept
	{
		auto status = transfer_(op, cb);

		if(status != i2c::status::enqueued && status != i2c::status::busy)
		{
			callback(op, status, cb);
		}

		return status;
	}

	/** Get the current bus status.
	 *
	 * @returns The current bus status.
	 */
	i2c::status busStatus() const noexcept
	{
		return bus_status_;
	}

	/** Get the current baudrate.
	 *
	 * @returns The currently configured baudrate.
	 */
	i2c::baud baudrate() const noexcept
	{
		return baud_;
	}

	/** Set a new baudrate.
	 *
	 * Since each comm bus driver is different, the baudrate update request is forwarded to
	 *	the derived class, which must implement the pure virtual function baudrate_().
	 *
	 * @param baud The new baudrate for the bus.
	 * @returns The baudrate the bus is configured to use. The returned baudrate may not match the
	 * 	input under the following conditions:
	 *	- Input baudrate was invalid (no update)
	 *	- Input baudrate was rounded by the bus driver
	 */
	i2c::baud baudrate(i2c::baud baud) noexcept
	{
		baud_ = baudrate_(baud);

		return baud_;
	}

	/** Perform an I2C bus sweep to identify active devices.
	 *
	 * The sweep function pings all I2C addresses. Devices which ACK are stored in a list
	 * and returned via callback.
	 *
	 * @param[in,out] found_list Caller's memory which will contain the successfully found ping
	 *	addresses.
	 * @param[in] cb The callback which will be called to indicate that the sweep is complete.
	 *	When the cb is called, found_list is valid and can be used by the caller.
	 */
	void sweep(sweep_list_t& found_list, const sweep_cb_t& cb) noexcept;

  protected:
	/** Handle callbacks.
	 *
	 * @param op The operation which was completed. This will be passed to the callback.
	 * @param status The result of the operation.
	 * @param cb The callback function to invoke.
	 */
	void callback(const i2c::op_t& op, i2c::status status, const cb_t& cb) noexcept
	{
		if(cb)
		{
			cb(op, status);
		}
	}

	/** Configure the I2C bus for operation.
	 *
	 * This function is implemented by the derived class. It is responsible for
	 * configuring the target hardware for operation.
	 *
	 * @param pullups The target pull-up configuration.
	 */
	virtual void configure_(i2c::pullups pullups) noexcept = 0;

	/** The drived I2C class's baudrate update implementation.
	 *
	 * Derived classes override this baudrate function to handle baudrate change requests.
	 *
	 * @param baud The new baudrate for the bus.
	 * @returns The baudrate the bus is configured to use. The returned baudrate may not match the
	 * 	input under the following conditions:
	 *	- Input baudrate was invalid (no update)
	 *	- Input baudrate was rounded by the bus driver
	 */
	virtual i2c::baud baudrate_(i2c::baud baud) noexcept = 0;

	/** The derived comm class's transfer implementation.
	 *
	 * Derived classes override this transfer function to handle specific
	 * transfer operations for each device.
	 *
	 * Callback is passed in for drivers which enqueue operations or use AO model.
	 * The base class handles the callback, so there is no need to worry about invoking
	 * the callback from a client driver.
	 *
	 * Just mark your callback `(void)callback` if you aren't using it.
	 *
	 * To indicate that a transfer has been enqueued for later processing, return
	 * `i2c::status::enqueued`
	 */
	virtual i2c::status transfer_(const i2c::op_t& op, const cb_t& cb) noexcept = 0;

	/** Configure pull-ups.
	 *
	 * This function is implemented by the derived class. It is responsible for
	 * configuring the target hardware to use the requested pullup setting.
	 *
	 * @param pullups The target pull-up setting.
	 * @returns The configured pull-up setting.
	 */
	virtual i2c::pullups setPullups_(i2c::pullups pullups) noexcept = 0;

  protected:
	/// Tracks the active pull-up configuration.
	i2c::pullups pullups_ = i2c::pullups::external;
	/// Tracks the status of the I2C bus.
	i2c::state state_ = i2c::state::idle;
	/// Indicates whether the driver is started or stopped
	bool started_ = false;
	/// Stores the current bus status.
	i2c::status bus_status_ = i2c::status::ok;
	/// Stores the currently configured baudrate.
	i2c::baud baud_;
};

} // namespace i2c

/// @}
// End group

} // namespace embvm

#endif // I2C_DRIVER_HPP_
