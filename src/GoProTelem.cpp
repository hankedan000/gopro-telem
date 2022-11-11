#include "GoProTelem/GoProTelem.h"

#include <stdexcept>
#include "GPMF_parser.h"
#include "GPMF_mp4reader.h"
#include "GPMF_utils.h"

namespace gpt
{

	GPMF_LEVELS
	toGPMF_LEVELS(
		gpt::GPMF_Levels level)
	{
		switch (level)
		{
			case gpt::GPMF_Levels::GPMF_CURRENT_LEVEL:
				return GPMF_LEVELS::GPMF_CURRENT_LEVEL;
			case gpt::GPMF_Levels::GPMF_RECURSE_LEVELS:
				return GPMF_LEVELS::GPMF_RECURSE_LEVELS;
			case gpt::GPMF_Levels::GPMF_TOLERANT:
				return GPMF_LEVELS::GPMF_TOLERANT;
		}
		throw std::runtime_error("unsupported level conversion!");
	}

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

	FourCC::FourCC()
	 : fourCC_(::GPMF_KEY_END)
	{
	}

	FourCC::FourCC(
		FourCC_uint fourCC)
	 : fourCC_(fourCC)
	{
	}

	FourCC_uint
	FourCC::uint()
	{
		return fourCC_;
	}

	std::string
	FourCC::toString()
	{
		std::string strOut("    ");
		strOut[3] = (fourCC_ >> 24) & 0xFF;
		strOut[2] = (fourCC_ >> 16) & 0xFF;
		strOut[1] = (fourCC_ >> 8)  & 0xFF;
		strOut[0] = (fourCC_)       & 0xFF;
		return strOut;
	}

	GPMF_Stream::~GPMF_Stream()
	{
		auto gs = reinterpret_cast<GPMF_stream *>(stream_);
		GPMF_Free(gs);
		delete(gs);
		stream_ = nullptr;
	}

	bool
	GPMF_Stream::resetState()
	{
		return GPMF_OK == GPMF_ResetState(
			reinterpret_cast<GPMF_stream *>(stream_));
	}

	bool
	GPMF_Stream::validate(
		gpt::GPMF_Levels level)
	{
		return GPMF_OK == GPMF_Validate(
			reinterpret_cast<GPMF_stream *>(stream_),
			toGPMF_LEVELS(level));
	}

	bool
	GPMF_Stream::next(
		gpt::GPMF_Levels level)
	{
		return GPMF_OK == GPMF_Next(
			reinterpret_cast<GPMF_stream *>(stream_),
			toGPMF_LEVELS(level));
	}

	bool
	GPMF_Stream::findPrev(
		FourCC fourCC,
		gpt::GPMF_Levels level)
	{
		return GPMF_OK == GPMF_FindPrev(
			reinterpret_cast<GPMF_stream *>(stream_),
			fourCC.uint(),
			toGPMF_LEVELS(level));
	}

	bool
	GPMF_Stream::findNext(
		FourCC fourCC,
		gpt::GPMF_Levels level)
	{
		return GPMF_OK == GPMF_FindNext(
			reinterpret_cast<GPMF_stream *>(stream_),
			fourCC.uint(),
			toGPMF_LEVELS(level));
	}

	bool
	GPMF_Stream::seekToSamples()
	{
		return GPMF_OK == GPMF_SeekToSamples(
			reinterpret_cast<GPMF_stream *>(stream_));
	}

	FourCC
	GPMF_Stream::key()
	{
		return GPMF_Key(reinterpret_cast<GPMF_stream *>(stream_));
	}

	char
	GPMF_Stream::type()
	{
		return GPMF_Type(reinterpret_cast<GPMF_stream *>(stream_));
	}

	uint32_t
	GPMF_Stream::structSize()
	{
		return GPMF_StructSize(reinterpret_cast<GPMF_stream *>(stream_));
	}

	uint32_t
	GPMF_Stream::repeat()
	{
		return GPMF_Repeat(reinterpret_cast<GPMF_stream *>(stream_));
	}

	uint32_t
	GPMF_Stream::payloadSampleCount()
	{
		return GPMF_PayloadSampleCount(reinterpret_cast<GPMF_stream *>(stream_));
	}

	uint32_t
	GPMF_Stream::elementsInStruct()
	{
		return GPMF_ElementsInStruct(reinterpret_cast<GPMF_stream *>(stream_));
	}

	uint32_t
	GPMF_Stream::rawDataSize()
	{
		return GPMF_RawDataSize(reinterpret_cast<GPMF_stream *>(stream_));
	}

	void *
	GPMF_Stream::rawData()
	{
		return GPMF_RawData(reinterpret_cast<GPMF_stream *>(stream_));
	}

	GPMF_Stream::GPMF_Stream(
		uint32_t *buffer,
		uint32_t datasize)
	 : stream_(new GPMF_stream)
	{
		auto gs = reinterpret_cast<GPMF_stream *>(stream_);
		auto ret = GPMF_Init(gs, buffer, datasize);
		if (ret != GPMF_OK)
		{
			std::runtime_error("failed to init GPMF_stream from payload buffer");
		}
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