#pragma once

#include <vector>

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
		Quat &out,
		const Quat &a,
		const Quat &b,
		double ratio);

	void
	lerp(
		CoordLL &out,
		const CoordLL &a,
		const CoordLL &b,
		double ratio);

	void
	lerp(
		GPS_Sample &out,
		const GPS_Sample &a,
		const GPS_Sample &b,
		double ratio);

	template <class TimedSample_T>
	inline
	bool
	findLerpIndex(
		size_t &index,
		const std::vector<TimedSample_T> &sampVec,
		double searchTime)
	{
		bool found = false;
		while (true)
		{
			auto nextIdx = index + 1;
			if (nextIdx > (sampVec.size() - 1))
			{
				break;
			}

			if (sampVec.at(index).t_offset <= searchTime &&
				searchTime <= sampVec.at(nextIdx).t_offset)
			{
				found = true;
				break;
			}
			index++;
		}
		return found;
	}

	/**
	 * Linear interpolates between two TimeSample classes
	 * 
	 * @param[out] out
	 * the sample type to populate the interpolated result into
	 * 
	 * @param[in] a
	 * the earlier sample to interpolate between
	 * 
	 * @param[in] b
	 * the later sample to interpolate between
	 * 
	 * @param[in] timePoint
	 * the time to interpolate to (relative to 'a' and 'b')
	 */
	template <class NonTimedSample_T, class TimedSample_T>
	inline
	void
	lerpTimedSample(
		NonTimedSample_T &out,
		const TimedSample_T &a,
		const TimedSample_T &b,
		double timePoint)
	{
		const double dt = b.t_offset - a.t_offset;
		const double ratio = (timePoint - a.t_offset) / dt;
		lerp(out, a, b, ratio);
	}

}