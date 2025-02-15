#pragma once

#include <unordered_map>
#include <variant>
#include <istream>
#include <cstdio>
#include <string>

#include "tokenizer_utils.h"

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const;
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

const std::unordered_map<char, Token> kOneSymbolMapping = {
    {'(', Token{BracketToken::OPEN}},
    {')', Token{BracketToken::CLOSE}},
    {'.', Token{DotToken{}}},
    {'\'', Token{QuoteToken{}}},
};

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    Token GetTokenMove();

    std::istream* in_;
    Token last_token_;
    bool empty_;
};

bool IsValidTokenStart(char symbol);

bool IsAlphabet(char symbol);

bool IsDigit(char symbol);

bool IsOneSymbolToken(char symbol);

bool IsMathOp(char symbol);

bool IsSpecialSymbolStart(char symbol);