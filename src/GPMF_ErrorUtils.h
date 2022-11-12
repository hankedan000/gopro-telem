#pragma once

#include <string>

#include "GPMF_common.h"

namespace gpt
{

	std::string
	gpmfErrorToString(
		GPMF_ERROR error);

	std::string
	gpmfErrorToString(
		uint32_t error);

}
