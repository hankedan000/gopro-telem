#include "GoProTelem/GoProTelem.h"

#include <stdexcept>
#include "GPMF_parser.h"
#include "GPMF_mp4reader.h"
#include "GPMF_utils.h"
#include "GoProTelem/SampleMath.h"

namespace gpt
{

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

	std::vector<AcclTimedSample>
	getPayloadAcclSamples(
		GPMF_PayloadPtr pl)
	{
		std::vector<AcclTimedSample> sampsOut;

		double inTime = pl->inTime();
		double outTime = pl->outTime();
		auto stream = pl->getStream();

		unsigned int sampIdx = 0;
		while (stream->findNext(gpt::GPMF_KEY_ACCL, gpt::GPMF_RECURSE_LEVELS))
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
			if (elements != 3)
			{
				throw std::runtime_error(
					"invalid number of elements in ACCL sample. expected 3. actual " + std::to_string(elements));
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
					sampOut.y = tmpbuffer[ss*elements+0];
					sampOut.x = tmpbuffer[ss*elements+1];// GoPro docs says this is -y
					sampOut.z = tmpbuffer[ss*elements+2];
					sampIdx++;
				}
			}

			free(tmpbuffer);
		}

		return sampsOut;
	}

	std::vector<AcclTimedSample>
	getAcclSamples(
		MP4_Source &mp4)
	{
		std::vector<AcclTimedSample> sampsOut;

		size_t payloadCount = mp4.payloadCount();
		for (size_t pIdx=0; pIdx<payloadCount; pIdx++)
		{
			// printf("pIdx = %ld\n", pIdx);
			auto payloadPtr = mp4.getPayload(pIdx);
			auto pSamps = getPayloadAcclSamples(payloadPtr);
			sampsOut.insert(sampsOut.end(), pSamps.begin(), pSamps.end());
		}

		return sampsOut;
	}

	std::vector<CombinedSample>
	getCombinedSamples(
		MP4_Source &mp4)
	{
		const size_t frameCount = mp4.frameCount();
		const double duration = mp4.duration();
		if (frameCount <= 1)
		{
			throw std::runtime_error(
				"not enough frames to combine samples with. frameCount = " + std::to_string(frameCount));
		}
		const double frameDt = duration / (frameCount - 1);

		std::vector<CombinedSample> sampsOut;
		sampsOut.resize(frameCount);

		const auto gpsSamps = getGPS_Samples(mp4);
		const auto acclSamps = getAcclSamples(mp4);

		size_t gpsIdx = 0;
		size_t acclIdx = 0;
		for (size_t ff=0; ff<frameCount; ff++)
		{
			auto &sampOut = sampsOut.at(ff);
			sampOut.t_offset = ff * frameDt;

			// GPS sample interpolation
			{
				// find next two samples to interpolate between
				bool gpsFound = false;
				while (true)
				{
					auto nextIdx = gpsIdx + 1;
					if (nextIdx >= (gpsSamps.size() - 1))
					{
						break;
					}

					if (gpsSamps.at(gpsIdx).t_offset <= sampOut.t_offset &&
						sampOut.t_offset <= gpsSamps.at(nextIdx).t_offset)
					{
						gpsFound = true;
						break;
					}
					gpsIdx++;
				}

				// perform interpolation
				auto &sampA = gpsSamps.at(gpsIdx);
				auto &sampB = gpsSamps.at(gpsIdx+1);
				if (gpsFound)
				{
					lerpTimedSample(sampOut.gps, sampA, sampB, sampOut.t_offset);
				}
				else if (gpsIdx == 0)
				{
					sampOut.gps = sampA;
				}
				else
				{
					sampOut.gps = sampB;
				}
			}

			// accelerometer sample interpolation
			{
				// find next two samples to interpolate between
				bool acclFound = false;
				while (true)
				{
					auto nextIdx = acclIdx + 1;
					if (nextIdx >= (acclSamps.size() - 1))
					{
						break;
					}

					if (acclSamps.at(acclIdx).t_offset <= sampOut.t_offset &&
						sampOut.t_offset <= acclSamps.at(nextIdx).t_offset)
					{
						acclFound = true;
						break;
					}
					acclIdx++;
				}

				// perform interpolation
				auto &sampA = acclSamps.at(acclIdx);
				auto &sampB = acclSamps.at(acclIdx+1);
				if (acclFound)
				{
					lerpTimedSample(sampOut.accl, sampA, sampB, sampOut.t_offset);
				}
				else if (acclIdx == 0)
				{
					sampOut.accl = sampA;
				}
				else
				{
					sampOut.accl = sampB;
				}
			}
		}

		return sampsOut;
	}

}