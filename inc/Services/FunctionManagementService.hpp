#ifndef ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP
#define ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP

#include "etl/map.h"
#include "etl/String.hpp"
#include "Message.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"

#define FUNC_MAP_SIZE     5     // size of the function map (number of elements)
#define FUNC_NAME_LENGTH  32      // max length of the function name
#define MAX_ARG_LENGTH    32      // maximum argument byte string length

/**
 * Implementation of the ST[08] function management service
 *
 * This class implements a skeleton framework for the ST[08] service as described in
 * ECSS-E-ST-70-41C, pages 157-159. Final implementation is dependent on subsystem requirements
 * which are, as of this writing, undefined yet.
 *
 * Caveats:
 * 1) Function names shall be exactly MAXFUNCNAMELENGTH-lengthed in order to be properly read
 * and stored!  (not sure if this is a caveat though, as ECSS-E-ST-70-41C stipulates for ST[08]
 * that all function names must be fixed-length character strings)
 *
 * You have been warned.
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 */

 /**
 * Usage of the include() function:
 *
 * @code
 * void foo(String<MAX_ARG_LENGTH> b) {
 * 		std::cout << "SPAAAACE!" << std::endl;
 * 	}
 *
 * void bar(String<MAX_ARG_LENGTH> b) {
 * 		std::cout << "I HAZ A CUBESAT THAT SNAPS PIX!" << std::endl;
 * 	}
 *
 * void baz(String<MAX_ARG_LENGTH> b) {
 * 		std::cout << "QWERTYUIOP" << std::endl;
 * 	}
 *
 * 	FunctionManagementService::FunctionManagementService() {
 * 		include(String<FUNC_NAME_LENGTH>("foo"), &foo);
 * 		include(String<FUNC_NAME_LENGTH>("bar"), &bar);
 * 		include(String<FUNC_NAME_LENGTH>("baz"), &baz);
 * 	}
 */

typedef String<FUNC_NAME_LENGTH> functionName;
typedef etl::map<functionName, void(*)(String<MAX_ARG_LENGTH>), FUNC_MAP_SIZE>
FunctionMap;

class FunctionManagementService : public Service {
	/**
	 * Map of the function names to their respective pointers. Size controlled by FUNC_MAP_SIZE
	 */
	FunctionMap funcPtrIndex;


public:
	/**
	 * Constructs the function pointer index with all the necessary functions at initialization time
	 * These functions need to be in scope. Un-default when needed.
	 *
	 * @param None
	 */
	FunctionManagementService() = default;

	/**
	 * Calls the function described in the TC[8,1] message *msg*, passing the arguments contained
	 * and, if non-existent, generates a failed start of execution notification. Returns an unneeded
	 * int, for testing purposes.
	 * @param msg A TC[8,1] message
	 */
	void call(Message& msg);

	/**
	 * Includes a new function in the pointer map. This enables it to be called by way of a valid
	 * TC [8,1] message.
	 *
	 * @param funcName the function's name. Max. length is FUNC_NAME_LENGTH bytes.
	 * @param ptr pointer to a function of void return type and a MAX_ARG_LENGTH-lengthed byte
	 * string as argument (which contains the actual arguments of the function)
	 */
	void include(String<FUNC_NAME_LENGTH> funcName, void(*ptr)(String<MAX_ARG_LENGTH>));

	int getMapSize() { return funcPtrIndex.size(); }
};

#endif //ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP