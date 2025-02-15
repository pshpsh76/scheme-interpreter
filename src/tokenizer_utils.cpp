#include "tokenizer_utils.h"

bool IsAlphabet(char symbol) {
    return (symbol >= token_symbols::alphabet::kLowLettersStart &&
            symbol <= token_symbols::alphabet::kLowLettersEnd) ||
           (symbol >= token_symbols::alphabet::kBigLettersStart &&
            symbol <= token_symbols::alphabet::kBigLettersEnd);
}

bool IsDigit(char symbol) {
    return symbol >= token_symbols::digits::kDigitsStart &&
           symbol <= token_symbols::digits::kDigitsEnd;
}

bool IsSign(char symbol) {
    return symbol == token_symbols::signs::kPlus || symbol == token_symbols::signs::kMinus;
}

bool IsNumberToken(char symbol) {
    return IsDigit(symbol) || IsSign(symbol);
}

bool IsOneSymbolToken(char symbol) {
    switch (symbol) {
        case token_symbols::one_symbol_tokens::kOpenParen:
            return true;
        case token_symbols::one_symbol_tokens::kCloseParen:
            return true;
        case token_symbols::one_symbol_tokens::kDot:
            return true;
        case token_symbols::one_symbol_tokens::kQuote:
            return true;
        default:
            return false;
    }
    return false;
}

bool IsMathOp(char symbol) {
    switch (symbol) {
        case token_symbols::math_ops::kDiv:
            return true;
        case token_symbols::math_ops::kMultiply:
            return true;
        case token_symbols::math_ops::kPlus:
            return true;
        case token_symbols::math_ops::kMinus:
            return true;
        case token_symbols::math_ops::kLess:
            return true;
        case token_symbols::math_ops::kGreater:
            return true;
        case token_symbols::math_ops::kEqual:
            return true;
        default:
            return false;
    }
    return false;
}

bool IsSpecialSymbolStart(char symbol) {
    switch (symbol) {
        case token_symbols::special_symbols::kHash:
            return true;
        default:
            return false;
    }
}

bool IsSpecialSymbol(char symbol) {
    switch (symbol) {
        case token_symbols::special_symbols::kHash:
            return true;
        case token_symbols::special_symbols::kQuestionMark:
            return true;
        case token_symbols::special_symbols::kExclamation:
            return true;
        default:
            return false;
    }
}

bool IsValidTokenStart(char symbol) {
    return IsAlphabet(symbol) || IsDigit(symbol) || IsMathOp(symbol) || IsOneSymbolToken(symbol) ||
           IsSpecialSymbolStart(symbol);
}

bool IsValidTokenSymbol(char symbol) {
    return IsAlphabet(symbol) || IsDigit(symbol) || IsMathOp(symbol) || IsSpecialSymbol(symbol);
}