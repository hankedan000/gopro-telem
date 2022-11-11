#pragma once

#include <memory>
#include <string>

#include "GoProTelem/SampleTypes.h"

namespace gpt
{

	using MP4_Handle = size_t;
	using PayloadResHandle = size_t;

	// forward declarations
	class MP4_Source;
	class GPMF_Payload;
	class GPMF_Stream;

	using GPMF_StreamPtr = std::shared_ptr<GPMF_Stream>;

	class GPMF_Stream
	{
	public:
		~GPMF_Stream();

	private:
		// only allow construction by the GPMF_Payload class
		friend class GPMF_Payload;

		/**
		 * @param[in] buffer
		 * a pointer to the GPMF_Payload's raw data pointer
		 * 
		 * @param[in] datasize
		 * the size of the GPMF_Payload
		 */
		GPMF_Stream(
			uint32_t *buffer,
			uint32_t datasize);

	private:
		// can be casted to a (GPMF_stream *)
		void *stream_;

	};

	using GPMF_PayloadPtr = std::shared_ptr<GPMF_Payload>;

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

	class MP4_Source
	{
	public:
		MP4_Source();

		MP4_Source(
			const MP4_Source &other);

		~MP4_Source();

		/**
		 * @return
		 * 0 on success -1 on error
		 */
		int
		open(
			const std::string &filepath);

		void
		close();

		MP4_Handle
		handle();

		float
		duration();

		/**
		 * @return
		 * video's frame rate (frames per second)
		 */
		float
		fps();

		/**
		 * @return
		 * total number of frames in the video
		 */
		size_t
		frameCount();

		size_t
		payloadCount();

		GPMF_PayloadPtr
		getPayload(
			uint32_t index);

	private:
		void
		assertValid() const;

	private:
		std::string filepath_;
		MP4_Handle mp4Handle_;

	};

}