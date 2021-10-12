#ifndef ECSS_SERVICES_TIME_HPP
#define ECSS_SERVICES_TIME_HPP

#include <cstdint>
#include <algorithm>
#include <type_traits>
#include <typeinfo>
#include <stdexcept>
#include "macros.hpp"
#include <etl/array.h>
#include "Helpers/TimeFormats.hpp"

// SEE CCSDS 301.0-B-4
// CUSTOM EPOCH FOR ALL ACUBESAT TIMESTAMPS IS 01 JAN 2020, EXCEPT UTC (UNIX)

///////////// CLASS DECLARATION ////////////////
/**
 * A class that represents an instant in time, with convenient conversion
 * to and from usual time and date representations
 *
 * @note
 * This class uses internally TAI time, and handles UTC leap seconds at conversion to and
 * from UTC time system.
 */
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
class Instant {

private:
	typedef typename std::conditional<seconds_counter_bytes < 4 && fractional_counter_bytes < 3, uint8_t, uint16_t>::type CUC_header_t;
	typedef typename std::conditional<(seconds_counter_bytes + fractional_counter_bytes) < 4, uint32_t, uint64_t>::type tai_counter_t;
	tai_counter_t tai_counter;
	CUC_header_t CUC_header = build_CUC_header<CUC_header_t, seconds_counter_bytes,fractional_counter_bytes>();

public:

	/**
	 * Initialize the instant at epoch
	 *
	 */
	Instant();

	/**
	 * Initialize the instant from a duration from epoch in TAI
	 *
	 * @param seconds an integer number of seconds from Acubesat custom epoch
	 */
	Instant(int seconds);

	/**
	 * Initialize the instant from the bytes of a CUC time stamp
	 *
	 * @param timestamp a complete CUC time stamp including header, of the maximum possible size, zero padded to the right
	 */
	Instant(etl::array<uint8_t, 9> timestamp);

	/**
	 * Initialize the instant from a UTC timestamp struct
	 *
	 * @param timestamp a UTC timestamp, from Unix Epoch
	 */
	Instant(UTC_Timestamp timestamp);

	/**
	 * Initialize the instant from an Acubesat CDS timestamp
	 *
	 * @param timestamp a CDS timestamp, in the format from DDJF_TTC, from epoch 2020.01.01
	 */
	Instant(Acubesat_CDS_timestamp timestamp);

	/**
	 * Get the representation as seconds from epoch in TAI
	 *
	 * @return the seconds elapsed in TAI since 1 Jan 1958, cut to the integer part
	 */
	const int as_TAI_seconds();

	/**
	 * Get the representation as CUC formatted bytes
	 *
	 * @return the instant, represented in the CCSDS CUC format
	 */
	const etl::array<uint8_t, 9> as_CUC_timestamp();

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is smaller than \p Instant
	 */
	bool operator<(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const;

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is greater than \p Instant
	 */
	bool operator>(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const;

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is equal to \p Instant
	 */
	bool operator==(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const;

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is different from \p Instant
	 */
	bool operator!=(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const;

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is smaller than or equal to \p Instant
	 */
	bool operator<=(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const;

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is greater than or equal to \p Instant
	 */
	bool operator>=(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const;

	/**
	 * Check internal variables have right sizes from template, use for debug
	 *
	 *
	 * @return [size_of CUC_header, size_of tai_counter]
	 */
	const std::type_info& check_header_type();
};
////////////////////////////////////////////////

////// TEMPLATED METHODS INSTANCIATION /////////
#include "Instant.tpp"
////////////////////////////////////////////////

#endif