#pragma once

#include <cstddef>

template<class T>
class SharedPtr {
public:

    SharedPtr() : _ptr(nullptr), _ref_count(nullptr) {}

    static SharedPtr MakeShared(const T& value) {
        return SharedPtr(new T(value));
    }

    static SharedPtr MakeShared(T* value) {
        return SharedPtr(value);
    }

    SharedPtr(const SharedPtr& other) {
        _ptr = other._ptr;
        _ref_count = other._ref_count;

        if (_ref_count != nullptr) {
            ++(*_ref_count);
        }
    }

    ~SharedPtr() {
        if (_ref_count == nullptr) {
            return;
        }

        if (*_ref_count == 1) {
            delete _ptr;
            delete _ref_count;
        } else {
            --(*_ref_count);
        }
    }

    T operator*() {
        return *_ptr;
    }

    T* operator->() {
        return _ptr;
    }

private:
    SharedPtr(T* ptr) : _ptr(ptr), _ref_count(new int(1)) {}

    T* _ptr;
    int* _ref_count;
};

template<class T>
SharedPtr<T> MakeShared(const T& value) {
    return SharedPtr<T>::MakeShared(value);
}

template<class T>
SharedPtr<T> MakeShared(T* value) {
    return SharedPtr<T>::MakeShared(value);
}
