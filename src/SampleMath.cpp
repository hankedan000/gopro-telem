#include "GoProTelem/SampleMath.h"

namespace gpt
{

	double
	lerp(
		double a,
		double b,
		double ratio)
	{
		const double slope = (b - a);
		return (slope != 0.0 ? a + (slope * ratio) : a);
	}

	void
	lerp(
		FloatXYZ &out,
		const FloatXYZ &a,
		const FloatXYZ &b,
		double ratio)
	{
		out.x = lerp(a.x, b.x, ratio);
		out.y = lerp(a.y, b.y, ratio);
		out.z = lerp(a.z, b.z, ratio);
	}

	void
	lerp(
		Quat &out,
		const Quat &a,
		const Quat &b,
		double ratio)
	{
		out.x = lerp(a.x, b.x, ratio);
		out.y = lerp(a.y, b.y, ratio);
		out.z = lerp(a.z, b.z, ratio);
		out.w = lerp(a.w, b.w, ratio);
	}

	void
	lerp(
		CoordLL &out,
		const CoordLL &a,
		const CoordLL &b,
		double ratio)
	{
		out.lat = lerp(a.lat, b.lat, ratio);
		out.lon = lerp(a.lon, b.lon, ratio);
	}

	void
	lerp(
		GPS_Sample &out,
		const GPS_Sample &a,
		const GPS_Sample &b,
		double ratio)
	{
		lerp(out.coord, a.coord, b.coord, ratio);
		out.altitude = lerp(a.altitude, b.altitude, ratio);
		out.speed2D = lerp(a.speed2D, b.speed2D, ratio);
		out.speed3D = lerp(a.speed3D, b.speed3D, ratio);
	}

}