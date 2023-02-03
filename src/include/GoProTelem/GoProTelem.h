#pragma once

#include <vector>

#include "GoProTelem/FourCC.h"
#include "GoProTelem/GPMF_Payload.h"
#include "GoProTelem/GPMF_Stream.h"
#include "GoProTelem/MP4_Source.h"
#include "GoProTelem/SampleTypes.h"

// includes for std::filesystem
// std::filesystem wasn't fully supported until C++17.
// the experimental support that exists in C++14 is good enough for
// what we need, so fallback to that if possible.
#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace _gpt_fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace _gpt_fs = std::experimental::filesystem;
#  endif
#endif

namespace gpt
{

	// so we can use gpt::filesystem in all our code and toggle the underlying
	// std version that is used
	namespace filesystem = _gpt_fs;

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

	std::vector<AcclTimedSample>
	getAcclSamples(
		MP4_Source &mp4);

	std::vector<GyroTimedSample>
	getPayloadGyroSamples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz = -1);

	std::vector<GyroTimedSample>
	getGyroSamples(
		MP4_Source &mp4);

	std::vector<GravTimedSample>
	getPayloadGravSamples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz = -1);

	std::vector<GravTimedSample>
	getGravSamples(
		MP4_Source &mp4);

	std::vector<OrientationTimedSample>
	getPayloadCoriSamples(
		GPMF_PayloadPtr pl,
		double timeOffset_sec,
		double sampleRate_hz = -1);

	std::vector<OrientationTimedSample>
	getCoriSamples(
		MP4_Source &mp4);

	std::vector<CombinedSample>
	getCombinedSamples(
		MP4_Source &mp4);

	std::vector<CombinedTimedSample>
	getCombinedTimedSamples(
		MP4_Source &mp4);

}