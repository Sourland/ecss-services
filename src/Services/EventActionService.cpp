#include "Services/EventActionService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"

/**
 * @todo: Check if a uint16_t is needed (in case of changing the size of
 * eventActionDefinitionArray
 */
void EventActionService::addEventActionDefinitions(Message message) {
	// TC[19,1]

	if (message.messageType == 1 && message.packetType == Message::TC && message.serviceType ==
	                                                                     19) {
		uint16_t index;
		uint16_t applicationID = message.readEnum16();
		uint16_t eventDefinitionID = message.readEnum16();
		bool accepted = true;
		for (index = 0; index < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; index++) {
			if (eventActionDefinitionArray[index].applicationId == applicationID &&
			    eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID &&
			    eventActionDefinitionArray[index].enabled == true) {
				// @todo: throw a failed start of execution error
				accepted = false;
			}
		}
		if (accepted){
			for (index = 0; index < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; index++) {
				// @todo: throw an error if it's full
				if (eventActionDefinitionArray[index].empty == true) {
					break;
				}
			}
			if (index < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE) {
				eventActionDefinitionArray[index].empty = false;
				eventActionDefinitionArray[index].enabled = true;
				eventActionDefinitionArray[index].applicationId = applicationID;
				eventActionDefinitionArray[index].eventDefinitionID = eventDefinitionID;
				if (message.dataSize - 4 > ECSS_EVENT_SERVICE_STRING_SIZE) {
					ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::MessageTooLarge);
				} else {
					char data[ECSS_EVENT_SERVICE_STRING_SIZE];
					message.readString(data, message.dataSize - 4);
					eventActionDefinitionArray[index].request = String<ECSS_EVENT_SERVICE_STRING_SIZE>(
						data);
				}
			}
		}
	}
}

void EventActionService::deleteEventActionDefinitions(Message message) {
	// TC[19,2]
	if (message.messageType == 2 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t numberOfEventActionDefinitions = message.readUint16();
		for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			for (uint16_t index = 0; index < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; index++) {
				if (eventActionDefinitionArray[index].applicationId == applicationID &&
				    eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID &&
				    eventActionDefinitionArray[index].enabled == true) {
					eventActionDefinitionArray[index].empty = true;
					eventActionDefinitionArray[index].eventDefinitionID = 65535;
					eventActionDefinitionArray[index].request = "";
					eventActionDefinitionArray[index].applicationId = 0;
					eventActionDefinitionArray[index].enabled = false;
				}
			}
		}

	}
}

void EventActionService::deleteAllEventActionDefinitions(Message message) {
	// TC[19,3]
	if (message.messageType == 3 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		setEventActionFunctionStatus(false);
		for (uint16_t index = 0; index < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; index++) {
			if (eventActionDefinitionArray[index].empty == false) {
				eventActionDefinitionArray[index].empty = true;
				eventActionDefinitionArray[index].enabled = false;
				eventActionDefinitionArray[index].eventDefinitionID = 65535;
				eventActionDefinitionArray[index].request = "";
				eventActionDefinitionArray[index].applicationId = 0;
			}
		}
	}
}

void EventActionService::enableEventActionDefinitions(Message message) {
	// TC[19,4]
	if (message.messageType == 4 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t numberOfEventActionDefinitions = message.readUint16();
		if (numberOfEventActionDefinitions != 0){
			for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
				uint16_t applicationID = message.readEnum16();
				uint16_t eventDefinitionID = message.readEnum16();
				for (uint16_t index = 0; index < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; index++) {
					if (eventActionDefinitionArray[index].applicationId == applicationID &&
					    eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID) {
						eventActionDefinitionArray[index].enabled = true;
					}
				}
			}
		} else {
			for (uint16_t index = 0; index < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; index++) {
				if (eventActionDefinitionArray[index].empty == false){
					eventActionDefinitionArray[index].enabled = true;
				}
			}
		}
	}
}

void EventActionService::disableEventActionDefinitions(Message message) {
	// TC[19,5]
	if (message.messageType == 5 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t numberOfEventActionDefinitions = message.readUint16();
		if (numberOfEventActionDefinitions != 0){
			for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
				uint16_t applicationID = message.readEnum16();
				uint16_t eventDefinitionID = message.readEnum16();
				for (uint16_t index = 0; index < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; index++) {
					if (eventActionDefinitionArray[index].applicationId == applicationID &&
					    eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID) {
						eventActionDefinitionArray[index].enabled = false;
					}
				}
			}
		} else {
			for (uint16_t index = 0; index < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; index++) {
				if (eventActionDefinitionArray[index].empty == false){
					eventActionDefinitionArray[index].enabled = false;
				}
			}
		}
	}
}

void EventActionService::requestEventActionDefinitionStatus(Message message) {
	// TC[19,6]
	if (message.messageType == 6 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		eventActionStatusReport();
	}
}

void EventActionService::eventActionStatusReport() {
	// TM[19,7]
	Message report = createTM(7);
	uint8_t count = 0;
	for (uint16_t i = 0; i < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; i++) {
		if (eventActionDefinitionArray[i].empty == false) {
			count++;
		}
	}
	report.appendUint8(count);
	for (uint16_t i = 0; i < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; i++) {
		if (eventActionDefinitionArray[i].empty == false) {
			report.appendEnum16(eventActionDefinitionArray[i].applicationId);
			report.appendEnum16(eventActionDefinitionArray[i].eventDefinitionID);
			report.appendBoolean(eventActionDefinitionArray[i].enabled);
		}
	}
	storeMessage(report);
}

void EventActionService::enableEventActionFunction(Message message) {
	// TC[19,8]
	if (message.messageType == 8 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		setEventActionFunctionStatus(true);
	}
}

void EventActionService::disableEventActionFunction(Message message) {
	// TC[19,9]
	if (message.messageType == 9 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		setEventActionFunctionStatus(false);
	}
}

// Should I use the name execute here instead of executeAction?
// Should I use applicationID too?
void EventActionService::executeAction(uint16_t eventID) {
	// Custom function
	if (eventActionFunctionStatus) {
		for (uint16_t i = 0; i < ECSS_EVENT_ACTION_STRUCT_ARRAY_SIZE; i++) {
			if (eventActionDefinitionArray[i].empty == false &&
			    eventActionDefinitionArray[i].enabled ==
			    true) {
				if (eventActionDefinitionArray[i].eventDefinitionID == eventID) {
					MessageParser messageParser;
					Message message = messageParser.parseRequestTC(
						eventActionDefinitionArray[i].request);
					messageParser.execute(message);
				}
			}
		}
	}
}