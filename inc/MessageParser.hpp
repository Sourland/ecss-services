#ifndef ECSS_SERVICES_MESSAGEPARSER_HPP
#define ECSS_SERVICES_MESSAGEPARSER_HPP

#include <Services/EventActionService.hpp>
#include "Message.hpp"

/**
 * A generic class responsible for the execution of the incoming telemetry and telecommand
 * packets.
 *
 * @todo Make the connection between the ST[01]-request verification service and the services
 * that initiate it
 * @todo Implement the execute function in the upcoming services or generally in the upcoming
 * activities
 *
 */

class MessageParser {
public:

	/**
	 * It is responsible to call the suitable function that executes the proper service. The way that
	 * the services are selected is based on the serviceType of the \p message
	 *
	 * @todo The implementation of the execute function should correspond to the numbers of the
	 * services/activities that have been created
	 * @todo execute() needs to be redefined. The /p message isn't enough to call some
	 * subservices. More arguments are needed.
	 */
	void execute(Message &message);

	/**
	 * Parse a message that contains the CCSDS and ECSS packet headers, as well as the data
	 *
	 * As defined in CCSDS 133.0-B-1
	 *
	 * @param data The data of the message (not null-terminated)
	 * @param length The size of the message
	 * @return A new object that represents the parsed message
	 */
	Message parse(uint8_t *data, uint32_t length);

	/**
	 * @todo: elaborate on this comment
	 * Create a message so that a string can be parsed
	 *
	 * Note: conversion of char* to unsigned char* should flow without any problems according to
	 * this great analysis:
	 * stackoverflow.com/questions/15078638/can-i-turn-unsigned-char-into-char-and-vice-versa
	 */
	Message parseRequestTC(String<ECSS_EVENT_SERVICE_STRING_SIZE> data);

	Message parseRequestTC(uint8_t* data);

	String<ECSS_EVENT_SERVICE_STRING_SIZE> convertTCToStr(Message& message);

private:
	/**
	 * Parse the ECSS Telecommand packet secondary header
	 *
	 * As specified in section 7.4.4.1 of the standard
	 *
	 * @todo Implement the acknowledgement flags
	 * @todo Store and parse the source ID, if needed
	 *
	 * @param data The data of the header (not null-terminated)
	 * @param length The size of the header
	 * @param message The Message to modify based on the header
	 */
	void parseTC(const uint8_t *data, uint16_t length, Message &message);

	/**
	 * Parse the ECSS Telemetry packet secondary header
	 *
	 * As specified in section 7.4.3.1 of the standard
	 *
	 * @param data The data of the header (not null-terminated)
	 * @param length The size of the header
	 * @param message The Message to modify based on the header
	 */
	void parseTM(const uint8_t *data, uint16_t length, Message &message);
};


#endif //ECSS_SERVICES_MESSAGEPARSER_HPP