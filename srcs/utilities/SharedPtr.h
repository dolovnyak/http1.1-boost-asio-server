#pragma once

template<class T>
class SharedPtr {
public:

    static SharedPtr MakeShared(const T& value) {
        return SharedPtr(new T(value));
    }

    static SharedPtr MakeShared(T* value) {
        return SharedPtr(value);
    }

    SharedPtr(const SharedPtr& other) {
        _ptr = other._ptr;
        _ref_count = other._ref_count;
        ++(*_ref_count);
    }

//    SharedPtr& operator=(const SharedPtr& other) {
//        if (this != &other) {
//            --(*_ref_count);
//            if (*_ref_count == 0) {
//                delete _ptr;
//                delete _ref_count;
//            }
//
//            _ptr = other._ptr;
//            _ref_count = other._ref_count;
//            ++(*_ref_count);
//        }
//        return *this;
//    }

    ~SharedPtr() {
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
