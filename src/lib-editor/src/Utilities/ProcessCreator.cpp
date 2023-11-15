#include "include/Utilities/ProcessCreator.hpp"
#include <cstdlib>
#include <tchar.h>
#include <windows.h>

static std::string FormatWinApiError(DWORD errorId)
{
    LPSTR lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorId,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPSTR)&lpMsgBuf,
        0,
        NULL);

    std::string message = lpMsgBuf;

    LocalFree(lpMsgBuf);

    return message;
}

std::expected<ReturnFlag, ErrorMessage> ProcessCreator::Exec(
    const std::filesystem::path& pathToBinary,
    const std::string& params,
    const std::filesystem::path& workingDirectory) const noexcept
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process.
    if (!CreateProcess(
            NULL, // No module name (use command line)
            (pathToBinary.string() + " " + params).data(), // Command line
            NULL,                             // Process handle not inheritable
            NULL,                             // Thread handle not inheritable
            FALSE,                            // Set handle inheritance to FALSE
            0,                                // No creation flags
            NULL,                             // Use parent's environment block
            workingDirectory.string().data(), // working directory
            &si, // Pointer to STARTUPINFO structure
            &pi) // Pointer to PROCESS_INFORMATION structure
    )
    {
        return std::unexpected(FormatWinApiError(GetLastError()));
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return ReturnFlag::Success;
}
