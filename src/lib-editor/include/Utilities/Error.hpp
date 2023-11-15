#pragma once

#include <string>

class BaseError {};

using ErrorMessage = std::string;

enum class ReturnFlag : bool
{
	Success = true,
	Failure = false,
};
