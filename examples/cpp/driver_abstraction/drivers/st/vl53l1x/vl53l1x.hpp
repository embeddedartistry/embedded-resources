#ifndef ST_V53L1X_HPP_
#define ST_V53L1X_HPP_

#include <cstdint>
#include <driver/i2c.hpp>
#include <driver/time_of_flight.hpp>
#include <endian/endian.hpp>
#include <etl/list.h>
#include <etl/pool.h>

// TODO: make this a configuration option that we get from the build system
#define ENABLE_THREADING

#ifdef ENABLE_THREADING
#include <mutex>
#define VL53L1X_LOCK() lock_.lock()
#define VL53L1X_UNLOCK() lock_.unlock()
#else
#define VL53L1X_UNLOCK()
#define VL53L1X_LOCK()
#endif

// Reference for future improvements:
// https://github.com/pololu/vl53l1x-st-api-arduino/tree/controller/vl53l1x-st-api
// TODO: how can I make this queue size customizable without having template hell?

namespace embdrv
{
/// Default I2C address of the VL53L1X part.
/// This may not be the actual part used in the target hardware.
inline constexpr uint8_t VL53L1X_DEFAULT_I2C_ADDR = 0x52;

/** ST VL53L1X Time-of-Flight Sensor Driver
 *
 * This driver is configured to work with I2C. You must initialize the device with
 * an I2C controller.
 *
 * @code
 * embdrv::vl53l1x tof0{i2c0, SPARKFUN_VL53L1X_ADDR};
 * @endcode
 *
 * @ingroup FrameworkDrivers
 */
class vl53l1x final : public embvm::tof::sensor
{
  public:
	/// Alias for signalRate() callback function.
	using rate_cb_t = stdext::inplace_function<void(uint16_t)>;

	/// Alias for the rangeStatus() callback function.
	using status_cb_t = stdext::inplace_function<void(embvm::tof::status)>;

	/// Parameters for custom user zones.
	/// Each value must be between 0-15.
	struct roi
	{
		/// Defines the top-left X coordinate for the ROI.
		uint8_t topLeftX;
		/// Defines the top-left Y coordinate for the ROI.
		uint8_t topLeftY;
		/// Defines the bottom-right X coordinate for the ROI.
		uint8_t bottomRightX;
		/// Defines the bottom-right X coordinate for the ROI.
		uint8_t bottomRightY;
	};

	/** Create a VL53L1X device.
	 *
	 * @param i2c The i2c controller driver that this device is connected to.
	 * @param address The i2c address of the sensor.
	 */
	explicit vl53l1x(embvm::i2c::controller& i2c, uint8_t address = VL53L1X_DEFAULT_I2C_ADDR) noexcept
		: embvm::tof::sensor("ST VL53L1X ToF"), i2c_(i2c), address_(address)
	{
	}

	void read() noexcept final;
	void reset() noexcept final;

	[[nodiscard]] embvm::tof::distance_t
		getMaxRangeForModeDark(embvm::tof::mode m) const noexcept final;
	[[nodiscard]] embvm::tof::distance_t
		getMaxRangeForModeStrongLight(embvm::tof::mode m) const noexcept final;
	embvm::tof::mode mode(embvm::tof::mode m) noexcept final;

	/// Get the current signal rate.
	/// When called, registered signal rate callbacks will be invoked with the signal rate.
	/// To register a callback, use registerSignalRateCallback().
	void signalRate() noexcept;

	/// Get the ranging status
	/// When called, registered range status callbacks will be invoked with the signal rate.
	/// To register a callback, use registerStatusCallback().
	void rangeStatus() noexcept;

	// todo; Get signal rate (results from last measurement, signal rate)
	// todo: getRangeStatus(); Returns the results from the last measurement, 0 = valid

	// setUserRoi(roi*) - set custom sensor zone
	// setCenter(uint8_t centerX, uint8_t centerY) - center of a custom zone
	// setZoneSize(uint8_t width, uint8_t height) // size of custom zone
	// roi* getUserROI

	void registerReadCallback(const read_cb_t& cb) noexcept final
	{
		read_cb_list_.insert(read_cb_list_.end(), cb);
	}

	void registerReadCallback(read_cb_t&& cb) noexcept final
	{
		read_cb_list_.insert(read_cb_list_.end(), cb);
	}

	/// Register a callback for use with the signalRate() function
	/// @param cb The callback to invoke hen signalRate() completes.
	void registerSignalRateCallback(const rate_cb_t& cb) noexcept
	{
		rate_cb_list_.insert(rate_cb_list_.end(), cb);
	}

	/// @overload void registerSignalRateCallback(const rate_cb_t& cb)
	void registerSignalRateCallback(rate_cb_t&& cb) noexcept
	{
		rate_cb_list_.insert(rate_cb_list_.end(), cb);
	}

	/// Register a callback for use with the rangeStatus() function
	/// @param cb The callback to invoke when rangeStatus() completes.
	void registerStatusCallback(const status_cb_t& cb) noexcept
	{
		status_cb_list_.insert(status_cb_list_.end(), cb);
	}

	/// @overload void registerStatusCallback(const status_cb_t& cb)
	void registerStatusCallback(status_cb_t&& cb) noexcept
	{
		status_cb_list_.insert(status_cb_list_.end(), cb);
	}

	void start() noexcept final;
	void stop() noexcept final;

  private:
	/// Convenience function to read a register from the VL51L1X device.
	/// @param reg_buf the buffer containing the TX register data.
	/// @param rx_buffer The buffer to store the received contents into.
	/// @param rx_size the number of expected RX bytes.
	/// @param cb The callback function to invoke when the register read completes.
	void readReg(const uint16_t* reg_buf, uint8_t* rx_buffer, size_t rx_size,
				 const embvm::i2c::controller::cb_t& cb) noexcept;

	/// Convenience function to write to a register on the VL51L1X device.
	/// @param reg The register to write to.
	/// @param tx_buffer The buffer containing the tx data to send to the target register.
	/// @param tx_size The number of bytes to transmit
	/// @param cb The callback function to invoke when the register write completes.
	void writeReg(uint16_t reg, const uint8_t* tx_buffer, size_t tx_size,
				  const embvm::i2c::controller::cb_t& cb) noexcept;

  private:
	/// Check the model ID of the VL53L1X sensor
	void checkModelID() noexcept;

	/// Read the trim values of the sensor and update the config_block_
	void readTrim() noexcept;

	/// Trigger a range measurement
	void startMeasurement() noexcept;

	/// Read the oscillator calibration and store it in the private variable osc_calibrate_val_
	void readOscillatorCal() noexcept;

	/// Clear the VL53L1X pending interrupt
	void clearInterrupt() noexcept;

	/// Check if the range data is ready
	void dataReady() noexcept;

	/// Read the range data from the device
	void readData() noexcept;

	/// Local driver callback which is invoked when data is ready
	void dataReadyCb(embvm::i2c::op_t op, embvm::i2c::status status) noexcept;

	/// Function that starts the sensor firmware and begins measurements
	void kickoffMeasurementOnceFirmwareReady() noexcept;

	/// Private callback function to get the range status
	void rangeStatusCb(uint8_t status) noexcept;

	/// Convert the raw range status code to the framework ToF statuses
	/// @param status The raw sensor status code.
	/// @returns The equivalent framework standard status code.
	[[nodiscard]] static embvm::tof::status convertStatus(uint8_t status) noexcept;

	template<typename TData>
	void destroy(const TData* ptr) noexcept
	{
		VL53L1X_LOCK();
		i2c_pool_.template destroy<TData>(ptr);
		VL53L1X_UNLOCK();
	}

	template<typename TData>
	TData* create(TData input = 0) noexcept
	{
		static_assert(std::is_integral<TData>::value,
					  "This function is designed to work with integral types");

		VL53L1X_LOCK();
		auto r = i2c_pool_.template create<TData>(input);
		VL53L1X_UNLOCK();

		return r;
	}

  private:
	/// The I2C controller bus that this device is connected to.
	embvm::i2c::controller& i2c_;

	/// The I2C address for the VL53L1X
	const uint8_t address_;

	/// The model ID for the VL53L1X, read from the device during start().
	uint8_t model_id_ = 0;
	uint16_t osc_calibrate_val_ = 0;

	/// Static memory pool used for I2C transactions.
	etl::generic_pool<sizeof(uint32_t), alignof(uint32_t), 64> i2c_pool_{};

#ifdef ENABLE_THREADING
	std::mutex lock_{};
#endif

	/// List that stores the read() callback functions.
	etl::list<read_cb_t, 2> read_cb_list_{};

	/// List that stores the rangeStatus() callback functions.
	etl::list<status_cb_t, 2> status_cb_list_{};

	/// List that stores the signalRate() callback functions.
	etl::list<rate_cb_t, 2> rate_cb_list_{};

	/// This is 135 bytes to be written every time to the VL53L1X to initiate a measurement
	/// 0x29 is written to memory location 0x01, which is the register for the I2C address which
	/// is indeed 0x29. So this makes sense. We could evaluate the default register settings of a
	/// given device against this set of bytes and write only the ones that are different but it's
	/// faster, easier, and perhaps fewer code words to write the config block as is. The block was
	/// obtained via inspection of the ST P-NUCLEO-53L1A1
	/// Prepended two bytes to serve as address holding
	// NOLINTNEXTLINE
	std::array<uint8_t, 137> config_block_ = {
		0x00, 0x00, 0x29, 0x02, 0x10, 0x00, 0x28, 0xBC, 0x7A, 0x81, // 8
		0x80, 0x07, 0x95, 0x00, 0xED, 0xFF, 0xF7, 0xFD, // 16
		0x9E, 0x0E, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, // 24
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, // 32
		0x28, 0x00, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, // 40
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, // 48
		0x02, 0x00, 0x02, 0x08, 0x00, 0x08, 0x10, 0x01, // 56
		0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x02, // 64
		0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x0B, 0x00, // 72
		0x00, 0x02, 0x0A, 0x21, 0x00, 0x00, 0x02, 0x00, // 80
		0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x38, 0xFF, // 88
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x0F, // 96
		0x00, 0xA5, 0x0D, 0x00, 0x80, 0x00, 0x0C, 0x08, // 104
		0xB8, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x10, 0x00, // 112
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0F, // 120
		0x0D, 0x0E, 0x0E, 0x01, 0x00, 0x02, 0xC7, 0xFF, // 128
		0x8B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x40 // 129 - 135 (0x81 - 0x87)
	};

#if 0
const uint8_t VL51L1X_DEFAULT_CONFIGURATION[] = {
0x00, /* 0x2d : set bit 2 and 5 to 1 for fast plus mode (1MHz I2C), else don't touch */
0x00, /* 0x2e : bit 0 if I2C pulled up at 1.8V, else set bit 0 to 1 (pull up at AVDD) */
0x00, /* 0x2f : bit 0 if GPIO pulled up at 1.8V, else set bit 0 to 1 (pull up at AVDD) */
0x01, /* 0x30 : set bit 4 to 0 for active high interrupt and 1 for active low (bits 3:0 must be 0x1), use SetInterruptPolarity() */
0x02, /* 0x31 : bit 1 = interrupt depending on the polarity, use CheckForDataReady() */
0x00, /* 0x32 : not user-modifiable */
0x02, /* 0x33 : not user-modifiable */
0x08, /* 0x34 : not user-modifiable */
0x00, /* 0x35 : not user-modifiable */
0x08, /* 0x36 : not user-modifiable */
0x10, /* 0x37 : not user-modifiable */
0x01, /* 0x38 : not user-modifiable */
0x01, /* 0x39 : not user-modifiable */
0x00, /* 0x3a : not user-modifiable */
0x00, /* 0x3b : not user-modifiable */
0x00, /* 0x3c : not user-modifiable */
0x00, /* 0x3d : not user-modifiable */
0xff, /* 0x3e : not user-modifiable */
0x00, /* 0x3f : not user-modifiable */
0x0F, /* 0x40 : not user-modifiable */
0x00, /* 0x41 : not user-modifiable */
0x00, /* 0x42 : not user-modifiable */
0x00, /* 0x43 : not user-modifiable */
0x00, /* 0x44 : not user-modifiable */
0x00, /* 0x45 : not user-modifiable */
0x20, /* 0x46 : interrupt configuration 0->level low detection, 1-> level high, 2-> Out of window, 3->In window, 0x20-> New sample ready , TBC */
0x0b, /* 0x47 : not user-modifiable */
0x00, /* 0x48 : not user-modifiable */
0x00, /* 0x49 : not user-modifiable */
0x02, /* 0x4a : not user-modifiable */
0x0a, /* 0x4b : not user-modifiable */
0x21, /* 0x4c : not user-modifiable */
0x00, /* 0x4d : not user-modifiable */
0x00, /* 0x4e : not user-modifiable */
0x05, /* 0x4f : not user-modifiable */
0x00, /* 0x50 : not user-modifiable */
0x00, /* 0x51 : not user-modifiable */
0x00, /* 0x52 : not user-modifiable */
0x00, /* 0x53 : not user-modifiable */
0xc8, /* 0x54 : not user-modifiable */
0x00, /* 0x55 : not user-modifiable */
0x00, /* 0x56 : not user-modifiable */
0x38, /* 0x57 : not user-modifiable */
0xff, /* 0x58 : not user-modifiable */
0x01, /* 0x59 : not user-modifiable */
0x00, /* 0x5a : not user-modifiable */
0x08, /* 0x5b : not user-modifiable */
0x00, /* 0x5c : not user-modifiable */
0x00, /* 0x5d : not user-modifiable */
0x01, /* 0x5e : not user-modifiable */
0xdb, /* 0x5f : not user-modifiable */
0x0f, /* 0x60 : not user-modifiable */
0x01, /* 0x61 : not user-modifiable */
0xf1, /* 0x62 : not user-modifiable */
0x0d, /* 0x63 : not user-modifiable */
0x01, /* 0x64 : Sigma threshold MSB (mm in 14.2 format for MSB+LSB), use SetSigmaThreshold(), default value 90 mm  */
0x68, /* 0x65 : Sigma threshold LSB */
0x00, /* 0x66 : Min count Rate MSB (MCPS in 9.7 format for MSB+LSB), use SetSignalThreshold() */
0x80, /* 0x67 : Min count Rate LSB */
0x08, /* 0x68 : not user-modifiable */
0xb8, /* 0x69 : not user-modifiable */
0x00, /* 0x6a : not user-modifiable */
0x00, /* 0x6b : not user-modifiable */
0x00, /* 0x6c : Intermeasurement period MSB, 32 bits register, use SetIntermeasurementInMs() */
0x00, /* 0x6d : Intermeasurement period */
0x0f, /* 0x6e : Intermeasurement period */
0x89, /* 0x6f : Intermeasurement period LSB */
0x00, /* 0x70 : not user-modifiable */
0x00, /* 0x71 : not user-modifiable */
0x00, /* 0x72 : distance threshold high MSB (in mm, MSB+LSB), use SetD:tanceThreshold() */
0x00, /* 0x73 : distance threshold high LSB */
0x00, /* 0x74 : distance threshold low MSB ( in mm, MSB+LSB), use SetD:tanceThreshold() */
0x00, /* 0x75 : distance threshold low LSB */
0x00, /* 0x76 : not user-modifiable */
0x01, /* 0x77 : not user-modifiable */
0x0f, /* 0x78 : not user-modifiable */
0x0d, /* 0x79 : not user-modifiable */
0x0e, /* 0x7a : not user-modifiable */
0x0e, /* 0x7b : not user-modifiable */
0x00, /* 0x7c : not user-modifiable */
0x00, /* 0x7d : not user-modifiable */
0x02, /* 0x7e : not user-modifiable */
0xc7, /* 0x7f : ROI center, use SetROI() */
0xff, /* 0x80 : XY ROI (X=Width, Y=Height), use SetROI() */
0x9B, /* 0x81 : not user-modifiable */
0x00, /* 0x82 : not user-modifiable */
0x00, /* 0x83 : not user-modifiable */
0x00, /* 0x84 : not user-modifiable */
0x01, /* 0x85 : not user-modifiable */
0x00, /* 0x86 : clear interrupt, use ClearInterrupt() */
0x00  /* 0x87 : start ranging, use StartRanging() or StopRanging(), If you want an automatic start after VL53L1X_init() call, put 0x40 in location 0x87 */
};
#endif
};

} // namespace embdrv

#endif // ST_V53L1X_HPP_
