#include "GoProTelem/GPMF_Payload.h"

#include <stdexcept>
#include "GPMF_parser.h"
#include "GPMF_mp4reader.h"
#include "GPMF_utils.h"

namespace gpt
{

	GPMF_Payload::~GPMF_Payload()
	{
		if (resHandle_)
		{
			FreePayloadResource(mp4Handle_,resHandle_);
			resHandle_ = 0;
			payloadData_ = nullptr;
		}
	}

	double
	GPMF_Payload::inTime()
	{
		return inTime_;
	}

	double
	GPMF_Payload::outTime()
	{
		return outTime_;
	}

	GPMF_StreamPtr
	GPMF_Payload::getStream()
	{
		return GPMF_StreamPtr(new GPMF_Stream(payloadData_, payloadSize_));
	}

	GPMF_Payload::GPMF_Payload(
		MP4_Handle mp4Handle,
		uint32_t index)
	 : mp4Handle_(mp4Handle)
	 , index_(index)
	 , payloadSize_(0)
	 , resHandle_(0)
	 , payloadData_(nullptr)
	{
		payloadSize_ = GetPayloadSize(mp4Handle, index);
		resHandle_ = GetPayloadResource(mp4Handle, resHandle_, payloadSize_);
		payloadData_ = GetPayload(mp4Handle, resHandle_, index);
		if (GetPayloadTime(mp4Handle, index, &inTime_, &outTime_) != GPMF_OK)
		{
			throw std::runtime_error("failed to get payload in/out times!");
		}
	}

}