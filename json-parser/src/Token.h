#pragma once

#include <string>

class Token {
  protected:
    std::string value;
  public:
    Token(const std::string& val) : value(val) {}
    virtual ~Token() = default;
    virtual std::string format() const = 0;
    virtual std::string display() const = 0;
};

class ObjectStartToken : public Token {
  public:
    ObjectStartToken() : Token("{") {}
    std::string format() const override {
      return this->value;
    }
    std::string display() const override {
      return "ObjectStartToken(" + this->value + ")";
    }
};

class ObjectEndToken : public Token {
  public:
    ObjectEndToken() : Token("}") {}
    std::string format() const override {
      return this->value;
    }
    std::string display() const override {
      return "ObjectEndToken(" + this->value + ")";
    }
};

class ArrayStartToken : public Token {
  public:
    ArrayStartToken() : Token("[") {}
    std::string format() const override {
      return this->value;
    }
    std::string display() const override {
      return "ArrayStartToken(" + this->value + ")";
    }
};

class ArrayEndToken : public Token {
  public:
    ArrayEndToken() : Token("]") {}
    std::string format() const override {
      return this->value;
    }
    std::string display() const override {
      return "ArrayEndToken(" + this->value + ")";
    }
};

class ColonToken : public Token {
  public:
    ColonToken() : Token(":") {}
    std::string format() const override {
      return this->value;
    }
    std::string display() const override {
      return "ColonToken(" + this->value + ")";
    }
};

class CommaToken : public Token {
  public:
    CommaToken() : Token(",") {}
    std::string format() const override {
      return this->value;
    }
    std::string display() const override {
      return "CommaToken(" + this->value + ")";
    }
};

class StringToken : public Token {
  public:
    StringToken(const std::string& value) : Token(value) {}
    std::string format() const override {
      return "\"" + this->value + "\"";
    }
    std::string display() const override {
      return "StringToken(" + this->value + ")";
    }
};

class NumberToken : public Token {
  public:
    NumberToken(const std::string& value) : Token(value) {}
    std::string format() const override {
      return this->value;
    }
    std::string display() const override {
      return "NumberToken(" + this->value + ")";
    }
};

class BooleanToken : public Token {
  public:
    BooleanToken(bool isTrue) : Token(isTrue ? "true" : "false") {}
    std::string format() const override {
      return this->value;
    }
    std::string display() const override {
      return "BooleanToken(" + this->value + ")";
    }
};

class NullToken : public Token {
  public:
    NullToken() : Token("null") {}
    std::string format() const override {
      return this->value;
    }
    std::string display() const override {
      return "NullToken(" + this->value + ")";
    }
};
