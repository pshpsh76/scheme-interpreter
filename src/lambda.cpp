#include "lambda.h"
#include <cstddef>
#include <memory>
#include <vector>
#include "error.h"
#include "object.h"
#include "runtime.h"
#include "scope.h"

LambdaFunction::LambdaFunction(const std::vector<std::string>& params, ObjectPtr body,
                               ScopePtr scope)
    : params_(params), body_(body), captured_scope_(scope) {
    AddDependency(scope);
    AddDependency(body);
}

ObjectPtr LambdaFunction::operator()(ObjectPtr obj) {
    auto args = GetSafeArgs(obj, params_.size(), params_.size());
    ScopePtr scope = runtime::New<Scope>(captured_scope_);
    ScopeGuard guard{scope};
    for (size_t i = 0; i < params_.size(); ++i) {
        scope->Define(params_[i], args[i]);
    }
    ObjectPtr res = GetArgsVector(body_).back();
    return res;
}

ObjectPtr Lambda::operator()(ObjectPtr obj) {
    if (!Is<Cell>(obj)) {
        throw SyntaxError{"Lambda wrong syntax"};
    }
    auto args = GetSafeArgs<true, Symbol, false>(As<Cell>(obj)->GetFirst(), 0, kMaxArgs);
    std::vector<std::string> params;
    for (const auto& symb : args) {
        params.push_back(As<Symbol>(symb)->GetName());
    }
    ObjectPtr body = As<Cell>(obj)->GetSecond();
    if (body == nullptr) {
        throw SyntaxError{"Lambda wrong syntax"};
    }
    ScopePtr current_scope = ScopeStack::GetInstance().Current();
    return runtime::New<LambdaFunction>(params, body, current_scope);
}