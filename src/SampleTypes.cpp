#include "GoProTelem/SampleTypes.h"

#include <iomanip>
#include <sstream>

namespace gpt
{

	std::string
	TimedSample::toString(
		bool withBraces) const
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) <<
			"t_offset: " << t_offset <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	FloatXYZ::toString(
			bool withBraces) const
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) << std::showpos <<
			"x: " << x << ", "
			"y: " << y << ", "
			"z: " << z <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	CoordLL::toString(
			bool withBraces) const
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			"lat: " << std::fixed << std::setprecision(6) << lat << ", "
			"lon: " << lon <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	AcclSample::toString(
			bool withBraces) const
	{
		return FloatXYZ::toString(withBraces);
	}

	std::string
	GyroSample::toString(
			bool withBraces) const
	{
		return FloatXYZ::toString(withBraces);
	}

	std::string
	GravSample::toString(
			bool withBraces) const
	{
		return FloatXYZ::toString(withBraces);
	}

	std::string
	GPS_Sample::toString(
			bool withBraces) const
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			"coord: " << coord.toString(true) << ", " <<
			std::fixed << std::setprecision(3) <<
			"altitude: " << altitude << ", " <<
			"speed2D: " << speed2D << ", " <<
			"speed3D: " << speed3D <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	GPS_TimedSample::toString(
			bool withBraces) const
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			TimedSample::toString() << ", " <<
			GPS_Sample::toString() <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	AcclTimedSample::toString(
			bool withBraces) const
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			TimedSample::toString() << ", " <<
			AcclSample::toString() <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	GyroTimedSample::toString(
			bool withBraces) const
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			TimedSample::toString() << ", " <<
			GyroSample::toString() <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	GravTimedSample::toString(
			bool withBraces) const
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			TimedSample::toString() << ", " <<
			GravSample::toString() <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	CombinedSample::toString(
		bool withBraces) const
	{
		std::stringstream ss;
		ss << (withBraces ? "{ \n" : "") <<
			(withBraces ? "  " : "") << TimedSample::toString() << ", " << std::endl <<
			(withBraces ? "  " : "") << "accl: " << accl.toString(true) << std::endl <<
			(withBraces ? "  " : "") << "gyro: " << gyro.toString(true) << std::endl <<
			(withBraces ? "  " : "") << "grav: " << grav.toString(true) << std::endl <<
			(withBraces ? "  " : "") << "gps: " << gps.toString(true) <<
			(withBraces ? "\n}" : "");
		return ss.str();
	}
}