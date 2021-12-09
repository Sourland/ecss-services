#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/StorageAndRetrievalService.hpp"

StorageAndRetrievalService& storageAndRetrieval = Services.storageAndRetrieval;

uint32_t timestamps1[6] = {2, 4, 5, 7, 9, 11};
uint32_t timestamps2[5] = {0, 1, 4, 15, 22};
uint32_t timestamps3[4] = {4, 7, 9, 14};
uint32_t timestamps4[8] = {4, 6, 34, 40, 44, 51, 52, 58};

void initializePacketStores() {
	uint16_t numOfPacketStores = 4;
	uint8_t concatenatedPacketStoreNames[] = "ps2ps25ps799ps5555";
	uint16_t offsets[5] = {0, 3, 7, 12, 18};
	uint16_t sizes[4] = {100, 200, 550, 340};
	uint8_t virtualChannels[4] = {4, 6, 1, 2};
	uint8_t packetStoreTypeCode[2] = {0, 1};

	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize];
		for (auto& data : packetStoreData) {
			data = 0;
		}
		std::copy(concatenatedPacketStoreNames + offsets[i], concatenatedPacketStoreNames + offsets[i + 1],
		          packetStoreData);
		String<ECSSMaxPacketStoreIdSize> packetStoreId(packetStoreData);
		PacketStore newPacketStore;
		newPacketStore.sizeInBytes = sizes[i];
		newPacketStore.packetStoreType = ((i % 2) == 0) ? PacketStore::Circular : PacketStore::Bounded;
		newPacketStore.virtualChannel = virtualChannels[i];
		storageAndRetrieval.packetStores.insert({packetStoreId, newPacketStore});
	}
}

void validPacketStoreCreationRequest(Message& request) {
	uint16_t numOfPacketStores = 4;
	request.appendUint16(numOfPacketStores);
	uint8_t concatenatedPacketStoreNames[] = "ps2ps25ps799ps5555";
	uint16_t offsets[5] = {0, 3, 7, 12, 18};
	uint16_t sizes[4] = {100, 200, 550, 340};
	uint8_t virtualChannels[4] = {4, 6, 1, 2};
	uint8_t packetStoreTypeCode[2] = {0, 1};

	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize];
		for (auto& data : packetStoreData) {
			data = 0;
		}
		std::copy(concatenatedPacketStoreNames + offsets[i], concatenatedPacketStoreNames + offsets[i + 1],
		          packetStoreData);
		String<ECSSMaxPacketStoreIdSize> packetStoreId(packetStoreData);
		request.appendOctetString(packetStoreId);
		request.appendUint16(sizes[i]);
		if ((i % 2) == 0) {
			request.appendUint8(packetStoreTypeCode[0]);
		} else {
			request.appendUint8(packetStoreTypeCode[1]);
		}
		request.appendUint8(virtualChannels[i]);
	}
}

void invalidPacketStoreCreationRequest(Message& request) {
	uint16_t numOfPacketStores = 5;
	request.appendUint16(numOfPacketStores);
	uint8_t concatenatedPacketStoreNames[] = "ps2ps1ps2ps44ps0000";
	uint16_t offsets[6] = {0, 3, 6, 9, 13, 19};
	uint16_t sizes[5] = {33, 55, 66, 77, 99};
	uint8_t virtualChannels[5] = {5, StorageAndRetrievalService::VirtualChannels::MIN - 1, 1,
	                              StorageAndRetrievalService::VirtualChannels::MAX + 1, 2};
	uint8_t packetStoreTypeCode[2] = {0, 1};

	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize];
		for (auto& x : packetStoreData) {
			x = 0;
		}
		std::copy(concatenatedPacketStoreNames + offsets[i], concatenatedPacketStoreNames + offsets[i + 1],
		          packetStoreData); // for(auto x:packetStoreData){std::cout<<x<<" ";}std::cout<<std::endl;
		String<ECSSMaxPacketStoreIdSize> packetStoreId(packetStoreData);
		request.appendOctetString(packetStoreId);
		request.appendUint16(sizes[i]);
		if ((i % 2) == 0) {
			request.appendUint8(packetStoreTypeCode[0]);
		} else {
			request.appendUint8(packetStoreTypeCode[1]);
		}
		request.appendUint8(virtualChannels[i]);
	}
}

etl::array<String<ECSSMaxPacketStoreIdSize>, 4> validPacketStoreIds() {
	uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
	uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
	uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps799";
	uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps5555";

	String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
	String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
	String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
	String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);

	etl::array<String<ECSSMaxPacketStoreIdSize>, 4> validPacketStores = {id, id2, id3, id4};
	return validPacketStores;
}

etl::array<String<ECSSMaxPacketStoreIdSize>, 4> invalidPacketStoreIds() {
	uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps1";
	uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps36";
	uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps999";
	uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps1234";

	String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
	String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
	String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
	String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);

	etl::array<String<ECSSMaxPacketStoreIdSize>, 4> validPacketStores = {id, id2, id3, id4};
	return validPacketStores;
}

void padWithZeros(etl::array<String<ECSSMaxPacketStoreIdSize>, 4>& packetStoreIds) {
	uint8_t offsets[] = {3, 4, 5, 6};
	int index = 0;
	// Padding every empty position with zeros, to avoid memory garbage collection, which leads to a faulty result.
	for (auto& packetStoreId : packetStoreIds) {
		uint8_t startingPosition = offsets[index++];
		for (uint8_t i = startingPosition; i < ECSSMaxPacketStoreIdSize; i++) {
			packetStoreId[i] = 0;
		}
	}
}

void addTelemetryPacketsInPacketStores() {
	auto packetStoreIds = validPacketStoreIds();

	for (auto& timestamp : timestamps1) {
		Message tmPacket;
		Services.storageAndRetrieval.packetStores[packetStoreIds[0]].storedTelemetryPackets.push_back(
		    {timestamp, tmPacket});
	}
	for (auto& timestamp : timestamps2) {
		Message tmPacket;
		Services.storageAndRetrieval.packetStores[packetStoreIds[1]].storedTelemetryPackets.push_back(
		    {timestamp, tmPacket});
	}
	for (auto& timestamp : timestamps3) {
		Message tmPacket;
		Services.storageAndRetrieval.packetStores[packetStoreIds[2]].storedTelemetryPackets.push_back(
		    {timestamp, tmPacket});
	}
	for (auto& timestamp : timestamps4) {
		Message tmPacket;
		Services.storageAndRetrieval.packetStores[packetStoreIds[3]].storedTelemetryPackets.push_back(
		    {timestamp, tmPacket});
	}
}

TEST_CASE("Read packet store ID from a message") {
	SECTION("First successful read") {
		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "Racoon";
		String<ECSSMaxPacketStoreIdSize> packetStoreId(packetStoreData);

		Message message;
		message.appendOctetString(packetStoreId);

		packetStoreId = storageAndRetrieval.readPacketStoreId(message);
		REQUIRE(std::equal(std::begin(packetStoreData), std::end(packetStoreData), std::begin(packetStoreId)));
	}

	SECTION("Second successful read") {
		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "OMGitsRealTeo";
		String<ECSSMaxPacketStoreIdSize> packetStoreId(packetStoreData);

		Message message;
		message.appendOctetString(packetStoreId);

		packetStoreId = storageAndRetrieval.readPacketStoreId(message);
		REQUIRE(std::equal(std::begin(packetStoreData), std::end(packetStoreData), std::begin(packetStoreId)));
	}
}

TEST_CASE("Creating packet stores") {
	SECTION("Valid packet store creation request") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CreatePacketStores, Message::TC, 1);
		validPacketStoreCreationRequest(request);
		REQUIRE(storageAndRetrieval.packetStores.empty());

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);

		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[0]].sizeInBytes == 100);
		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[1]].sizeInBytes == 200);
		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[2]].sizeInBytes == 550);
		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[3]].sizeInBytes == 340);

		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[0]].virtualChannel == 4);
		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[1]].virtualChannel == 6);
		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[2]].virtualChannel == 1);
		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[3]].virtualChannel == 2);

		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[0]].packetStoreType ==
		      PacketStore::PacketStoreType::Circular);
		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[1]].packetStoreType ==
		      PacketStore::PacketStoreType::Bounded);
		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[2]].packetStoreType ==
		      PacketStore::PacketStoreType::Circular);
		CHECK(Services.storageAndRetrieval.packetStores[packetStoreIds[3]].packetStoreType ==
		      PacketStore::PacketStoreType::Bounded);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid packet store creation request") {
		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		String<ECSSMaxPacketStoreIdSize> existingPacketStoreId(packetStoreData);
		PacketStore existingPacketStore;
		storageAndRetrieval.packetStores.insert({existingPacketStoreId, existingPacketStore});
		REQUIRE(storageAndRetrieval.packetStores.size() == 1);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CreatePacketStores, Message::TC, 1);
		invalidPacketStoreCreationRequest(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidVirtualChannel) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AlreadyExistingPacketStore) == 2);
		REQUIRE(storageAndRetrieval.packetStores.size() == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Exceeding the maximum number of packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CreatePacketStores, Message::TC, 1);
		invalidPacketStoreCreationRequest(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MaxNumberOfPacketStoresReached) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Deleting packet stores") {
	SECTION("Valid deletion of specified packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores.empty());

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid deletion of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores.empty());

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid deletion of specified packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			request.appendOctetString(packetStoreId);
		}

		storageAndRetrieval.packetStores[packetStoreIds[0]].storageStatus = true;
		storageAndRetrieval.packetStores[packetStoreIds[1]].byTimeRangeRetrievalStatus = true;
		storageAndRetrieval.packetStores[packetStoreIds[2]].openRetrievalStatus = PacketStore::InProgress;
		storageAndRetrieval.packetStores[packetStoreIds[3]].storageStatus = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketStoreWithStorageStatusEnabled) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithByTimeRangeRetrieval) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithOpenRetrievalInProgress) == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid deletion request of non existing packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = invalidPacketStoreIds();

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) == storageAndRetrieval.packetStores.end());
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 4);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid deletion of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
		}

		storageAndRetrieval.packetStores[packetStoreIds[0]].storageStatus = true;
		storageAndRetrieval.packetStores[packetStoreIds[1]].byTimeRangeRetrievalStatus = true;
		storageAndRetrieval.packetStores[packetStoreIds[2]].openRetrievalStatus = PacketStore::InProgress;
		storageAndRetrieval.packetStores[packetStoreIds[3]].openRetrievalStatus = PacketStore::InProgress;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketStoreWithStorageStatusEnabled) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithByTimeRangeRetrieval) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithOpenRetrievalInProgress) == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Both valid and invalid deletion requests") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 8;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : correctPacketStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			request.appendOctetString(packetStoreId);
		}

		for (auto& packetStoreId : wrongPacketStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) == storageAndRetrieval.packetStores.end());
			request.appendOctetString(packetStoreId);
		}

		storageAndRetrieval.packetStores[correctPacketStoreIds[0]].storageStatus = true;
		storageAndRetrieval.packetStores[correctPacketStoreIds[1]].byTimeRangeRetrievalStatus = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		REQUIRE(storageAndRetrieval.packetStores.size() == 2);
		REQUIRE(storageAndRetrieval.packetStores.find(correctPacketStoreIds[0]) !=
		        storageAndRetrieval.packetStores.end());
		REQUIRE(storageAndRetrieval.packetStores.find(correctPacketStoreIds[1]) !=
		        storageAndRetrieval.packetStores.end());
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketStoreWithStorageStatusEnabled) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithByTimeRangeRetrieval) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 4);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Enabling the storage of packet stores") {
	SECTION("Valid enabling of storage") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::EnableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 2;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreIds[i]) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreIds[i]].storageStatus = false;
			request.appendOctetString(packetStoreIds[i]);
		}
		storageAndRetrieval.packetStores[packetStoreIds[2]].storageStatus = false;
		storageAndRetrieval.packetStores[packetStoreIds[3]].storageStatus = false;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].storageStatus == true);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].storageStatus == true);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].storageStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].storageStatus == false);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid enabling of storage") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = invalidPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::EnableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreIds[i]) == storageAndRetrieval.packetStores.end());
			request.appendOctetString(packetStoreIds[i]);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 3);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Enabling the storage of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::EnableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (auto& packetStore : storageAndRetrieval.packetStores) {
			REQUIRE(packetStore.second.storageStatus == true);
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Disabling the storage of packet stores") {
	SECTION("Valid disabling of storage") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DisableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 2;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreIds[i]) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreIds[i]].storageStatus = true;
			request.appendOctetString(packetStoreIds[i]);
		}
		storageAndRetrieval.packetStores[packetStoreIds[2]].storageStatus = true;
		storageAndRetrieval.packetStores[packetStoreIds[3]].storageStatus = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].storageStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].storageStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].storageStatus == true);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].storageStatus == true);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid disabling of storage") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = invalidPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DisableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreIds[i]) == storageAndRetrieval.packetStores.end());
			request.appendOctetString(packetStoreIds[i]);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 3);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Enabling the storage of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DisableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (auto& packetStore : storageAndRetrieval.packetStores) {
			REQUIRE(packetStore.second.storageStatus == false);
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Changing the open retrieval start-time-tag") {
	SECTION("Successful change of the start-time-tag") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartingTime, Message::TC, 1);

		uint32_t startTimeTag = 200;
		uint16_t numOfPacketStores = 2;
		request.appendUint32(startTimeTag);
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			REQUIRE(storageAndRetrieval.packetStores[packetStoreId].openRetrievalStartTimeTag == 0);
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].openRetrievalStartTimeTag == 200);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].openRetrievalStartTimeTag == 200);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].openRetrievalStartTimeTag == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed change of the start-time-tag") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartingTime, Message::TC, 1);

		uint32_t startTimeTag = 200;
		uint16_t numOfPacketStores = 6;
		request.appendUint32(startTimeTag);
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = correctPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			REQUIRE(storageAndRetrieval.packetStores[packetStoreId].openRetrievalStartTimeTag == 0);
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::InProgress;
			request.appendOctetString(packetStoreId);
		}

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) == storageAndRetrieval.packetStores.end());
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 3);

		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[0]].openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[1]].openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[2]].openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[3]].openRetrievalStartTimeTag == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Both successful and failed attempts to change the start-time-tag of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartingTime, Message::TC, 1);

		uint32_t startTimeTag = 200;
		uint16_t numOfPacketStores = 0;
		request.appendUint32(startTimeTag);
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			REQUIRE(storageAndRetrieval.packetStores[packetStoreId].openRetrievalStartTimeTag == 0);
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
		}
		storageAndRetrieval.packetStores[packetStoreIds[2]].openRetrievalStatus = PacketStore::InProgress;
		storageAndRetrieval.packetStores[packetStoreIds[3]].openRetrievalStatus = PacketStore::InProgress;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 2);

		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].openRetrievalStartTimeTag == 200);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].openRetrievalStartTimeTag == 200);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].openRetrievalStartTimeTag == 0);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Resuming the open retrieval process") {
	SECTION("Successful resuming of the open retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].openRetrievalStatus == PacketStore::Suspended);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed resuming of the open retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = correctPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = true;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			request.appendOctetString(packetStoreId);
		}
		storageAndRetrieval.packetStores[correctPacketStoreIds[3]].openRetrievalStatus = PacketStore::Suspended;

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) == storageAndRetrieval.packetStores.end());
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 3);

		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[0]].openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[1]].openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[2]].openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[3]].openRetrievalStatus ==
		        PacketStore::Suspended);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Both successful and failed attempts to resume the open retrieval of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
		}
		storageAndRetrieval.packetStores[packetStoreIds[2]].byTimeRangeRetrievalStatus = true;
		storageAndRetrieval.packetStores[packetStoreIds[3]].byTimeRangeRetrievalStatus = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 2);

		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].openRetrievalStatus == PacketStore::Suspended);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Suspending the open retrieval process") {
	SECTION("Successful suspension of the open retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::InProgress;
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].openRetrievalStatus == PacketStore::InProgress);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed suspension of the open retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = correctPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::InProgress;
			request.appendOctetString(packetStoreId);
		}
		storageAndRetrieval.packetStores[correctPacketStoreIds[3]].openRetrievalStatus = PacketStore::InProgress;

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) == storageAndRetrieval.packetStores.end());
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 3);

		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[0]].openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[1]].openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[2]].openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[3]].openRetrievalStatus ==
		        PacketStore::InProgress);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Successful attempt to suspend the open retrieval of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::InProgress;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);

		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].openRetrievalStatus == PacketStore::Suspended);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Starting the by-time-range retrieval of packet stores") {
	SECTION("Successful starting of the by-time-range retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::StartByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		uint32_t timeTags1[4] = {20, 30, 40, 50};
		uint32_t timeTags2[4] = {60, 70, 80, 90};

		int index = 0;
		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			request.appendOctetString(packetStoreId);
			uint32_t timeTag1 = timeTags1[index];
			uint32_t timeTag2 = timeTags2[index++];
			request.appendUint32(timeTag1);
			request.appendUint32(timeTag2);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[i]].byTimeRangeRetrievalStatus == true);
			REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[i]].retrievalStartTime == timeTags1[i]);
			REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[i]].retrievalEndTime == timeTags2[i]);
		}
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].retrievalStartTime == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].retrievalEndTime == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed starting of the by-time-range retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::StartByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = correctPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus =
			    (i % 2 == 0) ? PacketStore::Suspended : PacketStore::InProgress;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = i % 2 == 0;

			request.appendOctetString(packetStoreId);
			uint32_t timeTag1 = 20;
			uint32_t timeTag2 = 40;
			request.appendUint32(timeTag1);
			request.appendUint32(timeTag2);
		}
		storageAndRetrieval.packetStores[correctPacketStoreIds[3]].byTimeRangeRetrievalStatus = false;

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) == storageAndRetrieval.packetStores.end());
			request.appendOctetString(packetStoreId);
			uint32_t timeTag1 = 20;
			uint32_t timeTag2 = 40;
			request.appendUint32(timeTag1);
			request.appendUint32(timeTag2);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ByTimeRangeRetrievalAlreadyEnabled) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithOpenRetrievalInProgress) == 1);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[i]].byTimeRangeRetrievalStatus ==
			        (i % 2 == 0));
			REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[i]].retrievalStartTime == 0);
			REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[i]].retrievalEndTime == 0);
		}
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[3]].byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[3]].retrievalStartTime == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid window requested") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::StartByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			request.appendOctetString(packetStoreId);
			uint32_t timeTag1 = 90;
			uint32_t timeTag2 = 20;
			request.appendUint32(timeTag1);
			request.appendUint32(timeTag2);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidTimeWindow) == 3);

		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].byTimeRangeRetrievalStatus == false);

		for (auto& packetStore : storageAndRetrieval.packetStores) {
			REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].retrievalStartTime == 0);
			REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].retrievalEndTime == 0);
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Aborting the by-time-range retrieval of packet stores") {
	SECTION("Successful aborting of the by-time-range retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = true;
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[i]].byTimeRangeRetrievalStatus == false);
		}
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].byTimeRangeRetrievalStatus == true);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed aborting of the by-time-range retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto wrongPacketStoreIds = invalidPacketStoreIds();

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStore : storageAndRetrieval.packetStores) {
			packetStore.second.byTimeRangeRetrievalStatus = true;
		}

		for (int i = 0; i < numOfPacketStores; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) == storageAndRetrieval.packetStores.end());
			request.appendOctetString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 3);
		for (auto& packetStore : storageAndRetrieval.packetStores) {
			REQUIRE(packetStore.second.byTimeRangeRetrievalStatus == true);
		}

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Successful abort of the by-time-range retrieval of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId : packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStores.find(packetStoreId) != storageAndRetrieval.packetStores.end());
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = true;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);

		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].byTimeRangeRetrievalStatus == false);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting the status of packet stores") {
	SECTION("Valid reporting of the packet store status") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps5555";

		int count = 0;
		for (auto& packetStoreId : packetStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = (count % 2 == 0);
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = (count % 2 != 0);
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus =
			    (count % 2 == 0) ? PacketStore::InProgress : PacketStore::Suspended;
			count++;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportStatusOfPacketStores, Message::TC, 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoresStatusReport);
		REQUIRE(report.readUint16() == 4);

		// Packet store 1
		uint8_t data[ECSSMaxPacketStoreIdSize];
		report.readOctetString(data);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData)));
		CHECK(report.readBoolean() == true);
		CHECK(report.readUint8() == 0);
		CHECK(report.readBoolean() == false);
		// Packet store 2
		report.readOctetString(data);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData2)));
		CHECK(report.readBoolean() == false);
		CHECK(report.readUint8() == 1);
		CHECK(report.readBoolean() == true);
		// Packet store 3
		report.readOctetString(data);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData4)));
		CHECK(report.readBoolean() == false);
		CHECK(report.readUint8() == 1);
		CHECK(report.readBoolean() == true);
		// Packet store 4
		report.readOctetString(data);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData3)));
		CHECK(report.readBoolean() == true);
		CHECK(report.readUint8() == 0);
		CHECK(report.readBoolean() == false);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting the configuration of packet stores") {
	SECTION("Valid reporting of the configuration") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps5555";

		int count = 0;
		for (auto& packetStoreId : packetStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].packetStoreType =
			    (count % 2 == 0) ? PacketStore::Circular : PacketStore::Bounded;
			count++;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportConfigurationOfPacketStores, Message::TC, 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoreConfigurationReport);
		REQUIRE(report.readUint16() == 4);

		// Packet store 1
		uint8_t data[ECSSMaxPacketStoreIdSize];
		report.readOctetString(data);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData)));
		CHECK(report.readUint16() == 100);
		CHECK(report.readUint8() == 0);
		CHECK(report.readUint8() == 4);
		// Packet store 2
		report.readOctetString(data);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData2)));
		CHECK(report.readUint16() == 200);
		CHECK(report.readUint8() == 1);
		CHECK(report.readUint8() == 6);
		// Packet store 3
		report.readOctetString(data);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData4)));
		CHECK(report.readUint16() == 340);
		CHECK(report.readUint8() == 1);
		CHECK(report.readUint8() == 2);
		// Packet store 4
		report.readOctetString(data);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData3)));
		CHECK(report.readUint16() == 550);
		CHECK(report.readUint8() == 0);
		CHECK(report.readUint8() == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Resizing the packet stores") {
	SECTION("Successful resizing of packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint16_t newSizes[4] = {11, 22, 33, 44};

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResizePacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		int index = 0;
		for (auto& packetStoreId : packetStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;

			request.appendOctetString(packetStoreId);
			request.appendUint16(newSizes[index]);
			index++;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[i]].sizeInBytes == newSizes[i]);
		}
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].sizeInBytes == 340);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed resizing of packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint16_t oldSizes[4] = {100, 200, 340, 550};

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResizePacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);
		int index = 0;
		for (auto& packetStoreId : packetStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = (index % 2 == 0);
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = (index == 1);
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus =
			    (index == 3) ? PacketStore::InProgress : PacketStore::Suspended;

			request.appendOctetString(packetStoreId);
			request.appendUint16(35);
			index++;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithStorageStatusEnabled) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithOpenRetrievalInProgress) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithByTimeRangeRetrieval) == 1);
		int i = 0;
		for (auto& packetStore : storageAndRetrieval.packetStores) {
			REQUIRE(packetStore.second.sizeInBytes == oldSizes[i++]);
		}

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Request to resize non existing packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = invalidPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint16_t oldSizes[4] = {100, 200, 340, 550};

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResizePacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);
		for (auto& packetStoreId : packetStoreIds) {
			request.appendOctetString(packetStoreId);
			request.appendUint16(35);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 4);
		int i = 0;
		for (auto& packetStore : storageAndRetrieval.packetStores) {
			REQUIRE(packetStore.second.sizeInBytes == oldSizes[i++]);
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Changing the packet store type to circular") {
	SECTION("Successful changing of type to circular") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		for (auto& packetStoreId : packetStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].packetStoreType = PacketStore::Bounded;
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);

		request.appendOctetString(packetStoreIds[0]);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].packetStoreType == PacketStore::Bounded);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);

		request2.appendOctetString(packetStoreIds[3]);
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].packetStoreType == PacketStore::Circular);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed changing of type to circular") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		int count = 0;
		for (auto& packetStoreId : correctPacketStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].packetStoreType = PacketStore::Bounded;
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = (count == 0);
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = (count == 1);
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus =
			    (count == 2) ? PacketStore::InProgress : PacketStore::Suspended;
			count++;
		}

		String<ECSSMaxPacketStoreIdSize> finalIds[4] = {wrongPacketStoreIds[0], correctPacketStoreIds[0],
		                                                correctPacketStoreIds[1], correctPacketStoreIds[2]};

		ErrorHandler::ExecutionStartErrorType expectedErrors[4] = {
		    ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress};

		for (int i = 0; i < 4; i++) {
			Message request(StorageAndRetrievalService::ServiceType,
			                StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);

			request.appendOctetString(finalIds[i]);
			MessageParser::execute(request);
			CHECK(ServiceTests::count() == i + 1);
			CHECK(ServiceTests::countThrownErrors(expectedErrors[i]) == 1);
		}

		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[0]].packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[1]].packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[2]].packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[3]].packetStoreType == PacketStore::Bounded);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Changing the packet store type to bounded") {
	SECTION("Successful changing of type to bounded") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		for (auto& packetStoreId : packetStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].packetStoreType = PacketStore::Circular;
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);

		request.appendOctetString(packetStoreIds[0]);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].packetStoreType == PacketStore::Circular);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);

		request2.appendOctetString(packetStoreIds[3]);
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].packetStoreType == PacketStore::Bounded);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed changing of type to bounded") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		int count = 0;
		for (auto& packetStoreId : correctPacketStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].packetStoreType = PacketStore::Circular;
			storageAndRetrieval.packetStores[packetStoreId].storageStatus = (count == 0);
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = (count == 1);
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus =
			    (count == 2) ? PacketStore::InProgress : PacketStore::Suspended;
			count++;
		}

		String<ECSSMaxPacketStoreIdSize> finalIds[4] = {wrongPacketStoreIds[0], correctPacketStoreIds[0],
		                                                correctPacketStoreIds[1], correctPacketStoreIds[2]};

		ErrorHandler::ExecutionStartErrorType expectedErrors[4] = {
		    ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress};

		for (int i = 0; i < 4; i++) {
			Message request(StorageAndRetrievalService::ServiceType,
			                StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);

			request.appendOctetString(finalIds[i]);
			MessageParser::execute(request);
			CHECK(ServiceTests::count() == i + 1);
			CHECK(ServiceTests::countThrownErrors(expectedErrors[i]) == 1);
		}

		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[0]].packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[1]].packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[2]].packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.packetStores[correctPacketStoreIds[3]].packetStoreType == PacketStore::Circular);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Changing the virtual channel of packet stores") {
	SECTION("Successful change of virtual channel") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint8_t virtualChannels[2] = {1, 5};

		for (auto& packetStoreId : packetStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);

		request.appendOctetString(packetStoreIds[0]);
		request.appendUint8(virtualChannels[0]);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].virtualChannel == virtualChannels[0]);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].virtualChannel == 6);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].virtualChannel == 1);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].virtualChannel == 2);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);

		request2.appendOctetString(packetStoreIds[3]);
		request2.appendUint8(virtualChannels[1]);

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[0]].virtualChannel == virtualChannels[0]);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[1]].virtualChannel == 6);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[2]].virtualChannel == 1);
		REQUIRE(storageAndRetrieval.packetStores[packetStoreIds[3]].virtualChannel == virtualChannels[1]);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed change of virtual channel") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		uint8_t oldVirtualChannels[4] = {4, 6, 2, 1};

		int count = 0;
		for (auto& packetStoreId : correctPacketStoreIds) {
			storageAndRetrieval.packetStores[packetStoreId].byTimeRangeRetrievalStatus = (count == 0);
			storageAndRetrieval.packetStores[packetStoreId].openRetrievalStatus =
			    (count == 1) ? PacketStore::InProgress : PacketStore::Suspended;
			count++;
		}

		String<ECSSMaxPacketStoreIdSize> finalIds[4] = {wrongPacketStoreIds[0], correctPacketStoreIds[0],
		                                                correctPacketStoreIds[1], correctPacketStoreIds[2]};

		ErrorHandler::ExecutionStartErrorType expectedErrors[4] = {
		    ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress,
		    ErrorHandler::ExecutionStartErrorType::InvalidVirtualChannel};

		for (int i = 0; i < 4; i++) {
			Message request(StorageAndRetrievalService::ServiceType,
			                StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);

			request.appendOctetString(finalIds[i]);
			request.appendUint8(i == 3 ? StorageAndRetrievalService::VirtualChannels::MAX + 1 : 3);
			MessageParser::execute(request);
			CHECK(ServiceTests::count() == i + 1);
			CHECK(ServiceTests::countThrownErrors(expectedErrors[i]) == 1);
		}

		int index = 0;
		for (auto& packetStore : storageAndRetrieval.packetStores) {
			REQUIRE(packetStore.second.virtualChannel == oldVirtualChannels[index]);
			index++;
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting the content summary of packet stores") {
	SECTION("Successful content summary report of specified packet stores") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 2;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			storageAndRetrieval.packetStores[packetStoreIds[i]].openRetrievalStartTimeTag = 5;
			request.appendOctetString(packetStoreIds[i]);
		}

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoreContentSummaryReport);
		REQUIRE(report.readUint16() == 2);

		// Packet store 1
		uint8_t data[ECSSMaxPacketStoreIdSize];
		report.readOctetString(data);
		CHECK(std::equal(std::begin(packetStoreData), std::end(packetStoreData), std::begin(data)));
		CHECK(report.readUint32() == timestamps1[0]);
		CHECK(report.readUint32() == timestamps1[5]);
		CHECK(report.readUint32() == 5);
		CHECK(report.readUint16() == 30);
		CHECK(report.readUint16() == 20);
		// Packet store 2
		report.readOctetString(data);
		CHECK(std::equal(std::begin(packetStoreData2), std::end(packetStoreData2), std::begin(data)));
		CHECK(report.readUint32() == timestamps2[0]);
		CHECK(report.readUint32() == timestamps2[4]);
		CHECK(report.readUint32() == 5);
		CHECK(report.readUint16() == 25);
		CHECK(report.readUint16() == 10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Successful content summary report of all packet stores") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		int count = 0;
		for (auto& packetStore : storageAndRetrieval.packetStores) {
			packetStore.second.openRetrievalStartTimeTag = (count == 3) ? 20 : 15;
			count++;
		}

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoreContentSummaryReport);
		REQUIRE(report.readUint16() == 4);

		// Packet store 1
		uint8_t data[ECSSMaxPacketStoreIdSize];
		report.readOctetString(data);
		CHECK(std::equal(std::begin(packetStoreData), std::end(packetStoreData), std::begin(data)));
		CHECK(report.readUint32() == timestamps1[0]);
		CHECK(report.readUint32() == timestamps1[5]);
		CHECK(report.readUint32() == 15);
		CHECK(report.readUint16() == 30);
		CHECK(report.readUint16() == 0);
		// Packet store 2
		report.readOctetString(data);
		CHECK(std::equal(std::begin(packetStoreData2), std::end(packetStoreData2), std::begin(data)));
		CHECK(report.readUint32() == timestamps2[0]);
		CHECK(report.readUint32() == timestamps2[4]);
		CHECK(report.readUint32() == 15);
		CHECK(report.readUint16() == 25);
		CHECK(report.readUint16() == 10);
		// Packet store 3
		report.readOctetString(data);
		CHECK(std::equal(std::begin(packetStoreData3), std::end(packetStoreData3), std::begin(data)));
		CHECK(report.readUint32() == timestamps4[0]);
		CHECK(report.readUint32() == timestamps4[7]);
		CHECK(report.readUint32() == 15);
		CHECK(report.readUint16() == 40);
		CHECK(report.readUint16() == 30);
		// Packet store 4
		report.readOctetString(data);
		CHECK(std::equal(std::begin(packetStoreData4), std::end(packetStoreData4), std::begin(data)));
		CHECK(report.readUint32() == timestamps3[0]);
		CHECK(report.readUint32() == timestamps3[3]);
		CHECK(report.readUint32() == 20);
		CHECK(report.readUint16() == 20);
		CHECK(report.readUint16() == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed content summary report of packet stores") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.packetStores.size() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		String<ECSSMaxPacketStoreIdSize> finalIds[3] = {wrongPacketStoreIds[0], wrongPacketStoreIds[1],
		                                                correctPacketStoreIds[0]};

		storageAndRetrieval.packetStores[correctPacketStoreIds[0]].openRetrievalStartTimeTag = 5;

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			request.appendOctetString(finalIds[i]);
		}

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 2);

		Message report = ServiceTests::get(2);
		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoreContentSummaryReport);
		REQUIRE(report.readUint16() == 1);

		// Packet store 1
		uint8_t data[ECSSMaxPacketStoreIdSize];
		report.readOctetString(data);
		CHECK(std::equal(std::begin(packetStoreData), std::end(packetStoreData), std::begin(data)));
		CHECK(report.readUint32() == timestamps1[0]);
		CHECK(report.readUint32() == timestamps1[5]);
		CHECK(report.readUint32() == 5);
		CHECK(report.readUint16() == 30);
		CHECK(report.readUint16() == 20);

		ServiceTests::reset();
		Services.reset();
	}
}

//	SECTION("Reporting the packet store content summary") {
//		Message request(StorageAndRetrievalService::ServiceType,
//		                StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);
//		uint16_t numOfPacketStores = 3;
//		request.appendUint16(numOfPacketStores);
//
//		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
//		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
//		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
//		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
//		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
//		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";
//
//		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
//		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
//		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
//		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
//		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
//		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);
//
//		request.appendOctetString(id);
//		request.appendOctetString(id2);
//		request.appendOctetString(id3);
//
//		MessageParser::execute(request);
//
//		CHECK(ServiceTests::count() == 59);
//		Message report = ServiceTests::get(58);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 9);
//
//		uint8_t data[ECSSMaxPacketStoreIdSize];
//		CHECK(report.readUint16() == 1);
//		report.readOctetString(data);
//		int index = 0;
//		for (auto& x : data) {
//			CHECK(x == packetStoreData[index++]);
//		}
//		CHECK(report.readUint32() == timestamps1[0]);
//		CHECK(report.readUint32() == timestamps1[5]);
//		CHECK(report.readUint32() == 15);
//		CHECK(report.readUint16() == 30);
//		CHECK(report.readUint16() == 0);
//
//		Message request2(StorageAndRetrievalService::ServiceType,
//		                 StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);
//		numOfPacketStores = 0;
//		request2.appendUint16(numOfPacketStores);
//		Services.storageAndRetrieval.packetStores[id5].openRetrievalStartTimeTag = 20;
//		Services.storageAndRetrieval.packetStores[id4].storedTelemetryPackets.clear();
//
//		MessageParser::execute(request2);
//
//		CHECK(ServiceTests::count() == 60);
//		report = ServiceTests::get(59);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 9);
//		CHECK(report.readUint16() == 4);
//		// For ps2
//		report.readOctetString(data);
//		index = 0;
//		for (auto& x : data) {
//			CHECK(x == packetStoreData[index++]);
//		}
//		CHECK(report.readUint32() == timestamps1[0]);
//		CHECK(report.readUint32() == timestamps1[5]);
//		CHECK(report.readUint32() == 15);
//		CHECK(report.readUint16() == 30);
//		CHECK(report.readUint16() == 0);
//		// For ps25
//		report.readOctetString(data);
//		index = 0;
//		for (auto& x : data) {
//			CHECK(x == packetStoreData6[index++]);
//		}
//		CHECK(report.readUint32() == timestamps2[0]);
//		CHECK(report.readUint32() == timestamps2[4]);
//		CHECK(report.readUint32() == 15);
//		CHECK(report.readUint16() == 25);
//		CHECK(report.readUint16() == 10);
//		// For ps5555
//		report.readOctetString(data);
//		index = 0;
//		for (auto& x : data) {
//			CHECK(x == packetStoreData5[index++]);
//		}
//		CHECK(report.readUint32() == timestamps4[0]);
//		CHECK(report.readUint32() == timestamps4[7]);
//		CHECK(report.readUint32() == 20);
//		CHECK(report.readUint16() == 40);
//		CHECK(report.readUint16() == 30);
//		// For ps7999
//		report.readOctetString(data);
//		index = 0;
//		for (auto& x : data) {
//			CHECK(x == packetStoreData4[index++]);
//		}
//		// Skip the rest because it was previously used to copy other packets into it.
//	}
//

// TEST_CASE("Storage And Retrieval Service") {

//	SECTION("Deleting packet store content") {
//		Message request(StorageAndRetrievalService::ServiceType,
//		                StorageAndRetrievalService::MessageType::DeletePacketStoreContent, Message::TC, 1);
//		addTelemetryPacketsInPacketStores();
//		uint32_t timeLimit = 7;
//		uint16_t numOfPacketStores = 6;
//		request.appendUint32(timeLimit);
//		request.appendUint16(numOfPacketStores);
//
//		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
//		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
//		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
//		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
//		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
//		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";
//
//		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
//		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
//		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
//		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
//		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
//		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);
//
//		request.appendOctetString(id);
//		request.appendOctetString(id2);
//		request.appendOctetString(id3);
//		request.appendOctetString(id4);
//		request.appendOctetString(id5);
//		request.appendOctetString(id6);
//
//		Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus = true;
//		MessageParser::execute(request);
//
//		CHECK(ServiceTests::count() == 33);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 17);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 4);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 4);
//
//		CHECK(Services.storageAndRetrieval.packetStores[id].storedTelemetryPackets.size() == 6); // none deleted
//		CHECK(Services.storageAndRetrieval.packetStores[id4].storedTelemetryPackets.size() == 4); // none deleted
//		CHECK(Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets.size() == 6); // 1 deleted
//		CHECK(Services.storageAndRetrieval.packetStores[id6].storedTelemetryPackets.size() == 2); // 3 deleted
//
//		int index = 0;
//		for (auto& timestamp : Services.storageAndRetrieval.packetStores[id].storedTelemetryPackets) {
//			CHECK(timestamp.first == timestamps1[index++]);
//		}
//		index = 0;
//		for (auto& timestamp : Services.storageAndRetrieval.packetStores[id4].storedTelemetryPackets) {
//			CHECK(timestamp.first == timestamps3[index++]);
//		}
//		index = 2;
//		for (auto& timestamp : Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets) {
//			CHECK(timestamp.first == timestamps4[index++]);
//		}
//		index = 3;
//		for (auto& timestamp : Services.storageAndRetrieval.packetStores[id6].storedTelemetryPackets) {
//			CHECK(timestamp.first == timestamps2[index++]);
//		}
//
//		timeLimit = 55;
//		numOfPacketStores = 0;
//		request.appendUint32(timeLimit);
//		request.appendUint16(numOfPacketStores);
//
//		MessageParser::execute(request);
//
//		CHECK(Services.storageAndRetrieval.packetStores[id].storedTelemetryPackets.empty()); // all remaining deleted
//		CHECK(Services.storageAndRetrieval.packetStores[id4].storedTelemetryPackets.empty()); // all remaining deleted
//		CHECK(Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets.size() == 1); // 1 remained
//		CHECK(Services.storageAndRetrieval.packetStores[id6].storedTelemetryPackets.empty()); // all remaining deleted
//		Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets.pop_front(); // delete all to fill later
//		CHECK(Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets.empty());
//	}
//
//
//	SECTION("Copying packets in time window") {
//		Message request(StorageAndRetrievalService::ServiceType,
//		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
//		addTelemetryPacketsInPacketStores();
//
//		/**
//		 * CASE 1:
//		 *
//		 * 	(tag1)--------(earlier packet timestamp)-----(..more packets)-----(tag2)--------(latest packet timestamp)
//		 * 				left-most packet in deque				tag2 somewehere inside		right-most packet in deque
//		 */
//		request.appendUint16(0);
//		uint32_t timeTag1 = 0;
//		uint32_t timeTag2 = 4;
//		uint8_t fromPacketStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
//		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId(fromPacketStoreData);
//		uint8_t toPacketStoreData[ECSSMaxPacketStoreIdSize] = "ps799";
//		String<ECSSMaxPacketStoreIdSize> toPacketStoreId(toPacketStoreData);
//		Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.clear(); // empty ps799
//		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());
//
//		request.appendUint32(timeTag1);
//		request.appendUint32(timeTag2);
//		request.appendOctetString(fromPacketStoreId);
//		request.appendOctetString(toPacketStoreId);
//
//		MessageParser::execute(request);
//		CHECK(ServiceTests::count() == 34);
//
//		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.size() == 2);
//		int index = 0;
//		for (auto& tmPacket : Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets) {
//			CHECK(tmPacket.first == timestamps1[index++]);
//		}
//
//		/**
//		 * CASE 2:
//		 *
//		 * 	(earlier packet timestamp)-------(tag1)-----(..more packets)-----(tag2)--------(latest packet timestamp)
//		 * 	left-most packet in deque						both tag1 and tag2 inside 		right-most packet in deque
//		 */
//		Message request2(StorageAndRetrievalService::ServiceType,
//		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
//		request2.appendUint16(0);
//		timeTag1 = 35;
//		timeTag2 = 52;
//		uint8_t fromPacketStoreData2[ECSSMaxPacketStoreIdSize] = "ps5555";
//		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId2(fromPacketStoreData2);
//		Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.clear(); // empty ps799
//		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());
//
//		request2.appendUint32(timeTag1);
//		request2.appendUint32(timeTag2);
//		request2.appendOctetString(fromPacketStoreId2);
//		request2.appendOctetString(toPacketStoreId);
//
//		MessageParser::execute(request2);
//		CHECK(ServiceTests::count() == 34);
//
//		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.size() == 4);
//		index = 3;
//		for (auto& tmPacket : Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets) {
//			CHECK(tmPacket.first == timestamps4[index++]);
//		}
//
//		/**
//		 * CASE 3:
//		 *
//		 * 	(earlier packet timestamp)-------(tag1)-----(..more packets)-----(latest packet timestamp)--------(tag2)
//		 * 	left-most packet in deque		tag1 inside						right-most packet in deque
//		 */
//		Message request3(StorageAndRetrievalService::ServiceType,
//		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
//		request3.appendUint16(0);
//		timeTag1 = 3;
//		timeTag2 = 27;
//		uint8_t fromPacketStoreData3[ECSSMaxPacketStoreIdSize] = "ps25";
//		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId3(fromPacketStoreData3);
//		Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.clear(); // empty ps799
//		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());
//
//		request3.appendUint32(timeTag1);
//		request3.appendUint32(timeTag2);
//		request3.appendOctetString(fromPacketStoreId3);
//		request3.appendOctetString(toPacketStoreId);
//
//		MessageParser::execute(request3);
//		CHECK(ServiceTests::count() == 34);
//
//		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.size() == 3);
//		index = 2;
//		for (auto& tmPacket : Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets) {
//			CHECK(tmPacket.first == timestamps2[index++]);
//		}
//
//		/**
//		 * CASE 4: Invalid packet store
//		 */
//		// Invalid fromPacket
//		Message request4(StorageAndRetrievalService::ServiceType,
//		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
//		request4.appendUint16(0);
//		uint8_t fromPacketStoreData4[ECSSMaxPacketStoreIdSize] = "ps21";
//		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId4(fromPacketStoreData4);
//
//		request4.appendUint32(timeTag1);
//		request4.appendUint32(timeTag2);
//		request4.appendOctetString(fromPacketStoreId4);
//		request4.appendOctetString(toPacketStoreId);
//
//		MessageParser::execute(request4);
//		CHECK(ServiceTests::count() == 35);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 1);
//
//		// Invalid toPacket
//		Message request44(StorageAndRetrievalService::ServiceType,
//		                  StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
//		request44.appendUint16(0);
//
//		request44.appendUint32(timeTag1);
//		request44.appendUint32(timeTag2);
//		request44.appendOctetString(fromPacketStoreId3);
//		request44.appendOctetString(fromPacketStoreId4);
//
//		MessageParser::execute(request44);
//		CHECK(ServiceTests::count() == 36);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 2);
//
//		/**
//		 * CASE 5: Invalid time window
//		 */
//		Message request5(StorageAndRetrievalService::ServiceType,
//		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
//		request5.appendUint16(0);
//		timeTag1 = 5;
//		timeTag2 = 3;
//		uint8_t fromPacketStoreData5[ECSSMaxPacketStoreIdSize] = "ps2";
//		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId5(fromPacketStoreData5);
//
//		request5.appendUint32(timeTag1);
//		request5.appendUint32(timeTag2);
//		request5.appendOctetString(fromPacketStoreId5);
//		request5.appendOctetString(toPacketStoreId);
//
//		MessageParser::execute(request5);
//		CHECK(ServiceTests::count() == 37);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidTimeWindow) == 2);
//
//		/**
//		 * CASE 6: Destination packet NOT empty
//		 */
//		Message request6(StorageAndRetrievalService::ServiceType,
//		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
//		request6.appendUint16(0);
//		timeTag1 = 3;
//		timeTag2 = 7;
//
//		request6.appendUint32(timeTag1);
//		request6.appendUint32(timeTag2);
//		request6.appendOctetString(fromPacketStoreId5);
//		request6.appendOctetString(toPacketStoreId);
//
//		MessageParser::execute(request6);
//		CHECK(ServiceTests::count() == 38);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DestinationPacketStoreNotEmtpy) == 1);
//
//		/**
//		 * CASE 7: No packets inside the requested time window in the source packet
//		 */
//		Message request7(StorageAndRetrievalService::ServiceType,
//		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
//		request7.appendUint16(0);
//		uint8_t fromPacketStoreData7[ECSSMaxPacketStoreIdSize] = "ps5555";
//		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId7(fromPacketStoreData7);
//
//		Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.clear(); // empty ps799
//		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());
//		timeTag1 = 0;
//		timeTag2 = 3;
//
//		request7.appendUint32(timeTag1);
//		request7.appendUint32(timeTag2);
//		request7.appendOctetString(fromPacketStoreId7);
//		request7.appendOctetString(toPacketStoreId);
//
//		MessageParser::execute(request7);
//		CHECK(ServiceTests::count() == 39);
//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::CopyOfPacketsFailed) == 1);
//		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());
//
//		/**
//		 * todo: test the 'afterTimeTag' and 'beforeTimeTag' as well
//		 */
//	}
//

// }