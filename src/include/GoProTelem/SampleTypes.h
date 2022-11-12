#pragma once

#include <bitset>

namespace gpt
{

	class TimedSample
	{
	public:
		double t_offset;

		std::string
		toString(
			bool withBraces = false) const;
	};

	class FloatXYZ
	{
	public:
		float x,y,z;
	};

	class CoordLL
	{
	public:
		double lat,lon;// deg

		std::string
		toString(
			bool withBraces = false) const;
	};

	class AcclSample : public FloatXYZ
	{
	};

	class GyroSample : public FloatXYZ
	{
	};

	class GPS_Sample
	{
	public:
		CoordLL coord;
		double altitude;// m
		double speed2D; // m/s
		double speed3D; // m/s

		std::string
		toString(
			bool withBraces = false) const;
	};

	class AcclTimedSample : public FloatXYZ, public TimedSample
	{
	};

	class GyroTimedSample : public GyroSample, public TimedSample
	{
	};

	class GPS_TimedSample : public GPS_Sample, public TimedSample
	{
	public:
		std::string
		toString(
			bool withBraces = false) const;
	};

	const size_t SAMPLE_INFO_HAS_ACCL = 1;
	const size_t SAMPLE_INFO_HAS_GYRO = 2;
	const size_t SAMPLE_INFO_HAS_GPS  = 3;
	using SampleInfoBitSet = std::bitset<32>;

	class CombinedSample : public TimedSample
	{
		AcclSample accl;
		GyroSample gyro;
		GPS_Sample gps;
	};

}