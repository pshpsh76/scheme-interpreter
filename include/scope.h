#pragma once

#include "object.h"
#include <stack>
#include <memory>

class Scope;
using ScopePtr = Scope*;

class Scope : public Object {
public:
    explicit Scope(ScopePtr parent = nullptr);
    void Define(const std::string& name, ObjectPtr obj);
    void Set(const std::string& name, ObjectPtr obj);
    ObjectPtr Get(const std::string& name);
    ObjectPtr Eval() override {
        return this;
    }
    std::string ToString() override {
        return "Scope";
    }

private:
    ScopePtr parent_;
    std::unordered_map<std::string, ObjectPtr> mapping_;
};

class ScopeStack {
public:
    static ScopeStack& GetInstance() {
        static ScopeStack instance;
        return instance;
    }

    ScopePtr Current();

    void Push(ScopePtr scope);

    void Pop();

private:
    ScopeStack() = default;
    std::stack<ScopePtr> scopes_;
};

class ScopeGuard {
public:
    explicit ScopeGuard(ScopePtr scope) {
        ScopeStack::GetInstance().Push(scope);
    }

    ~ScopeGuard() {
        ScopeStack::GetInstance().Pop();
    }

    ScopePtr CurrentScope() {
        return ScopeStack::GetInstance().Current();
    }
};

ScopePtr CreateGlobalScope();