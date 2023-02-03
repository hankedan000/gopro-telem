#pragma once

#include <bitset>

namespace gpt
{

	struct CoordLL
	{
		double lat,lon;// deg
	};

	std::string
	toString(
		const CoordLL &coord,
		bool withBraces = false);

	struct AcclSample
	{
		float x,y,z;// m/s^2
	};

	std::string
	toString(
		const AcclSample &sample,
		bool withBraces = false);

	struct AcclTimedSample
	{
		AcclSample sample;

		// time offset relative to video's start in seconds.
		// This value is based off the GPMF payload's in/out times.
		double t_offset;
	};

	std::string
	toString(
		const AcclTimedSample &sample,
		bool withBraces = false);

	struct GyroSample
	{
		float x,y,z;// rad/s
	};

	std::string
	toString(
		const GyroSample &sample,
		bool withBraces = false);

	struct GyroTimedSample
	{
		GyroSample sample;

		// time offset relative to video's start in seconds.
		// This value is based off the GPMF payload's in/out times.
		double t_offset;
	};

	std::string
	toString(
		const GyroTimedSample &sample,
		bool withBraces = false);

	struct GravSample
	{
		float x,y,z;// G-force
	};

	std::string
	toString(
		const GravSample &sample,
		bool withBraces = false);

	struct GravTimedSample
	{
		GravSample sample;

		// time offset relative to video's start in seconds.
		// This value is based off the GPMF payload's in/out times.
		double t_offset;
	};

	std::string
	toString(
		const GravTimedSample &sample,
		bool withBraces = false);

	struct OrientationSample
	{
		float w,x,y,z;
	};

	std::string
	toString(
		const OrientationSample &sample,
		bool withBraces = false);

	struct OrientationTimedSample
	{
		OrientationSample sample;

		// time offset relative to video's start in seconds.
		// This value is based off the GPMF payload's in/out times.
		double t_offset;
	};

	std::string
	toString(
		const OrientationTimedSample &sample,
		bool withBraces = false);

	struct GPS_Sample
	{
		CoordLL coord;
		double altitude;// m
		double speed2D; // m/s
		double speed3D; // m/s
	};

	std::string
	toString(
		const GPS_Sample &sample,
		bool withBraces = false);

	struct GPS_TimedSample
	{
		GPS_Sample sample;

		// time offset relative to video's start in seconds.
		// This value is based off the GPMF payload's in/out times.
		double t_offset;
	};

	std::string
	toString(
		const GPS_TimedSample &sample,
		bool withBraces = false);

	const size_t SAMPLE_INFO_HAS_ACCL = 1;
	const size_t SAMPLE_INFO_HAS_GYRO = 2;
	const size_t SAMPLE_INFO_HAS_GPS  = 3;
	using SampleInfoBitSet = std::bitset<32>;

	struct CombinedSample
	{
		AcclSample accl;
		GyroSample gyro;
		GravSample grav;
		OrientationSample cori;// camera orientation
		GPS_Sample gps;
	};

	std::string
	toString(
		const CombinedSample &sample,
		bool withBraces = false);

	struct CombinedTimedSample
	{
		CombinedSample sample;

		// time offset relative to video's start in seconds.
		// This value is based off the GPMF payload's in/out times.
		double t_offset;
	};

	std::string
	toString(
		const CombinedTimedSample &sample,
		bool withBraces = false);

}