#include "GPMF_ErrorUtils.h"

namespace gpt
{

	std::string
	gpmfErrorToString(
		GPMF_ERROR error)
	{
		switch (error)
		{
			case GPMF_ERROR::GPMF_OK:
				return "GPMF_OK";
			case GPMF_ERROR::GPMF_ERROR_MEMORY:
				return "GPMF_ERROR_MEMORY";
			case GPMF_ERROR::GPMF_ERROR_BAD_STRUCTURE:
				return "GPMF_ERROR_BAD_STRUCTURE";
			case GPMF_ERROR::GPMF_ERROR_BUFFER_END:
				return "GPMF_ERROR_BUFFER_END";
			case GPMF_ERROR::GPMF_ERROR_FIND:
				return "GPMF_ERROR_FIND";
			case GPMF_ERROR::GPMF_ERROR_LAST:
				return "GPMF_ERROR_LAST";
			case GPMF_ERROR::GPMF_ERROR_TYPE_NOT_SUPPORTED:
				return "GPMF_ERROR_TYPE_NOT_SUPPORTED";
			case GPMF_ERROR::GPMF_ERROR_SCALE_NOT_SUPPORTED:
				return "GPMF_ERROR_SCALE_NOT_SUPPORTED";
			case GPMF_ERROR::GPMF_ERROR_SCALE_COUNT:
				return "GPMF_ERROR_SCALE_COUNT";
			case GPMF_ERROR::GPMF_ERROR_UNKNOWN_TYPE:
				return "GPMF_ERROR_UNKNOWN_TYPE";
			case GPMF_ERROR::GPMF_ERROR_RESERVED:
				return "GPMF_ERROR_RESERVED";
			default:
				return "**INVALID_ERROR**";
		}
	}

	std::string
	gpmfErrorToString(
		uint32_t error)
	{
		return gpmfErrorToString((GPMF_ERROR)error);
	}

}