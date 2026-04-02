#pragma once
#include <string>
#include <Windows.h>

namespace MiString
{
    // UTF-8のstd::stringをUTF-16のstd::wstringに変換する関数
    inline std::wstring ToWString(const std::string& str)
    {
        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring result(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
        return result;
    }

}