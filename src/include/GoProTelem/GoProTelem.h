#pragma once

#include <vector>

#include "GoProTelem/FourCC.h"
#include "GoProTelem/GPMF_Payload.h"
#include "GoProTelem/GPMF_Stream.h"
#include "GoProTelem/MP4_Source.h"
#include "GoProTelem/SampleTypes.h"

namespace gpt
{

	/**
	 * @param[in] pl
	 * the payload to extract samples from
	 * 
	 * @param[in] timeOffset_sec
	 * allows caller to offset the timestamps of the extracted samples.
	 * 
	 * @param[in] sampleRate_hz
	 * allows caller to provide external rate for the GPS samples.
	 * if negative, then sample rate will be computed based on the
	 * payload's duration and sample count.
	 */
	std::vector<GPS_TimedSample>
	getPayloadGPS_Samples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz = -1);

	std::vector<GPS_TimedSample>
	getGPS_Samples(
		MP4_Source &mp4);

	std::vector<AcclTimedSample>
	getPayloadAcclSamples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz = -1);

	std::vector<GyroTimedSample>
	getGyroSamples(
		MP4_Source &mp4);

	std::vector<GyroTimedSample>
	getPayloadGyroSamples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz = -1);

	std::vector<AcclTimedSample>
	getAcclSamples(
		MP4_Source &mp4);

	std::vector<CombinedSample>
	getCombinedSamples(
		MP4_Source &mp4);

}