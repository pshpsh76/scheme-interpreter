#include "lib.h"
#include <cstddef>
#include <functional>
#include <limits>
#include <type_traits>
#include <vector>
#include "error.h"
#include "lambda.h"
#include "object.h"
#include "runtime.h"
#include "scope.h"

// Quote

ObjectPtr Quote::operator()(ObjectPtr obj) {
    if (obj == nullptr) {
        throw RuntimeError{"Invalid args for quote"};
    }
    if (!Is<Cell>(obj)) {
        throw RuntimeError{"Invalid args type for quote"};
    }
    return As<Cell>(obj)->GetFirst();
}

// End of Quote

// NumPredicate

ObjectPtr NumPredicate::operator()(ObjectPtr obj) {
    auto arg = GetSafeArgs(obj, 1, 1)[0];
    return runtime::New<Symbol>(Is<Number>(arg));
}

// End of NumPredicate

// Abs

ObjectPtr Abs::operator()(ObjectPtr obj) {
    auto arg = GetSafeArgs<true, Number>(obj, 1, 1)[0];
    return runtime::New<Number>(std::abs(As<Number>(arg)->GetValue()));
}

// End of Abs

// Boolean
ObjectPtr BoolPredicate::operator()(ObjectPtr obj) {
    auto arg = GetSafeArgs(obj, 1, 1)[0];
    // return runtime::New<Symbol>(Is<True>(arg) || Is<False>(arg));
    bool res = Is<Symbol>(arg);
    if (!res) {
        return runtime::New<Symbol>(false);
    }
    const std::string& name = As<Symbol>(arg)->GetName();
    return runtime::New<Symbol>(name == kTrueSymbol || name == kFalseSymbol);
}

ObjectPtr LogicalNot::operator()(ObjectPtr obj) {
    auto arg = GetSafeArgs(obj, 1, 1)[0];
    return runtime::New<Symbol>(!ToBoolean(arg));
}

ObjectPtr LogicalAnd::operator()(ObjectPtr obj) {
    auto args =
        GetSafeArgs<false, std::false_type, false>(obj, 0, std::numeric_limits<size_t>::max());
    ObjectPtr res = runtime::New<Symbol>(true);
    for (size_t i = 0; i < args.size(); ++i) {
        res = args[i]->Eval();
        if (!ToBoolean(res)) {
            return res;
        }
    }
    return res;
}

ObjectPtr LogicalOr::operator()(ObjectPtr obj) {
    auto args =
        GetSafeArgs<false, std::false_type, false>(obj, 0, std::numeric_limits<size_t>::max());
    ObjectPtr res = runtime::New<Symbol>(false);
    for (size_t i = 0; i < args.size(); ++i) {
        res = args[i]->Eval();
        if (ToBoolean(res)) {
            return res;
        }
    }
    return res;
}

// List functions

const std::function<void(ObjectPtr)> kDummy = [](ObjectPtr) { return; };

std::pair<int, bool> GetListInfo(CellPtr arg) {
    int cnt_levels = 0;
    bool sec_non_null = false;
    std::function<void(ObjectPtr)> count = [&cnt_levels](ObjectPtr) { ++cnt_levels; };
    std::function<void(ObjectPtr)> check_null = [&sec_non_null](ObjectPtr obj) {
        sec_non_null = obj != nullptr;
    };
    Visitor visitor(count, check_null);
    visitor.VisitCell(arg);
    return {cnt_levels, sec_non_null};
}

ObjectPtr PairPredicate::operator()(ObjectPtr obj) {
    auto arg = As<Cell>(GetSafeArgs(obj, 1, 1)[0]);
    const auto [cnt_levels, sec_non_null] = GetListInfo(arg);
    return runtime::New<Symbol>(cnt_levels == 2 || (sec_non_null && cnt_levels == 1));
}

ObjectPtr NullPredicate::operator()(ObjectPtr obj) {
    auto arg = As<Cell>(GetSafeArgs(obj, 1, 1)[0]);
    return runtime::New<Symbol>(arg == nullptr);
}

ObjectPtr ListPredicate::operator()(ObjectPtr obj) {
    auto arg = As<Cell>(GetSafeArgs(obj, 1, 1)[0]);
    const auto [cnt_levels, sec_non_null] = GetListInfo(arg);
    return runtime::New<Symbol>(arg == nullptr || !sec_non_null);
}

ObjectPtr PairCons::operator()(ObjectPtr obj) {
    auto args = GetSafeArgs(obj, 2, 2);
    return runtime::New<Cell>(args[0], args[1]);
}

ObjectPtr PairCar::operator()(ObjectPtr obj) {
    auto first = As<Cell>(GetSafeArgs<true, Cell>(obj, 1, 1)[0])->GetFirst();
    return first;
}

ObjectPtr PairCdr::operator()(ObjectPtr obj) {
    auto second = As<Cell>(GetSafeArgs<true, Cell>(obj, 1, 1)[0])->GetSecond();
    return second;
}

ObjectPtr ListCtor::operator()(ObjectPtr obj) {
    return obj;
}

ObjectPtr ListRef::operator()(ObjectPtr obj) {
    auto args = GetSafeArgs(obj, 2, 2);
    if (!Is<Cell>(args[0]) || !Is<Number>(args[1])) {
        throw RuntimeError{"Wrong types for list-ref"};
    }
    size_t index = As<Number>(args[1])->GetValue();
    auto element = GetSafeArgs(args[0], index + 1, kMaxArgs)[index];
    return element;
}

ObjectPtr ListTail::operator()(ObjectPtr obj) {
    auto args = GetSafeArgs(obj, 2, 2);
    if (!Is<Cell>(args[0]) || !Is<Number>(args[1])) {
        throw RuntimeError{"Wrong types for list-tail"};
    }
    size_t index = As<Number>(args[1])->GetValue();
    auto full_list = GetSafeArgs(args[0], index, kMaxArgs);
    if (index == full_list.size()) {
        return nullptr;
    }
    CellPtr main_tail = runtime::New<Cell>();
    CellPtr cur_tail = main_tail;
    for (size_t i = index; i < full_list.size(); ++i) {
        cur_tail->SetFirst(full_list[i]);
        if (i < full_list.size() - 1) {
            cur_tail->SetSecond(runtime::New<Cell>());
            cur_tail = As<Cell>(cur_tail->GetSecond());
        }
    }
    return main_tail;
}

// If

ObjectPtr If::operator()(ObjectPtr obj) {
    Args args;
    args = GetSafeArgs<false, std::false_type, false, SyntaxError>(obj, 2, 3);
    bool value = ToBoolean(args[0]->Eval());
    if (value) {
        return args[1]->Eval();
    } else if (args.size() > 2) {
        return args[2]->Eval();
    }
    return nullptr;
}

// Variables

ObjectPtr SymbolPredicate::operator()(ObjectPtr obj) {
    ObjectPtr arg = GetSafeArgs(obj, 1, 1)[0];
    return runtime::New<Symbol>(Is<Symbol>(arg));
}

ObjectPtr Define::operator()(ObjectPtr obj) {
    Args args = GetSafeArgs<false, std::false_type, false, SyntaxError>(obj, 2, kMaxArgs);
    if (!Is<Symbol>(args[0])) {
        return DefineLambda(obj);
    }
    if (args.size() > 2) {
        throw SyntaxError{"Not correct args count define"};
    }
    ScopeStack::GetInstance().Current()->Define(As<Symbol>(args[0])->GetName(),
                                                DeepCopy(args[1]->Eval()));
    return nullptr;
}

ObjectPtr Define::DefineLambda(ObjectPtr obj) {
    if (!Is<Cell>(obj)) {
        throw SyntaxError{"Not correct defining lambda"};
    }
    Args args =
        (GetSafeArgs<true, Symbol, false, SyntaxError>(As<Cell>(obj)->GetFirst(), 1, kMaxArgs));
    std::string name = As<Symbol>(args[0])->GetName();
    std::vector<std::string> params;
    for (size_t i = 1; i < args.size(); ++i) {
        params.push_back(As<Symbol>(args[i])->GetName());
    }
    ObjectPtr body = As<Cell>(obj)->GetSecond();
    if (body == nullptr) {
        throw SyntaxError{"Lambda wrong syntax"};
    }
    ScopeStack::GetInstance().Current()->Define(
        name, runtime::New<LambdaFunction>(params, body, ScopeStack::GetInstance().Current()));
    return nullptr;
}

ObjectPtr Set::operator()(ObjectPtr obj) {
    Args args = GetSafeArgs<false, std::false_type, false, SyntaxError>(obj, 2, 2);
    if (!Is<Symbol>(args[0])) {
        throw SyntaxError{"You cannot set not Symbol"};
    }
    ScopeStack::GetInstance().Current()->Set(As<Symbol>(args[0])->GetName(),
                                             DeepCopy(args[1]->Eval()));
    return nullptr;
}

ObjectPtr SetCar::operator()(ObjectPtr obj) {
    Args args = GetSafeArgs<false, std::false_type, false, SyntaxError>(obj, 2, 2);
    ObjectPtr var = args[0]->Eval();
    if (!Is<Cell>(var)) {
        throw SyntaxError{"This command only for pairs"};
    }
    CellPtr cell = As<Cell>(var);
    // cell->SetFirst(DeepCopy(args[1]->Eval()));
    cell->SetFirst(args[1]->Eval());
    return nullptr;
}

ObjectPtr SetCdr::operator()(ObjectPtr obj) {
    Args args = GetSafeArgs<false, std::false_type, false, SyntaxError>(obj, 2, 2);
    ObjectPtr var = args[0]->Eval();
    if (!Is<Cell>(var)) {
        throw SyntaxError{"This command only for pairs"};
    }
    // As<Cell>(var)->SetSecond(DeepCopy(args[1]->Eval()));
    As<Cell>(var)->SetSecond(args[1]->Eval());
    return nullptr;
}