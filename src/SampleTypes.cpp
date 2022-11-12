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

}