#include "GoProTelem/GPMF_Stream.h"

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string.h>
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

	size_t
	GPMF_Stream::klvTotalSizePadded()
	{
		size_t totalSize = 0;
		totalSize += 4;// 4 bytes for key (FourCC)
		totalSize += 4;// 4 bytes for length

		size_t valueSize = structSize() * repeat();
		valueSize = std::ceil(valueSize / 4.0) * 4;
		totalSize += valueSize;
		return totalSize;
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

	enum Endianness
	{
		eEndianAuto,
		eEndianLittle,
		eEndianBig
	};

	template<typename INT_T>
	INT_T
	rawDataToInt(
		void *rawdata,
		Endianness endianness = Endianness::eEndianAuto)
	{
		if (endianness == Endianness::eEndianAuto)
		{
			uint32_t test = 0x1;
			endianness = (
				((uint8_t*)(&test))[0] == 0x1 ?
				Endianness::eEndianLittle :
				Endianness::eEndianBig );
		}

		INT_T outInt;
		if (endianness == Endianness::eEndianBig)
		{
			// rawdata is already stored in big endian, so just do regular case
			outInt = *reinterpret_cast<INT_T*>(rawdata);
		}
		else
		{
			for (size_t i=0; i<sizeof(INT_T); i++)
			{
				outInt = (outInt << 8) | reinterpret_cast<uint8_t*>(rawdata)[i];
			}
		}
		return outInt;
	}

	float
	rawDataToFloat(
		void *rawdata)
	{
		auto intValue = rawDataToInt<uint32_t>(rawdata);
		return *reinterpret_cast<float*>(&intValue);
	}

	double
	rawDataToDouble(
		void *rawdata)
	{
		auto intValue = rawDataToInt<uint64_t>(rawdata);
		return *reinterpret_cast<double*>(&intValue);
	}

	template <typename INT_T>
	std::string
	rawDataIntToString(
		void *rawdata)
	{
		std::stringstream ss;
		auto intData = rawDataToInt<INT_T>(rawdata);
		ss << "0x" << std::hex << intData << " (" << std::dec << intData << ")";
		return ss.str();
	}

	// specialize for uint8_t so we can print the number correctly; otherwise it would print as a char
	template <>
	std::string
	rawDataIntToString<uint8_t>(
		void *rawdata)
	{
		std::stringstream ss;
		unsigned int intData = rawDataToInt<uint8_t>(rawdata);
		ss << "0x" << std::hex << (intData & 0xFF) << " (" << std::dec << intData << ")";
		return ss.str();
	}

	// specialize for int8_t so we can print the number correctly; otherwise it would print as a char
	template <>
	std::string
	rawDataIntToString<int8_t>(
		void *rawdata)
	{
		std::stringstream ss;
		int intData = rawDataToInt<int8_t>(rawdata);
		ss << "0x" << std::hex << (intData & 0xFF) << " (" << std::dec << intData << ")";
		return ss.str();
	}

	std::string
	GPMF_Stream::klvToString()
	{
		std::stringstream ss;

		ss << key() << " ";
		char type = this->type();
		std::string typeStr(1,type);
		ss << (type == '\0' ? "null" : typeStr) << " ";
		ss << structSize() << " ";
		ss << repeat() << " ";

		switch (type)
		{
			case 'b':// single byte signed integer
				ss << rawDataIntToString<int8_t>(this->rawData());
				break;
			case 'B':// single byte unsigned integer
				ss << rawDataIntToString<uint8_t>(this->rawData());
				break;
			case 'c':// ASCII string
			{
				char tmpStr[256];
				size_t numChars = this->structSize();
				if (numChars >= (sizeof(tmpStr) - 1))
				{
					printf("string is larger than buffer! can't read in string.\n");
					break;
				}
				::memcpy((void*)tmpStr,this->rawData(),numChars);
				tmpStr[numChars] = '\0';// GPMF doesn't gaurantee strings are null-terminated
				ss << tmpStr;
				break;
			}
			case 'd':// 64-bit double precision (IEEE 754)
				ss << rawDataToDouble(this->rawData());
				break;
			case 'f':// 32-bit float (IEEE 754)
				ss << rawDataToFloat(this->rawData());
				break;
			case 'F':// 32-bit four character key -- FourCC
			{
				gpt::FourCC fourCC(rawDataToInt<uint32_t>(this->rawData(),Endianness::eEndianLittle));
				ss << fourCC.toString();
				break;
			}
			case 'j':// 64-bit signed integer
				ss << rawDataIntToString<int64_t>(this->rawData());
				break;
			case 'J':// 64-bit unsigned integer
				ss << rawDataIntToString<uint64_t>(this->rawData());
				break;
			case 'l':// 32-bit signed integer
				ss << rawDataIntToString<int32_t>(this->rawData());
				break;
			case 'L':// 32-bit unsigned integer
				ss << rawDataIntToString<uint32_t>(this->rawData());
				break;
			case 's':// 16-bit signed integer
				ss << rawDataIntToString<int16_t>(this->rawData());
				break;
			case 'S':// 16-bit unsigned integer
				ss << rawDataIntToString<uint16_t>(this->rawData());
				break;
			case 'U':// Date + UTC Time format yymmddhhmmss.sss - (years 20xx covered)
			{
				char utcdata[17];
				memcpy(utcdata,this->rawData(),16);
				utcdata[16] = '\0';
				ss << utcdata << " (yymmddhhmmss.sss)";
				break;
			}
		}

		return ss.str();
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