#include "GoProTelem/GPMF_Stream.h"

#include <sstream>
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

	std::string
	GPMF_Stream::strmToString()
	{
		if (key() != gpt::GPMF_KEY_STREAM)
		{
			return "**NOT A STRM**";
		}

		std::stringstream ss;
		pushState();
		if (seekToSamples())
		{
			FourCC key = this->key();
			char type = this->type();
			uint32_t elements = elementsInStruct();
			uint32_t samples = payloadSampleCount();
			if (samples)
			{
				ss << "STRM of " << key.toString() << " of type " << type << " with " << samples << " samples";
				if (elements > 1)
				{
					ss << " -- " << elements << " elements per sample";
				}
			}
		}
		popState();

		return ss.str();
	}

	bool
	GPMF_Stream::resetState()
	{
		return GPMF_OK == GPMF_ResetState(
			reinterpret_cast<GPMF_stream *>(stream_));
	}

	void
	GPMF_Stream::pushState()
	{
		auto currState = reinterpret_cast<GPMF_stream *>(stream_);
		GPMF_stream *state = new GPMF_stream;
		if (GPMF_OK == GPMF_CopyState(currState,state))
		{
			stateStack_.push(state);
		}
		else
		{
			delete(state);
		}
	}

	void
	GPMF_Stream::popState()
	{
		if (stateStack_.empty())
		{
			return;
		}

		auto currState = reinterpret_cast<GPMF_stream *>(stream_);
		auto state = reinterpret_cast<GPMF_stream *>(stateStack_.top());
		stateStack_.pop();
		int copyRet = GPMF_CopyState(state,currState);
		delete(state);
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

	bool
	GPMF_Stream::getScaledDataDoubles(
		void *buffer,
		uint32_t buffersize,
		uint32_t sample_offset,
		uint32_t read_samples)
	{
		return GPMF_OK == GPMF_ScaledData(
			reinterpret_cast<GPMF_stream *>(stream_),
			buffer,
			buffersize,
			sample_offset,
			read_samples,
			GPMF_TYPE_DOUBLE);
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