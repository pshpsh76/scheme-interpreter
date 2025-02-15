#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include "object.h"
#include "runtime.h"

class Quote : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "'";
    }
};

class NumPredicate : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "number?";
    }
};

template <typename T>
struct ReturnType;

template <typename R, typename... Args>
struct ReturnType<R(Args...)> {
    using Type = R;
};

template <typename bin_op, size_t min_cnt = 0, size_t max_cnt = kMaxArgs, int init = 1>
class Folder : public Function {
    ObjectPtr operator()(ObjectPtr obj) override {
        auto args = GetSafeArgs<true, Number>(obj, min_cnt, max_cnt);
        bin_op func;
        using RetType = decltype(func(0, 0));
        if constexpr (std::is_same_v<RetType, bool>) {
            bool res = init;
            for (size_t i = 1; i < args.size(); ++i) {
                if (!func(As<Number>(args[i - 1])->GetValue(), As<Number>(args[i])->GetValue())) {
                    res = false;
                }
            }
            return runtime::New<Symbol>(res);
        } else {
            int res;
            size_t start_index = 0;
            if constexpr (min_cnt >= 2) {
                res = func(As<Number>(args[0])->GetValue(), As<Number>(args[1])->GetValue());
                start_index = 2;
            } else {
                res = init;
            }
            for (size_t i = start_index; i < args.size(); ++i) {
                res = func(res, As<Number>(args[i])->GetValue());
            }
            return runtime::New<Number>(res);
        }
    }
    std::string ToString() override {
        return "";
    }
};

// Compare Numbers
using Equal = Folder<std::equal_to<int>>;
using Greater = Folder<std::greater<int>>;
using Less = Folder<std::less<int>>;
using GreaterEq = Folder<std::greater_equal<int>>;
using LessEq = Folder<std::less_equal<int>>;

// Calc Numbers
using Plus = Folder<std::plus<int>, 0, kMaxArgs, 0>;
using Minus = Folder<std::minus<int>, 2, kMaxArgs, 0>;
using Multiply = Folder<std::multiplies<int>, 0, kMaxArgs, 1>;
using Div = Folder<std::divides<int>, 2, kMaxArgs, 0>;

// Special Functions

template <typename T>
class MaxFunctor {
public:
    T operator()(const T& lhs, const T& rhs) {
        return std::max(lhs, rhs);
    }
};

template <typename T>
class MinFunctor {
public:
    T operator()(const T& lhs, const T& rhs) {
        return std::min(lhs, rhs);
    }
};

using Max = Folder<MaxFunctor<int>, 1, kMaxArgs, std::numeric_limits<int>::min()>;
using Min = Folder<MinFunctor<int>, 1, kMaxArgs, std::numeric_limits<int>::max()>;

class Abs : public Function {
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "abs";
    }
};

// Boolean
class BoolPredicate : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "boolean?";
    }
};

class LogicalNot : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "not";
    }
};

class LogicalAnd : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "and";
    }
};

class LogicalOr : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "or";
    }
};

// List functions

class PairPredicate : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "pair?";
    }
};

class NullPredicate : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "null?";
    }
};

class ListPredicate : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "list?";
    }
};

class PairCons : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "cons";
    }
};

class PairCar : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "car";
    }
};

class PairCdr : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "cdr";
    }
};

class ListCtor : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "list";
    }
};

class ListRef : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "list-ref";
    }
};

class ListTail : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "list-tail";
    }
};

// If

class If : public Function {
public:
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "if";
    }
};

// Variables

class SymbolPredicate : public Function {
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "symbol?";
    }
};

class Define : public Function {
    ObjectPtr operator()(ObjectPtr) override;
    ObjectPtr DefineLambda(ObjectPtr);
    std::string ToString() override {
        return "define";
    }
};

class Set : public Function {
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "set!";
    }
};

class SetCar : public Function {
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "set-car!";
    }
};

class SetCdr : public Function {
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "set-cdr!";
    }
};