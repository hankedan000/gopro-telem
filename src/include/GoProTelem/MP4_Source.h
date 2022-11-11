#pragma once

#include <memory>
#include <string>

#include "GoProTelem/GoProTelem_Types.h"
#include "GoProTelem/GPMF_Payload.h"

namespace gpt
{

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