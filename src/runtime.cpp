#include "runtime.h"
#include <cstddef>
#include "object.h"

void GC::AddRoot(ObjectPtr ptr) {
    roots_.push_back(ptr);
}

void GC::Collect() {
    for (ObjectPtr obj : allocations_) {
        obj->marked = false;
    }
    for (ObjectPtr root : roots_) {
        if (root != nullptr) {
            root->Mark();
        }
    }
    for (size_t i = 0; i < allocations_.size(); ++i) {
        if (!allocations_[i]->marked) {
            std::swap(allocations_.back(), allocations_[i]);
            delete allocations_.back();
            allocations_.pop_back();
            --i;
        }
    }
}

GC::~GC() {
    for (ObjectPtr ptr : allocations_) {
        delete ptr;
    }
}