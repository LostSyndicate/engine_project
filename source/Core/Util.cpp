//
// Created by losts_n1cs3jj on 11/2/2022.
//

#include "Core/Util.h"

std::string ReplaceOccurrences(const std::string& value,
                              const std::string& occurrence,
                              const std::string& replacement)
{
    if (value.empty())
        return value;
    std::string result;
    size_t pos = 0, lpos = 0;
    while ((pos = value.find(occurrence, pos)) != std::string::npos)
    {
        result += value.substr(lpos, pos - lpos);
        result += replacement;
        pos += occurrence.size();
        lpos = pos;
    }
    if (pos != lpos)
        result += value.substr(lpos, pos - lpos);
    return result;
}

std::string smEscapeSpace(char c)
{
    switch (c)
    {
        case '\n':
            return "\\n";
        case '\t':
            return "\\t";
        case '\r':
            return "\\r";
        case 0:
            return "(null)";
        default:
            return std::string(1, c);
    }
}

int smGetUTF8CharPoint(const char* utf8_string,
                       int offset,
                       unsigned int* ch_point)
{
    const unsigned char* uc = (const unsigned char*)utf8_string + offset;
    if (ch_point != NULL) *ch_point = 63;
    if ((0x80 & uc[0]) == 0x00) // 1
    {
        if (ch_point != NULL) *ch_point = uc[0];
        return 1;
    }
    else if ((0xE0 & uc[0]) == 0xC0) // 2
    {
        if (ch_point != NULL) *ch_point = ((0x1F & uc[0]) << 6) | (0x3F & uc[1]);
        return 2;
    }
    else if ((0xF0 & uc[0]) == 0xE0) // 3
    {
        if (ch_point != NULL) *ch_point = ((0x0F & uc[0]) << 12) | ((0x3F & uc[1]) << 6) | (0x3F & uc[2]);
        return 3;
    }
    else if ((0xF8 & uc[0]) == 0xF0) // 4
    {
        if (ch_point != NULL) *ch_point = ((0x07 & uc[0]) << 18) | ((0x3F & uc[1]) << 12) | ((0x3F & uc[2]) << 6) | (0x3F & uc[3]);
        return 4;
    }
    else
    {
        return 1;
    }
}

int smUTF8CharCount(const char* utf8_string, int offset)
{
    int count = 0;
    const char* str = utf8_string + offset;
    while (*str != 0)
    {
        int c = smGetUTF8CharPoint(str, 0, NULL);
        count++;
        str += c;
    }
    return count;
}

int smUTF8LastCharIndex(const char* utf8_string, int offset, int* out_size)
{
    int index = 0;
    const char* str = utf8_string + offset;
    while (*str != 0)
    {
        int count = smGetUTF8CharPoint(str, 0, NULL);
        str += count;
        if (*str == 0)
        {
            if (out_size != NULL)
                *out_size = count;
            return index;
        }
        index += count;
    }
    return index;
}

int smUTF8AtPos(const char* utf8_string, int offset, int position, int* out_size)
{
    int index = 0;
    int count = 0;
    const char* str = utf8_string + offset;
    while (count < position && *str != 0)
    {
        int c = smGetUTF8CharPoint(str, 0, NULL);
        str += c;
        index += c;
        if (out_size != NULL)
            *out_size = c;
        count++;
    }
    return index;
}