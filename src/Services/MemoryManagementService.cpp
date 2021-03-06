#include "ECSS_Configuration.hpp"
#ifdef SERVICE_MEMORY

#include "Services/MemoryManagementService.hpp"
#include <cerrno>
#include <etl/String.hpp>

// Define the constructors for the classes
MemoryManagementService::MemoryManagementService() : rawDataMemorySubservice(*this) {
	serviceType = MemoryManagementService::ServiceType;
}

MemoryManagementService::RawDataMemoryManagement::RawDataMemoryManagement(MemoryManagementService& parent)
    : mainService(parent) {}

// Function declarations for the raw data memory management subservice
void MemoryManagementService::RawDataMemoryManagement::loadRawData(Message& request) {
	/**
	 * Bear in mind that there is currently no error checking for invalid parameters.
	 * A future version will include error checking and the corresponding error report/notification,
	 * as the manual implies.
	 *
	 * @todo Add error checking and reporting for the parameters
	 * @todo Add failure reporting
	 */
	// Check if we have the correct packet
	request.assertTC(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::LoadRawMemoryDataAreas);

	// Read the memory ID from the request
	auto memoryID = MemoryManagementService::MemoryID(request.readEnum8());

	// Check for a valid memory ID first
	if (mainService.memoryIdValidator(MemoryManagementService::MemoryID(memoryID))) {
		// Variable declaration
		uint8_t readData[ECSSMaxStringSize]; // Preallocate the array
		uint16_t iterationCount = request.readUint16(); // Get the iteration count

		if (memoryID == MemoryManagementService::MemoryID::FLASH) {
			// todo: Define FLASH specific access code when we transfer to embedded
		} else {
			for (std::size_t j = 0; j < iterationCount; j++) {
				uint64_t startAddress = request.readUint64(); // Start address of the memory
				uint16_t dataLength = request.readOctetString(readData); // Data length to load
				uint16_t checksum = request.readBits(16); // Get the CRC checksum from the message

				// Continue only if the checksum passes
				if (mainService.dataValidator(readData, checksum, dataLength)) {
					if (mainService.addressValidator(memoryID, startAddress) &&
					    mainService.addressValidator(memoryID, startAddress + dataLength)) {
						for (std::size_t i = 0; i < dataLength; i++) {
							*(reinterpret_cast<uint8_t*>(startAddress) + i) = readData[i];
						}

						// Read the loaded data for checksum validation and perform a check
						for (std::size_t i = 0; i < dataLength; i++) {
							readData[i] = *(reinterpret_cast<uint8_t*>(startAddress) + i);
						}
						if (checksum != CRCHelper::calculateCRC(readData, dataLength)) {
							ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
						}
					} else {
						ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
					}
				} else {
					ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
					continue; // Continue to the next command
				}
			}
		}
	} else {
		// todo: Send a failed start of execution
	}
}

void MemoryManagementService::RawDataMemoryManagement::dumpRawData(Message& request) {
	// Check if we have the correct packet
	request.assertTC(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::DumpRawMemoryData);

	// Create the report message object of telemetry message subtype 6
	Message report = mainService.createTM(MemoryManagementService::MessageType::DumpRawMemoryDataReport);
	uint8_t memoryID = request.readEnum8(); // Read the memory ID from the request

	// Check for a valid memory ID first
	if (mainService.memoryIdValidator(MemoryManagementService::MemoryID(memoryID))) {
		// Variable declaration
		uint8_t readData[ECSSMaxStringSize]; // Preallocate the array
		uint16_t iterationCount = request.readUint16(); // Get the iteration count

		// Append the data to report message
		report.appendEnum8(memoryID); // Memory ID
		report.appendUint16(iterationCount); // Iteration count

		// Iterate N times, as specified in the command message
		for (std::size_t j = 0; j < iterationCount; j++) {
			uint64_t startAddress = request.readUint64(); // Data length to read
			uint16_t readLength = request.readUint16(); // Start address for the memory read

			// Read memory data, an octet at a time, checking for a valid address first
			if (mainService.addressValidator(MemoryManagementService::MemoryID(memoryID), startAddress) &&
			    mainService.addressValidator(MemoryManagementService::MemoryID(memoryID), startAddress + readLength)) {
				for (std::size_t i = 0; i < readLength; i++) {
					readData[i] = *(reinterpret_cast<uint8_t*>(startAddress) + i);
				}

				// This part is repeated N-times (N = iteration count)
				report.appendUint64(startAddress); // Start address
				report.appendOctetString(String<1024>(readData, readLength)); // Save the
				// read data
				report.appendBits(16, CRCHelper::calculateCRC(readData, readLength));
			} else {
				ErrorHandler::reportError(request, ErrorHandler::AddressOutOfRange);
			}
		}

		mainService.storeMessage(report); // Save the report message
		request.resetRead(); // Reset the reading count
	} else {
		// todo: Send a failed start of execution
	}
}

void MemoryManagementService::RawDataMemoryManagement::checkRawData(Message& request) {
	// Check if we have the correct packet
	request.assertTC(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::CheckRawMemoryData);

	// Create the report message object of telemetry message subtype 10
	Message report = mainService.createTM(MemoryManagementService::MessageType::CheckRawMemoryDataReport);
	uint8_t memoryID = request.readEnum8(); // Read the memory ID from the request

	if (mainService.memoryIdValidator(MemoryManagementService::MemoryID(memoryID))) {
		// Variable declaration
		uint8_t readData[ECSSMaxStringSize]; // Preallocate the array
		uint16_t iterationCount = request.readUint16(); // Get the iteration count

		// Append the data to report message
		report.appendEnum8(memoryID); // Memory ID
		report.appendUint16(iterationCount); // Iteration count

		// Iterate N times, as specified in the command message
		for (std::size_t j = 0; j < iterationCount; j++) {
			uint64_t startAddress = request.readUint64(); // Data length to read
			uint16_t readLength = request.readUint16(); // Start address for the memory read

			// Check whether the first and the last addresses are within the limits
			if (mainService.addressValidator(MemoryManagementService::MemoryID(memoryID), startAddress) &&
			    mainService.addressValidator(MemoryManagementService::MemoryID(memoryID), startAddress + readLength)) {
				// Read memory data and save them for checksum calculation
				for (std::size_t i = 0; i < readLength; i++) {
					readData[i] = *(reinterpret_cast<uint8_t*>(startAddress) + i);
				}

				// This part is repeated N-times (N = iteration count)
				report.appendUint64(startAddress); // Start address
				report.appendUint16(readLength); // Save the read data
				report.appendBits(16, CRCHelper::calculateCRC(readData, readLength)); // Append CRC
			} else {
				ErrorHandler::reportError(request, ErrorHandler::AddressOutOfRange);
			}
		}

		mainService.storeMessage(report); // Save the report message
		request.resetRead(); // Reset the reading count
	} else {
		// todo: Send a failed start of execution report
	}
}

// Private function declaration section
bool MemoryManagementService::addressValidator(MemoryManagementService::MemoryID memId, uint64_t address) {
	bool validIndicator = false;

	switch (memId) {
		case MemoryManagementService::MemoryID::DTCMRAM:
			if ((address >= DTCMRAMLowerLim) && (address <= DTCMRAMUpperLim)) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::ITCMRAM:
			if ((address >= ITCMRAMLowerLim) && (address <= ITCMRAMUpperLim)) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::RAM_D1:
			if ((address >= RAMD1LowerLim) && (address <= RAMD1UpperLim)) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::RAM_D2:
			if ((address >= RAMD2LowerLim) && (address <= RAMD2UpperLim)) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::RAM_D3:
			if ((address >= RAMD3LowerLim) && (address <= RAMD3UpperLim)) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::FLASH:
			if ((address >= FlashLowerLim) && (address <= FlashUpperLim)) {
				validIndicator = true;
			}
			break;

		default:
			validIndicator = true; // todo: Implemented so addresses from PC can be read. Remove.
			break;
	}

	return validIndicator;
}

inline bool MemoryManagementService::memoryIdValidator(MemoryManagementService::MemoryID memId) {
	return (memId == MemoryManagementService::MemoryID::RAM_D1) ||
	       (memId == MemoryManagementService::MemoryID::RAM_D2) ||
	       (memId == MemoryManagementService::MemoryID::RAM_D3) ||
	       (memId == MemoryManagementService::MemoryID::DTCMRAM) ||
	       (memId == MemoryManagementService::MemoryID::ITCMRAM) ||
	       (memId == MemoryManagementService::MemoryID::FLASH) ||
	       (memId == MemoryManagementService::MemoryID::EXTERNAL);
}

inline bool MemoryManagementService::dataValidator(const uint8_t* data, uint16_t checksum, uint16_t length) {
	return (checksum == CRCHelper::calculateCRC(data, length));
}

void MemoryManagementService::execute(Message& message) {
	switch (message.messageType) {
		case LoadRawMemoryDataAreas:
			rawDataMemorySubservice.loadRawData(message);
			break;
		case DumpRawMemoryData:
			rawDataMemorySubservice.dumpRawData(message);
			break;
		case CheckRawMemoryData:
			rawDataMemorySubservice.checkRawData(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
