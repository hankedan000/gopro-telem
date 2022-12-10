#include "GoProTelem/GPMF_Payload.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "GPMF_parser.h"
#include "GPMF_mp4reader.h"
#include "GPMF_utils.h"

namespace gpt
{

	std::string
	PayloadSensorInfo::toString(
		const std::string &tabStr) const
	{
		std::stringstream ss;
        ss              << tabStr << "fourCC:           " << fourCC.toString();
        ss << std::endl << tabStr << "name:             " << name;
        ss << std::endl << tabStr << "samplesInPayload: " << samplesInPayload;
        ss << std::endl << tabStr << "totalSamples:     " << totalSamples;
        ss << std::endl << tabStr << "measuredRate:     " << std::fixed << std::setprecision(3) << measuredRate_hz << " (Hz)";
        if (hasTemperature)
        {
            ss << std::endl << tabStr << "temperature:      " << temperature_c << " (C)";
        }
        if ( ! siUnit.empty())
        {
            ss << std::endl << tabStr << "siUnit:           " << siUnit;
        }
		return ss.str();
	}

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

	bool
	GPMF_Payload::getSensorInfo(
		FourCC sensor,
		PayloadSensorInfo &sensorInfo)
	{
		sensorInfo.fourCC = sensor;

		auto stream = getStream();
		if ( ! stream->findNext(sensor,GPMF_Levels::GPMF_RECURSE_LEVELS))
		{
			return false;
		}

		sensorInfo.samplesInPayload = stream->repeat();
		if (sensorInfo.samplesInPayload > 1)
		{
			double duration = outTime() - inTime();
			sensorInfo.measuredRate_hz = (sensorInfo.samplesInPayload - 1) / duration;
		}

		stream->pushState();
		if (stream->findPrev(GPMF_KEY_STREAM_NAME,GPMF_Levels::GPMF_CURRENT_LEVEL))
		{
			sensorInfo.name = stream->readString();
		}
		stream->popState();

		stream->pushState();
		if (stream->findPrev(GPMF_KEY_TOTAL_SAMPLES,GPMF_Levels::GPMF_CURRENT_LEVEL))
		{
			sensorInfo.totalSamples = stream->readUINT32();
		}
		stream->popState();

		stream->pushState();
		sensorInfo.hasTemperature = false;
		if (stream->findPrev(GPMF_KEY_TEMPERATURE_C,GPMF_Levels::GPMF_CURRENT_LEVEL))
		{
			sensorInfo.hasTemperature = true;
			sensorInfo.temperature_c = stream->readFloat();
		}
		stream->popState();

		stream->pushState();
		if (stream->findPrev(GPMF_KEY_SI_UNITS,GPMF_Levels::GPMF_CURRENT_LEVEL))
		{
			sensorInfo.siUnit = stream->readString();
		}
		stream->popState();

		return true;
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