#include "GoProTelem/MP4_Source.h"

#include <stdexcept>
#include "GPMF_mp4reader.h"

namespace gpt
{

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

	void
	MP4_Source::assertValid() const
	{
		if (mp4Handle_ == 0)
		{
			throw std::runtime_error("handle is null (was MP4_Source opened successfully?)");
		}
	}

}