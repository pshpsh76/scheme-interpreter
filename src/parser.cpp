#include <parser.h>
#include "error.h"
#include "object.h"
#include "tokenizer.h"
#include "runtime.h"

ObjectPtr Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError{"Empty object"};
    }
    Token token = tokenizer->GetToken();
    tokenizer->Next();
    if (BracketToken* bracket = std::get_if<BracketToken>(&token)) {
        if (*bracket == BracketToken::OPEN) {
            return ReadList(tokenizer);
        }
        throw SyntaxError{"Close bracket before open"};
    } else if (ConstantToken* number = std::get_if<ConstantToken>(&token)) {
        return runtime::New<Number>(number->value);
    } else if (DotToken* dot = std::get_if<DotToken>(&token)) {
        throw SyntaxError{"Dot without cell"};
    } else if (std::get_if<QuoteToken>(&token)) {
        CellPtr res = runtime::New<Cell>(runtime::New<Symbol>("quote"), nullptr);
        ObjectPtr obj = Read(tokenizer);
        res->SetSecond(runtime::New<Cell>(obj, nullptr));
        return res;
    } else if (SymbolToken* symbol = std::get_if<SymbolToken>(&token)) {
        return runtime::New<Symbol>(symbol->name);
    }
    throw SyntaxError{"Undefined token"};
}

CellPtr ReadList(Tokenizer* tokenizer) {
    Token token = tokenizer->GetToken();
    if (IsEndOfCell(token)) {
        tokenizer->Next();
        return nullptr;
    }
    CellPtr main_list = runtime::New<Cell>();
    CellPtr cur_list = main_list;
    bool should_end = false;
    bool is_first_token = true;
    while (!tokenizer->IsEnd()) {
        if (IsEndOfCell(token)) {
            tokenizer->Next();
            if (!should_end) {
                cur_list->SetSecond(nullptr);
            }
            return main_list;
        }
        if (should_end) {
            NotFoundCloseBracket();
        }
        if (IsSeparator(token)) {
            if (is_first_token) {
                throw SyntaxError{"dot cannot be in list begin"};
            }
            tokenizer->Next();
            cur_list->SetSecond(Read(tokenizer));
            should_end = true;
        } else {
            cur_list->SetFirst(Read(tokenizer));
            cur_list->SetSecond(runtime::New<Cell>());
        }
        token = tokenizer->GetToken();
        if (IsStepInto(token)) {
            cur_list = As<Cell>(cur_list->GetSecond());
        }
        is_first_token = false;
    }
    if (!IsEndOfCell(token)) {
        NotFoundCloseBracket();
    }
    if (!should_end) {
        cur_list->SetSecond(nullptr);
    }
    tokenizer->Next();
    return main_list;
}

bool IsSeparator(const Token& token) {
    return std::get_if<DotToken>(&token) != nullptr;
}

bool IsEndOfCell(const Token& token) {
    if (const BracketToken* bracket = std::get_if<BracketToken>(&token)) {
        return *bracket == BracketToken::CLOSE;
    }
    return false;
}

bool IsStepInto(const Token& token) {
    return !IsSeparator(token) && !IsEndOfCell(token);
}

void NotFoundCloseBracket() {
    throw SyntaxError{"Not found close bracket"};
}