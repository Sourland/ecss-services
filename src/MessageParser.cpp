#include <Services/EventActionService.hpp>
#include "ErrorHandler.hpp"
#include "MessageParser.hpp"
#include "macros.hpp"
#include "Services/TestService.hpp"
#include "Services/RequestVerificationService.hpp"

TestService TestService::instance;
RequestVerificationService RequestVerificationService::instance;

void MessageParser::execute(Message &message) {
	switch (message.serviceType) {
		case 1:
			RequestVerificationService::instance.execute(message);
			break;
		case 17:
			TestService::instance.execute(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::UnknownMessageType);
			break;
	}
}

Message MessageParser::parse(uint8_t *data, uint32_t length) {
	assertI(length >= 6, ErrorHandler::UnacceptablePacket);

	uint16_t packetHeaderIdentification = (data[0] << 8) | data[1];
	uint16_t packetSequenceControl = (data[2] << 8) | data[3];
	uint16_t packetDataLength = (data[4] << 8) | data[5];

	// Individual fields of the CCSDS Space Packet primary header
	uint8_t versionNumber = data[0] >> 5;
	Message::PacketType packetType = ((data[0] & 0x10) == 0) ? Message::TM : Message::TC;
	uint8_t secondaryHeaderFlag = data[0] & static_cast<uint8_t>(0x08);
	uint16_t APID = packetHeaderIdentification & static_cast<uint16_t>(0x07ff);
	auto sequenceFlags = static_cast<uint8_t>(packetSequenceControl >> 14);

	// Returning an internal error, since the Message is not available yet
	assertI(versionNumber == 0, ErrorHandler::UnacceptablePacket);
	assertI(secondaryHeaderFlag == 1, ErrorHandler::UnacceptablePacket);
	assertI(sequenceFlags == 0x3, ErrorHandler::UnacceptablePacket);
	assertI(packetDataLength == length - 6, ErrorHandler::UnacceptablePacket);

	Message message(0, 0, packetType, APID);

	if (packetType == Message::TC) {
		parseTC(data + 6, packetDataLength, message);
	} else {
		parseTM(data + 6, packetDataLength, message);
	}

	return message;
}

void MessageParser::parseTC(const uint8_t *data, uint16_t length, Message &message) {
	ErrorHandler::assertRequest(length >= 5, message, ErrorHandler::UnacceptableMessage);

	// Individual fields of the TC header
	uint8_t pusVersion = data[0] >> 4;
	uint8_t serviceType = data[1];
	uint8_t messageType = data[2];

	// todo: Fix this parsing function, because it assumes PUS header in data, which is not true
	//  with the current implementation
	// ErrorHandler::assertRequest(pusVersion == 2, message, ErrorHandler::UnacceptableMessage);

	// Remove the length of the header
	length -= 5;

	// Copy the data to the message
	// TODO: See if memcpy is needed for this
	message.serviceType = serviceType;
	message.messageType = messageType;
	memcpy(message.data, data + 5, length);
	message.dataSize = length;
}

Message MessageParser::parseRequestTC(String<ECSS_EVENT_SERVICE_STRING_SIZE> data) {
	Message message;
	uint8_t *dataInt = reinterpret_cast<uint8_t *>(data.data());
	message.packetType = Message::TC;
	parseTC(dataInt, ECSS_EVENT_SERVICE_STRING_SIZE, message);
	return message;
}

Message MessageParser::parseRequestTC(uint8_t* data) {
	Message message;
	message.packetType = Message::TC;
	parseTC(data, ECSS_EVENT_SERVICE_STRING_SIZE, message);
	return message;
}

String<ECSS_EVENT_SERVICE_STRING_SIZE> MessageParser::convertTCToStr(Message &message) {
	uint8_t tempString[ECSS_EVENT_SERVICE_STRING_SIZE] = {0};

	tempString[0] = ECSS_PUS_VERSION << 4; // Assign the pusVersion = 2
	tempString[1] = message.serviceType;
	tempString[2] = message.messageType;
	memcpy(tempString + 5, message.data, ECSS_EVENT_SERVICE_STRING_SIZE - 5);
	String<ECSS_EVENT_SERVICE_STRING_SIZE> dataString(tempString);

	return dataString;
}

void MessageParser::parseTM(const uint8_t *data, uint16_t length, Message &message) {
	ErrorHandler::assertRequest(length >= 5, message, ErrorHandler::UnacceptableMessage);

	// Individual fields of the TM header
	uint8_t pusVersion = data[0] >> 4;
	uint8_t serviceType = data[1];
	uint8_t messageType = data[2];

	ErrorHandler::assertRequest(pusVersion == 2, message, ErrorHandler::UnacceptableMessage);

	// Remove the length of the header
	length -= 5;

	// Copy the data to the message
	// TODO: See if memcpy is needed for this
	message.serviceType = serviceType;
	message.messageType = messageType;
	memcpy(message.data, data + 5, length);
	message.dataSize = length;
}