namespace token_symbols {

namespace alphabet {
constexpr const char kLowLettersStart = 'a';
constexpr const char kLowLettersEnd = 'z';
constexpr const char kBigLettersStart = 'A';
constexpr const char kBigLettersEnd = 'Z';
}  // namespace alphabet

namespace digits {
constexpr const char kDigitsStart = '0';
constexpr const char kDigitsEnd = '9';
}  // namespace digits

namespace one_symbol_tokens {
constexpr const char kOpenParen = '(';
constexpr const char kCloseParen = ')';
constexpr const char kQuote = '\'';
constexpr const char kDot = '.';
}  // namespace one_symbol_tokens

namespace math_ops {
constexpr const char kPlus = '+';
constexpr const char kMinus = '-';
constexpr const char kEqual = '=';
constexpr const char kLess = '<';
constexpr const char kGreater = '>';
constexpr const char kMultiply = '*';
constexpr const char kDiv = '/';
}  // namespace math_ops

namespace signs {
constexpr const char kPlus = '+';
constexpr const char kMinus = '-';
}  // namespace signs

namespace special_symbols {
constexpr const char kHash = '#';
constexpr const char kQuestionMark = '?';
constexpr const char kExclamation = '!';
}  // namespace special_symbols

}  // namespace token_symbols

enum class TokenTypes {
    Symbol,
    Number,
    Quote,
    Dot,
    Boolean,
    None,
};

bool IsValidTokenSymbol(char symbol);

bool IsDigit(char symbol);

bool IsSign(char symbol);
