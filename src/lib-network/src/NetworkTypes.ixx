module;

#include <cstdint>
#include <expected>
#include <string>

export module NetworkTypes;

export import Error;

export using PlayerIdType = std::uint8_t;
export using ChecksumType = std::uint64_t;
export using ExpectedLog = std::expected<std::string, ErrorMessage>;
