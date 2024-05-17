#pragma once

#include <Error.hpp>
#include <cstdint>
#include <expected>
#include <string>

using PlayerIdxType = std::uint8_t;
using ChecksumType = std::uint64_t;
using ExpectedLog = std::expected<std::string, ErrorMessage>;
