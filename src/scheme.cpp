#include "scheme.h"
#include "error.h"
#include "object.h"
#include "scope.h"
#include "tokenizer.h"
#include "parser.h"
#include "runtime.h"

#include <exception>
#include <memory>
#include <sstream>

Interpreter::Interpreter() {
    ScopePtr global_scope = CreateGlobalScope();
    ScopeStack::GetInstance().Push(global_scope);
    GC::GetInstance().AddRoot(global_scope);
}

Interpreter::~Interpreter() {
    ScopeStack::GetInstance().Pop();
}

std::string Interpreter::Run(const std::string& str) {
    try {
        std::stringstream stream{str};
        Tokenizer tokenizer{&stream};
        ObjectPtr ast = Read(&tokenizer);
        if (!tokenizer.IsEnd()) {
            throw SyntaxError{"Not correct command"};
        }
        if (ast == nullptr) {
            throw SyntaxError{"No exression was provided"};
        }
        ObjectPtr res = ast->Eval();
        if (res == nullptr) {
            GC::GetInstance().Collect();
            return "()";
        }
        auto ans = res->ToString();
        res = nullptr;
        GC::GetInstance().Collect();
        return ans;
    } catch (std::exception) {
        GC::GetInstance().Collect();
        throw;
    }
}
