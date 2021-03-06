template <uint8_t secondsBytes, uint8_t fractionalBytes>
constexpr bool TimeStamp<secondsBytes, fractionalBytes>::areSecondsValid(TimeStamp::TAICounter_t seconds) {
	return seconds < maxSecondCounterValue;
}

template <uint8_t secondsBytes, uint8_t fractionalBytes>
TimeStamp<secondsBytes, fractionalBytes>::TimeStamp(uint64_t taiSecondsFromEpoch) {
	ASSERT_INTERNAL(areSecondsValid((taiSecondsFromEpoch)), ErrorHandler::InternalErrorType::InvalidTimeStampInput);

	taiCounter = static_cast<TAICounter_t>(taiSecondsFromEpoch) << 8 * fractionalBytes;
}

template <uint8_t secondsBytes, uint8_t fractionalBytes>
TimeStamp<secondsBytes, fractionalBytes>::TimeStamp(Time::CustomCUC_t customCUCTimestamp) {
	ASSERT_INTERNAL(areSecondsValid((customCUCTimestamp.elapsed100msTicks / 10)),
	                ErrorHandler::InternalErrorType::InvalidTimeStampInput);
	taiCounter = static_cast<TAICounter_t>(customCUCTimestamp.elapsed100msTicks / 10);
	if (fractionalBytes > 0) {
		TAICounter_t fractionalPart = static_cast<TAICounter_t>(customCUCTimestamp.elapsed100msTicks) - 10 * taiCounter;
		taiCounter = taiCounter << 8;
		taiCounter += fractionalPart * 256 / 10;
		taiCounter = taiCounter << 8 * (fractionalBytes - 1);
	}
}

template <uint8_t secondsCounter, uint8_t fractionalBytes>
TimeStamp<secondsCounter, fractionalBytes>::TimeStamp(etl::array<uint8_t, Time::CUCTimestampMaximumSize> timestamp) {
	// process header
	uint8_t headerSize = 1;
	if ((timestamp[0] & 0b10000000U) != 0) {
		headerSize = 2;
	}

	uint8_t inputSecondsBytes = ((timestamp[0] & 0b00001100U) >> 2U) + 1U;
	uint8_t inputFractionalBytes = (timestamp[0] & 0b00000011U) >> 0U;

	if (headerSize == 2) {
		inputSecondsBytes += (timestamp[1] & 0b01100000U) >> 5U;
		inputFractionalBytes += (timestamp[1] & 0b00011100U) >> 2U;
	}

	// check input validity (useless bytes set to 0)
	for (int i = headerSize + inputSecondsBytes + inputFractionalBytes; i < Time::CUCTimestampMaximumSize; i++) {
		if (timestamp[i] != 0) {
			ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::InvalidTimeStampInput);
			break;
		}
	}

	// do checks wrt template precision parameters
	ASSERT_INTERNAL(inputSecondsBytes <= secondsCounter, ErrorHandler::InternalErrorType::InvalidTimeStampInput);
	ASSERT_INTERNAL(inputFractionalBytes <= fractionalBytes, ErrorHandler::InternalErrorType::InvalidTimeStampInput);

	// put timestamp into internal counter
	taiCounter = 0;
	// add seconds until run out of bytes on input array
	for (auto i = 0; i < inputSecondsBytes + inputFractionalBytes; i++) {
		taiCounter = taiCounter << 8;
		taiCounter += timestamp[headerSize + i];
	}
	// pad rightmost bytes to full length
	taiCounter = taiCounter << 8 * (fractionalBytes - inputFractionalBytes);
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::TimeStamp(const UTCTimestamp& timestamp) {
	TAICounter_t seconds = 0;
	for (int year = Time::Epoch.year; year < timestamp.year; ++year) {
		seconds += (Time::isLeapYear(year) ? 366 : 365) * Time::SecondsPerDay;
	}
	for (int month = Time::Epoch.month; month < timestamp.month; ++month) {
		seconds += Time::DaysOfMonth[month - 1] * Time::SecondsPerDay;
		if ((month == 2U) && Time::isLeapYear(timestamp.year)) {
			seconds += Time::SecondsPerDay;
		}
	}
	seconds += (timestamp.day - Time::Epoch.day) * Time::SecondsPerDay;
	seconds += timestamp.hour * Time::SecondsPerHour;
	seconds += timestamp.minute * Time::SecondsPerMinute;
	seconds += timestamp.second;
	// TODO: Add check that `seconds` is within bounds (?)
	taiCounter = static_cast<TAICounter_t>(seconds) << 8 * fractional_counter_bytes;
}

template <uint8_t secondsBytes, uint8_t fractionalBytes>
typename TimeStamp<secondsBytes, fractionalBytes>::TAICounter_t
TimeStamp<secondsBytes, fractionalBytes>::asTAIseconds() {
	return taiCounter >> (8 * fractionalBytes);
}

template <uint8_t secondsBytes, uint8_t fractionalBytes>
Time::CustomCUC_t TimeStamp<secondsBytes, fractionalBytes>::asCustomCUCTimestamp() {
	TAICounter_t temp = taiCounter;
	Time::CustomCUC_t return_s = {0};
	if (fractionalBytes > 0) {
		temp = temp >> 8 * (fractionalBytes - 1);
		return_s.elapsed100msTicks += temp * 10 / 256;
	} else {
		return_s.elapsed100msTicks += temp * 10;
	}
	return return_s;
}

template <uint8_t secondsBytes, uint8_t fractionalBytes>
template <typename T>
T TimeStamp<secondsBytes, fractionalBytes>::asTAIseconds() {
	static_assert(std::is_floating_point_v<T>, "TimeStamp::asTAIseconds() only accepts numeric types.");
	static_assert(std::numeric_limits<T>::max() >= maxSecondCounterValue);

	TAICounter_t decimalPart = taiCounter >> (8 * fractionalBytes);

	T fractionalPart = taiCounter - (decimalPart << (8 * fractionalBytes));
	T fractionalPartMax = (1U << (8U * fractionalBytes)) - 1U;

	return decimalPart + fractionalPart / fractionalPartMax;
}

template <uint8_t secondsBytes, uint8_t fractionalBytes>
etl::array<uint8_t, Time::CUCTimestampMaximumSize> TimeStamp<secondsBytes, fractionalBytes>::toCUCtimestamp() {
	etl::array<uint8_t, Time::CUCTimestampMaximumSize> returnArray = {0};

	static constexpr uint8_t headerBytes = (secondsBytes < 4 && fractionalBytes < 3) ? 1 : 2;

	if (headerBytes == 1) {
		returnArray[0] = static_cast<uint8_t>(CUCHeader);
	} else {
		returnArray[1] = static_cast<uint8_t>(CUCHeader);
		returnArray[0] = static_cast<uint8_t>(CUCHeader >> 8);
	}

	for (auto byte = 0; byte < secondsBytes + fractionalBytes; byte++) {
		uint8_t taiCounterIndex = 8 * (secondsBytes + fractionalBytes - byte - 1);
		returnArray[headerBytes + byte] = taiCounter >> taiCounterIndex;
	}

	return returnArray;
}

template <uint8_t secondsBytes, uint8_t fractionalBytes>
UTCTimestamp TimeStamp<secondsBytes, fractionalBytes>::toUTCtimestamp() {
	using namespace Time;

	uint32_t totalSeconds = asTAIseconds();

	uint16_t yearUTC = Epoch.year;
	uint8_t monthUTC = Epoch.month;
	uint8_t dayUTC = Epoch.day;
	uint8_t hour = 0;
	uint8_t minute = 0;
	uint8_t second = 0;

	// calculate years
	while (totalSeconds >= (isLeapYear(yearUTC) ? 366 : 365) * SecondsPerDay) {
		totalSeconds -= (isLeapYear(yearUTC) ? 366 : 365) * SecondsPerDay;
		yearUTC++;
	}

	// calculate months
	int currentMonth = 0;
	while (totalSeconds >= (DaysOfMonth[currentMonth] * SecondsPerDay)) {
		monthUTC++;
		totalSeconds -= (DaysOfMonth[currentMonth] * SecondsPerDay);
		currentMonth++;
		if ((currentMonth == 1U) && isLeapYear(yearUTC)) {
			if (totalSeconds <= (28 * SecondsPerDay)) {
				break;
			}
			monthUTC++;
			totalSeconds -= 29 * SecondsPerDay;
			currentMonth++;
		}
	}

	dayUTC = totalSeconds / SecondsPerDay;
	totalSeconds -= dayUTC * SecondsPerDay;
	dayUTC++; // add 1 day because we start count from 1 January (and not 0 January!)

	hour = totalSeconds / SecondsPerHour;
	totalSeconds -= hour * SecondsPerHour;

	minute = totalSeconds / SecondsPerMinute;
	totalSeconds -= minute * SecondsPerMinute;

	second = totalSeconds;

	return {yearUTC, monthUTC, dayUTC, hour, minute, second};
}
