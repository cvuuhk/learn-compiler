#include <spdlog/spdlog.h>
#include "JsonLexer.h"

int main(int argc, char* argv[]) {
  std::string input = R"(
    {
      "object": {
        "nested": {
          "key": "value"
        }
      },
      "array": [
        "first",
        "second",
        {
          "inner": "element"
        },
        [
          "nested",
          "array"
        ]
      ],
      "string": "A plain string",
      "escaped": "Line 1\nLine 2\tTabbed\rCarriage return\\Backslash\"Quoted",
      "unicode": "\u0048\u0065\u006c\u006c\u006f \u4e16\u754c",
      "surrogate_pair": "\ud83d\ude00\ud83c\udf08",
      "empty_string": "",
      "null_value": null,
      "boolean_true": true,
      "boolean_false": false,
      "vv": [ 42, -17, 0, 3.14159, -2.71828, 1e10, -5e-3, 6.022e23, -1.6e-19, 1.0, -0, 9007199254740991, -9007199254740991, 123.456e789, -123.456e-789 ]
    })";
  spdlog::info("input = {}", input);

  JsonLexer lexer;
  auto tokens = lexer.lex(input);
  for (const auto& token : tokens) {
    spdlog::info("{}", token->display());
  }
}



