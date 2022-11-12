#include "GoProTelem/GoProTelem.h"

#include <stdexcept>
#include "GPMF_parser.h"
#include "GPMF_mp4reader.h"
#include "GPMF_utils.h"

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

	std::vector<GPS_TimedSample>
	getPayloadGPS_Samples(
		GPMF_PayloadPtr pl)
	{
		std::vector<GPS_TimedSample> sampsOut;

		double inTime = pl->inTime();
		double outTime = pl->outTime();
		auto stream = pl->getStream();

		unsigned int sampIdx = 0;
		while (stream->findNext(gpt::GPMF_KEY_GPS5, gpt::GPMF_RECURSE_LEVELS))
		{
			char* rawdata = (char*)stream->rawData();
			FourCC key = stream->key();
			char type = stream->type();
			uint32_t samples = stream->repeat();
			uint32_t elements = stream->elementsInStruct();
			// printf("%d samples of type %c -- %d elements per samples\n", samples, type, elements);

			if (samples == 0)
			{
				continue;
			}
			if (elements != 5)
			{
				throw std::runtime_error(
					"invalid number of elements in GPS sample. expected 5. actual " + std::to_string(elements));
			}

			double samp_dt = (outTime - inTime) / samples;
			size_t buffersize = samples * elements * sizeof(double);
			double* ptr, * tmpbuffer = (double*)malloc(buffersize);

			if ( ! stream->getScaledDataDoubles(tmpbuffer,buffersize,0,samples))
			{
				printf("FAILED TO READ GPS samples!\n");
			}
			else
			{
				sampsOut.resize(sampsOut.size() + samples);
				for (unsigned int ss=0; ss<samples; ss++)
				{
					auto &sampOut = sampsOut.at(sampIdx);
					sampOut.t_offset  = inTime + samp_dt * sampIdx;
					sampOut.coord.lat = tmpbuffer[ss*elements+0];
					sampOut.coord.lon = tmpbuffer[ss*elements+1];
					sampOut.altitude  = tmpbuffer[ss*elements+2];
					sampOut.speed2D   = tmpbuffer[ss*elements+3];
					sampOut.speed3D   = tmpbuffer[ss*elements+4];
					sampIdx++;
				}
			}

			free(tmpbuffer);
		}

		return sampsOut;
	}

	std::vector<GPS_TimedSample>
	getGPS_Samples(
		MP4_Source &mp4)
	{
		std::vector<GPS_TimedSample> sampsOut;

		size_t payloadCount = mp4.payloadCount();
		for (size_t pIdx=0; pIdx<payloadCount; pIdx++)
		{
			// printf("pIdx = %ld\n", pIdx);
			auto payloadPtr = mp4.getPayload(pIdx);
			auto pSamps = getPayloadGPS_Samples(payloadPtr);
			sampsOut.insert(sampsOut.end(), pSamps.begin(), pSamps.end());
		}

		return sampsOut;
	}

}