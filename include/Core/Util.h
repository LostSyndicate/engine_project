//
// Created by losts_n1cs3jj on 11/2/2022.
//

#ifndef SM3_UTIL_H
#define SM3_UTIL_H

#include <string>

std::string ReplaceOccurrences(const std::string& value,
                              const std::string& occurrence,
                              const std::string& replacement);

std::string smEscapeSpace(char c);

// returns number of bytes read in the string.
// unicode_char_point outputs a question mark (?) when reading invalid UTF8.
int smGetUTF8CharPoint(const char* utf8_string,
                       int offset,
                       unsigned int* unicode_char_point);

// returns how many characters are encoded in the utf-8 string.
int smUTF8CharCount(const char* utf8_string, int offset);

// returns the index of the last character in the utf-8 encoded string.
int smUTF8LastCharIndex(const char* utf8_string, int offset, int* out_size);

// returns the index after reaching 'position'.
// for example: position=2 "ABC", returns the index at B. This is useful for cursor positions.
int smUTF8AtPos(const char* utf8_string, int offset, int position, int* out_size);

#endif //SM3_UTIL_H
