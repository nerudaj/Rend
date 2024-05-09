#pragma once

#include <expected>
#include <string>

// NOTE: This header needs a complete redesign, but not as a part of this project

class BaseError
{
};

using ErrorMessage = std::string;

enum class ReturnFlag : bool {
    Success = true,
    Failure = false,
};

using ExpectSuccess = std::expected<ReturnFlag, ErrorMessage>;
