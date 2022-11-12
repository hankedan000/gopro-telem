#pragma once

#include <vector>

#include "GoProTelem/FourCC.h"
#include "GoProTelem/GPMF_Payload.h"
#include "GoProTelem/GPMF_Stream.h"
#include "GoProTelem/MP4_Source.h"
#include "GoProTelem/SampleTypes.h"

namespace gpt
{

	std::vector<GPS_TimedSample>
	getPayloadGPS_Samples(
		GPMF_PayloadPtr pl);

	std::vector<GPS_TimedSample>
	getGPS_Samples(
		MP4_Source &mp4);

}