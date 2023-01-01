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

		std::string
		toString(
			bool withBraces = false) const;
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
		// m/s^2

	public:
		std::string
		toString(
			bool withBraces = false) const;
	};

	class GyroSample : public FloatXYZ
	{
		// rad/s

	public:
		std::string
		toString(
			bool withBraces = false) const;
	};

	class GravSample : public FloatXYZ
	{
		// rad/s

	public:
		std::string
		toString(
			bool withBraces = false) const;
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

	class AcclTimedSample : public AcclSample, public TimedSample
	{
	public:
		std::string
		toString(
			bool withBraces = false) const;
	};

	class GyroTimedSample : public GyroSample, public TimedSample
	{
	public:
		std::string
		toString(
			bool withBraces = false) const;
	};

	class GravTimedSample : public GravSample, public TimedSample
	{
	public:
		std::string
		toString(
			bool withBraces = false) const;
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
	public:
		AcclSample accl;
		GyroSample gyro;
		GravSample grav;
		GPS_Sample gps;

		std::string
		toString(
			bool withBraces = false) const;
	};

}