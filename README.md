# SimpleVector & ArrayPtr 

Educational Simple Vector with move semantics, iteration, growth, and safe ownership with a tiny `ArrayPtr<T>` wrapper.

## Features

- **`ArrayPtr<T>`** – minimal owner for `new T[]` with move semantics
- **`SimpleVector<T>`** – push/pop, insert/erase, `reserve`, `resize`, comparisons
- **Iterators**: `begin()`, `end()`, `cbegin()`, `cend()`
- **Header-only**: just include the headers

## Layout

```
.
├── array_ptr.h        # Tiny move-only owner of the pointer wrapper
├── simple_vector.h    # The vector-like container (template, header-only)
└── main.cpp           # Minimal demo / tests
```
## 🔧 Requirements

- C++20 (or newer) compiler 

