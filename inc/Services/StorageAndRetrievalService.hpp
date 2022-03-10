#ifndef ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP
#define ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/PacketStore.hpp"
#include "etl/map.h"

/**
 * Implementation of ST[15] Storage and Retrieval Service, as defined in ECSS-E-ST-70-41C.
 *
 * This Service:
 * - provides the capability to select reports generated by other services and store them into packet stores.
 * - allows the ground system to manage the reports in the packet stores and request their downlink.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class StorageAndRetrievalService : public Service {
public:
	/**
	 * The type of timestamps that the Storage and Retrieval Subservice assigns to each incoming packet.
	 */
	enum TimeStampType : uint8_t { StorageBased = 0, PacketBased = 1 };

	/**
	 * Different types of packet retrieval from a packet store, relative to a specified time-tag.
	 */
	enum TimeWindowType : uint8_t { FromTagToTag = 0, AfterTimeTag = 1, BeforeTimeTag = 2 };

	/**
	 * The type of timestamps that the subservice sets to each incoming telemetry packet.
	 */
	const TimeStampType timeStamping = PacketBased;

private:
	typedef String<ECSSMaxPacketStoreKeySize> packetStoreKey;

	/**
	 * All packet stores, held by the Storage and Retrieval Service. Each packet store has its ID as key.
	 */
	etl::map<packetStoreKey, PacketStore, ECSSMaxPacketStores> packetStores;

	/**
	 * Helper function that reads the packet store ID string from a TM[15] message
	 */
	static String<ECSSMaxPacketStoreIdSize> readPacketStoreId(Message& message);

	/**
	 * Helper function that, given a time-limit, deletes every packet stored in the specified packet-store, up to the
	 * requested time.
	 *
	 * @param packetStoreId required to access the correct packet store.
	 * @param timeLimit the limit until which, packets are deleted.
	 */
	void deleteContentUntil(const String<ECSSMaxPacketStoreIdSize>& packetStoreId, uint32_t timeLimit);

	/**
	 * Copies all TM packets from source packet store to the target packet-store, that fall between the two specified
	 * time-tags as per 6.15.3.8.4.d(1) of the standard.
	 *
	 * @param request used to read the time-tags, the packet store IDs and to raise errors.
	 */
	void copyFromTagToTag(Message& request);

	/**
	 * Copies all TM packets from source packet store to the target packet-store, whose time-stamp is after the
	 * specified time-tag as per 6.15.3.8.4.d(2) of the standard.
	 *
	 * @param request used to read the time-tag, the packet store IDs and to raise errors.
	 */
	void copyAfterTimeTag(Message& request);

	/**
	 * Copies all TM packets from source packet store to the target packet-store, whose time-stamp is before the
	 * specified time-tag as per 6.15.3.8.4.d(3) of the standard.
	 *
	 * @param request used to raise errors.
	 */
	void copyBeforeTimeTag(Message& request);

	/**
	 * Checks if the two requested packet stores exist.
	 *
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content.
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool checkPacketStores(const String<ECSSMaxPacketStoreIdSize>& fromPacketStoreId,
	                       const String<ECSSMaxPacketStoreIdSize>& toPacketStoreId, Message& request);

	/**
	 * Checks whether the time window makes logical sense (end time should be after the start time)
	 *
	 * @param request used to raise errors.
	 */
	static bool checkTimeWindow(uint32_t startTime, uint32_t endTime, Message& request);

	/**
	 * Checks if the destination packet store is empty, in order to proceed with the copying of packets.
	 *
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content. Needed for error
	 * checking.
	 * @param request used to raise errors.
	 */
	bool checkDestinationPacketStore(const String<ECSSMaxPacketStoreIdSize>& toPacketStoreId, Message& request);

	/**
	 * Checks if there are no stored timestamps that fall between the two specified time-tags.
	 *
	 * @param fromPacketStoreId  the source packet store, whose content is to be copied. Needed for error checking.
	 * @param request used to raise errors.
	 *
	 * @note
	 * This function assumes that `startTime` and `endTime` are valid at this point, so any necessary error checking
	 * regarding these variables, should have already occurred.
	 */
	bool noTimestampInTimeWindow(const String<ECSSMaxPacketStoreIdSize>& fromPacketStoreId, uint32_t startTime,
	                             uint32_t endTime, Message& request);

	/**
	 * Checks if there are no stored timestamps that fall between the two specified time-tags.
	 *
	 * @param isAfterTimeTag true indicates that we are examining the case of AfterTimeTag. Otherwise, we are referring
	 * to the case of BeforeTimeTag.
	 * @param request used to raise errors.
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 */
	bool noTimestampInTimeWindow(const String<ECSSMaxPacketStoreIdSize>& fromPacketStoreId, uint32_t timeTag,
	                             Message& request, bool isAfterTimeTag);

	/**
	 * Performs all the necessary error checking for the case of FromTagToTag copying of packets.
	 *
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content.
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool failedFromTagToTag(const String<ECSSMaxPacketStoreIdSize>& fromPacketStoreId,
	                        const String<ECSSMaxPacketStoreIdSize>& toPacketStoreId, uint32_t startTime,
	                        uint32_t endTime, Message& request);

	/**
	 * Performs all the necessary error checking for the case of AfterTimeTag copying of packets.
	 *
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content.
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool failedAfterTimeTag(const String<ECSSMaxPacketStoreIdSize>& fromPacketStoreId,
	                        const String<ECSSMaxPacketStoreIdSize>& toPacketStoreId, uint32_t startTime,
	                        Message& request);



	/**
	 * Performs all the necessary error checking for the case of BeforeTimeTag copying of packets.
	 *
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content.
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool failedBeforeTimeTag(const String<ECSSMaxPacketStoreIdSize>& fromPacketStoreId,
	                         const String<ECSSMaxPacketStoreIdSize>& toPacketStoreId, uint32_t endTime,
	                         Message& request);

	/**
	 * Performs the necessary error checking for a request to start the by-time-range retrieval process.
	 *
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool failedStartOfByTimeRangeRetrieval(const String<ECSSMaxPacketStoreIdSize>& packetStoreId, Message& request);

	/**
	 * Forms the content summary of the specified packet-store and appends it to a report message.
	 */
	void createContentSummary(Message& report, const String<ECSSMaxPacketStoreIdSize>& packetStoreId);

public:
	inline static const uint8_t ServiceType = 15;

	enum MessageType : uint8_t {
		EnableStorageInPacketStores = 1,
		DisableStorageInPacketStores = 2,
		StartByTimeRangeRetrieval = 9,
		DeletePacketStoreContent = 11,
		ReportContentSummaryOfPacketStores = 12,
		PacketStoreContentSummaryReport = 13,
		ChangeOpenRetrievalStartingTime = 14,
		ResumeOpenRetrievalOfPacketStores = 15,
		SuspendOpenRetrievalOfPacketStores = 16,
		AbortByTimeRangeRetrieval = 17,
		ReportStatusOfPacketStores = 18,
		PacketStoresStatusReport = 19,
		CreatePacketStores = 20,
		DeletePacketStores = 21,
		ReportConfigurationOfPacketStores = 22,
		PacketStoreConfigurationReport = 23,
		CopyPacketsInTimeWindow = 24,
		ResizePacketStores = 25,
		ChangeTypeToCircular = 26,
		ChangeTypeToBounded = 27,
		ChangeVirtualChannel = 28
	};

	StorageAndRetrievalService() = default;

	/**
	 * Adds new packet store into packet stores.
	 */
	void addPacketStore(const String<ECSSMaxPacketStoreIdSize>& packetStoreId, const PacketStore& packetStore);

	/**
	 * Adds telemetry to the specified packet store and timestamps it.
	 */
	void addTelemetryToPacketStore(const String<ECSSMaxPacketStoreIdSize>& packetStoreId, uint32_t timestamp);

	/**
	 * Deletes the content from all the packet stores.
	 */
	void resetPacketStores();

	/**
	 * Returns the current number of existing packet stores.
	 */
	uint16_t currentNumberOfPacketStores();

	/**
	 * Returns the packet store with the specified packet store ID.
	 */
	PacketStore& getPacketStore(const String<ECSSMaxPacketStoreIdSize>& packetStoreId);

	/**
	 * Returns true if the specified packet store is present in packet stores.
	 */
	bool packetStoreExists(const String<ECSSMaxPacketStoreIdSize>& packetStoreId);

	/**
	 * The purpose of this function is to avoid duplicating the same checks in different TC implementations. It
	 * performs the error checking, which is common across the functions and then, it performs the specified job.
	 *
	 * @param function the job to be done after the error checking.
	 */
	void executeOnPacketStores(Message& request, const std::function<void(PacketStore&)>& function);

	/**
	 * TC[15,1] request to enable the packet stores' storage function
	 */
	void enableStorageFunction(Message& request);

	/**
	 * TC[15,2] request to disable the packet stores' storage function
	 */
	void disableStorageFunction(Message& request);

	/**
	 * TC[15,9] start the by-time-range retrieval of packet stores
	 */
	void startByTimeRangeRetrieval(Message& request);

	/**
	 * TC[15,11] delete the packet store content up to the specified time
	 */
	void deletePacketStoreContent(Message& request);

	/**
	 * This function takes a TC[15,12] 'report the packet store content summary' as argument and responds with a TM[15,
	 * 13] 'packet store content summary report' report message.
	 */
	void packetStoreContentSummaryReport(Message& request);

	/**
	 * TC[15,14] change the open retrieval start time tag
	 */
	void changeOpenRetrievalStartTimeTag(Message& request);

	/**
	 * TC[15,15] resume the open retrieval of packet stores
	 */
	void resumeOpenRetrievalOfPacketStores(Message& request);

	/**
	 * TC[15,16] suspend the open retrieval of packet stores
	 */
	void suspendOpenRetrievalOfPacketStores(Message& request);

	/**
	 * TC[15,17] abort the by-time-range retrieval of packet stores
	 */
	void abortByTimeRangeRetrieval(Message& request);

	/**
	 * This function takes a TC[15,18] 'report the status of packet stores' request as argument and responds with a
	 * TM[15,19] 'packet stores status' report message.
	 */
	void packetStoresStatusReport(Message& request);

	/**
	 * TC[15,20] create packet stores
	 */
	void createPacketStores(Message& request);

	/**
	 * TC[15,21] delete packet stores
	 */
	void deletePacketStores(Message& request);

	/**
	 * This function takes a TC[15,22] 'report the packet store configuration' as argument and responds with a TM[15,
	 * 23] 'packet store configuration report' report message.
	 */
	void packetStoreConfigurationReport(Message& request);

	/**
	 * TC[15,24] copy the packets contained into a packet store, selected by the time window
	 */
	void copyPacketsInTimeWindow(Message& request);

	/**
	 * TC[15,25] resize packet stores
	 */
	void resizePacketStores(Message& request);

	/**
	 * TC[15,26] change the packet store type to circular
	 */
	void changeTypeToCircular(Message& request);

	/**
	 * TC[15,27] change the packet store type to bounded
	 */
	void changeTypeToBounded(Message& request);

	/**
	 * TC[15,28] change the virtual channel used by a packet store
	 */
	void changeVirtualChannel(Message& request);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param request Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& request);
};

#endif
