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

		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
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
		double *tmpbuffer = (double*)malloc(buffersize);
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
				sampOut.sample.coord.lat = tmpbuffer[ss*elements+0];
				sampOut.sample.coord.lon = tmpbuffer[ss*elements+1];
				sampOut.sample.altitude  = tmpbuffer[ss*elements+2];
				sampOut.sample.speed2D   = tmpbuffer[ss*elements+3];
				sampOut.sample.speed3D   = tmpbuffer[ss*elements+4];
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

		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
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
		double *tmpbuffer = (double*)malloc(buffersize);
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
				sampOut.t_offset = timeOffset_sec + samp_dt * ss;
				// Hero 9 data order determined imperically
				sampOut.sample.x = tmpbuffer[ss*elements+1] * -1;
				sampOut.sample.y = tmpbuffer[ss*elements+2] * -1;
				sampOut.sample.z = tmpbuffer[ss*elements+0] * -1;
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

		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
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
		double *tmpbuffer = (double*)malloc(buffersize);
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
				sampOut.sample.x = tmpbuffer[ss*elements+1];
				sampOut.sample.y = tmpbuffer[ss*elements+2];
				sampOut.sample.z = tmpbuffer[ss*elements+0];
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

		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
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
		double *tmpbuffer = (double*)malloc(buffersize);
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
				sampOut.sample.x = tmpbuffer[ss*elements+0] * -1;
				sampOut.sample.y = tmpbuffer[ss*elements+2] * -1;
				sampOut.sample.z = tmpbuffer[ss*elements+1] * -1;
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

		uint32_t samples = stream->repeat();
		uint32_t elements = stream->elementsInStruct();
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
		double *tmpbuffer = (double*)malloc(buffersize);
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
				// Hero 9 data order determined imperically
				sampOut.sample.w = tmpbuffer[ss*elements+0];
				sampOut.sample.x = tmpbuffer[ss*elements+1];
				sampOut.sample.y = tmpbuffer[ss*elements+3];
				sampOut.sample.z = tmpbuffer[ss*elements+2];
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
			double outTime = ff * frameDt;

			// GPS sample interpolation
			{
				// find next two samples to interpolate between
				bool gpsFound = findLerpIndex(gpsIdx,gpsSamps,outTime);

				// perform interpolation
				if (gpsFound)
				{
					auto &sampA = gpsSamps.at(gpsIdx);
					auto &sampB = gpsSamps.at(gpsIdx+1);
					lerpTimedSample(sampOut.gps, sampA, sampB, outTime);
				}
				else if (gpsIdx == 0)
				{
					sampOut.gps = gpsSamps.at(gpsIdx).sample;
				}
				else
				{
					sampOut.gps = gpsSamps.at(gpsSamps.size() - 1).sample;
				}
			}

			// accelerometer sample interpolation
			{
				// find next two samples to interpolate between
				bool acclFound = findLerpIndex(acclIdx,acclSamps,outTime);

				// perform interpolation
				if (acclFound)
				{
					auto &sampA = acclSamps.at(acclIdx);
					auto &sampB = acclSamps.at(acclIdx+1);
					lerpTimedSample(sampOut.accl, sampA, sampB, outTime);
				}
				else if (acclIdx == 0)
				{
					sampOut.accl = acclSamps.at(acclIdx).sample;
				}
				else
				{
					sampOut.accl = acclSamps.at(acclSamps.size() - 1).sample;
				}
			}

			// gyro sample interpolation
			{
				// find next two samples to interpolate between
				bool gyroFound = findLerpIndex(gyroIdx,gyroSamps,outTime);

				// perform interpolation
				if (gyroFound)
				{
					auto &sampA = gyroSamps.at(gyroIdx);
					auto &sampB = gyroSamps.at(gyroIdx+1);
					lerpTimedSample(sampOut.gyro, sampA, sampB, outTime);
				}
				else if (gyroIdx == 0)
				{
					sampOut.gyro = gyroSamps.at(gyroIdx).sample;
				}
				else
				{
					sampOut.gyro = gyroSamps.at(gyroSamps.size() - 1).sample;
				}
			}

			// grav sample interpolation
			{
				// find next two samples to interpolate between
				bool gravFound = findLerpIndex(gravIdx,gravSamps,outTime);

				// perform interpolation
				if (gravFound)
				{
					auto &sampA = gravSamps.at(gravIdx);
					auto &sampB = gravSamps.at(gravIdx+1);
					lerpTimedSample(sampOut.grav, sampA, sampB, outTime);
				}
				else if (gravIdx == 0)
				{
					sampOut.grav = gravSamps.at(gravIdx).sample;
				}
				else
				{
					sampOut.grav = gravSamps.at(gravSamps.size() - 1).sample;
				}
			}

			// camera orientation sample interpolation
			{
				// find next two samples to interpolate between
				bool coriFound = findLerpIndex(coriIdx,coriSamps,outTime);

				// perform interpolation
				if (coriFound)
				{
					auto &sampA = coriSamps.at(coriIdx);
					auto &sampB = coriSamps.at(coriIdx+1);
					lerpTimedSample(sampOut.cori, sampA, sampB, outTime);
				}
				else if (coriIdx == 0)
				{
					sampOut.cori = coriSamps.at(coriIdx).sample;
				}
				else
				{
					sampOut.cori = coriSamps.at(coriSamps.size() - 1).sample;
				}
			}
		}

		return sampsOut;
	}

	std::vector<CombinedTimedSample>
	getCombinedTimedSamples(
		MP4_Source &mp4)
	{
		const size_t frameCount = mp4.frameCount();
		if (frameCount <= 1)
		{
			throw std::runtime_error(
				"not enough frames to combine samples with. frameCount = " + std::to_string(frameCount));
		}
		const double frameDt = 1.0 / mp4.fps();

		auto combinedSamples = getCombinedSamples(mp4);
		std::vector<CombinedTimedSample> sampsOut;
		sampsOut.resize(combinedSamples.size());
		for (size_t i=0; i<combinedSamples.size(); i++)
		{
			auto &sampOut = sampsOut.at(i);
			sampOut.t_offset = frameDt * i;
			sampOut.sample = combinedSamples.at(i);
		}
		return sampsOut;
	}

}