#ifndef ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP

#include "etl/map.h"
#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Parameters/SystemParameters.hpp"
#include "Helpers/HousekeepingStructure.hpp"

/**
 * Implementation of the ST[03] Housekeeping Reporting Service. The job of the Housekeeping Service is to store
 * parameters in the housekeeping structures so that it can generate housekeeping reports periodically.
 *
 * @author Petridis Konstantinos <petridkon@gmail.com>
 */
class HousekeepingService : Service {
public:
	inline static const uint8_t ServiceType = 3;

	/**
	 * Map containing the housekeeping structures. Map[i] contains the housekeeping structure with ID = i.
	 */
	etl::map<uint16_t, HousekeepingStructure, ECSS_MAX_HOUSEKEEPING_STRUCTS> housekeepingStructures;

	enum MessageType : uint8_t {
		ReportHousekeepingParameters = 0,
		CreateHousekeepingReportStructure = 1,
		DeleteHousekeepingReportStructure = 3,
		EnablePeriodicHousekeepingParametersReport = 5,
		DisablePeriodicHousekeepingParametersReport = 6,
		ReportHousekeepingStructures = 9,
		HousekeepingStructuresReport = 10,
		HousekeepingParametersReport = 25,
		GenerateOneShotHousekeepingReport = 27,
		AppendParametersToHousekeepingStructure = 29,
		ModifyCollectionIntervalOfStructures = 31,
		ReportHousekeepingPeriodicProperties = 33,
		HousekeepingPeriodicPropertiesReport = 35,
	};

	HousekeepingService() = default;

	/**
	 * This function gets a request to report the housekeeping parameters and checks whether its valid or not
	 */
	void reportHousekeepingParameters(Message& request);

	/**
	 * This function creates and stores the housekeeping parameter value report. The purpose for this
	 * functionality, not existing in the previous function is to create new request messages when generating
	 * one-shot housekeeping parameter reports, but rather just call a function using the structure id.
	 */
	void housekeepingParametersReport(uint16_t structureId);

	/**
	 * Implementation of TC[3,5]. Request to enable the periodic housekeeping parameters reporting for a specific
	 * housekeeping structure.
	 */
	void enablePeriodicHousekeepingParametersReport(Message& request);

	/**
	 * Implementation of TC[3,6]. Request to disable the periodic housekeeping parameters reporting for a specific
	 * housekeeping structure.
	 */
	void disablePeriodicHousekeepingParametersReport(Message& request);

	/**
	 * Implementation of TC[3,1]. Request to create a housekeeping parameters report structure.
	 */
	void createHousekeepingReportStructure(Message& request);

	/**
	 * Implementation of TC[3,3]. Request to delete a housekeeping parameters report structure.
	 */
	void deleteHousekeepingReportStructure(Message& request);

	/**
	 * This function takes a structure ID as argument and constructs/stores a TM[3,10] housekeeping structure report.
	 */
	void housekeepingStructureReport(uint16_t structIdToReport);

	/**
	 * This function gets a message type TC[3,9] request.
	 */
	void reportHousekeepingStructures(Message& request);

	/**
	 * This function takes as argument a message type TC[3,27].
	 */
	void generateOneShotHousekeepingReport(Message& request);

	/**
	 * This function creates and stores a TM[3,25] one shot housekeeping report message.
	 */
	void oneShotHousekeepingReport(Message& request);

	/**
	 * This function appends new parameters to an already existing housekeeping structure (TC[3,29]).
	 */
	void appendParametersToHousekeepingStructure(Message& request);

	/**
	 * This function modifies the collection interval of specified structures (TC[3,31]).
	 */
	void modifyCollectionIntervalOfStructures(Message& request);

	/**
	 * This function takes as argument, a message type TC[3,33].
	 */
	void reportHousekeepingPeriodicProperties(Message& request);

	/**
	 * Creates and stores a TM[3,35] housekeeping periodic properties report, for each requested housekeeping structure.
	 */
	void housekeepingPeriodicPropertiesReport(Message& request);

	/**
	 * It is responsible to call the suitable function that executes a TC packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);
};

#endif