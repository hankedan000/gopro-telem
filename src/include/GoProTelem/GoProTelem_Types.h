#pragma once

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
}