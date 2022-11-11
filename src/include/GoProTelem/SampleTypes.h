#pragma once

#include <bitset>

namespace gpt
{

	class TimedSample
	{
		double t_offset;
	};

	class FloatXYZ
	{
		float x,y,z;
	};

	class CoordLL
	{
		float lat,lon;
	};

	class AcclSample : public TimedSample, FloatXYZ
	{
	};

	class GyroSample : public TimedSample, FloatXYZ
	{
	};

	class GPS_Sample : public TimedSample, CoordLL
	{
	};

	const size_t SAMPLE_INFO_HAS_ACCL = 1;
	const size_t SAMPLE_INFO_HAS_GYRO = 2;
	const size_t SAMPLE_INFO_HAS_GPS  = 3;
	using SampleInfoBitSet = std::bitset<32>;

	class CombinedSample : public TimedSample
	{
		FloatXYZ accl;
		FloatXYZ gyro;
		CoordLL gps_coord;
	};

}