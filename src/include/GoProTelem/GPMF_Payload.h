#pragma once

#include <memory>

#include "GoProTelem/Types.h"
#include "GoProTelem/GPMF_Stream.h"

namespace gpt
{

	// forward declarations
	class MP4_Source;
	class GPMF_Payload;

	using GPMF_PayloadPtr = std::shared_ptr<GPMF_Payload>;

	struct PayloadSensorInfo
	{
		// sensor's FourCC key
		FourCC fourCC;
		std::string name;
		size_t samplesInPayload;
		// total samples since first first payload in MP4
		size_t totalSamples;
		double measuredRate_hz;
		bool hasTemperature;
		double temperature_c;
		std::string siUnit;

		std::string
		toString(
			const std::string &tabStr = "") const;
	};

	class GPMF_Payload
	{
	public:
		~GPMF_Payload();

		static
		GPMF_PayloadPtr
		get(
			MP4_Source &mp4,
			uint32_t index);

		double
		inTime();

		double
		outTime();

		GPMF_StreamPtr
		getStream();

		/**
		 * @return
		 * false if the sensor was not found in the payload, true otherwise.
		 */
		bool
		getSensorInfo(
			FourCC sensor,
			PayloadSensorInfo &sensorInfo);

	private:
		// only allow construction by the MP4_Source class
		friend class MP4_Source;

		GPMF_Payload(
			MP4_Handle mp4Handle,
			uint32_t index);

	private:
		MP4_Handle mp4Handle_;
		uint32_t index_;
		size_t payloadSize_;
		PayloadResHandle resHandle_;
		uint32_t *payloadData_;

		double inTime_,outTime_;

	};

}