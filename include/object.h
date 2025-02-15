#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>
#include <numeric>
#include "error.h"
#include "scheme_fwd.h"

class Object;
using ObjectPtr = Object*;

class Object {
public:
    virtual ~Object() = default;
    virtual ObjectPtr Eval() = 0;
    virtual std::string ToString() = 0;

protected:
    Object() = default;

    bool marked = true;
    std::unordered_set<ObjectPtr> dependencies;
    void AddDependency(ObjectPtr);
    void RemoveDependency(ObjectPtr);
    void Mark();
    friend GC;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and conversion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
T* As(const ObjectPtr& obj) {
    return dynamic_cast<T*>(obj);
}

template <class T>
bool Is(const ObjectPtr& obj) {
    return dynamic_cast<T*>(obj) != nullptr;
}

///////////////////////////////////////////////////////////////////////////////

class Number : public Object {
public:
    explicit Number(int);
    int GetValue() const;
    ObjectPtr Eval() override;
    std::string ToString() override;

private:
    int value_;
};

class Symbol : public Object {
public:
    explicit Symbol(const std::string&);
    explicit Symbol(const char*);
    explicit Symbol(bool);
    const std::string& GetName() const;
    ObjectPtr Eval() override;
    std::string ToString() override {
        return name_;
    }

private:
    std::string name_;
};

const std::string kTrueSymbol = "#t";
const std::string kFalseSymbol = "#f";

class Cell : public Object {
public:
    Cell() = default;
    Cell(ObjectPtr, ObjectPtr);
    Cell(ObjectPtr, std::nullptr_t);
    ObjectPtr GetFirst() const;
    ObjectPtr GetSecond() const;
    void SetFirst(const ObjectPtr&);
    void SetSecond(const ObjectPtr&);
    ObjectPtr Eval() override;
    std::string ToString() override;  // TODO
private:
    ObjectPtr first_, second_;
};

using CellPtr = Cell*;

class Function : public Object {
public:
    ObjectPtr Eval() override;
    virtual ObjectPtr operator()(ObjectPtr) = 0;
    std::string ToString() override = 0;
};

using FunctionPtr = Function*;

using Args = std::vector<ObjectPtr>;

template <bool IsEval = true>
Args GetArgsVector(ObjectPtr arg_list) {
    std::vector<ObjectPtr> args;
    while (arg_list != nullptr) {
        if (Is<Cell>(arg_list)) {
            auto cell = As<Cell>(arg_list);
            if constexpr (IsEval) {
                if (cell->GetFirst() != nullptr) {
                    args.push_back(cell->GetFirst()->Eval());
                } else {
                    args.push_back(nullptr);
                }
            } else {
                args.push_back(cell->GetFirst());
            }
            arg_list = cell->GetSecond();
        } else {
            if constexpr (IsEval) {
                args.push_back(arg_list->Eval());
            } else {
                args.push_back(arg_list);
            }
            arg_list = nullptr;
        }
    }
    return args;
}

template <typename Type>
bool CheckArgsType(const Args& args) {
    for (const auto& arg : args) {
        if (!Is<Type>(arg)) {
            return false;
        }
    }
    return true;
}

template <bool CheckType = false, typename Type = std::false_type, typename Error = RuntimeError>
void EnsureArgs(const Args& args, size_t min_cnt, size_t max_cnt) {
    if constexpr (CheckType) {
        if (!CheckArgsType<Type>(args)) {
            throw Error{"Wrong argument types"};
        }
    }
    if (args.size() > max_cnt || args.size() < min_cnt) {
        throw Error{"Wrong argument number"};
    }
}

template <bool CheckType = false, typename Type = std::false_type, bool IsEval = true,
          typename Error = RuntimeError>
Args GetSafeArgs(ObjectPtr obj, size_t min_cnt, size_t max_cnt) {
    auto res = GetArgsVector<IsEval>(obj);
    EnsureArgs<CheckType, Type, Error>(res, min_cnt, max_cnt);

    return res;
}

constexpr size_t kMaxArgs = std::numeric_limits<size_t>::max();

class Visitor {
public:
    Visitor(const std::function<void(ObjectPtr)>& first,
            const std::function<void(ObjectPtr)>& second)
        : first_call_(first), second_call_(second) {
    }
    void VisitCell(ObjectPtr obj) const;

private:
    const std::function<void(ObjectPtr)>&first_call_, second_call_;
};

bool ToBoolean(ObjectPtr);

ObjectPtr DeepCopy(ObjectPtr obj);
