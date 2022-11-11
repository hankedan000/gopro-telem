#pragma once

#include <memory>
#include <string>

#include "GoProTelem/SampleTypes.h"

namespace gpt
{

	// mirrored enum from GoPro's library
	enum GPMF_Levels
	{
		GPMF_CURRENT_LEVEL = 0,  // search or validate within the current GPMF next level
		GPMF_RECURSE_LEVELS = 1, // search or validate recursing all levels
		GPMF_TOLERANT = 2		 // Ignore minor errors like unknown datatypes if the structure is otherwise valid. 
	};

	using MP4_Handle = size_t;
	using PayloadResHandle = size_t;
	using FourCC_uint = uint32_t;

	class FourCC
	{
	public:
		FourCC();

		FourCC(
			FourCC_uint fourCC);

		FourCC_uint
		uint();

		std::string
		toString();

	private:
		FourCC_uint fourCC_;

	};

#define MAKEID(a,b,c,d)			(((d&0xff)<<24)|((c&0xff)<<16)|((b&0xff)<<8)|(a&0xff))

	// Internal Metadata structure and formatting tags
	const FourCC GPMF_KEY_DEVICE =			MAKEID('D','E','V','C');//DEVC - nested device data to speed the parsing of multiple devices in post 
	const FourCC GPMF_KEY_DEVICE_ID =		MAKEID('D','V','I','D');//DVID - unique id per stream for a metadata source (in camera or external input) (single 4 byte int)
	const FourCC GPMF_KEY_DEVICE_NAME =		MAKEID('D','V','N','M');//DVNM - human readable device type/name (char string)
	const FourCC GPMF_KEY_STREAM =			MAKEID('S','T','R','M');//STRM - nested channel/stream of telemetry data
	const FourCC GPMF_KEY_STREAM_NAME =		MAKEID('S','T','N','M');//STNM - human readable telemetry/metadata stream type/name (char string)
	const FourCC GPMF_KEY_SI_UNITS =		MAKEID('S','I','U','N');//SIUN - Display string for metadata units where inputs are in SI units "uT","rad/s","km/s","m/s","mm/s" etc.
	const FourCC GPMF_KEY_UNITS =			MAKEID('U','N','I','T');//UNIT - Freedform display string for metadata units (char sting like "RPM", "MPH", "km/h", etc)
	const FourCC GPMF_KEY_MATRIX =			MAKEID('M','T','R','X');//MTRX - 2D matrix for any sensor calibration.
	const FourCC GPMF_KEY_ORIENTATION_IN =	MAKEID('O','R','I','N');//ORIN - input 'n' channel data orientation, lowercase is negative, e.g. "Zxy" or "ABGR".
	const FourCC GPMF_KEY_ORIENTATION_OUT =	MAKEID('O','R','I','O');//ORIO - output 'n' channel data orientation, e.g. "XYZ" or "RGBA".
	const FourCC GPMF_KEY_SCALE =			MAKEID('S','C','A','L');//SCAL - divisor for input data to scale to the correct units.
	const FourCC GPMF_KEY_TYPE =			MAKEID('T','Y','P','E');//TYPE - Type define for complex data structures
	const FourCC GPMF_KEY_TOTAL_SAMPLES =	MAKEID('T','S','M','P');//TOTL - Total Sample Count including the current payload 	
	const FourCC GPMF_KEY_TICK =			MAKEID('T','I','C','K');//TICK - Beginning of data timing (arrival) in milliseconds. 
	const FourCC GPMF_KEY_TOCK =			MAKEID('T','O','C','K');//TOCK - End of data timing (arrival)  in milliseconds. 
	const FourCC GPMF_KEY_TIME_OFFSET =		MAKEID('T','I','M','O');//TIMO - Time offset of the metadata stream that follows (single 4 byte float)
	const FourCC GPMF_KEY_TIMING_OFFSET =	MAKEID('T','I','M','O');//TIMO - duplicated, as older code might use the other version of TIMO
	const FourCC GPMF_KEY_TIME_STAMP =		MAKEID('S','T','M','P');//STMP - Time stamp for the first sample. 
	const FourCC GPMF_KEY_TIME_STAMPS =		MAKEID('S','T','P','S');//STPS - Stream of all the timestamps delivered (Generally don't use this. This would be if your sensor has no peroidic times, yet precision is required, or for debugging.) 
	const FourCC GPMF_KEY_PREFORMATTED =	MAKEID('P','F','R','M');//PFRM - GPMF data
	const FourCC GPMF_KEY_TEMPERATURE_C =	MAKEID('T','M','P','C');//TMPC - Temperature in Celsius
	const FourCC GPMF_KEY_EMPTY_PAYLOADS =	MAKEID('E','M','P','T');//EMPT - Payloads that are empty since the device start (e.g. BLE disconnect.)
	const FourCC GPMF_KEY_QUANTIZE =		MAKEID('Q','U','A','N');//QUAN - quantize used to enable stream compression - 1 -  enable, 2+ enable and quantize by this value
	const FourCC GPMF_KEY_VERSION =			MAKEID('V','E','R','S');//VERS - version of the metadata stream (debugging)
	const FourCC GPMF_KEY_FREESPACE =		MAKEID('F','R','E','E');//FREE - n bytes reserved for more metadata added to an existing stream
	const FourCC GPMF_KEY_REMARK =			MAKEID('R','M','R','K');//RMRK - adding comments to the bitstream (debugging)
	const FourCC GPMF_KEY_END =				0;                      //(null)

	// forward declarations
	class MP4_Source;
	class GPMF_Payload;
	class GPMF_Stream;

	using GPMF_StreamPtr = std::shared_ptr<GPMF_Stream>;

	class GPMF_Stream
	{
	public:
		~GPMF_Stream();

		// Prepare GPMF data methods
		bool
		resetState();

		// Validate GPMF structure from the current location
		bool
		validate(
			gpt::GPMF_Levels level);

		// Navigate through GPMF data methods
		bool
		next(
			gpt::GPMF_Levels level);

		bool
		findPrev(
			FourCC fourCC,
			gpt::GPMF_Levels level);

		bool
		findNext(
			FourCC fourCC,
			gpt::GPMF_Levels level);

		bool
		seekToSamples();

		// Get information about the current GPMF KLV
		FourCC   key();					//return the current Key (FourCC)
		char     type();				//return the current Type (GPMF_Type)
		uint32_t structSize();			//return the current sample structure size
		uint32_t repeat();				//return the current repeat or the number of samples of this structure
		uint32_t payloadSampleCount();	//return the current number of samples of this structure, supporting multisample entries.
		uint32_t elementsInStruct();	//return the current number elements within the structure (e.g. 3-axis gyro)
		uint32_t rawDataSize();			//return the data size for the current GPMF KLV 
		void *   rawData();				//return a pointer the KLV data (which is Bigendian if the type is known.)

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