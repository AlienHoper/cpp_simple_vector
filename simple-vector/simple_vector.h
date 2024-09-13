#pragma once

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <new>
#include <type_traits>
#include "array_ptr.h"

class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity_to_reserve)
        : reserve_capacity(capacity_to_reserve) {}

    size_t GetCapacity() const {
        return reserve_capacity;
    }

private:
    size_t reserve_capacity;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept : size_(0), capacity_(0), data_() {}

    explicit SimpleVector(size_t size)
        : size_(size), capacity_(size), data_(size) {}

    SimpleVector(size_t size, const Type& value)
        : size_(size), capacity_(size), data_(size) {}

    SimpleVector(std::initializer_list<Type> init)
        : size_(init.size()), capacity_(init.size()), data_(init.size()) {
        std::move(init.begin(), init.end(), data_.Get());
    }

    SimpleVector(const SimpleVector& other)
        : size_(other.size_), capacity_(other.capacity_), data_(other.capacity_) {
        std::copy(other.begin(), other.end(), data_.Get());
    }


    SimpleVector& operator=(const SimpleVector& rhs) requires(std::is_copy_assignable_v<Type>) {
        if (this != &rhs) {
            SimpleVector temp(rhs);
            swap(temp);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other) noexcept
        : size_(other.size_), capacity_(other.capacity_), data_(std::move(other.data_)) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (this != &rhs) {
            data_ = std::move(rhs.data_);
            size_ = std::exchange(rhs.size_, 0);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        return *this;
    }

    SimpleVector(const ReserveProxyObj& reserve_obj)
        : size_(0), capacity_(reserve_obj.GetCapacity()), data_(reserve_obj.GetCapacity()) {}

    ~SimpleVector() = default;

    void PushBack(Type&& item) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_data(new_capacity);
            std::move(data_.Get(), data_.Get() + size_, new_data.Get());
            data_.swap(new_data);
            capacity_ = new_capacity;
        }
        new (data_.Get() + size_) Type(std::move(item));
        ++size_;
    }
    
    void PushBack(const Type& item) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_data(new_capacity);
            std::move(data_.Get(), data_.Get() + size_, new_data.Get());
            data_.swap(new_data);
            capacity_ = new_capacity;
        }
        new (data_.Get() + size_) Type(item);
        ++size_;
    }


    void PopBack() noexcept {
        assert(size_ > 0);
        --size_;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        size_t index = pos - begin();
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_data(new_capacity);
            std::move(data_.Get(), data_.Get() + index, new_data.Get());
            new (new_data.Get() + index) Type(std::move(value));
            std::move(data_.Get() + index, data_.Get() + size_, new_data.Get() + index + 1);
            data_.swap(new_data);
            capacity_ = new_capacity;
        } else {
            std::move_backward(begin() + index, end(), end() + 1);
            new (data_.Get() + index) Type(std::move(value));
        }
        ++size_;
        return begin() + index;
    }
    
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        size_t index = pos - begin();
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_data(new_capacity);
            std::move(data_.Get(), data_.Get() + index, new_data.Get());
            new (new_data.Get() + index) Type(value);
            std::move(data_.Get() + index, data_.Get() + size_, new_data.Get() + index + 1);
            data_.swap(new_data);
            capacity_ = new_capacity;
        } else {
            std::move_backward(begin() + index, end(), end() + 1);
            new (data_.Get() + index) Type(value);
        }
        ++size_;
        return begin() + index;
    }



    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        size_t index = pos - begin();
        std::move(data_.Get() + index + 1, data_.Get() + size_, data_.Get() + index);
        --size_;
        return begin() + index;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        data_.swap(other.data_);
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return data_[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            size_t new_capacity = std::max(new_size, capacity_ * 2);
            ArrayPtr<Type> new_data(new_capacity);
            std::move(data_.Get(), data_.Get() + size_, new_data.Get());
            data_.swap(new_data);
            capacity_ = new_capacity;
        }
        size_ = new_size;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> new_data(new_capacity);
            std::move(data_.Get(), data_.Get() + size_, new_data.Get());
            data_.swap(new_data);
            capacity_ = new_capacity;
        }
    }

    Iterator begin() noexcept {
        return data_.Get();
    }

    Iterator end() noexcept {
        return data_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return data_.Get();
    }

    ConstIterator end() const noexcept {
        return data_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return begin();
    }

    ConstIterator cend() const noexcept {
        return end();
    }

    bool operator==(const SimpleVector& rhs) const {
        return size_ == rhs.size_ && std::equal(begin(), end(), rhs.begin());
    }

    bool operator!=(const SimpleVector& rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const SimpleVector& rhs) const {
        return std::lexicographical_compare(begin(), end(), rhs.begin(), rhs.end());
    }

    bool operator<=(const SimpleVector& rhs) const {
        return !(rhs < *this);
    }

    bool operator>(const SimpleVector& rhs) const {
        return rhs < *this;
    }

    bool operator>=(const SimpleVector& rhs) const {
        return !(*this < rhs);
    }

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<Type> data_;
};
