#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER
#include "Services/ParameterStatisticsService.hpp"

bool supportsStandardDeviation = false;

void ParameterStatisticsService :: reportParameterStatistics(Message& resetFlag) {

	ErrorHandler::assertRequest(resetFlag.packetType == Message::TC, resetFlag,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.messageType == MessageType::ReportParameterStatistics,
	                            resetFlag, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.serviceType == ServiceType, resetFlag,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message statisticsReport(ServiceType,MessageType::ParameterStatisticsReport, Message::TM, 1);

	bool resetFlagValue = resetFlag.readBoolean();
	uint16_t numOfParameters = systemParameters.parametersArray.size();
	uint16_t numOfValidParameters = 0;

	// TODO: Here is the end time
	//       append start time
	//       append end time

	for (uint16_t i = 0; i < numOfParameters; i++) {

	    uint16_t currId = i;
		Statistic currentStatistic = systemStatistics.statisticsMap.at(currId).get();
	    uint16_t numOfSamples = currentStatistic.sampleCounter;

		if (numOfSamples == 0) {
			continue;
		}
		statisticsReport.appendUint16(currId);
		statisticsReport.appendUint16(numOfSamples);

		currentStatistic.appendStatisticsToMessage(statisticsReport); // WORKS! MAGIC!
		numOfValidParameters++;
	}

	statisticsReport.resetRead();
	statisticsReport.appendUint16(numOfValidParameters);
	storeMessage(statisticsReport);

	if (resetFlagValue or ParameterStatisticsService :: hasAutomaticStatisticsReset) {
		resetParameterStatistics();
	}
	// Here add start time

}

void ParameterStatisticsService :: resetParameterStatistics() {

	//TODO: Stop the evaluation of parameter statistics
	for(auto &it: systemStatistics.statisticsMap) {
		it.second.get().resetStatistics();
	}
	//TODO: Restart the evaluation of parameter statistics
}

void ParameterStatisticsService :: enablePeriodicStatisticsReporting(Message& request) {

	// Dummy value
	uint16_t SAMPLING_PARAMETER_INTERVAL = 5; //TODO: The sampling interval of each parameter, "timeInterval" requested
	                                          //      should not exceed it. It has to be defined as a constant.

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::EnablePeriodicParameterReporting,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t timeInterval = request.readUint16();

	ErrorHandler::assertRequest(timeInterval >= SAMPLING_PARAMETER_INTERVAL, request,
	                            ErrorHandler::ExecutionStartErrorType::InvalidReportingRateError);

	periodicStatisticsReportingStatus = true;
	reportingInterval = timeInterval;

	uint16_t numOfParameters = systemParameters.parametersArray.size();

	//Only generate ONE parameter statistics report after every interval passes.
	while (periodicStatisticsReportingStatus) {

		for (int i = 0; i < numOfParameters; i++) {

			Message statisticsReport(ServiceType,MessageType::ParameterStatisticsReport, Message::TM, 1);
			/*
			 * TODO:
			 *      1. append start time to parameterReport
			 *      2. append end time
			 */

			Statistic currentStatistic = systemStatistics.statisticsMap.at(i).get();
			uint16_t numOfSamples = currentStatistic.sampleCounter;

			if(numOfSamples == 0) {
				continue;
			}
			statisticsReport.appendUint16(numOfParameters);
			statisticsReport.appendUint16(i);

			currentStatistic.appendStatisticsToMessage(statisticsReport);

			/*
			 * TODO: put the message into a queue and continue constructing the next report, and when
			 *      it's ready put that in the queue as well, another FreeRTOS task will be accountable of
			 *      keeping track of time, and when the interval passes, it's gonna pop the next
			 *      reportMessage from the queue and report it.
			 */
		}

		//TODO: systematically reset the parameters' statistics.

	}
}

void ParameterStatisticsService :: disablePeriodicStatisticsReporting(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DisablePeriodicParameterReporting,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	ParameterStatisticsService :: periodicStatisticsReportingStatus = false;
	ParameterStatisticsService :: reportingInterval = 0;
}

void ParameterStatisticsService :: addOrUpdateStatisticsDefinitions(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::AddOrUpdateParameterStatisticsDefinitions,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = request.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {

		uint16_t currentId = request.readUint16();
		if (currentId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingParameter);
		}
		bool exists = systemStatistics.statisticsMap.find(currentId) != systemStatistics.statisticsMap.end();
		ErrorHandler::assertRequest(numOfStatisticsDefinitions < MAX_NUM_OF_DEFINITIONS, request,
		                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

		if (request.hasTimeIntervals) {
			uint16_t interval = request.readUint16();
			ErrorHandler::assertRequest(interval <= reportingInterval, request,
			                            ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
			if (not exists) {
				Statistic newStat;
				systemStatistics.statisticsMap.insert({currentId, newStat});
				systemStatistics.statisticsMap.at(currentId).get().setSelfSamplingInterval(interval);
				// TODO: start the evaluation of statistics for this parameter. //add boolean value on statistic
				//       that says if evaluation is enabled
			} else {
				systemStatistics.statisticsMap.at(currentId).get().setSelfSamplingInterval(interval);
				systemStatistics.statisticsMap.at(currentId).get().resetStatistics();
			}
		}
	}
}

void ParameterStatisticsService :: deleteStatisticsDefinitions(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DeleteParameterStatisticsDefinitions,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = request.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {

		uint16_t currentId = request.readUint16();
		if (currentId < systemParameters.parametersArray.size()) {

			systemStatistics.statisticsMap.erase(currentId);

		} else {
			ErrorHandler::reportError(request, ErrorHandler::GetNonExistingParameter);
		}
	}
	// If list of definitions is empty, stop the periodic reporting.
	if (systemStatistics.statisticsMap.empty()) {
		periodicStatisticsReportingStatus = false;
	}
}

void ParameterStatisticsService :: deleteAllStatisticsDefinitions() {

	systemStatistics.statisticsMap.clear();
	// Stop the periodic reporting because there are no defined parameters.
	periodicStatisticsReportingStatus = false;
}

void ParameterStatisticsService :: reportStatisticsDefinitions(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportParameterStatisticsDefinitions,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message definitionsReport(ServiceType,MessageType::ParameterStatisticsDefinitionsReport,Message::TM, 1);

	uint16_t currReportingInterval = 0;
	if (periodicStatisticsReportingStatus) {
		currReportingInterval = reportingInterval;
	}

	uint16_t numOfParameters = systemParameters.parametersArray.size();
	definitionsReport.appendUint16(currReportingInterval); // Append interval

	uint16_t numOfDefinedParameters = 0;
	for (auto &it : systemStatistics.statisticsMap) {
		uint16_t currentSamplingInterval = it.second.get().selfSamplingInterval;
		if (currentSamplingInterval != 0) {
			numOfDefinedParameters++;
		}
	}

	definitionsReport.appendUint16(numOfDefinedParameters);

	for (int i = 0; i < numOfParameters; i++) {
		uint16_t currentId = i;
		uint16_t samplingInterval = systemStatistics.statisticsMap.at(currentId).get().selfSamplingInterval;

		if (samplingInterval != 0 and supportsSamplingInterval) {
			definitionsReport.appendUint16(currentId);
			definitionsReport.appendUint16(samplingInterval);
		}
	}

	storeMessage(definitionsReport);

}

#endif
