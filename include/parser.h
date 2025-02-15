#pragma once

#include "object.h"
#include "tokenizer.h"

ObjectPtr Read(Tokenizer* tokenizer);

CellPtr ReadList(Tokenizer* tokenizer);

bool IsEndOfCell(const Token& token);

bool IsSeparator(const Token& token);

bool IsStepInto(const Token& token);

void NotFoundCloseBracket();