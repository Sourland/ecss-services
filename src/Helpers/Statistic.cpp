#include "Helpers/Statistic.hpp"

void Statistic::updateStatistics(double value) {
	if (value > max) {
		max = value;
		timeOfMaxValue = TimeGetter::currentTimeCustomCUC();
	}
	if (value < min) {
		min = value;
		timeOfMinValue = TimeGetter::currentTimeCustomCUC();
	}
	if (sampleCounter + 1 > 0) {
		mean = (mean * sampleCounter + value) / (sampleCounter + 1);
	}
	sumOfSquares += pow(value, 2);
	sampleCounter++;
}

void Statistic::appendStatisticsToMessage(Message& report) {
	report.appendFloat(static_cast<float>(max));
	report.appendUint64(timeOfMaxValue);
	report.appendFloat(static_cast<float>(min));
	report.appendUint64(timeOfMinValue);
	report.appendFloat(static_cast<float>(mean));

	if (SupportsStandardDeviation) {
		double standardDeviation = 0;
		if (sampleCounter == 0) {
			standardDeviation = 0;
		} else {
			double meanOfSquares = sumOfSquares / sampleCounter;
			standardDeviation = sqrt(abs(meanOfSquares - pow(mean, 2)));
		}
		report.appendFloat(static_cast<float>(standardDeviation));
	}
}

void Statistic::setSelfSamplingInterval(uint16_t samplingInterval) {
	this->selfSamplingInterval = samplingInterval;
}

void Statistic::resetStatistics() {
	max = -std::numeric_limits<double>::infinity();
	min = std::numeric_limits<double>::infinity();
	timeOfMaxValue = 0;
	timeOfMinValue = 0;
	mean = 0;
	sumOfSquares = 0;
	sampleCounter = 0;
}

bool Statistic::statisticsAreInitialized() {
	return (sampleCounter == 0 and mean == 0 and sumOfSquares == 0 and timeOfMaxValue == 0 and timeOfMinValue == 0 and
	        max == -std::numeric_limits<double>::infinity() and min == std::numeric_limits<double>::infinity());
}
