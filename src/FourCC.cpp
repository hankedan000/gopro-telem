#include "GoProTelem/FourCC.h"

#include "GPMF_common.h"

namespace gpt
{

	FourCC::FourCC()
	 : fourCC_(::GPMF_KEY_END)
	{
	}

	FourCC::FourCC(
		FourCC_uint fourCC)
	 : fourCC_(fourCC)
	{
	}

	FourCC_uint
	FourCC::uint() const
	{
		return fourCC_;
	}

	std::string
	FourCC::toString() const
	{
		std::string strOut("    ");
		strOut[3] = (fourCC_ >> 24) & 0xFF;
		strOut[2] = (fourCC_ >> 16) & 0xFF;
		strOut[1] = (fourCC_ >> 8)  & 0xFF;
		strOut[0] = (fourCC_)       & 0xFF;
		return strOut;
	}

	std::ostream &
	operator<<(
		std::ostream &out,
		const FourCC &fourCC)
	{
		out << fourCC.toString();
		return out;
	}

	bool
	operator==(
		const FourCC &lhs,
		const FourCC &rhs)
	{
		return lhs.uint() == rhs.uint();
	}

	bool
	operator!=(
		const FourCC &lhs,
		const FourCC &rhs)
	{
		return ! (lhs == rhs);
	}

	bool
	operator<(
		const FourCC &lhs,
		const FourCC &rhs)
	{
		return lhs.uint() < rhs.uint();
	}

}