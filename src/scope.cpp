#include "scope.h"
#include "runtime.h"
#include "lib.h"
#include "lambda.h"

Scope::Scope(ScopePtr parent) : parent_(parent) {
}

void Scope::Define(const std::string& name, ObjectPtr obj) {
    AddDependency(obj);
    mapping_[name] = obj;
}

void Scope::Set(const std::string& name, ObjectPtr obj) {
    if (mapping_.contains(name)) {
        RemoveDependency(mapping_[name]);
        mapping_[name] = obj;
        AddDependency(obj);
    } else if (parent_) {
        parent_->Set(name, obj);
    } else {
        throw NameError{"Symbol " + name + " not defined"};
    }
}

ObjectPtr Scope::Get(const std::string& name) {
    if (mapping_.contains(name)) {
        return mapping_[name];
    } else if (parent_) {
        return parent_->Get(name);
    } else {
        throw NameError{"Symbol " + name + " not defined"};
    }
}

ScopePtr ScopeStack::Current() {
    if (scopes_.empty()) {
        throw std::runtime_error{"Scope stack is empty"};
    }
    return scopes_.top();
}

void ScopeStack::Push(ScopePtr scope) {
    scopes_.push(scope);
}

void ScopeStack::Pop() {
    if (scopes_.empty()) {
        throw std::runtime_error{"Scope stack is empty"};
    }
    scopes_.pop();
}

ScopePtr CreateGlobalScope() {
    ScopePtr global_scope = runtime::New<Scope>();

    global_scope->Define(kTrueSymbol, runtime::New<Symbol>(kTrueSymbol));
    global_scope->Define(kFalseSymbol, runtime::New<Symbol>(kFalseSymbol));

    global_scope->Define("quote", runtime::New<Quote>());
    global_scope->Define("number?", runtime::New<NumPredicate>());
    global_scope->Define("=", runtime::New<Equal>());
    global_scope->Define(">", runtime::New<Greater>());
    global_scope->Define("<", runtime::New<Less>());
    global_scope->Define(">=", runtime::New<GreaterEq>());
    global_scope->Define("<=", runtime::New<LessEq>());
    global_scope->Define("+", runtime::New<Plus>());
    global_scope->Define("-", runtime::New<Minus>());
    global_scope->Define("*", runtime::New<Multiply>());
    global_scope->Define("/", runtime::New<Div>());
    global_scope->Define("max", runtime::New<Max>());
    global_scope->Define("min", runtime::New<Min>());
    global_scope->Define("abs", runtime::New<Abs>());
    global_scope->Define("boolean?", runtime::New<BoolPredicate>());
    global_scope->Define("not", runtime::New<LogicalNot>());
    global_scope->Define("and", runtime::New<LogicalAnd>());
    global_scope->Define("or", runtime::New<LogicalOr>());
    global_scope->Define("pair?", runtime::New<PairPredicate>());
    global_scope->Define("null?", runtime::New<NullPredicate>());
    global_scope->Define("list?", runtime::New<ListPredicate>());
    global_scope->Define("cons", runtime::New<PairCons>());
    global_scope->Define("car", runtime::New<PairCar>());
    global_scope->Define("cdr", runtime::New<PairCdr>());
    global_scope->Define("list", runtime::New<ListCtor>());
    global_scope->Define("list-ref", runtime::New<ListRef>());
    global_scope->Define("list-tail", runtime::New<ListTail>());
    global_scope->Define("if", runtime::New<If>());
    global_scope->Define("symbol?", runtime::New<SymbolPredicate>());
    global_scope->Define("define", runtime::New<Define>());
    global_scope->Define("set!", runtime::New<Set>());
    global_scope->Define("set-car!", runtime::New<SetCar>());
    global_scope->Define("set-cdr!", runtime::New<SetCdr>());
    global_scope->Define("lambda", runtime::New<Lambda>());

    return global_scope;
}