#pragma once

#include <cstdint>
#include <ostream>
#include <string>

namespace gpt
{

#define MAKE_FOURCC_ID(a,b,c,d)			(((d&0xff)<<24)|((c&0xff)<<16)|((b&0xff)<<8)|(a&0xff))

	using FourCC_uint = uint32_t;

	class FourCC
	{
	public:
		FourCC();

		FourCC(
			FourCC_uint fourCC);

		FourCC_uint
		uint() const;

		std::string
		toString() const;

	private:
		FourCC_uint fourCC_;

	};

	std::ostream &
	operator<<(
		std::ostream &out,
		const FourCC &fourCC);

	bool
	operator==(
		const FourCC &lhs,
		const FourCC &rhs);

	bool
	operator!=(
		const FourCC &lhs,
		const FourCC &rhs);

	bool
	operator<(
		const FourCC &lhs,
		const FourCC &rhs);

}