#include "object.h"
#include <cstddef>
#include <string>
#include "error.h"
#include "scope.h"
#include "runtime.h"

// Object

void Object::RemoveDependency(ObjectPtr ptr) {
    dependencies.erase(ptr);
}

void Object::AddDependency(ObjectPtr ptr) {
    dependencies.insert(ptr);
}

void Object::Mark() {
    marked = true;
    for (ObjectPtr neighbour : dependencies) {
        if (neighbour == nullptr || neighbour->marked) {
            continue;
        }
        neighbour->Mark();
    }
}

// End of Object

// Number

Number::Number(int value) : value_(value) {
}

int Number::GetValue() const {
    return value_;
}

std::string Number::ToString() {
    return std::to_string(value_);
}

ObjectPtr Number::Eval() {
    return this;
}

// End of Number

// Symbol

Symbol::Symbol(const std::string& name) : name_(name) {
}

Symbol::Symbol(const char* name) : name_(name) {
}

Symbol::Symbol(bool value) {
    name_ = value ? kTrueSymbol : kFalseSymbol;
}

const std::string& Symbol::GetName() const {
    return name_;
}

ObjectPtr Symbol::Eval() {
    return ScopeStack::GetInstance().Current()->Get(GetName());
}

// End of Symbol

// Cell

Cell::Cell(ObjectPtr first, ObjectPtr second) : first_(first), second_(second) {
    AddDependency(first_);
    AddDependency(second_);
}

Cell::Cell(ObjectPtr first, std::nullptr_t) : first_(first), second_(nullptr) {
    AddDependency(first);
}

ObjectPtr Cell::GetFirst() const {
    return first_;
}

ObjectPtr Cell::GetSecond() const {
    return second_;
}

void Cell::SetFirst(const ObjectPtr& obj) {
    if (first_ != nullptr) {
        RemoveDependency(first_);
    }
    first_ = obj;
    AddDependency(first_);
}

void Cell::SetSecond(const ObjectPtr& obj) {
    if (second_ != nullptr) {
        RemoveDependency(second_);
    }
    second_ = obj;
    AddDependency(second_);
}

ObjectPtr Cell::Eval() {
    if (GetFirst() == nullptr) {
        throw RuntimeError{"Nothing to calculate"};
    }
    ObjectPtr func = GetFirst()->Eval();
    if (!Is<Function>(func)) {
        throw RuntimeError{"Cell is not evaluating"};
    }
    return (*As<Function>(func))(GetSecond());
}

std::string Cell::ToString() {
    std::string res = "(";
    ObjectPtr cur_obj = this;
    while (cur_obj != nullptr) {
        if (!Is<Cell>(cur_obj)) {
            res += ". ";
            res += cur_obj->ToString();
            break;
        }
        CellPtr list = As<Cell>(cur_obj);
        if (list->GetFirst() == nullptr) {
            res += "()";
        } else {
            res += list->GetFirst()->ToString();
        }
        if (list->GetSecond() != nullptr) {
            res.push_back(' ');
        }
        cur_obj = list->GetSecond();
    }
    return res + ")";
}

// End of Cell

// Function

ObjectPtr Function::Eval() {
    return this;
}

// End of Function

bool ToBoolean(ObjectPtr obj) {
    if (Is<Symbol>(obj)) {
        return As<Symbol>(obj)->GetName() != kFalseSymbol;
    }
    return true;
}

void Visitor::VisitCell(ObjectPtr obj) const {
    while (obj != nullptr) {
        if (Is<Cell>(obj)) {
            first_call_(As<Cell>(obj)->GetFirst());
            obj = As<Cell>(obj)->GetSecond();
        } else {
            second_call_(As<Object>(obj));
            obj = nullptr;
        }
    }
}

ObjectPtr DeepCopy(ObjectPtr obj) {
    if (obj == nullptr) {
        return nullptr;
    }

    if (Is<Number>(obj)) {
        return runtime::New<Number>(As<Number>(obj)->GetValue());
    }

    if (Is<Symbol>(obj)) {
        return runtime::New<Symbol>(As<Symbol>(obj)->GetName());
    }

    if (Is<Cell>(obj)) {
        ObjectPtr first_copy = DeepCopy(As<Cell>(obj)->GetFirst());
        ObjectPtr second_copy = DeepCopy(As<Cell>(obj)->GetSecond());
        return runtime::New<Cell>(first_copy, second_copy);
    }

    if (Is<Function>(obj)) {
        return obj;
    }

    throw RuntimeError{"Unsupported object type for deep copy"};
}