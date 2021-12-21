#include "Platform/x86/Parameters/SystemParameters.hpp"
#include "catch2/catch.hpp"
#include "../Services/ServiceTests.hpp"

TEST_CASE("Getting a reference of a parameter") {
	SECTION("Valid parameter requested") {
		uint16_t parameterId = 7;
		REQUIRE(static_cast<Parameter<uint16_t>&>(Services.parameterManagement.getParameter(parameterId)->get())
		            .getValue() == 55);

		parameterId = 11;
		REQUIRE(static_cast<Parameter<uint16_t>&>(Services.parameterManagement.getParameter(parameterId)->get())
		            .getValue() == 1);
	}

	SECTION("Invalid parameter requested") {
		uint16_t parameterId = Services.parameterManagement.parametersArray.size() + 1;
		REQUIRE(not Services.parameterManagement.getParameter(parameterId));

		parameterId = parameterId + 24;
		REQUIRE(not Services.parameterManagement.getParameter(parameterId));
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistentParameter) == 2);
	}
}