#include "GoProTelem/MP4_Source.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "GPMF_mp4reader.h"

namespace gpt
{

	std::string
	SensorInfo::toString(
		const std::string &tabStr) const
	{
		std::stringstream ss;
        ss              << tabStr << "fourCC:           " << fourCC.toString();
        ss << std::endl << tabStr << "name:             " << name;
        ss << std::endl << tabStr << "totalSamples:     " << totalSamples;
        ss << std::endl << tabStr << "measuredRate:     " << std::fixed << std::setprecision(3) << measuredRate_hz << " (Hz)";
        if ( ! siUnit.empty())
        {
            ss << std::endl << tabStr << "siUnit:           " << siUnit;
        }
		return ss.str();
	}

	MP4_Source::MP4_Source()
	 : filepath_()
	 , mp4Handle_(0)
	{
	}

	MP4_Source::MP4_Source(
		const MP4_Source &other)
	 : filepath_(other.filepath_)
	 , mp4Handle_(other.mp4Handle_)
	{
	}

	MP4_Source::~MP4_Source()
	{
		this->close();
	}

	int
	MP4_Source::open(
		const std::string &filepath)
	{
		filepath_ = filepath;

#if 1 // Search for GPMF Track
		mp4Handle_ = OpenMP4Source(&filepath_[0], MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE, 0);
#else // look for a global GPMF payload in the moov header, within 'udta'
		mp4Handle_ = OpenMP4SourceUDTA(filepath.c_str(), 0);  //Search for GPMF payload with MP4's udta
#endif

		if (mp4Handle_ == 0)
		{
			return -1;
		}
		return 0;
	}

	void
	MP4_Source::close()
	{
		if (mp4Handle_ != 0)
		{
			CloseSource(mp4Handle_);
			mp4Handle_ = 0;
		}
	}

	MP4_Handle
	MP4_Source::handle()
	{
		return mp4Handle_;
	}

	float
	MP4_Source::duration()
	{
		assertValid();
		return GetDuration(mp4Handle_);
	}

	float
	MP4_Source::fps()
	{
		assertValid();
		uint32_t fps_numer, fps_denom;
		GetVideoFrameRateAndCount(mp4Handle_, &fps_numer, &fps_denom);
		return (float)(fps_numer)/fps_denom;
	}

	size_t
	MP4_Source::frameCount()
	{
		assertValid();
		uint32_t fps_numer, fps_denom;
		return GetVideoFrameRateAndCount(mp4Handle_, &fps_numer, &fps_denom);
	}

	size_t
	MP4_Source::payloadCount()
	{
		assertValid();
		return GetNumberPayloads(mp4Handle_);
	}

	GPMF_PayloadPtr
	MP4_Source::getPayload(
		uint32_t index)
	{
		return GPMF_PayloadPtr(new GPMF_Payload(mp4Handle_, index));
	}

	bool
	MP4_Source::getSensorInfo(
		FourCC sensor,
		SensorInfo &sensorInfo)
	{
		if (payloadCount() == 0)
		{
			return false;
		}

		auto lastPayload = getPayload(payloadCount() - 1);
		PayloadSensorInfo payloadSensorInfo;
		if ( ! lastPayload->getSensorInfo(sensor,payloadSensorInfo))
		{
			return false;
		}

		sensorInfo.fourCC = payloadSensorInfo.fourCC;
		sensorInfo.name = payloadSensorInfo.name;
		sensorInfo.siUnit = payloadSensorInfo.siUnit;
		sensorInfo.totalSamples = payloadSensorInfo.totalSamples;
		sensorInfo.measuredRate_hz = 0.0;
		if (sensorInfo.totalSamples > 1)
		{
			sensorInfo.measuredRate_hz = (sensorInfo.totalSamples - 1) / duration();
		}

		return true;
	}

	void
	MP4_Source::assertValid() const
	{
		if (mp4Handle_ == 0)
		{
			throw std::runtime_error("handle is null (was MP4_Source opened successfully?)");
		}
	}

}