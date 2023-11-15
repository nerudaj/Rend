#include <include/Interfaces/FileApiInterface.hpp>

/**
 * Usage:
 * auto fileApiMock = GC<FileApiStub>();
 * auto fileApiSpy = Mock<FileApiInterface>(*fileApiMock);
 *
 * Use fileApiMock for dependency injection and set expectations on the spy
 * object.
 */
class FileApiStub final : public FileApiInterface
{
public:
    std::string resolveAppdata() const override
    {
        return "";
    }

    std::optional<std::string>
    getSaveFileName(const char* filter) const override
    {
        return "";
    }

    std::optional<std::string>
    getOpenFileName(const char* filter) const override
    {
        return "";
    }

    std::optional<std::filesystem::path> selectFolder() const override
    {
        return {};
    }
};
