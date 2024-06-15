#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace patterns {
    static uint64_t scan_ex(uint64_t pModuleBaseAddress, const char* sSignature, int& bytesBeforeTarget, size_t nSelectResultIndex = NULL)
    {
        static auto patternToByte = [](const char* pattern, int& byteCounter)
        {
            auto bytes = std::vector<int>{};
            const auto start = const_cast<char*>(pattern);
            const auto end = const_cast<char*>(pattern) + strlen(pattern);
            int byteDistance = 0;

            for (auto current = start; current < end; ++current)
            {
                if (*current == '?')
                {
                    if (byteCounter == -1) {
                        byteCounter = byteDistance;
                    }

                    ++current;
                    if (*current == '?')
                        ++current;
                    bytes.push_back(-1);
                }
                else
                    bytes.push_back(strtoul((const char*)current, &current, 16));

                byteDistance++;
            }
            return bytes;
        };

        const auto dosHeader = (PIMAGE_DOS_HEADER)pModuleBaseAddress;
        const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModuleBaseAddress + dosHeader->e_lfanew);

        const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = patternToByte(sSignature, bytesBeforeTarget);
        const auto scanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);

        const auto s = patternBytes.size();
        const auto d = patternBytes.data();

        size_t nFoundResults = 0;

        for (auto i = 0ul; i < sizeOfImage - s; ++i)
        {
            bool found = true;

            for (auto j = 0ul; j < s; ++j)
            {
                if (scanBytes[i + j] != d[j] && d[j] != -1)
                {
                    found = false;
                    break;
                }
            }

            if (found)
            {
                if (nSelectResultIndex != 0)
                {
                    if (nFoundResults < nSelectResultIndex)
                    {
                        nFoundResults++;
                        found = false;
                    }
                    else
                        return reinterpret_cast<uint64_t>(&scanBytes[i]);
                }
                else
                    return reinterpret_cast<uint64_t>(&scanBytes[i]);
            }
        }

        return NULL;
    }

    static uint64_t resolve_addy(uint64_t address, int instructionSize, int bytesBeforeTarget) // resolve relative address (x86-64 assembly)
    {
        if (!address)
            return NULL;

        if (bytesBeforeTarget == -1)
            bytesBeforeTarget = (instructionSize - 4);

        int32_t offset = *(int32_t*)(address + bytesBeforeTarget);
        return address + instructionSize + offset;
    }

    static uint64_t scan(uint64_t moduleBase, const char* sSignature, int instructionLength = NULL, int bytesBeforeTarget = -1)
    {
        auto ret = scan_ex(moduleBase, sSignature, bytesBeforeTarget);

        if (instructionLength != 0)
            ret = resolve_addy(ret, instructionLength, bytesBeforeTarget);

        // komentar za anti-reverse
        //if (ret != NULL) {
        //    std::cout << ("[SCAN] Matching pattern found!\n [ ") << sSignature;
        //    std::cout << (" ]\n   -> 0x") << std::hex << ret << (" (offset 0x") << std::hex << ret - moduleBase << ")\n\n";
        //}
        //else {
        //    std::cout << ("[SCAN] Couldn't locate matching pattern.\n\n");
        //}

        return ret;
    }

    static uint64_t scan_confirm(bool& success, uint64_t moduleBase, const char* sSignature, int instructionLength = NULL, int bytesBeforeTarget = -1) {
        uint64_t retVal = scan(moduleBase, sSignature, instructionLength, bytesBeforeTarget);
        (retVal != NULL) ? success = true : success = false;
        return retVal;
    }
}