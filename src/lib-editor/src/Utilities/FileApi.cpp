#include "Utilities/FileApi.hpp"
//  #include <WinUser.h>
#include <Windows.h>
#include <codecvt>
#include <locale>
#include <shlobj.h>
#include <shlobj_core.h>
#include <stdexcept>

std::string FileApi::resolveAppdata() const
{
    PWSTR raw;
    std::wstring result;
    if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &raw) == S_OK)
    {
        result = std::wstring(raw);
    }
    CoTaskMemFree(raw);

    // Need to convert wstring to string
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    return converter.to_bytes(result);
}

std::optional<std::string> FileApi::getSaveFileName(const char* filter) const
{
    char filename[MAX_PATH];

    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner =
        NULL; // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH - 1;
    ofn.lpstrTitle = "Select a File";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (!GetSaveFileNameA(&ofn))
    {
        return {};
    }

    filename[MAX_PATH - 1] = '\0';
    return std::string(filename);
}

std::optional<std::string> FileApi::getOpenFileName(const char* filter) const
{
    char filename[MAX_PATH];

    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner =
        NULL; // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH - 1;
    ofn.lpstrTitle = "Select a File";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (!GetOpenFileNameA(&ofn))
    {
        return {};
    }

    filename[MAX_PATH - 1] = '\0';
    return std::string(filename);
}

std::optional<std::filesystem::path> FileApi::selectFolder() const
{
    TCHAR path[MAX_PATH];
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Select a folder";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    decltype(selectFolder()) result = {};

    if (!pidl)
    {
        return {};
    }

    SHGetPathFromIDList(pidl, path);
    CoTaskMemFree(pidl);
    return path;
}
