#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Token.h"
#include "spdlog/spdlog.h"
#include "util.h"

class JsonLexer {
  private:
  enum class State {
    INIT,
    IN_STRING,
    IN_TRUE,
    IN_FALSE,
    IN_NULL,
    IN_ESCAPE,
    IN_UNICODE_ESCAPE,
    AFTER_HIGH_SURROGATE,
    BEFORE_LOW_SURROGATE,
    IN_LOW_SURROGATE,

    AFTER_NUMBER_INTEGER_SIGN,
    AFTER_NUMBER_LEADING_ZERO,
    IN_NUMBER_INTEGER,
    AFTER_NUMBER_INTEGER,
    AFTER_NUMBER_POINT,
    IN_NUMBER_FRACTION_DIGIT,
    AFTER_NUMBER_FRACTION,
    IN_NUMBER_EXPONENT,
    AFTER_NUMBER_EXPONENT_SIGN,
    IN_NUMBER_EXPONENT_DIGIT,
  };

  public:
  std::vector<std::unique_ptr<Token>> lex(const std::string& input) {
    std::vector<std::unique_ptr<Token>> tokens;

    auto state = State::INIT;
    std::string buffer, unicodeBuffer;
    unicodeBuffer.reserve(4);
    size_t i = 0;
    uint32_t highSurrogate = 0;
    while (i < input.length()) {
      char c = input[i++];
      switch (state) {
        case State::INIT:
          if (c == '{') {
            tokens.push_back(std::make_unique<ObjectStartToken>());
          } else if (c == '}') {
            tokens.push_back(std::make_unique<ObjectEndToken>());
          } else if (c == ':') {
            tokens.push_back(std::make_unique<ColonToken>());
          } else if (c == ',') {
            tokens.push_back(std::make_unique<CommaToken>());
          } else if (c == '[') {
            tokens.push_back(std::make_unique<ArrayStartToken>());
          } else if (c == ']') {
            tokens.push_back(std::make_unique<ArrayEndToken>());
          } else if (c == '"') {
            state = State::IN_STRING;
          } else if (c == '-') {
            state = State::AFTER_NUMBER_INTEGER_SIGN;
            buffer += c;
          } else if (c == '0') {
            state = State::AFTER_NUMBER_LEADING_ZERO;
            buffer += c;
          } else if (util::isDigit(c)) {
            state = State::IN_NUMBER_INTEGER;
            buffer += c;
          } else if (c == 't') {
            state = State::IN_TRUE;
            buffer += c;
          } else if (c == 'f') {
            state = State::IN_FALSE;
            buffer += c;
          } else if (c == 'n') {
            state = State::IN_NULL;
            buffer += c;
          } else if (util::isBlank(c)) {
            // skip, do nothing
          } else {
            spdlog::info("未知的字符：{}", c);
            exit(1);
          }
          break;
        case State::AFTER_NUMBER_INTEGER_SIGN:
          if (c == '0') {
            state = State::AFTER_NUMBER_LEADING_ZERO;
            buffer += c;
          } else if (util::isDigit(c)) {
            state = State::IN_NUMBER_INTEGER;
            buffer += c;
          } else {
            spdlog::info("负号后面紧跟的不是数字：{}", c);
            exit(1);
          }
          break;
        case State::AFTER_NUMBER_LEADING_ZERO:
          if (util::isDigit(c)) {
            spdlog::info("前导零非法：{}{}", buffer, c);
            exit(1);
          } else {
            state = State::AFTER_NUMBER_INTEGER;
            i--;
          }
          break;
        case State::IN_NUMBER_INTEGER:
          if (util::isDigit(c)) {
            buffer += c;
          } else {
            state = State::AFTER_NUMBER_INTEGER;
            i--;
          }
          break;
        case State::AFTER_NUMBER_INTEGER:
          if (c == '.') {
            state = State::AFTER_NUMBER_POINT;
            buffer += c;
          } else if (c == 'e' || c == 'E') {
            state = State::IN_NUMBER_EXPONENT;
            buffer += c;
          } else {
            state = State::INIT;
            i--;
            tokens.push_back(std::make_unique<NumberToken>(buffer));
            buffer.clear();
          }
          break;
        case State::AFTER_NUMBER_POINT:
          if (util::isDigit(c)) {
            state = State::IN_NUMBER_FRACTION_DIGIT;
            buffer += c;
          } else {
            spdlog::info("小数点后面紧跟的不是数字：{}", c);
            exit(1);
          }
          break;
        case State::IN_NUMBER_FRACTION_DIGIT:
          if (util::isDigit(c)) {
            state = State::IN_NUMBER_FRACTION_DIGIT;
            buffer += c;
          } else {
            state = State::AFTER_NUMBER_FRACTION;
            i--;
          }
          break;
        case State::AFTER_NUMBER_FRACTION:
          if (c == 'e' || c == 'E') {
            state = State::IN_NUMBER_EXPONENT;
            buffer += c;
          } else {
            state = State::INIT;
            i--;
            tokens.push_back(std::make_unique<NumberToken>(buffer));
            buffer.clear();
          }
          break;
        case State::IN_NUMBER_EXPONENT:
          if (c == '-' || c == '+') {
            state = State::AFTER_NUMBER_EXPONENT_SIGN;
            buffer += c;
          } else if(util::isDigit(c)) {
            state = State::IN_NUMBER_EXPONENT_DIGIT;
            buffer += c;
          } else {
            spdlog::info("非数字：{}", c);
            exit(1);
          }
          break;
        case State::AFTER_NUMBER_EXPONENT_SIGN:
          if (util::isDigit(c)) {
            state = State::IN_NUMBER_EXPONENT_DIGIT;
            buffer += c;
          } else {
            spdlog::info("非数字：{}", c);
            exit(1);
          }
          break;
        case State::IN_NUMBER_EXPONENT_DIGIT:
          if (util::isDigit(c)) {
            buffer += c;
          } else {
            state = State::INIT;
            i--;
            tokens.push_back(std::make_unique<NumberToken>(buffer));
            buffer.clear();
          }
          break;
        case State::IN_STRING:
          if (c == '"') {
            state = State::INIT;
            tokens.push_back(std::make_unique<StringToken>(buffer));
            buffer.clear();
          } else if (c == '\\') {
            state = State::IN_ESCAPE;
          } else {
            buffer += c;
          }
          break;
        case State::IN_TRUE:
          buffer += c;
          if (buffer == "tr" || buffer == "tru") {
          } else if (buffer == "true") {
            state = State::INIT;
            tokens.push_back(std::make_unique<BooleanToken>(true));
            buffer.clear();
          } else {
            spdlog::info("未知的字符：{}", c);
            exit(1);
          }
          break;
        case State::IN_FALSE:
          buffer += c;
          if (buffer == "fa" || buffer == "fal" || buffer == "fals") {
          } else if (buffer == "false") {
            state = State::INIT;
            tokens.push_back(std::make_unique<BooleanToken>(false));
            buffer.clear();
          } else {
            spdlog::info("未知的字符：{}", c);
            exit(1);
          }
          break;
        case State::IN_NULL:
          buffer += c;
          if (buffer == "nu" || buffer == "nul") {
          } else if (buffer == "null") {
            state = State::INIT;
            tokens.push_back(std::make_unique<NullToken>());
            buffer.clear();
          } else {
            spdlog::info("未知的字符：{}", c);
            exit(1);
          }
          break;
        case State::IN_ESCAPE:
          switch (c) {
            case '"':
              state = State::IN_STRING;
              buffer += '"';
              break;
            case '\\':
              state = State::IN_STRING;
              buffer += '\\';
              break;
            case '/':
              state = State::IN_STRING;
              buffer += '/';
              break;
            case 'b':
              state = State::IN_STRING;
              buffer += '\b';
              break;
            case 'f':
              state = State::IN_STRING;
              buffer += '\f';
              break;
            case 'n':
              state = State::IN_STRING;
              buffer += '\n';
              break;
            case 'r':
              state = State::IN_STRING;
              buffer += '\r';
              break;
            case 't':
              state = State::IN_STRING;
              buffer += '\t';
              break;
            case 'u':
              state = State::IN_UNICODE_ESCAPE;
              break;
            default:
              spdlog::info("未知的转义字符：{}", c);
              exit(1);
          }
          break;
        case State::IN_UNICODE_ESCAPE:
          if (util::isHexDigit(c)) {
            unicodeBuffer += c;
          } else {
            spdlog::info("未知的 unicode 转义字符：{}", c);
            exit(1);
          }
          if (unicodeBuffer.size() == 4) {
            auto codePoint = util::strToCodePoint(unicodeBuffer);
            unicodeBuffer.clear();
            if (util::isHighSurrogate(codePoint)) {
              state = State::AFTER_HIGH_SURROGATE;
              highSurrogate = codePoint;
            } else if (util::isLowSurrogate(codePoint)) {
              spdlog::info("码点 {:#x} 缺少高位代理", codePoint);
              exit(1);
            } else {
              state = State::IN_STRING;
              buffer += util::codePointToUtf8(codePoint);
            }
          }
          break;
        case State::AFTER_HIGH_SURROGATE:
          if (c == '\\') {
            state = State::BEFORE_LOW_SURROGATE;
          } else {
            spdlog::info("高位代理后必须紧跟低位代理");
            exit(1);
          }
          break;
        case State::BEFORE_LOW_SURROGATE:
          if (c == 'u') {
            state = State::IN_LOW_SURROGATE;
          } else {
            spdlog::info("高位代理后必须紧跟低位代理");
            exit(1);
          }
          break;
        case State::IN_LOW_SURROGATE:
          if (util::isHexDigit(c)) {
            unicodeBuffer += c;
          } else {
            spdlog::info("未知的 unicode 转义字符：{}", c);
            exit(1);
          }
          if (unicodeBuffer.size() == 4) {
            auto codePoint = util::strToCodePoint(unicodeBuffer);
            unicodeBuffer.clear();
            if (util::isLowSurrogate(codePoint)) {
              state = State::IN_STRING;
              auto cp = util::mergeSurrogate(highSurrogate, codePoint);
              buffer += util::codePointToUtf8(cp);
              highSurrogate = 0;
            } else {
              spdlog::info("码点 {:#x} 不是低位代理", codePoint);
              exit(1);
            }
          }
          break;
      }
    }

    if (!unicodeBuffer.empty()) {
      spdlog::info("不完整的 unicode 转义序列：{}", unicodeBuffer);
      exit(1);
    }
    if (highSurrogate != 0) {
      spdlog::info("未配对的 unicode 转义序列");
      exit(1);
    }

    if (buffer.empty()) {
      return tokens;
    }

    switch (state) {
      case State::AFTER_NUMBER_INTEGER_SIGN:
      case State::AFTER_NUMBER_POINT:
      case State::IN_NUMBER_EXPONENT:
      case State::AFTER_NUMBER_EXPONENT_SIGN:
        spdlog::info("非法的数值：{}", buffer);
        exit(1);
      case State::AFTER_NUMBER_LEADING_ZERO:
      case State::IN_NUMBER_INTEGER:
      case State::AFTER_NUMBER_INTEGER:
      case State::IN_NUMBER_FRACTION_DIGIT:
      case State::AFTER_NUMBER_FRACTION:
      case State::IN_NUMBER_EXPONENT_DIGIT:
        tokens.push_back(std::make_unique<NumberToken>(buffer));
        buffer.clear();
        break;
      case State::IN_TRUE:
      case State::IN_FALSE:
      case State::IN_NULL:
        spdlog::info("不全的关键字：{}", buffer);
        exit(1);
      default:
        spdlog::info("未闭合的字符串：{}", buffer);
        exit(1);
    }

    return tokens;
  }
};
