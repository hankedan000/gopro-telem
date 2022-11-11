#pragma once

#include <bitset>

namespace gpt
{
	const size_t SAMPLE_INFO_HAS_ACCL = 1;
	const size_t SAMPLE_INFO_HAS_GYRO = 2;
	const size_t SAMPLE_INFO_HAS_GPS  = 3;
	using SampleInfoBitSet = std::bitset<32>;

	struct FloatXYZ
	{
		float x,y,z;
	};

	struct CoordLL
	{
		float lat,lon;
	};

	struct Sample
	{
		// sample's time offset is seconds from beginning of data stream
		double t_offset;

		FloatXYZ accl;
		FloatXYZ gyro;
		CoordLL gps_coord;

		SampleInfoBitSet info;
	};

}