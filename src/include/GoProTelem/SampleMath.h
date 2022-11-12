#pragma once

#include "GoProTelem/SampleTypes.h"

namespace gpt
{

	double
	lerp(
		double a,
		double b,
		double ratio);

	void
	lerp(
		FloatXYZ &out,
		const FloatXYZ &a,
		const FloatXYZ &b,
		double ratio);

	void
	lerp(
		CoordLL &out,
		const CoordLL &a,
		const CoordLL &b,
		double ratio);

	void
	lerp(
		AcclSample &out,
		const AcclSample &a,
		const AcclSample &b,
		double ratio);

	void
	lerp(
		GPS_Sample &out,
		const GPS_Sample &a,
		const GPS_Sample &b,
		double ratio);

}