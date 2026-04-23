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

    // 特定記号を境に文字列を分割する関数
    inline std::vector<std::string> Split(const std::string& str, char delimiter)
    {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = str.find(delimiter);
        while (end != std::string::npos)
        {
            result.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delimiter, start);
        }
        result.push_back(str.substr(start));
        return result;
    }

}