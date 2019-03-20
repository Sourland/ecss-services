#ifndef ECSS_SERVICES_TIMEBASEDSCHEDULINGSERVICE_HPP
#define ECSS_SERVICES_TIMEBASEDSCHEDULINGSERVICE_HPP

#include "etl/vector.h"
#include "etl/iterator.h"
#include "etl/String.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "MessageParser.hpp"
#include "Helpers/CRCHelper.hpp"
#include "Helpers/TimeHelper.hpp"

// Include platform specific files
#include "Platform/x86/TimeGetter.hpp"

/**
 * @def SUB_SCHEDULES_ENABLED
 * @brief Indicates whether sub-schedules are supported
 *
 * @details Sub-schedules are currently not implemented so this has no effect
 */
/**
 * @def GROUPS_ENABLED
 * @brief Indicates whether scheduling groups are enabled
 */
#define GROUPS_ENABLED          0
#define SUB_SCHEDULES_ENABLED   0


/**
 * @brief Namespace to access private members during test
 *
 * @details Define a namespace for the access of the private members to avoid conflicts
 */
namespace unit_test {
	struct Tester;
} // namespace unit_test

/**
 * @class TimeBasedSchedulingService
 * @brief An implementation of the ECSS standard ST[11] service
 *
 * @details This service is taking care of the timed release of a received TC packet from the
 * ground.
 * @todo Define whether the parsed absolute release time is saved in the scheduled activity as an
 * uint32_t or in the time format specified by the time management service.
 */
class TimeBasedSchedulingService : public Service {
private:
	/**
	 * @brief Indicator of the schedule execution
	 *
	 * @details The schedule execution indicator will be updated by the process that is running
	 * the time scheduling service.
	 */
	bool executionFunctionStatus = false; // True indicates "enabled" and False "disabled" state

	/**
	 * @brief Number of activities currently in the schedule
	 * @todo Define the maximum allowed number of activities in the schedule
	 */
	uint8_t currentNumberOfActivities = 0; // Keep track of the number of activities
	MessageParser msgParser; // Parse TC packets

	/**
	 * @brief Request identifier of the received packet
	 *
	 * @details The request identifier consists of the application process ID, the packet
	 * sequence count and the source ID, all defined in the ECSS standard.
	 * @var applicationID Application process ID
	 * @var sequenceCount Packet sequence count
	 * @var sourceID Packet source ID
	 */
	struct RequestID {
		uint16_t applicationID = 0;
		uint16_t sequenceCount = 0;
		uint8_t sourceID = 0;

		bool operator!=(const RequestID &rightSide) const {
			return (sequenceCount != rightSide.sequenceCount) or
			       (applicationID != rightSide.applicationID) or (sourceID != rightSide.sourceID);
		}
	};

	/**
	 * @brief Instances of activities to run in the schedule
	 *
	 * @details All scheduled activities must contain the request they exist for, their release
	 * time and the corresponding request identifier.
	 * @var request Contains the received TC request
	 * @var requestID Contains the unique request identifier for that activity
	 * @var requestReleaseTime The absolute time is seconds of the request release
	 */
	struct ScheduledActivity {
		Message request; // Hold the received command request
		RequestID requestID; // Request ID, characteristic of the definition
		uint32_t requestReleaseTime = 0; // Keep the command release time
		// todo: If we decide to use sub-schedules, the ID of that has to be defined
		// todo: If groups are used, then the group ID has to be defined here
	};

	/**
	 * @brief Hold the scheduled activities
	 *
	 * @details The scheduled activities in this vector are ordered by their release time, as the
	 * standard requests.
	 */
	etl::vector<ScheduledActivity, ECSS_MAX_NUMBER_OF_TIME_SCHED_ACTIVITIES> scheduledActivities;

	/**
	 * @brief Define a friend in order to be able to access private members during testing
	 *
	 * @details The private members defined in this class, must not in any way be public to avoid
	 * misuse. During testing, access to private members for verification is required, so an
	 * access friend structure is defined here.
	 */
	friend struct ::unit_test::Tester;


public:
	TimeBasedSchedulingService();

	/**
	 * @brief TC[11,1] enable the time-based schedule execution function
	 *
	 * @details Enables the time-based command execution scheduling
	 * @param request Provide the received message as a parameter
	 */
	void enableScheduleExecution(Message &request);

	/**
	 * @breif TC[11,2] disable the time-based schedule execution function
	 *
	 * @details Disables the time-based command execution scheduling
	 * @param request Provide the received message as a parameter
	 */
	void disableScheduleExecution(Message &request);

	/**
	 * @brief TC[11,3] reset the time-based schedule
	 *
	 * @details Resets the time-based command execution schedule, by clearing all scheduled
	 * activities.
	 * @param request Provide the received message as a parameter
	 */
	void resetSchedule(Message &request);

	/**
	 * @brief TC[11,4] insert activities into the time based schedule
	 *
	 * @details Add activities into the schedule for future execution. The activities are inserted
	 * by ascending order of their release time. This done to avoid confusion during the
	 * execution of the schedule and also to make things easier whenever a release time sorted
	 * report is requested by he corresponding service.
	 * @param request Provide the received message as a parameter
	 * @throws failed_start_of_execution If there is request to be inserted and the maximum
	 * number of activities in the current schedule has been reached, then a failed start of
	 * execution report is being issued. Also if the release time of the request is less than a
	 * set time margin, defined in ECSS_TIME_MARGIN_FOR_ACTIVATION, from the current time a
	 * failed start of execution report is issued.
	 */
	void insertActivities(Message &request);

	/**
	 * @brief TC[11,15] time-shift all scheduled activities
	 *
	 * @details All scheduled activities are shifted per user request. The relative time offset
	 * received and tested against the current time.
	 * @param request Provide the received message as a parameter
	 * @throws failed_start_of_execution If the release time of the request is less than a
	 * set time margin, defined in ECSS_TIME_MARGIN_FOR_ACTIVATION, from the current time a
	 * failed start of execution report is issued for that instruction.
	 */
	void timeShiftAllActivities(Message &request);

	/**
	 * @brief TC[11,16] detail-report all activities
	 *
	 * @details Send a detailed report about the status of all the activities
	 * on the current schedule. Generates a TM[11,10] response.
	 * @param request Provide the received message as a parameter
	 * @todo Replace the time parsing with the time parser
	 * @todo Define whether the request.resetRead() will be handled by the storeMessage
	 */
	void detailReportAllActivities(Message &request);

	/**
	 * @brief TC[11,9] detail-report activities identified by request identifier
	 *
	 * @details Send a detailed report about the status of the requested activities, based on the
	 * provided request identifier. Generates a TM[11,10] response. The matched activities are
	 * contained in the report, in an ascending order based on their release time.
	 * @param request Provide the received message as a parameter
	 * @todo Define whether the request.resetRead() will be handled by the storeMessage
	 * @throws failed_start_of_execution If a requested activity, identified by the provided
	 * request identifier is not found in the schedule issue a failed start of execution report
	 * for that instruction.
	 */
	void detailReportActivitiesByID(Message &request);

	/**
	 * @brief TC[11,12] summary-report activities identified by request identifier
	 *
	 * @details Send a summary report about the status of the requested activities. Generates a
	 * TM[11,13] response, with activities ordered in an ascending order, based on their release
	 * time.
	 * @param request Provide the received message as a parameter
	 * @todo Define whether the request.resetRead() will be handled by the storeMessage
	 * @throws failed_start_of_execution If a requested activity, identified by the provided
	 * request identifier is not found in the schedule issue a failed start of execution report
	 * for that instruction.
	 */
	void summaryReportActivitiesByID(Message &request);

	/**
	 * @brief TC[11,5] delete time-based scheduled activities identified by a request identifier
	 *
	 * @details Delete certain activities by using the unique request identifier.
	 * @param request Provide the received message as a parameter
	 * @throws failed_start_of_execution If a requested activity, identified by the provided
	 * request identifier is not found in the schedule issue a failed start of execution report
	 * for that instruction.
	 */
	void deleteActivitiesByID(Message &request);

	/**
	 * @brief TC[11,7] time-shift scheduled activities identified by a request identifier
	 *
	 * @details Time-shift certain activities by using the unique request identifier
	 * @param request Provide the received message as a parameter
	 * @throws failed_start_of_execution If the requested time offset is less than the earliest
	 * time from the currently scheduled activities plus the ECSS_TIME_MARGIN_FOR_ACTIVATION,
	 * then the request is rejected and a failed start of execution report is issued. Also if an
	 * activity with a specified request identifier is not found, generate a failed start of
	 * execution for that specific instruction.
	 */
	void timeShiftActivitiesByID(Message &request);
};

#endif //ECSS_SERVICES_TIMEBASEDSCHEDULINGSERVICE_HPP
