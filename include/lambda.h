#pragma once

#include <string>
#include <vector>
#include "object.h"
#include "scope.h"

class LambdaFunction : public Function {
public:
    LambdaFunction(const std::vector<std::string>& params, ObjectPtr body, ScopePtr scope);
    ObjectPtr operator()(ObjectPtr) override;
    std::string ToString() override {
        return "lambda_func";
    }

private:
    std::vector<std::string> params_;
    ObjectPtr body_;
    ScopePtr captured_scope_;
};

class Lambda : public Function {
public:
    ObjectPtr operator()(ObjectPtr args) override;
    std::string ToString() override {
        return "lambda";
    }
};