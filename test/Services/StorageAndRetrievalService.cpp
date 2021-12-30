#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/StorageAndRetrievalService.hpp"

void initializePacketStores() {
	uint16_t numOfPacketStores = 4;
	uint8_t concatenatedPacketStoreNames[] = "ps2ps25ps799ps5555";
	uint16_t offsets[5] = {0, 3, 7, 12, 18};
	uint16_t sizes[4] = {100, 200, 550, 340};
	uint8_t virtualChannels[4] = {4, 6, 1, 2};

	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		std::fill(std::begin(packetStoreData), std::end(packetStoreData), 0);
		std::copy(concatenatedPacketStoreNames + offsets[i], concatenatedPacketStoreNames + offsets[i + 1],
		          packetStoreData);

		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		PacketStore newPacketStore;
		newPacketStore.sizeInBytes = sizes[i];
		newPacketStore.packetStoreType = ((i % 2) == 0) ? PacketStore::Circular : PacketStore::Bounded;
		newPacketStore.virtualChannel = virtualChannels[i];
		Services.storageAndRetrieval.packetStores.insert({packetStoreId, newPacketStore});
	}
}

etl::array<String<ECSS_MAX_PACKET_STORE_ID_SIZE>, 4> validPacketStoreIds() {
	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
	uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";
	uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
	uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";

	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);

	etl::array<String<ECSS_MAX_PACKET_STORE_ID_SIZE>, 4> validPacketStores = {id, id2, id3, id4};
	return validPacketStores;
}

void padWithZeros(etl::array<String<ECSS_MAX_PACKET_STORE_ID_SIZE>, 4>& packetStoreIds) {
	uint8_t offsets[] = {3, 4, 5, 6};
	int index = 0;
	// Padding every empty position with zeros, to avoid memory garbage collection, which leads to a faulty result.
	for (auto& packetStoreId : packetStoreIds) {
		uint8_t startingPosition = offsets[index++];
		for (uint8_t i = startingPosition; i < ECSS_MAX_PACKET_STORE_ID_SIZE; i++) {
			packetStoreId[i] = 0;
		}
	}
}

StorageAndRetrievalService::PacketSelectionSubservice packetSelection =
    StorageAndRetrievalService::PacketSelectionSubservice(Services.storageAndRetrieval, 0, 0, 0, 0, 0);

uint8_t apps[] = {0, 1};
uint8_t services[2][2] = {{MemoryManagementService::ServiceType, RequestVerificationService::ServiceType},
                          {EventActionService::ServiceType, ParameterService::ServiceType}};
uint8_t reports[] = {StorageAndRetrievalService::MessageType::EventReportConfigurationContentReport,
                     ParameterService::MessageType::ParameterValuesReport,
                     EventActionService::MessageType::EventActionStatusReport};
uint8_t reports2[] = {StorageAndRetrievalService::MessageType::EventReportConfigurationContentReport,
                      ParameterService::MessageType::ParameterValuesReport,
                      EventReportService::MessageType::InformativeEventReport,
                      StorageAndRetrievalService::MessageType::AppConfigurationContentReport};

void initializeAppProcessConfiguration(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, bool makeSame) {
	uint8_t numOfApplications = 2;

	typedef etl::vector<uint8_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> reportTypes;
	typedef etl::map<uint8_t, reportTypes, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> serviceTypes;
	etl::map<uint16_t, serviceTypes, ECSS_MAX_APPLICATION_PROCESS_DEFINITIONS> applicationDefinitions;

	auto& configuration = packetSelection.applicationProcessConfiguration.definitions[packetStoreId];

	for (int i = 0; i < numOfApplications; i++) {
		uint8_t applicationId = apps[i];
		uint8_t numOfServices = 2;

		for (int j = 0; j < numOfServices; j++) {
			uint8_t serviceId = services[i][j];
			uint16_t numOfMessageTypes = 4;
			if (makeSame) {
				numOfMessageTypes = 3;
			}

			for (int k = 0; k < numOfMessageTypes; k++) {
				uint8_t messageType = 0;
				if (makeSame) {
					messageType = reports[k];
				} else {
					messageType = reports2[k];
				}
				configuration[applicationId].serviceTypeDefinitions[serviceId].push_back(messageType);
			}
		}
	}
}

Message buildRequest(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, bool isDeletionRequest) {
	uint8_t numOfApplications = 2;

	typedef etl::vector<uint8_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> reportTypes;
	typedef etl::map<uint8_t, reportTypes, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> serviceTypes;
	etl::map<uint8_t, serviceTypes, ECSS_MAX_APPLICATION_PROCESS_DEFINITIONS> applicationDefinitions;

	for (int i = 0; i < numOfApplications; i++) {
		uint8_t applicationId = apps[i];
		uint8_t numOfServices = 2;

		for (int j = 0; j < numOfServices; j++) {
			uint8_t serviceId = services[i][j];
			uint16_t numOfMessageTypes = 3;

			for (int k = 0; k < numOfMessageTypes; k++) {
				uint8_t messageType = reports[k];
				applicationDefinitions[applicationId][serviceId].push_back(messageType);
			}
		}
	}

	StorageAndRetrievalService::MessageType messageType =
	    (isDeletionRequest) ? StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration
	                        : StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration;

	Message request(StorageAndRetrievalService::ServiceType, messageType, Message::TC, 1);

	request.appendOctetString(packetStoreId);
	request.appendUint8(applicationDefinitions.size());

	for (auto& applicationDefinition : applicationDefinitions) {
		request.appendUint8(applicationDefinition.first);
		request.appendUint8(applicationDefinition.second.size());

		for (auto& serviceDefinition : applicationDefinition.second) {
			request.appendUint8(serviceDefinition.first);
			request.appendUint16(serviceDefinition.second.size());

			for (auto reportDefinition : serviceDefinition.second) {
				request.appendUint8(reportDefinition);
			}
		}
	}

	packetSelection.controlledApplications.push_back(0);
	packetSelection.controlledApplications.push_back(1);

	return request;
}

Message buildReportTypeAdditionRequest2(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId) {
	uint8_t numOfApplications = 2;

	typedef etl::vector<uint8_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> reportTypes;
	typedef etl::map<uint8_t, reportTypes, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> serviceTypes;
	etl::map<uint8_t, serviceTypes, ECSS_MAX_APPLICATION_PROCESS_DEFINITIONS> applicationDefinitions;

	for (int i = 0; i < numOfApplications; i++) {
		uint8_t applicationId = apps[i];
		uint8_t numOfServices = 2;

		for (int j = 0; j < numOfServices; j++) {
			uint8_t serviceId = services[i][j];
			applicationDefinitions[applicationId][serviceId].clear();
		}
	}

	Message request(StorageAndRetrievalService::ServiceType,
	                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration, Message::TC, 1);

	request.appendOctetString(packetStoreId);
	request.appendUint8(applicationDefinitions.size());

	for (auto& applicationDefinition : applicationDefinitions) {
		request.appendUint8(applicationDefinition.first);
		request.appendUint8(applicationDefinition.second.size());

		for (auto& serviceDefinition : applicationDefinition.second) {
			request.appendUint8(serviceDefinition.first);
			request.appendUint16(serviceDefinition.second.size());
		}
	}

	packetSelection.controlledApplications.push_back(0);
	packetSelection.controlledApplications.push_back(1);

	return request;
}

Message buildReportTypeAdditionRequest3(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId) {
	uint8_t numOfApplications = 2;

	typedef etl::vector<uint8_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> reportTypes;
	typedef etl::map<uint8_t, reportTypes, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> serviceTypes;
	etl::map<uint8_t, serviceTypes, ECSS_MAX_APPLICATION_PROCESS_DEFINITIONS> applicationDefinitions;

	for (int i = 0; i < numOfApplications; i++) {
		uint8_t applicationId = apps[i];
		applicationDefinitions[applicationId].clear();
	}

	Message request(StorageAndRetrievalService::ServiceType,
	                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration, Message::TC, 1);

	request.appendOctetString(packetStoreId);
	request.appendUint8(applicationDefinitions.size());

	for (auto& applicationDefinition : applicationDefinitions) {
		request.appendUint8(applicationDefinition.first);
		request.appendUint8(applicationDefinition.second.size());
	}

	packetSelection.controlledApplications.push_back(0);
	packetSelection.controlledApplications.push_back(1);

	return request;
}

void resetAppProcessConfiguration() {
	packetSelection.applicationProcessConfiguration.definitions.clear();
}

TEST_CASE("Adding report types to an application process configuration") {
	SECTION("Valid addition of specified report types") {
		initializePacketStores();
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		auto request = buildRequest(packetStoreIds[0], false);
		packetSelection.addReportTypesToAppProcessConfiguration(request);

		int c1 = 0;
		int c2 = 0;
		int c3 = 0;
		REQUIRE(packetSelection.applicationProcessConfiguration.definitions[packetStoreIds[0]].size() == 2);

		for (const auto& applicationId :
		     packetSelection.applicationProcessConfiguration.definitions[packetStoreIds[0]]) {
			REQUIRE(applicationId.first == apps[c1]);
			REQUIRE(applicationId.second.serviceTypeDefinitions.size() == 2);

			for (const auto& serviceId : applicationId.second.serviceTypeDefinitions) {
				uint8_t targetServiceId = serviceId.first;
				REQUIRE(std::find(std::begin(services[c1]), std::end(services[c1]), targetServiceId) !=
				        std::end(services[c1]));
				REQUIRE(serviceId.second.size() == 3);

				for (auto reportId : serviceId.second) {
					REQUIRE(reportId == reports[c3++]);
				}
				c2++;
				c3 = 0;
			}
			c1++;
			c2 = 0;
		}
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid addition of all report types for a specified service type") {
		initializePacketStores();
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		auto request = buildReportTypeAdditionRequest2(packetStoreIds[0]);
		packetSelection.addReportTypesToAppProcessConfiguration(request);

		int c1 = 0;
		int c2 = 0;
		REQUIRE(packetSelection.applicationProcessConfiguration.definitions[packetStoreIds[0]].size() == 2);

		for (const auto& applicationId :
		     packetSelection.applicationProcessConfiguration.definitions[packetStoreIds[0]]) {
			REQUIRE(applicationId.first == apps[c1]);
			REQUIRE(applicationId.second.serviceTypeDefinitions.size() == 2);

			for (const auto& serviceId : applicationId.second.serviceTypeDefinitions) {
				uint8_t targetServiceId = serviceId.first;
				REQUIRE(std::find(std::begin(services[c1]), std::end(services[c1]), targetServiceId) !=
				        std::end(services[c1]));
				REQUIRE(serviceId.second.empty());
				c2++;
			}
			c1++;
			c2 = 0;
		}
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid addition of all report types for a specified application") {
		initializePacketStores();
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		auto request = buildReportTypeAdditionRequest3(packetStoreIds[0]);
		packetSelection.addReportTypesToAppProcessConfiguration(request);

		int c1 = 0;
		REQUIRE(packetSelection.applicationProcessConfiguration.definitions[packetStoreIds[0]].size() == 2);

		for (const auto& applicationId :
		     packetSelection.applicationProcessConfiguration.definitions[packetStoreIds[0]]) {
			REQUIRE(applicationId.first == apps[c1]);
			REQUIRE(applicationId.second.serviceTypeDefinitions.empty());
			c1++;
		}
		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Deleting report types from an application process configuration") {
	SECTION("Valid deletion of specified report types") {
		initializePacketStores();
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);
		auto packetStoreId = packetStoreIds[0];

		initializeAppProcessConfiguration(packetStoreId, false);
		auto& configuration = packetSelection.applicationProcessConfiguration.definitions[packetStoreId];

		REQUIRE(configuration.size() == 2);
		REQUIRE(configuration[0].serviceTypeDefinitions.size() == 2);
		REQUIRE(configuration[1].serviceTypeDefinitions.size() == 2);
		REQUIRE(configuration[0].serviceTypeDefinitions[1].size() == 4);
		REQUIRE(configuration[0].serviceTypeDefinitions[6].size() == 4);
		REQUIRE(configuration[1].serviceTypeDefinitions[19].size() == 4);
		REQUIRE(configuration[1].serviceTypeDefinitions[20].size() == 4);

		auto request = buildRequest(packetStoreId, true);
		packetSelection.deleteReportTypesFromAppProcessConfiguration(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingReportTypeDefinitionInService) == 4);

		REQUIRE(configuration.size() == 2);
		REQUIRE(configuration[0].serviceTypeDefinitions.size() == 2);
		REQUIRE(configuration[1].serviceTypeDefinitions.size() == 2);
		REQUIRE(configuration[0].serviceTypeDefinitions[1].size() == 2);
		REQUIRE(configuration[0].serviceTypeDefinitions[6].size() == 2);
		REQUIRE(configuration[1].serviceTypeDefinitions[19].size() == 2);
		REQUIRE(configuration[1].serviceTypeDefinitions[20].size() == 2);

		uint8_t expectedReportTypes[] = {EventReportService::MessageType::InformativeEventReport,
		                                 StorageAndRetrievalService::MessageType::AppConfigurationContentReport};

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				auto& listToCheck = configuration[apps[i]].serviceTypeDefinitions[services[i][j]];
				REQUIRE(std::equal(listToCheck.begin(), listToCheck.end(), std::begin(expectedReportTypes)));
			}
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid deletion of specified report types, deletion leads to empty configuration") {
		initializePacketStores();
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);
		auto packetStoreId = packetStoreIds[0];

		initializeAppProcessConfiguration(packetStoreId, true);
		auto& configuration = packetSelection.applicationProcessConfiguration.definitions[packetStoreId];

		REQUIRE(configuration.size() == 2);
		REQUIRE(configuration[0].serviceTypeDefinitions.size() == 2);
		REQUIRE(configuration[1].serviceTypeDefinitions.size() == 2);
		REQUIRE(configuration[0].serviceTypeDefinitions[1].size() == 3);
		REQUIRE(configuration[0].serviceTypeDefinitions[6].size() == 3);
		REQUIRE(configuration[1].serviceTypeDefinitions[19].size() == 3);
		REQUIRE(configuration[1].serviceTypeDefinitions[20].size() == 3);

		auto request = buildRequest(packetStoreId, true);
		packetSelection.deleteReportTypesFromAppProcessConfiguration(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(configuration.empty());

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of the application process configuration content") {
	SECTION("Correct reporting of the configuration content") {
		initializePacketStores();
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);
		initializeAppProcessConfiguration(packetStoreIds[0], true);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportAppConfigurationContent, Message::TC, 1);
		request.appendOctetString(packetStoreIds[0]);

		packetSelection.reportAppConfigurationContent(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::AppConfigurationContentReport);

		uint8_t data[ECSS_MAX_PACKET_STORE_ID_SIZE];
		report.readOctetString(data);
		uint8_t expectedData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";

		uint8_t services343[2][2] = {{EventActionService::ServiceType, ParameterService::ServiceType},
		                             {MemoryManagementService::ServiceType, RequestVerificationService::ServiceType}};

		REQUIRE(std::equal(std::begin(data), std::end(data), std::begin(expectedData)));
		REQUIRE(report.readUint8() == 2); // Number of applications
		REQUIRE(report.readUint8() == 0); // App-1
		REQUIRE(report.readUint8() == 2); // Number of services
		REQUIRE(report.readUint8() == RequestVerificationService::ServiceType); // Service-1
		REQUIRE(report.readUint16() == 3); // Number of reports
		REQUIRE(report.readEnum8() == StorageAndRetrievalService::MessageType::EventReportConfigurationContentReport);
		REQUIRE(report.readEnum8() == ParameterService::MessageType::ParameterValuesReport);
		REQUIRE(report.readEnum8() == EventActionService::MessageType::EventActionStatusReport);
		REQUIRE(report.readUint8() == MemoryManagementService::ServiceType); // Service-2
		REQUIRE(report.readUint16() == 3); // Number of reports
		REQUIRE(report.readEnum8() == StorageAndRetrievalService::MessageType::EventReportConfigurationContentReport);
		REQUIRE(report.readEnum8() == ParameterService::MessageType::ParameterValuesReport);
		REQUIRE(report.readEnum8() == EventActionService::MessageType::EventActionStatusReport);

		REQUIRE(report.readUint8() == 1); // App-2
		REQUIRE(report.readUint8() == 2); // Number of services
		REQUIRE(report.readUint8() == EventActionService::ServiceType); // Service-1
		REQUIRE(report.readUint16() == 3); // Number of reports
		REQUIRE(report.readEnum8() == StorageAndRetrievalService::MessageType::EventReportConfigurationContentReport);
		REQUIRE(report.readEnum8() == ParameterService::MessageType::ParameterValuesReport);
		REQUIRE(report.readEnum8() == EventActionService::MessageType::EventActionStatusReport);
		REQUIRE(report.readUint8() == ParameterService::ServiceType); // Service-2
		REQUIRE(report.readUint16() == 3); // Number of reports
		REQUIRE(report.readEnum8() == StorageAndRetrievalService::MessageType::EventReportConfigurationContentReport);
		REQUIRE(report.readEnum8() == ParameterService::MessageType::ParameterValuesReport);
		REQUIRE(report.readEnum8() == EventActionService::MessageType::EventActionStatusReport);

		ServiceTests::reset();
		Services.reset();
	}
}

// SECTION("Adding report types to application process storage control configuration") {
//	Message createPacketStores(StorageAndRetrievalService::ServiceType,
//	                           StorageAndRetrievalService::MessageType::CreatePacketStores, Message::TC, 1);
//	Services.storageAndRetrieval.packetStores.clear();
//	buildPacketCreationRequest(createPacketStores);
//
//	MessageParser::execute(createPacketStores);
//
//	Message request(StorageAndRetrievalService::ServiceType,
//	                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration, Message::TC, 1);
//
//	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
//	uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps7444";
//	uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps1111";
//	uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
//	uint8_t packetStoreData5[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";
//	uint8_t packetStoreData6[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";
//
//	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
//	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
//	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
//	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);
//	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id5(packetStoreData5);
//	String<ECSS_MAX_PACKET_STORE_ID_SIZE> id6(packetStoreData6);
//
//	CHECK(packetSelection.controlledAppProcesses.empty());
//	etl::vector<uint16_t, 5> applicationIdsToBeControlled = {1, 2, 3, 4, 5};
//	for (auto& appId : applicationIdsToBeControlled) {
//		packetSelection.controlledAppProcesses.push_back(appId);
//	}
//	CHECK(packetSelection.controlledAppProcesses.size() == applicationIdsToBeControlled.size());
//
//	request.appendOctetString(id2);
//	packetSelection.addReportTypesToAppProcessConfiguration(request);
//
//	CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 12);
//	CHECK(packetSelection.applicationProcessConfiguration.definitions.empty());
//
//	Message request2 = buildReportTypeAdditionRequest(id);
//	packetSelection.addReportTypesToAppProcessConfiguration(request2);
//
//	CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 12);
//	CHECK(packetSelection.applicationProcessConfiguration.definitions.size() == 1);
//	CHECK(packetSelection.applicationProcessConfiguration.definitions[id].size() == 2);
//	// App Ids added are (1,2)
//	//		CHECK(packetSelection.applicationProcessConfiguration.definitions[id][1].serviceTypeDefinitions.size() ==
//	//4);
//