#pragma once

template<class T>
class Optional {
public:
    Optional() : _has_value(false) {}

    Optional(const T& value) : _has_value(true), _value(value) {}

    Optional(const Optional& other) : _has_value(other._has_value), _value(other._value) {}

    Optional& operator=(const Optional& other) {
        _has_value = other._has_value;
        _value = other._value;
        return *this;
    }

    bool HasValue() const { return _has_value; }

    const T& Value() const { return _value; }

    T& Value() { return _value; }

    const T& operator*() const { return _value; }

    T& operator*() { return _value; }

    const T* operator->() const { return &_value; }

    T* operator->() { return &_value; }

private:
    bool _has_value;
    T _value;
};