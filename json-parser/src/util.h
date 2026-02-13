#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace util {
  inline bool isBlank(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
  }

  inline bool isDigit(char c) {
    return '0' <= c && c <= '9';
  }

  inline bool isHexDigit(char c) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
  }

  // 由调用方确保输入是合法的 hex 字符
  inline uint8_t charToHex(char c) {
    static constexpr auto hexTable = []() {
      std::array<uint8_t, 256> table{};
      for (int i = 0; i < 256; ++i) table[i] = 0;
      for (char c = '0'; c <= '9'; ++c) table[c] = c - '0';
      for (char c = 'a'; c <= 'f'; ++c) table[c] = 10 + (c - 'a');
      for (char c = 'A'; c <= 'F'; ++c) table[c] = 10 + (c - 'A');
      return table;
    }();

    return hexTable[static_cast<unsigned char>(c)];
  }

  // 由调用方确保输入是合法的
  inline uint32_t strToCodePoint(const std::string& input) {
    return (charToHex(input[0]) << 12)
      | (charToHex(input[1]) << 8)
      | (charToHex(input[2]) << 4)
      | charToHex(input[3])
      ;
  }

  // todo: 不使用异常如何处理非法参数？
  inline std::string codePointToUtf8(uint32_t codePoint) {
    if (codePoint > 0x10FFFF)
      throw "invalid code point, exceeds U+10FFFF";

    // [0xD800, 0xDFFF] 不代表合法值
    // RFC 3629 明确规定这些码点不能被编码
    if (0xD800 <= codePoint && codePoint <= 0xDFFF)
      throw "invalid code point, in surrogate range";

    std::string s;
    s.reserve(4);
    if (codePoint <= 0x7F) {
      s += static_cast<uint8_t>(codePoint);
    } else if (codePoint <= 0x7FF) {
      s += static_cast<uint8_t>(0xC0 | (codePoint >> 6));
      s += static_cast<uint8_t>(0x80 | (codePoint & 0x3F));
    } else if (codePoint <= 0xFFFF) {
      s += static_cast<uint8_t>(0xE0 | (codePoint >> 12));
      s += static_cast<uint8_t>(0x80 | ((codePoint >> 6) & 0x3F));
      s += static_cast<uint8_t>(0x80 | (codePoint & 0x3F));
    } else {
      s += static_cast<uint8_t>(0xF0 | (codePoint >> 18));
      s += static_cast<uint8_t>(0x80 | ((codePoint >> 12) & 0x3F));
      s += static_cast<uint8_t>(0x80 | ((codePoint >> 6) & 0x3F));
      s += static_cast<uint8_t>(0x80 | (codePoint & 0x3F));
    }

    return s;
  }

  inline bool isHighSurrogate(uint32_t codePoint) {
    return 0xD800 <= codePoint && codePoint <= 0xDBFF;
  }

  inline bool isLowSurrogate(uint32_t codePoint) {
    return 0xDC00 <= codePoint && codePoint <= 0xDFFF;
  }

  // 由调用方确保输入是合法的
  inline uint32_t mergeSurrogate(uint32_t high, uint32_t low) {
    return ((high - 0xD800) << 10) + (low - 0xDC00) + 0x10000;
  }
}
