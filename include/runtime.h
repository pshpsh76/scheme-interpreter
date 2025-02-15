#pragma once

#include <concepts>
#include <utility>
#include <vector>
#include "object.h"

template <typename T>
concept DerivedFrom = std::derived_from<T, Object>;

class GC {
public:
    static GC& GetInstance() {
        static GC gc;
        return gc;
    }
    void AddRoot(ObjectPtr ptr);
    template <DerivedFrom T, typename... Args>
    T* New(Args&&... args) {
        T* ptr = new T(std::forward<Args>(args)...);
        allocations_.push_back(ptr);
        return ptr;
    }
    void Collect();
    ~GC();

private:
    GC() = default;
    GC(const GC&) = delete;
    GC& operator=(const GC&) = delete;
    std::vector<ObjectPtr> roots_;
    std::vector<ObjectPtr> allocations_;
};

namespace runtime {
template <DerivedFrom T, typename... Args>
T* New(Args&&... args) {
    return GC::GetInstance().New<T>(std::forward<Args>(args)...);
}
}  // namespace runtime
