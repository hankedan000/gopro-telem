#include "GoProTelem/SampleTypes.h"

#include <iomanip>
#include <sstream>

namespace gpt
{

	std::string
	toString(
		const CoordLL &coord,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(6) <<
			"lat: " << coord.lat << ", "
			"lon: " << coord.lon <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const AcclSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) << std::showpos <<
			"x: " << sample.x << ", "
			"y: " << sample.y << ", "
			"z: " << sample.z <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const GyroSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) << std::showpos <<
			"x: " << sample.x << ", "
			"y: " << sample.y << ", "
			"z: " << sample.z <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const GravSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) << std::showpos <<
			"x: " << sample.x << ", "
			"y: " << sample.y << ", "
			"z: " << sample.z <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const OrientationSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) << std::showpos <<
			"w: " << sample.w << ", "
			"x: " << sample.x << ", "
			"y: " << sample.y << ", "
			"z: " << sample.z <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const GPS_Sample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			"coord: " << toString(sample.coord,true) << ", " <<
			std::fixed << std::setprecision(3) <<
			"altitude: " << sample.altitude << ", " <<
			"speed2D: " << sample.speed2D << ", " <<
			"speed3D: " << sample.speed3D <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const GPS_TimedSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) <<
			"t_offset: " << sample.t_offset << ", " <<
			toString(sample) <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const AcclTimedSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) <<
			"t_offset: " << sample.t_offset << ", " <<
			toString(sample) <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const GyroTimedSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) <<
			"t_offset: " << sample.t_offset << ", " <<
			toString(sample) <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const GravTimedSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) <<
			"t_offset: " << sample.t_offset << ", " <<
			toString(sample) <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const OrientationTimedSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ " : "") <<
			std::fixed << std::setprecision(3) <<
			"t_offset: " << sample.t_offset << ", " <<
			toString(sample) <<
			(withBraces ? " }" : "");
		return ss.str();
	}

	std::string
	toString(
		const CombinedSample &sample,
		bool withBraces)
	{
		std::stringstream ss;
		ss << (withBraces ? "{ \n" : "") <<
			std::fixed << std::setprecision(3) <<
			"t_offset: " << sample.t_offset << ", " <<
			(withBraces ? "  " : "") << "accl: " << toString(sample.accl,true) << std::endl <<
			(withBraces ? "  " : "") << "gyro: " << toString(sample.gyro,true) << std::endl <<
			(withBraces ? "  " : "") << "grav: " << toString(sample.grav,true) << std::endl <<
			(withBraces ? "  " : "") << "gps: " << toString(sample.gps,true) <<
			(withBraces ? "\n}" : "");
		return ss.str();
	}
}