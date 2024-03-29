module;

#include <expected>
#include <string>

export module Error;

export class BaseError
{
};

export using ErrorMessage = std::string;

export enum class ReturnFlag : bool {
    Success = true,
    Failure = false,
};

export using ExpectSuccess = std::expected<ReturnFlag, ErrorMessage>;
