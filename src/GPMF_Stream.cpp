#include "GoProTelem/GPMF_Stream.h"

#include <stdexcept>
#include "GPMF_parser.h"
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

}