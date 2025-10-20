#pragma once

#include <cassert>
#include <cstdlib>
#include <utility>

/// A simplified pointer that manages a dynamically allocated array

template <typename Type>
class ArrayPtr {
public:
    // Constructors

     // Default constructor: creates an empty ArrayPtr that does not own any memory
    ArrayPtr() = default;

    // Constructs an ArrayPtr from a newly allocated array of the specified size
    // If size == 0, no allocation is made and raw_ptr_ remains nullptr
    explicit ArrayPtr(size_t size) {
        raw_ptr_ = size ? new Type[size] : nullptr;
    }

    // Constructs an ArrayPtr that takes an existing raw pointer, does not allocate memory, just stores the pointer
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    // Copying is disabled to prevent two ArrayPtrs from owning the same memory
    ArrayPtr(const ArrayPtr&) = delete;

    // Copy assignment is disabled 
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    // Move constructor, after the move other is set to nullptr
    ArrayPtr(ArrayPtr&& other) noexcept : raw_ptr_(std::exchange(other.raw_ptr_, nullptr)) { }

    // Move assignment operator, releases any owned memory, then takes ownership from another ArrayPtr
    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
        if (this != &other) {
            delete[] raw_ptr_;
            std::swap(raw_ptr_,other.raw_ptr_);
            other.raw_ptr_ = nullptr;
        }
        return *this;
    }
    // Destructor
    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    // Releases the array and returns the raw pointer, warns about memory leaks
    [[nodiscard]] Type* Release() noexcept {
        Type* temp_address = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp_address;
    }
    //Returns a reference at the given index
    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }
    //Checking if the ArrayPtr owns any memory
    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }
    // Raw pointer read-only
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};

