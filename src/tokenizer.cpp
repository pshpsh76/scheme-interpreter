#include <tokenizer.h>

Tokenizer::Tokenizer(std::istream* in) : in_(in) {
    Next();
}

bool Tokenizer::IsEnd() {
    return in_->peek() == EOF;
}

void Tokenizer::Next() {
    empty_ = true;
    while (!IsEnd()) {
        char symbol = in_->peek();
        if (IsValidTokenStart(symbol)) {
            break;
        }
        in_->get();
    }
}

Token Tokenizer::GetTokenMove() {
    char first_symbol = in_->peek();
    if (IsOneSymbolToken(first_symbol)) {
        in_->get();
        return kOneSymbolMapping.at(first_symbol);
    }
    in_->get();
    if (IsDigit(first_symbol) || (IsSign(first_symbol) && IsDigit(in_->peek()))) {
        int sign = 1;
        int value = 0;
        if (first_symbol == token_symbols::signs::kMinus) {
            sign = -1;
        } else if (!IsSign(first_symbol)) {
            value = first_symbol - token_symbols::digits::kDigitsStart;
        }
        while (IsDigit(in_->peek())) {
            char digit = in_->get();
            value = value * 10 + (digit - token_symbols::digits::kDigitsStart);
        }
        return Token{ConstantToken{value * sign}};
    }
    std::string str_token;
    str_token.push_back(first_symbol);
    if (str_token == "+" || str_token == "-") {
        return Token{SymbolToken{str_token}};
    }
    while (!IsEnd()) {
        char symbol = in_->peek();
        if (!IsValidTokenSymbol(symbol)) {
            break;
        }
        in_->get();
        str_token.push_back(symbol);
    }
    return Token{SymbolToken{str_token}};
}

Token Tokenizer::GetToken() {
    if (!empty_) {
        return last_token_;
    }
    last_token_ = GetTokenMove();
    empty_ = false;
    return last_token_;
}

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
}
