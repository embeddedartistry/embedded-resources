#ifndef TIME_OF_FLIGHT_HPP_
#define TIME_OF_FLIGHT_HPP_

#include <inplace_function/inplace_function.hpp>
#include <string_view>

namespace embvm
{
/// @addtogroup FrameworkDriver
/// @{

/// Definitions, functions, and classes related to Time-of-Flight sensors.
namespace tof
{
/// Representation for the ToF range result in mm.
using distance_t = uint16_t;

/// Alias for the read() callback function.
using cb_t = stdext::inplace_function<void(distance_t)>;

/// Representation for an invalid range result.
static const distance_t INVALID_RANGE = UINT16_MAX;

/** ToF ranging mode options.
 *
 * This enum represents the different ToF ranging settings. Not every sensor
 * will have all of these modes; invalid modes should assert() or default
 * to another mode.
 */
enum class mode
{
	/// The sensor's default ranging setting.
	defaultRange = 0,
	/// Short-range mode.
	shortRange,
	/// Medium-range mode.
	medRange,
	/// Long-range mode.
	longRange,
};

enum class status
{
	/// Reading is valid.
	ok = 0,
	/// Measurement did not change.
	noChange,
	/// Measurement was not ready.
	notReady,
	/// Target is below min detection threshold.
	rangeValidMinRangeClipped,
	/// Range is OK but object is a result of multiple pulses merged together.
	rangeValidMergedPulse,
	/// Range is invalid.
	rangeInvalid,
	/// There was an unexpectedly large jump between signal readings.
	sigmaError,
	/// An error identifying or interpreting the signal occurred.
	signalError,
	/// Phase out of valid limits.
	outOfBoundsError,
	/// An error occurred with the sensor hardware.
	hardwareError,
	/// A sensor internal algorithm Errored.
	processingError,
	/// Crosstalk resulted in an invalid signal.
	crosstalkSignalError,
	/// The ROI setting provided to the sensor was invalid.
	invalidRoi,
	/// An unknown error occurred.
	unknown
};

/** This class defines common time-of-flight (ToF) sensor interfaces.
 *
 * tof::sensor is the base class for time-of-flight (ToF) sensor devices.
 *
 * # Define a ToF Driver
 *
 * To define a ToF driver, derive from the tof::sensor object:
 *
 * @code
 * class vl53l1x final : public tof::sensor
 * { ... };
 * @endcode
 *
 * Derived classes must implement the following functions:
 * - read()
 * - reset()
 * - registerReadCallback()
 * - mode()
 * - getMaxRangeForModeDark()
 * - getMaxRangeForModeStrongLight()
 *
 * Additional sensor-specific functions may be defined. They are simply not available in the
 * public/common interface.
 *
 * Derived classes must also implement pure virtual embvm::DriverBase functions:
 * - start_()
 * - stop_()
 *
 * # Using a ToF driver
 *
 * The ToF driver should be configured with the desired ranging mode (tof::mode) using the mode()
 * function.
 *
 * Once the sensor is configured, a callback should be registered via registerReadCallback().
 * The callback is required in order to receive results from the read() command.
 *
 * To get a range result from the sensor, call the read() function. The function operates
 * asynchronously and will return immediately. When the read operation completes, registered
 * callbacks will be called.
 */
class sensor
{
  protected:
	/** Default constructor.
	 *
	 * Initializes the tof::sensor instance with a generic name.
	 */
	sensor() noexcept : name_("Unidentified ToF Driver")
	{
		// Empty body
	}

	/** Construct an tof::sensor with a C-string name.
	 *
	 * Initializes the tof::sensor instance with a name.
	 *
	 * @param name The name of the ToF instance.
	 */
	sensor(const char* name) noexcept : name_(name)
	{
		// Empty body
	}

	/** Construct an tof::sensor with a std::string_view name.
	 *
	 * Initializes the tof::sensor instance with a name.
	 *
	 * @param name The name of the ToF instance.
	 */
	sensor(const std::string_view& name) noexcept :  name_(name)
	{
		// Empty body
	}

	/// Default destructor.
	~sensor() noexcept = default;

  public:
/// Turn on the device, put it into a fully operational state, and apply
/// necessary configuration options.
virtual void start() noexcept = 0;

/// Put the sensor into a non-operational state. If possible, the device should be powered down
/// or put into the lowest power state.
virtual void stop() noexcept = 0;

	/** Check the maximum range in the dark.
	 *
	 * @returns the sensor's maximum distance capability (in mm) in the dark.
	 */
	virtual tof::distance_t getMaxRangeForModeDark(tof::mode m) const noexcept = 0;

	/** Check the maximum range in strong light conditions.
	 *
	 * @returns the sensor's maximum distance capability (in mm) in strong light.
	 */
	virtual tof::distance_t getMaxRangeForModeStrongLight(tof::mode m) const noexcept = 0;

	/** Get the ToF ranging mode.
	 *
	 * @returns the currently configured ToF ranging mode.
	 */
	tof::mode mode() const noexcept
	{
		return mode_;
	}

	/** Set the ToF ranging mode.
	 *
	 * @param m The desired ToF ranging mode.
	 * @returns the active ToF mode.
	 */
	virtual tof::mode mode(tof::mode m) noexcept = 0;

	/** Register a callback for the read() function.
	 *
	 * The read() function works asynchronously, and the result will be provided
	 * to consumers through a callback function. When the read() operation completes,
	 * the callback will be invoked with the result.
	 *
	 * This function must be implemented by the derived class.
	 *
	 * @param cb The functor which will be called when read() completes.
	 */
	virtual void registerReadCallback(const cb_t& cb) noexcept = 0;

	/** Register a callback for the read() function.
	 *
	 * The read() function works asynchronously, and the result will be provided
	 * to consumers through a callback function. When the read() operation completes,
	 * the callback will be invoked with the resulting range (in mm).
	 *
	 * This function must be implemented by the derived class.
	 *
	 * @param cb The functor which will be called when read() completes.
	 */
	virtual void registerReadCallback(cb_t&& cb) noexcept = 0;

	/** Trigger a sensor read.
	 *
	 * Trigger an asynchronous read of the ToF sensor. The result will be provided
	 * to consumers through a callback function. When the read() operation completes,
	 * the callback will be invoked with the result.
	 *
	 * The sensor result will be presented as current range in mm.
	 */
	virtual void read() noexcept = 0;

	/// Reset the ToF sensor.
	virtual void reset() noexcept = 0;

  protected:
	/// The configured ranging mode for the ToF sensor.
	tof::mode mode_ = tof::mode::defaultRange;
	std::string_view name_;
};

} // namespace tof

/// @}
// End group

} // namespace embvm

#endif // TIME_OF_FLIGHT_HPP_
