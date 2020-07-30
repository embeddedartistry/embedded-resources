#include "vl53l1x.hpp"
#include "vl53l1x_registers.h"
#include <cassert>

// References:
// * https://github.com/pololu/vl53l1x-arduino/blob/controller/VL53L1X.cpp

using namespace embdrv;

namespace
{
constexpr uint8_t MODEL_ID = UINT8_C(0xEA);
constexpr embvm::tof::distance_t shortRangeDark = 1360; // mm
constexpr embvm::tof::distance_t medRangeDark = 2900; // mm
constexpr embvm::tof::distance_t longRangeDark = 3600; // mm
constexpr embvm::tof::distance_t shortRangeStrongLight = 1350; // mm
constexpr embvm::tof::distance_t medRangeStrongLight = 760; // mm
constexpr embvm::tof::distance_t longRangeStrongLight = 730; // mm

constexpr uint8_t periodA_shortRange = UINT8_C(7);
constexpr uint8_t periodB_shortRange = UINT8_C(5);
constexpr uint8_t phaseHigh_shortRange = UINT8_C(0x38);
constexpr uint8_t phaseInit_shortRange = UINT8_C(6);

constexpr uint8_t periodA_medRange = UINT8_C(0xB);
constexpr uint8_t periodB_medRange = UINT8_C(0x9);
constexpr uint8_t phaseHigh_medRange = UINT8_C(0x78);
constexpr uint8_t phaseInit_medRange = UINT8_C(10);

constexpr uint8_t periodA_longRange = UINT8_C(0xF);
constexpr uint8_t periodB_longRange = UINT8_C(0xD);
constexpr uint8_t phaseHigh_longRange = UINT8_C(0xB8);
constexpr uint8_t phaseInit_longRange = UINT8_C(14);

constexpr uint8_t VL53L1_DEVICEERROR_VCSELCONTINUITYTESTFAILURE = UINT8_C(1);
constexpr uint8_t VL53L1_DEVICEERROR_VCSELWATCHDOGTESTFAILURE = UINT8_C(2);
constexpr uint8_t VL53L1_DEVICEERROR_NOVHVVALUEFOUND = UINT8_C(3);
constexpr uint8_t VL53L1_DEVICEERROR_MSRCNOTARGET = UINT8_C(4);
constexpr uint8_t VL53L1_DEVICEERROR_RANGEPHASECHECK = UINT8_C(5);
constexpr uint8_t VL53L1_DEVICEERROR_SIGMATHRESHOLDCHECK = UINT8_C(6);
constexpr uint8_t VL53L1_DEVICEERROR_PHASECONSISTENCY = UINT8_C(7);
constexpr uint8_t VL53L1_DEVICEERROR_MINCLIP = UINT8_C(8);
constexpr uint8_t VL53L1_DEVICEERROR_RANGECOMPLETE = UINT8_C(9);
constexpr uint8_t VL53L1_DEVICEERROR_ALGOUNDERFLOW = UINT8_C(10);
constexpr uint8_t VL53L1_DEVICEERROR_ALGOOVERFLOW = UINT8_C(11);
constexpr uint8_t VL53L1_DEVICEERROR_RANGEIGNORETHRESHOLD = UINT8_C(12);
constexpr uint8_t VL53L1_DEVICEERROR_USERROICLIP = UINT8_C(13);
constexpr uint8_t VL53L1_DEVICEERROR_REFSPADCHARNOTENOUGHDPADS = UINT8_C(14);
constexpr uint8_t VL53L1_DEVICEERROR_REFSPADCHARMORETHANTARGET = UINT8_C(15);
constexpr uint8_t VL53L1_DEVICEERROR_REFSPADCHARLESSTHANTARGET = UINT8_C(16);
constexpr uint8_t VL53L1_DEVICEERROR_MULTCLIPFAIL = UINT8_C(17);
constexpr uint8_t VL53L1_DEVICEERROR_GPHSTREAMCOUNT0READY = UINT8_C(18);
constexpr uint8_t VL53L1_DEVICEERROR_RANGECOMPLETE_NO_WRAP_CHECK = UINT8_C(19);
constexpr uint8_t VL53L1_DEVICEERROR_EVENTCONSISTENCY = UINT8_C(20);
constexpr uint8_t VL53L1_DEVICEERROR_MINSIGNALEVENTCHECK = UINT8_C(21);
constexpr uint8_t VL53L1_DEVICEERROR_RANGECOMPLETE_MERGED_PULSE = UINT8_C(22);

constexpr uint8_t VL53L1_INT_CLR = UINT8_C(0x01);

constexpr uint16_t VL53L1X_TRIM_BYTE_COUNT = UINT16_C(37);

constexpr uint16_t MODEL_ID_REG = embutil::byteswap<uint16_t>(VL53L1_IDENTIFICATION_MODEL_ID);
constexpr uint16_t READ_RANGE_REG =
	embutil::byteswap<uint16_t>(VL53L1_RESULT_FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0);
constexpr uint16_t READ_SIGNAL_RATE_REG =
	embutil::byteswap<uint16_t>(VL53L1_RESULT_PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0);
constexpr uint16_t DATA_READY_REG = embutil::byteswap<uint16_t>(VL53L1_GPIO_TIO_HV_STATUS);
constexpr uint16_t RANGE_CONFIG_PERIOD_A_REG =
	embutil::byteswap<uint16_t>(VL53L1_RANGE_CONFIG_VCSEL_PERIOD_A);
constexpr uint16_t RANGE_CONFIG_PERIOD_B_REG =
	embutil::byteswap<uint16_t>(VL53L1_RANGE_CONFIG_VCSEL_PERIOD_B);
constexpr uint16_t RANGE_CONFIG_PERIOD_PHASE_HIGH_REG =
	embutil::byteswap<uint16_t>(VL53L1_RANGE_CONFIG_VALID_PHASE_HIGH);
constexpr uint16_t WOI_SD0_REG = embutil::byteswap<uint16_t>(VL53L1_SD_CONFIG_WOI_SD0);
constexpr uint16_t WOI_SD1_REG = embutil::byteswap<uint16_t>(VL53L1_SD_CONFIG_WOI_SD1);
constexpr uint16_t INITIAL_PHASE_SD0_REG =
	embutil::byteswap<uint16_t>(VL53L1_SD_CONFIG_INITIAL_PHASE_SD0);
constexpr uint16_t INITIAL_PHASE_SD1_REG =
	embutil::byteswap<uint16_t>(VL53L1_SD_CONFIG_INITIAL_PHASE_SD1);
constexpr uint16_t SOFT_RESET_REG = embutil::byteswap<uint16_t>(VL53L1_SOFT_RESET);
constexpr uint16_t FIRMWARE_SYSTEM_STATUS_REG =
	embutil::byteswap<uint16_t>(VL53L1_FIRMWARE_SYSTEM_STATUS);
constexpr uint16_t READ_RANGE_STATUS_REG = embutil::byteswap<uint16_t>(VL53L1_RESULT_RANGE_STATUS);
constexpr uint16_t SYSTEM_INTERUPT_CLEAR =
	embutil::byteswap<uint16_t>(VL53L1_SYSTEM_INTERRUPT_CLEAR);
constexpr uint16_t READ_OSC_CAL = embutil::byteswap<uint16_t>(VL53L1_RESULT_OSC_CALIBRATE_VAL);

} // namespace

void vl53l1x::dataReady() noexcept
{
	auto* reg = create(DATA_READY_REG);
	readReg(reg, reinterpret_cast<uint8_t*>(reg), sizeof(uint8_t),
			std::bind(&vl53l1x::dataReadyCb, this, std::placeholders::_1, std::placeholders::_2));
}

void vl53l1x::dataReadyCb(embvm::i2c::op_t op, embvm::i2c::status status) noexcept
{
	// check if sensor has new reading available
	// assumes interrupt is active low (GPIO_HV_MUX__CTRL bit 4 is 1)
	bool ready = ((op.rx_buffer[0] & 0x01) == 0);

	destroy<uint16_t>(reinterpret_cast<const uint16_t*>(op.tx_buffer));

	if(status == embvm::i2c::status::ok)
	{
		if(ready)
		{
			readData();
		}
		else
		{
			// Try until data is ready
			dataReady();
		}
	}
}

void vl53l1x::readData() noexcept
{
	auto* r = create(READ_RANGE_REG);
	auto* out = create<uint16_t>();

	readReg(r, reinterpret_cast<uint8_t*>(out), sizeof(uint16_t), [&](auto op, auto status) {
		auto range = embutil::byteswap<uint16_t>(
			*reinterpret_cast<uint16_t*>(static_cast<void*>(op.rx_buffer)));

		destroy<uint16_t>(
			reinterpret_cast<const uint16_t*>(static_cast<const void*>(op.tx_buffer)));
		destroy<uint16_t>(reinterpret_cast<uint16_t*>(static_cast<void*>(op.rx_buffer)));

		clearInterrupt();

		if(status == embvm::i2c::status::ok)
		{
			for(const auto& cb : read_cb_list_)
			{
				cb(range);
			}
		}
	});
}

// These error code mappings are just first-pass and arbitrary.
embvm::tof::status vl53l1x::convertStatus(uint8_t status) noexcept
{
	embvm::tof::status convertedStatus;
	switch(status)
	{
		case VL53L1_DEVICEERROR_GPHSTREAMCOUNT0READY:
			convertedStatus = embvm::tof::status::notReady;
			break;
		case VL53L1_DEVICEERROR_RANGECOMPLETE_NO_WRAP_CHECK:
			convertedStatus = embvm::tof::status::processingError;
			break;
		case VL53L1_DEVICEERROR_RANGEPHASECHECK:
		case VL53L1_DEVICEERROR_MINSIGNALEVENTCHECK:
			convertedStatus = embvm::tof::status::outOfBoundsError;
			break;
		case VL53L1_DEVICEERROR_MSRCNOTARGET:
		case VL53L1_DEVICEERROR_REFSPADCHARNOTENOUGHDPADS:
		case VL53L1_DEVICEERROR_REFSPADCHARMORETHANTARGET:
		case VL53L1_DEVICEERROR_REFSPADCHARLESSTHANTARGET:
		case VL53L1_DEVICEERROR_MULTCLIPFAIL:
			convertedStatus = embvm::tof::status::signalError;
			break;
		case VL53L1_DEVICEERROR_SIGMATHRESHOLDCHECK:
			convertedStatus = embvm::tof::status::sigmaError;
			break;
		case VL53L1_DEVICEERROR_PHASECONSISTENCY:
		case VL53L1_DEVICEERROR_ALGOOVERFLOW:
		case VL53L1_DEVICEERROR_ALGOUNDERFLOW:
		case VL53L1_DEVICEERROR_NOVHVVALUEFOUND:
		case VL53L1_DEVICEERROR_EVENTCONSISTENCY:
			convertedStatus = embvm::tof::status::processingError;
			break;
		case VL53L1_DEVICEERROR_RANGEIGNORETHRESHOLD:
			convertedStatus = embvm::tof::status::crosstalkSignalError;
			break;
		case VL53L1_DEVICEERROR_MINCLIP:
			convertedStatus = embvm::tof::status::rangeValidMinRangeClipped;
			break;
		case VL53L1_DEVICEERROR_USERROICLIP:
			convertedStatus = embvm::tof::status::invalidRoi;
			break;
		case VL53L1_DEVICEERROR_RANGECOMPLETE:
			convertedStatus = embvm::tof::status::ok;
			break;
		case VL53L1_DEVICEERROR_VCSELWATCHDOGTESTFAILURE:
		case VL53L1_DEVICEERROR_VCSELCONTINUITYTESTFAILURE:
			convertedStatus = embvm::tof::status::hardwareError;
			break;
		case VL53L1_DEVICEERROR_RANGECOMPLETE_MERGED_PULSE:
			convertedStatus = embvm::tof::status::rangeValidMergedPulse;
			break;
		default:
			// TODO: seems like the wrong choice
			convertedStatus = embvm::tof::status::noChange;
	}
	return convertedStatus;
}

void vl53l1x::rangeStatusCb(uint8_t status) noexcept
{
	auto s = convertStatus(status);

	for(const auto& cb : status_cb_list_)
	{
		cb(s);
	}
}

void vl53l1x::rangeStatus() noexcept
{
	auto* r = create(READ_RANGE_STATUS_REG);
	auto* status = create<uint8_t>();

	readReg(r, status, sizeof(uint8_t), [&](auto op, auto op_status) noexcept {
		auto value = op.rx_buffer[0];

		destroy<uint16_t>(
			reinterpret_cast<const uint16_t*>(static_cast<const void*>(op.tx_buffer)));
		destroy(op.rx_buffer);

		if(op_status == embvm::i2c::status::ok)
		{
			rangeStatusCb(value);
		}
	});
}

void vl53l1x::signalRate() noexcept
{
	auto* r = create(READ_SIGNAL_RATE_REG);
	auto* reading = create<uint16_t>();

	readReg(r, reinterpret_cast<uint8_t*>(reading), sizeof(uint16_t), [&](auto op, auto status) {
		auto rate = embutil::byteswap<uint16_t>(
			*reinterpret_cast<uint16_t*>(static_cast<void*>(op.rx_buffer)));

		destroy<uint16_t>(
			reinterpret_cast<const uint16_t*>(static_cast<const void*>(op.tx_buffer)));
		destroy<uint16_t>(reinterpret_cast<uint16_t*>(static_cast<void*>(op.rx_buffer)));

		if(status == embvm::i2c::status::ok)
		{
			for(const auto& cb : rate_cb_list_)
			{
				cb(rate);
			}
		}
	});
}

void vl53l1x::read() noexcept
{
	dataReady();
}

void vl53l1x::reset() noexcept
{
	auto* reset = create<uint8_t>(UINT8_C(0)); // start reset

	writeReg(SOFT_RESET_REG, reset, sizeof(uint8_t), [&](auto op, auto op_status) {
		if(op_status == embvm::i2c::status::ok)
		{
			// NOLINTNEXTLINE - We want to reuse the tx_buffer, and we know that it is not const
			const_cast<uint8_t*>(op.tx_buffer)[0] = 1; // exit reset
			writeReg(SOFT_RESET_REG, op.tx_buffer, sizeof(uint8_t), [&](auto op2, auto status) {
				(void)status;
				destroy(op2.tx_buffer);
				kickoffMeasurementOnceFirmwareReady();
			});
		}
		else
		{
			destroy(op.tx_buffer);
		}
	});
}

void vl53l1x::kickoffMeasurementOnceFirmwareReady() noexcept
{
	auto* r = create(FIRMWARE_SYSTEM_STATUS_REG);
	auto* rx = create<uint8_t>();

	readReg(r, rx, sizeof(uint8_t), [&](auto op, auto status) {
		auto ready = op.rx_buffer[0] & 0x01;

		destroy<uint16_t>(reinterpret_cast<const uint16_t*>(op.tx_buffer));
		destroy(op.rx_buffer);

		if(status == embvm::i2c::status::ok)
		{
			if(ready == 0)
			{
				kickoffMeasurementOnceFirmwareReady();
			}
			else
			{
				startMeasurement();
			}
		}
	});
}

void vl53l1x::start() noexcept
{
	if(!i2c_.started())
	{
		i2c_.start();
	}

	checkModelID();
	readOscillatorCal();
	readTrim();

	reset(); // Once reset completes, we read trim and kickoff measurements
}

void vl53l1x::stop() noexcept {}

void vl53l1x::readReg(const uint16_t* reg_buf, uint8_t* rx_buffer, size_t rx_size,
					  const embvm::i2c::controller::cb_t& cb) noexcept
{
	embvm::i2c::op_t t;
	t.op = embvm::i2c::operation::writeRead;
	t.address = address_;
	t.tx_size = sizeof(uint16_t);
	t.tx_buffer = reinterpret_cast<const uint8_t*>(reg_buf);
	t.rx_size = rx_size;
	t.rx_buffer = rx_buffer;

	i2c_.transfer(t, cb);
}

// TODO: can't I do a write-read?
// TODO: remove this stupid double byteswap! All the hardcoded valeus above are byteswapped!
void vl53l1x::writeReg(uint16_t reg, const uint8_t* tx_buffer, size_t tx_size,
					   const embvm::i2c::controller::cb_t& cb) noexcept
{
	embvm::i2c::op_t t;
	t.op = embvm::i2c::operation::write;
	t.address = address_;
	t.tx_size = sizeof(uint16_t);

	t.tx_buffer = reinterpret_cast<const uint8_t*>(create(embutil::byteswap(reg)));

	i2c_.transfer(t, [&](auto op, auto status) {
		(void)status;
		destroy<uint16_t>(reinterpret_cast<const uint16_t*>(op.tx_buffer));
	});

	t.tx_size = tx_size;
	t.tx_buffer = tx_buffer;

	i2c_.transfer(t, cb);
}

void vl53l1x::checkModelID() noexcept
{
	auto* r = create(MODEL_ID_REG);
	readReg(r, &model_id_, sizeof(model_id_), [&](auto op, auto status) {
		(void)status;
		assert(model_id_ == MODEL_ID && "Unexpected model ID found");
		destroy<uint16_t>(reinterpret_cast<const uint16_t*>(op.tx_buffer));
	});
}

void vl53l1x::readOscillatorCal() noexcept
{
	auto* r = create(READ_OSC_CAL);
	readReg(r, reinterpret_cast<uint8_t*>(&osc_calibrate_val_), sizeof(osc_calibrate_val_),
			[&](auto op, auto status) {
				(void)op;
				(void)status;
				destroy<uint16_t>(
					reinterpret_cast<const uint16_t*>(static_cast<const void*>(op.tx_buffer)));
			});
}

void vl53l1x::clearInterrupt() noexcept
{
	// TODO: byteswap?
	auto* int_clear = create(VL53L1_INT_CLR);
	writeReg(SYSTEM_INTERUPT_CLEAR, int_clear, sizeof(uint16_t), [&](auto op, auto status) {
		(void)op;
		(void)status;
		destroy<uint16_t>(
			reinterpret_cast<const uint16_t*>(static_cast<const void*>(op.tx_buffer)));
	});
}

void vl53l1x::readTrim() noexcept
{
	// TODO: I hate this allocation scheme with this kind of iteration... Can we read a block?
	// (Naive block read didn't work)
	// Constants lifted from Sparkfun's Arduino code

	for(uint16_t i = 1; i < VL53L1X_TRIM_BYTE_COUNT; i++)
	{
		auto* r = create(embutil::byteswap(i));

		// i + 1 because config block is 2+ prepended bytes, and the buffer start is at i - 1,
		// so that nets to an initial index of i + 1
		readReg(r, &config_block_[i + 1], sizeof(uint8_t), [&](auto op, auto status) {
			(void)op;
			(void)status;
			destroy<uint16_t>(reinterpret_cast<const uint16_t*>(op.tx_buffer));
		});
	}
}

void vl53l1x::startMeasurement() noexcept
{
	embvm::i2c::op_t t;
	t.op = embvm::i2c::operation::write;
	t.address = address_;
	t.tx_size = sizeof(config_block_);
	t.tx_buffer = reinterpret_cast<const uint8_t*>(config_block_.data());

	*reinterpret_cast<uint16_t*>(static_cast<void*>(&config_block_[0])) =
		embutil::byteswap<uint16_t>(1);

	i2c_.transfer(t);
}

embvm::tof::distance_t vl53l1x::getMaxRangeForModeDark(embvm::tof::mode m) const noexcept
{
	embvm::tof::distance_t d = embvm::tof::INVALID_RANGE;

	switch(m)
	{
		case embvm::tof::mode::defaultRange:
		case embvm::tof::mode::shortRange:
			d = shortRangeDark;
			break;
		case embvm::tof::mode::medRange:
			d = medRangeDark;
			break;
		case embvm::tof::mode::longRange:
			d = longRangeDark;
			break;
		default:
			break;
	}

	return d;
}

embvm::tof::distance_t vl53l1x::getMaxRangeForModeStrongLight(embvm::tof::mode m) const noexcept
{
	embvm::tof::distance_t d = embvm::tof::INVALID_RANGE;

	switch(m)
	{
		case embvm::tof::mode::defaultRange:
		case embvm::tof::mode::shortRange:
			d = shortRangeStrongLight;
			break;
		case embvm::tof::mode::medRange:
			d = medRangeStrongLight;
			break;
		case embvm::tof::mode::longRange:
			d = longRangeStrongLight;
			break;
		default:
			break;
	}

	return d;
}

embvm::tof::mode vl53l1x::mode(embvm::tof::mode m) noexcept
{
	auto* periodA = create<uint8_t>();
	auto* periodB = create<uint8_t>();
	auto* phaseHigh = create<uint8_t>();
	auto* phaseInit = create<uint8_t>();

	switch(m)
	{
		case embvm::tof::mode::medRange:
			*periodA = periodA_medRange;
			*periodB = periodB_medRange;
			*phaseHigh = phaseHigh_medRange;
			*phaseInit = phaseInit_medRange;
			break;
		case embvm::tof::mode::longRange:
			*periodA = periodA_longRange;
			*periodB = periodB_longRange;
			*phaseHigh = phaseHigh_longRange;
			*phaseInit = phaseInit_longRange;
			break;
		case embvm::tof::mode::defaultRange:
		case embvm::tof::mode::shortRange:
		default:
			*periodA = periodA_shortRange;
			*periodB = periodB_shortRange;
			*phaseHigh = phaseHigh_shortRange;
			*phaseInit = phaseInit_shortRange;
			break;
	}

	// Timing
	writeReg(RANGE_CONFIG_PERIOD_A_REG, periodA, sizeof(uint8_t), nullptr);
	writeReg(RANGE_CONFIG_PERIOD_B_REG, periodB, sizeof(uint8_t), nullptr);
	writeReg(RANGE_CONFIG_PERIOD_PHASE_HIGH_REG, phaseHigh, sizeof(uint8_t),
			 [&](auto op, auto status) {
				 (void)op;
				 (void)status;
				 destroy(op.tx_buffer);
			 });

	// Dynamic
	writeReg(WOI_SD0_REG, periodA, sizeof(uint8_t), [&](auto op, auto status) {
		(void)op;
		(void)status;
		destroy(op.tx_buffer);
	});

	writeReg(WOI_SD1_REG, periodB, sizeof(uint8_t), [&](auto op, auto status) {
		(void)op;
		(void)status;
		destroy(op.tx_buffer);
	});

	writeReg(INITIAL_PHASE_SD0_REG, phaseInit, sizeof(uint8_t), nullptr);
	writeReg(INITIAL_PHASE_SD1_REG, phaseInit, sizeof(uint8_t), [&](auto op, auto status) {
		(void)op;
		(void)status;
		destroy(op.tx_buffer);
	});

	return m;
}
