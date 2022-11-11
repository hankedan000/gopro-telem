#pragma once

#include <memory>

#include "GoProTelem/GoProTelem_Types.h"
#include "GoProTelem/GPMF_Stream.h"

namespace gpt
{

	// forward declarations
	class MP4_Source;
	class GPMF_Payload;

	using GPMF_PayloadPtr = std::shared_ptr<GPMF_Payload>;

	class GPMF_Payload
	{
	public:
		~GPMF_Payload();

		static
		GPMF_PayloadPtr
		get(
			MP4_Source &mp4,
			uint32_t index);

		double
		inTime();

		double
		outTime();

		GPMF_StreamPtr
		getStream();

	private:
		// only allow construction by the MP4_Source class
		friend class MP4_Source;

		GPMF_Payload(
			MP4_Handle mp4Handle,
			uint32_t index);

	private:
		MP4_Handle mp4Handle_;
		uint32_t index_;
		size_t payloadSize_;
		PayloadResHandle resHandle_;
		uint32_t *payloadData_;

		double inTime_,outTime_;

	};

}