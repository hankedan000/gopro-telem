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
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz)
	{
		std::vector<GPS_TimedSample> sampsOut;

		auto stream = pl->getStream();
		if ( ! stream->findNext(gpt::GPMF_KEY_GPS5, gpt::GPMF_RECURSE_LEVELS))
		{
			// payload doesn't contain any GPS samples
			return sampsOut;
		}

		char* rawdata = (char*)stream->rawData();
		FourCC key = stream->key();
		char type = stream->type();
		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
		// printf("%d samples of type %c -- %d elements per samples\n", samples, type, elements);

		if (samples == 0)
		{
			return sampsOut;
		}
		if (elements != 5)
		{
			throw std::runtime_error(
				"invalid number of elements in GPS sample. expected 5. actual " + std::to_string(elements));
		}

		double samp_dt = 0.0;
		if (sampleRate_hz > 0.0)
		{
			samp_dt = 1.0 / sampleRate_hz;
		}
		else if (sampleRate_hz < 0.0)
		{
			// negative rate means compute based on payload duration & sample count
			samp_dt = (pl->outTime() - pl->inTime()) / samples;
		}

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
				auto &sampOut = sampsOut.at(ss);
				sampOut.t_offset  = timeOffset_sec + samp_dt * ss;
				sampOut.coord.lat = tmpbuffer[ss*elements+0];
				sampOut.coord.lon = tmpbuffer[ss*elements+1];
				sampOut.altitude  = tmpbuffer[ss*elements+2];
				sampOut.speed2D   = tmpbuffer[ss*elements+3];
				sampOut.speed3D   = tmpbuffer[ss*elements+4];
			}
		}

		free(tmpbuffer);

		return sampsOut;
	}

	std::vector<GPS_TimedSample>
	getGPS_Samples(
		MP4_Source &mp4)
	{
		std::vector<GPS_TimedSample> sampsOut;

		MP4_SensorInfo sensorInfo;
		if ( ! mp4.getSensorInfo(GPMF_KEY_GPS5, sensorInfo))
		{
			return sampsOut;
		}

		double sampleRate_hz = sensorInfo.measuredRate_hz;
		double samp_dt = 1.0 / sampleRate_hz;
		double timeOffset_sec = 0.0;
		size_t payloadCount = mp4.payloadCount();
		for (size_t pIdx=0; pIdx<payloadCount; pIdx++)
		{
			// printf("pIdx = %ld\n", pIdx);
			auto payloadPtr = mp4.getPayload(pIdx);
			auto pSamps = getPayloadGPS_Samples(payloadPtr,timeOffset_sec,sampleRate_hz);
			sampsOut.insert(sampsOut.end(), pSamps.begin(), pSamps.end());
			if (pSamps.size() > 0)
			{
				timeOffset_sec = pSamps.back().t_offset + samp_dt;
			}
		}

		return sampsOut;
	}

	std::vector<AcclTimedSample>
	getPayloadAcclSamples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz)
	{
		std::vector<AcclTimedSample> sampsOut;

		auto stream = pl->getStream();
		if ( ! stream->findNext(gpt::GPMF_KEY_ACCL, gpt::GPMF_RECURSE_LEVELS))
		{
			// payload doesn't contain any ACCL samples
			return sampsOut;
		}

		char* rawdata = (char*)stream->rawData();
		FourCC key = stream->key();
		char type = stream->type();
		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
		// printf("%d samples of type %c -- %d elements per samples\n", samples, type, elements);

		if (samples == 0)
		{
			return sampsOut;
		}
		if (elements != 3)
		{
			throw std::runtime_error(
				"invalid number of elements in ACCL sample. expected 3. actual " + std::to_string(elements));
		}

		double samp_dt = 0.0;
		if (sampleRate_hz > 0.0)
		{
			samp_dt = 1.0 / sampleRate_hz;
		}
		else if (sampleRate_hz < 0.0)
		{
			// negative rate means compute based on payload duration & sample count
			samp_dt = (pl->outTime() - pl->inTime()) / samples;
		}

		size_t buffersize = samples * elements * sizeof(double);
		double* ptr, * tmpbuffer = (double*)malloc(buffersize);
		if ( ! stream->getScaledDataDoubles(tmpbuffer,buffersize,0,samples))
		{
			printf("Failed to read accl samples!\n");
		}
		else
		{
			sampsOut.resize(sampsOut.size() + samples);
			for (unsigned int ss=0; ss<samples; ss++)
			{
				auto &sampOut = sampsOut.at(ss);
				sampOut.t_offset  = timeOffset_sec + samp_dt * ss;
				// Hero 9 data order determined imperically
				sampOut.x = tmpbuffer[ss*elements+1] * -1;
				sampOut.y = tmpbuffer[ss*elements+2] * -1;
				sampOut.z = tmpbuffer[ss*elements+0] * -1;
			}
		}

		free(tmpbuffer);

		return sampsOut;
	}

	std::vector<AcclTimedSample>
	getAcclSamples(
		MP4_Source &mp4)
	{
		std::vector<AcclTimedSample> sampsOut;

		MP4_SensorInfo sensorInfo;
		if ( ! mp4.getSensorInfo(GPMF_KEY_ACCL, sensorInfo))
		{
			return sampsOut;
		}

		double sampleRate_hz = sensorInfo.measuredRate_hz;
		double samp_dt = 1.0 / sampleRate_hz;
		double timeOffset_sec = 0.0;
		size_t payloadCount = mp4.payloadCount();
		for (size_t pIdx=0; pIdx<payloadCount; pIdx++)
		{
			// printf("pIdx = %ld\n", pIdx);
			auto payloadPtr = mp4.getPayload(pIdx);
			auto pSamps = getPayloadAcclSamples(payloadPtr,timeOffset_sec,sampleRate_hz);
			sampsOut.insert(sampsOut.end(), pSamps.begin(), pSamps.end());
			if (pSamps.size() > 0)
			{
				timeOffset_sec = pSamps.back().t_offset + samp_dt;
			}
		}

		return sampsOut;
	}

	std::vector<GyroTimedSample>
	getPayloadGyroSamples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz)
	{
		std::vector<GyroTimedSample> sampsOut;

		auto stream = pl->getStream();
		if ( ! stream->findNext(gpt::GPMF_KEY_GYRO, gpt::GPMF_RECURSE_LEVELS))
		{
			// payload doesn't contain any GYRO samples
			return sampsOut;
		}

		char* rawdata = (char*)stream->rawData();
		FourCC key = stream->key();
		char type = stream->type();
		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
		// printf("%d samples of type %c -- %d elements per samples\n", samples, type, elements);

		if (samples == 0)
		{
			return sampsOut;
		}
		if (elements != 3)
		{
			throw std::runtime_error(
				"invalid number of elements in GYRO sample. expected 3. actual " + std::to_string(elements));
		}

		double samp_dt = 0.0;
		if (sampleRate_hz > 0.0)
		{
			samp_dt = 1.0 / sampleRate_hz;
		}
		else if (sampleRate_hz < 0.0)
		{
			// negative rate means compute based on payload duration & sample count
			samp_dt = (pl->outTime() - pl->inTime()) / samples;
		}

		size_t buffersize = samples * elements * sizeof(double);
		double* ptr, * tmpbuffer = (double*)malloc(buffersize);
		if ( ! stream->getScaledDataDoubles(tmpbuffer,buffersize,0,samples))
		{
			printf("Failed to read gyro samples!\n");
		}
		else
		{
			sampsOut.resize(sampsOut.size() + samples);
			for (unsigned int ss=0; ss<samples; ss++)
			{
				auto &sampOut = sampsOut.at(ss);
				sampOut.t_offset  = timeOffset_sec + samp_dt * ss;
				// Hero 9 data order determined imperically
				sampOut.x = tmpbuffer[ss*elements+1];
				sampOut.y = tmpbuffer[ss*elements+2];
				sampOut.z = tmpbuffer[ss*elements+0];
			}
		}

		free(tmpbuffer);

		return sampsOut;
	}

	std::vector<GyroTimedSample>
	getGyroSamples(
		MP4_Source &mp4)
	{
		std::vector<GyroTimedSample> sampsOut;

		MP4_SensorInfo sensorInfo;
		if ( ! mp4.getSensorInfo(GPMF_KEY_GYRO, sensorInfo))
		{
			return sampsOut;
		}

		double sampleRate_hz = sensorInfo.measuredRate_hz;
		double samp_dt = 1.0 / sampleRate_hz;
		double timeOffset_sec = 0.0;
		size_t payloadCount = mp4.payloadCount();
		for (size_t pIdx=0; pIdx<payloadCount; pIdx++)
		{
			// printf("pIdx = %ld\n", pIdx);
			auto payloadPtr = mp4.getPayload(pIdx);
			auto pSamps = getPayloadGyroSamples(payloadPtr,timeOffset_sec,sampleRate_hz);
			sampsOut.insert(sampsOut.end(), pSamps.begin(), pSamps.end());
			if (pSamps.size() > 0)
			{
				timeOffset_sec = pSamps.back().t_offset + samp_dt;
			}
		}

		return sampsOut;
	}

	std::vector<GravTimedSample>
	getPayloadGravSamples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz)
	{
		std::vector<GravTimedSample> sampsOut;

		auto stream = pl->getStream();
		if ( ! stream->findNext(gpt::GPMF_KEY_GRAV, gpt::GPMF_RECURSE_LEVELS))
		{
			// payload doesn't contain any GRAV samples
			return sampsOut;
		}

		char* rawdata = (char*)stream->rawData();
		FourCC key = stream->key();
		char type = stream->type();
		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
		// printf("%d samples of type %c -- %d elements per samples\n", samples, type, elements);

		if (samples == 0)
		{
			return sampsOut;
		}
		if (elements != 3)
		{
			throw std::runtime_error(
				"invalid number of elements in GRAV sample. expected 3. actual " + std::to_string(elements));
		}

		double samp_dt = 0.0;
		if (sampleRate_hz > 0.0)
		{
			samp_dt = 1.0 / sampleRate_hz;
		}
		else if (sampleRate_hz < 0.0)
		{
			// negative rate means compute based on payload duration & sample count
			samp_dt = (pl->outTime() - pl->inTime()) / samples;
		}

		size_t buffersize = samples * elements * sizeof(double);
		double* ptr, * tmpbuffer = (double*)malloc(buffersize);
		if ( ! stream->getScaledDataDoubles(tmpbuffer,buffersize,0,samples))
		{
			printf("Failed to read gravity samples!\n");
		}
		else
		{
			sampsOut.resize(sampsOut.size() + samples);
			for (unsigned int ss=0; ss<samples; ss++)
			{
				auto &sampOut = sampsOut.at(ss);
				sampOut.t_offset  = timeOffset_sec + samp_dt * ss;
				// Hero 9 data order determined imperically
				sampOut.x = tmpbuffer[ss*elements+0] * -1;
				sampOut.y = tmpbuffer[ss*elements+2] * -1;
				sampOut.z = tmpbuffer[ss*elements+1] * -1;
			}
		}

		free(tmpbuffer);

		return sampsOut;
	}

	std::vector<GravTimedSample>
	getGravSamples(
		MP4_Source &mp4)
	{
		std::vector<GravTimedSample> sampsOut;

		MP4_SensorInfo sensorInfo;
		if ( ! mp4.getSensorInfo(GPMF_KEY_GRAV, sensorInfo))
		{
			return sampsOut;
		}

		double sampleRate_hz = sensorInfo.measuredRate_hz;
		double samp_dt = 1.0 / sampleRate_hz;
		double timeOffset_sec = 0.0;
		size_t payloadCount = mp4.payloadCount();
		for (size_t pIdx=0; pIdx<payloadCount; pIdx++)
		{
			// printf("pIdx = %ld\n", pIdx);
			auto payloadPtr = mp4.getPayload(pIdx);
			auto pSamps = getPayloadGravSamples(payloadPtr,timeOffset_sec,sampleRate_hz);
			sampsOut.insert(sampsOut.end(), pSamps.begin(), pSamps.end());
			if (pSamps.size() > 0)
			{
				timeOffset_sec = pSamps.back().t_offset + samp_dt;
			}
		}

		return sampsOut;
	}

	std::vector<OrientationTimedSample>
	getPayloadCoriSamples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz)
	{
		std::vector<OrientationTimedSample> sampsOut;

		auto stream = pl->getStream();
		if ( ! stream->findNext(gpt::GPMF_KEY_CORI, gpt::GPMF_RECURSE_LEVELS))
		{
			// payload doesn't contain any GRAV samples
			return sampsOut;
		}

		char* rawdata = (char*)stream->rawData();
		FourCC key = stream->key();
		char type = stream->type();
		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
		// printf("%d samples of type %c -- %d elements per samples\n", samples, type, elements);

		if (samples == 0)
		{
			return sampsOut;
		}
		if (elements != 4)
		{
			throw std::runtime_error(
				"invalid number of elements in CORI sample. expected 4. actual " + std::to_string(elements));
		}

		double samp_dt = 0.0;
		if (sampleRate_hz > 0.0)
		{
			samp_dt = 1.0 / sampleRate_hz;
		}
		else if (sampleRate_hz < 0.0)
		{
			// negative rate means compute based on payload duration & sample count
			samp_dt = (pl->outTime() - pl->inTime()) / samples;
		}

		size_t buffersize = samples * elements * sizeof(double);
		double* ptr, * tmpbuffer = (double*)malloc(buffersize);
		if ( ! stream->getScaledDataDoubles(tmpbuffer,buffersize,0,samples))
		{
			printf("Failed to read camera orientation samples!\n");
		}
		else
		{
			sampsOut.resize(sampsOut.size() + samples);
			for (unsigned int ss=0; ss<samples; ss++)
			{
				auto &sampOut = sampsOut.at(ss);
				sampOut.t_offset  = timeOffset_sec + samp_dt * ss;
				// data order is just a guess for now...
				sampOut.x = tmpbuffer[ss*elements+0];
				sampOut.y = tmpbuffer[ss*elements+1];
				sampOut.z = tmpbuffer[ss*elements+2];
				sampOut.w = tmpbuffer[ss*elements+3];
			}
		}

		free(tmpbuffer);

		return sampsOut;
	}

	std::vector<OrientationTimedSample>
	getCoriSamples(
		MP4_Source &mp4)
	{
		std::vector<OrientationTimedSample> sampsOut;

		MP4_SensorInfo sensorInfo;
		if ( ! mp4.getSensorInfo(GPMF_KEY_CORI, sensorInfo))
		{
			return sampsOut;
		}

		double sampleRate_hz = sensorInfo.measuredRate_hz;
		double samp_dt = 1.0 / sampleRate_hz;
		double timeOffset_sec = 0.0;
		size_t payloadCount = mp4.payloadCount();
		for (size_t pIdx=0; pIdx<payloadCount; pIdx++)
		{
			// printf("pIdx = %ld\n", pIdx);
			auto payloadPtr = mp4.getPayload(pIdx);
			auto pSamps = getPayloadCoriSamples(payloadPtr,timeOffset_sec,sampleRate_hz);
			sampsOut.insert(sampsOut.end(), pSamps.begin(), pSamps.end());
			if (pSamps.size() > 0)
			{
				timeOffset_sec = pSamps.back().t_offset + samp_dt;
			}
		}

		return sampsOut;
	}

	std::vector<CombinedSample>
	getCombinedSamples(
		MP4_Source &mp4)
	{
		const size_t frameCount = mp4.frameCount();
		if (frameCount <= 1)
		{
			throw std::runtime_error(
				"not enough frames to combine samples with. frameCount = " + std::to_string(frameCount));
		}
		const double frameDt = 1.0 / mp4.fps();

		std::vector<CombinedSample> sampsOut;
		sampsOut.resize(frameCount);

		const auto gpsSamps = getGPS_Samples(mp4);
		const auto acclSamps = getAcclSamples(mp4);
		const auto gyroSamps = getGyroSamples(mp4);
		const auto gravSamps = getGravSamples(mp4);
		const auto coriSamps = getCoriSamples(mp4);

		size_t gpsIdx = 0;
		size_t acclIdx = 0;
		size_t gyroIdx = 0;
		size_t gravIdx = 0;
		size_t coriIdx = 0;
		for (size_t ff=0; ff<frameCount; ff++)
		{
			auto &sampOut = sampsOut.at(ff);
			sampOut.t_offset = ff * frameDt;

			// GPS sample interpolation
			{
				// find next two samples to interpolate between
				bool gpsFound = findLerpIndex(gpsIdx,gpsSamps,sampOut.t_offset);

				// perform interpolation
				if (gpsFound)
				{
					auto &sampA = gpsSamps.at(gpsIdx);
					auto &sampB = gpsSamps.at(gpsIdx+1);
					lerpTimedSample(sampOut.gps, sampA, sampB, sampOut.t_offset);
				}
				else if (gpsIdx == 0)
				{
					sampOut.gps = gpsSamps.at(gpsIdx);
				}
				else
				{
					sampOut.gps = gpsSamps.at(gpsSamps.size() - 1);
				}
			}

			// accelerometer sample interpolation
			{
				// find next two samples to interpolate between
				bool acclFound = findLerpIndex(acclIdx,acclSamps,sampOut.t_offset);

				// perform interpolation
				if (acclFound)
				{
					auto &sampA = acclSamps.at(acclIdx);
					auto &sampB = acclSamps.at(acclIdx+1);
					lerpTimedSample(sampOut.accl, sampA, sampB, sampOut.t_offset);
				}
				else if (acclIdx == 0)
				{
					sampOut.accl = acclSamps.at(acclIdx);
				}
				else
				{
					sampOut.accl = acclSamps.at(acclSamps.size() - 1);
				}
			}

			// gyro sample interpolation
			{
				// find next two samples to interpolate between
				bool gyroFound = findLerpIndex(gyroIdx,gyroSamps,sampOut.t_offset);

				// perform interpolation
				if (gyroFound)
				{
					auto &sampA = gyroSamps.at(gyroIdx);
					auto &sampB = gyroSamps.at(gyroIdx+1);
					lerpTimedSample(sampOut.gyro, sampA, sampB, sampOut.t_offset);
				}
				else if (gyroIdx == 0)
				{
					sampOut.gyro = gyroSamps.at(gyroIdx);
				}
				else
				{
					sampOut.gyro = gyroSamps.at(gyroSamps.size() - 1);
				}
			}

			// grav sample interpolation
			{
				// find next two samples to interpolate between
				bool gravFound = findLerpIndex(gravIdx,gravSamps,sampOut.t_offset);

				// perform interpolation
				if (gravFound)
				{
					auto &sampA = gravSamps.at(gravIdx);
					auto &sampB = gravSamps.at(gravIdx+1);
					lerpTimedSample(sampOut.grav, sampA, sampB, sampOut.t_offset);
				}
				else if (gravIdx == 0)
				{
					sampOut.grav = gravSamps.at(gravIdx);
				}
				else
				{
					sampOut.grav = gravSamps.at(gravSamps.size() - 1);
				}
			}

			// camera orientation sample interpolation
			{
				// find next two samples to interpolate between
				bool coriFound = findLerpIndex(coriIdx,coriSamps,sampOut.t_offset);

				// perform interpolation
				if (coriFound)
				{
					auto &sampA = coriSamps.at(coriIdx);
					auto &sampB = coriSamps.at(coriIdx+1);
					lerpTimedSample(sampOut.cori, sampA, sampB, sampOut.t_offset);
				}
				else if (coriIdx == 0)
				{
					sampOut.cori = coriSamps.at(coriIdx);
				}
				else
				{
					sampOut.cori = coriSamps.at(coriSamps.size() - 1);
				}
			}
		}

		return sampsOut;
	}

}