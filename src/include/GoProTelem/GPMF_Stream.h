#pragma once

#include <memory>
#include <stack>
#include <string>

#include "GoProTelem/FourCC.h"
#include "GoProTelem/Types.h"

namespace gpt
{

	// Internal Metadata structure and formatting tags
	const FourCC GPMF_KEY_DEVICE =			MAKE_FOURCC_ID('D','E','V','C');//DEVC - nested device data to speed the parsing of multiple devices in post 
	const FourCC GPMF_KEY_DEVICE_ID =		MAKE_FOURCC_ID('D','V','I','D');//DVID - unique id per stream for a metadata source (in camera or external input) (single 4 byte int)
	const FourCC GPMF_KEY_DEVICE_NAME =		MAKE_FOURCC_ID('D','V','N','M');//DVNM - human readable device type/name (char string)
	const FourCC GPMF_KEY_STREAM =			MAKE_FOURCC_ID('S','T','R','M');//STRM - nested channel/stream of telemetry data
	const FourCC GPMF_KEY_STREAM_NAME =		MAKE_FOURCC_ID('S','T','N','M');//STNM - human readable telemetry/metadata stream type/name (char string)
	const FourCC GPMF_KEY_SI_UNITS =		MAKE_FOURCC_ID('S','I','U','N');//SIUN - Display string for metadata units where inputs are in SI units "uT","rad/s","km/s","m/s","mm/s" etc.
	const FourCC GPMF_KEY_UNITS =			MAKE_FOURCC_ID('U','N','I','T');//UNIT - Freedform display string for metadata units (char sting like "RPM", "MPH", "km/h", etc)
	const FourCC GPMF_KEY_MATRIX =			MAKE_FOURCC_ID('M','T','R','X');//MTRX - 2D matrix for any sensor calibration.
	const FourCC GPMF_KEY_ORIENTATION_IN =	MAKE_FOURCC_ID('O','R','I','N');//ORIN - input 'n' channel data orientation, lowercase is negative, e.g. "Zxy" or "ABGR".
	const FourCC GPMF_KEY_ORIENTATION_OUT =	MAKE_FOURCC_ID('O','R','I','O');//ORIO - output 'n' channel data orientation, e.g. "XYZ" or "RGBA".
	const FourCC GPMF_KEY_SCALE =			MAKE_FOURCC_ID('S','C','A','L');//SCAL - divisor for input data to scale to the correct units.
	const FourCC GPMF_KEY_TYPE =			MAKE_FOURCC_ID('T','Y','P','E');//TYPE - Type define for complex data structures
	const FourCC GPMF_KEY_TOTAL_SAMPLES =	MAKE_FOURCC_ID('T','S','M','P');//TOTL - Total Sample Count including the current payload 	
	const FourCC GPMF_KEY_TICK =			MAKE_FOURCC_ID('T','I','C','K');//TICK - Beginning of data timing (arrival) in milliseconds. 
	const FourCC GPMF_KEY_TOCK =			MAKE_FOURCC_ID('T','O','C','K');//TOCK - End of data timing (arrival)  in milliseconds. 
	const FourCC GPMF_KEY_TIME_OFFSET =		MAKE_FOURCC_ID('T','I','M','O');//TIMO - Time offset of the metadata stream that follows (single 4 byte float)
	const FourCC GPMF_KEY_TIMING_OFFSET =	MAKE_FOURCC_ID('T','I','M','O');//TIMO - duplicated, as older code might use the other version of TIMO
	const FourCC GPMF_KEY_TIME_STAMP =		MAKE_FOURCC_ID('S','T','M','P');//STMP - Time stamp for the first sample. 
	const FourCC GPMF_KEY_TIME_STAMPS =		MAKE_FOURCC_ID('S','T','P','S');//STPS - Stream of all the timestamps delivered (Generally don't use this. This would be if your sensor has no peroidic times, yet precision is required, or for debugging.) 
	const FourCC GPMF_KEY_PREFORMATTED =	MAKE_FOURCC_ID('P','F','R','M');//PFRM - GPMF data
	const FourCC GPMF_KEY_TEMPERATURE_C =	MAKE_FOURCC_ID('T','M','P','C');//TMPC - Temperature in Celsius
	const FourCC GPMF_KEY_EMPTY_PAYLOADS =	MAKE_FOURCC_ID('E','M','P','T');//EMPT - Payloads that are empty since the device start (e.g. BLE disconnect.)
	const FourCC GPMF_KEY_QUANTIZE =		MAKE_FOURCC_ID('Q','U','A','N');//QUAN - quantize used to enable stream compression - 1 -  enable, 2+ enable and quantize by this value
	const FourCC GPMF_KEY_VERSION =			MAKE_FOURCC_ID('V','E','R','S');//VERS - version of the metadata stream (debugging)
	const FourCC GPMF_KEY_FREESPACE =		MAKE_FOURCC_ID('F','R','E','E');//FREE - n bytes reserved for more metadata added to an existing stream
	const FourCC GPMF_KEY_REMARK =			MAKE_FOURCC_ID('R','M','R','K');//RMRK - adding comments to the bitstream (debugging)
	const FourCC GPMF_KEY_END =				0;                      //(null)

	const FourCC GPMF_KEY_ACCL =			MAKE_FOURCC_ID('A','C','C','L');//ACCL - Accelerometer samples data
	const FourCC GPMF_KEY_GYRO =			MAKE_FOURCC_ID('G','Y','R','O');//GYRO - Gyroscope samples data
	const FourCC GPMF_KEY_GRAV =			MAKE_FOURCC_ID('G','R','A','V');//GRAV - Gravity vector
	const FourCC GPMF_KEY_CORI =			MAKE_FOURCC_ID('C','O','R','I');//CORI - Camera orientation
	const FourCC GPMF_KEY_GPS5 =			MAKE_FOURCC_ID('G','P','S','5');//GPS5 - GPS samples data

	// forward declarations
	class GPMF_Payload;
	class GPMF_Stream;

	using GPMF_StreamPtr = std::shared_ptr<GPMF_Stream>;

	class GPMF_Stream
	{
	public:
		~GPMF_Stream();

		std::string
		strmToString();

		// Prepare GPMF data methods
		bool
		resetState();

		void
		pushState();

		void
		popState();

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

		/**
		 * Return the total size of the current KLV in bytes
		 * 
		 * FourCC (Key) - 32bits
		 * Type-Size-Repeat (Length) - 32bits
		 * Raw data (Value) - 32bit aligned (structure size * repeat)
		 */
		size_t
		klvTotalSizePadded();

		bool
		getScaledDataDoubles(
			void *buffer,
			uint32_t buffersize,
			uint32_t sample_offset,
			uint32_t read_samples);

		/**
		 * @return
		 * a string representing the current KLV
		 * 
		 * FourCC (Key) Type-Size-Repeat (Length) Raw Data (Value)
		 * 
		 * Examples:
		 *    "DEVC null 4 7"
		 *    "DVID L 4 1 0x1001"
		 *    "DVNM c 1 6 Camera"
		 */
		std::string
		klvToString();

		uint8_t     readUINT8();
		int8_t      readINT8();
		uint16_t    readUINT16();
		int16_t     readINT16();
		uint32_t    readUINT32();
		int32_t     readINT32();
		uint64_t    readUINT64();
		int64_t     readINT64();
		float       readFloat();
		double      readDouble();
		std::string readString();

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

		enum Endianness
		{
			eEndianAuto,
			eEndianLittle,
			eEndianBig
		};

		template<typename INT_T>
		INT_T
		readInt(
			Endianness endianness = Endianness::eEndianAuto);

		void
		checkTypeAndThrow(
			char expectedType);

	private:
		// can be casted to a (GPMF_stream *)
		void *stream_;

		// a stack of (GPMF_stream *)
		std::stack<void *> stateStack_;

	};

}