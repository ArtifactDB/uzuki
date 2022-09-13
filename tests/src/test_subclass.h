#ifndef TEST_SUBCLASSES_H 
#define TEST_SUBCLASSES_H 

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <limits>

#include "uzuki/interfaces.hpp"

template<typename T>
void set_missing_internal(std::vector<T>& values, size_t i) {
    if constexpr(std::is_same<T, double>::value) {
        values[i] = std::numeric_limits<double>::quiet_NaN();
    }
    if constexpr(std::is_same<T, size_t>::value) {
        values[i] = -1;
    }
    if constexpr(std::is_same<T, unsigned char>::value) {
        values[i] = -1;
    }
    if constexpr(std::is_same<T, int32_t>::value) {
        values[i] = std::numeric_limits<int32_t>::min();
    }
    if constexpr(std::is_same<T, std::string>::value) {
        values[i] = "ich bin missing";
    }
}

/** Defining the simple vectors first. **/

template<typename T>
struct VectorBase { 
    VectorBase(size_t n) : values(n) {}

    size_t size() const { 
        return values.size(); 
    }

    void set(size_t i, T val) {
        values[i] = std::move(val);
        return;
    }

    void use_names(bool u) {
        has_names = u;
        if (u) {
            names.resize(values.size());
        } else {
            names.clear();
        }
        return;
    }

    void set_missing(size_t i) {
        set_missing_internal(values, i);
        return;
    }

    void set_name(size_t i, std::string name) {
        names[i] = std::move(name);
        return;
    }

    std::vector<T> values;
    bool has_names = false;
    std::vector<std::string> names;
};

template<typename T, uzuki::Type tt>
struct DefaultTypedVector : public uzuki::TypedVector<T, tt> {
    DefaultTypedVector(size_t n) : base(n) {}

    size_t size() const { 
        return base.size();
    }

    void set(size_t i, T val) {
        base.set(i, std::move(val));
        return;
    }

    void use_names(bool u) {
        base.use_names(u);
        return;
    }

    void set_missing(size_t i) {
        base.set_missing(i);
        return;
    }

    void set_name(size_t i, std::string name) {
        base.set_name(i, std::move(name));
        return;
    }

    VectorBase<T> base;
};

typedef DefaultTypedVector<int32_t, uzuki::INTEGER> DefaultIntegerVector; 
typedef DefaultTypedVector<double, uzuki::NUMBER> DefaultNumberVector;
typedef DefaultTypedVector<std::string, uzuki::STRING> DefaultStringVector;
typedef DefaultTypedVector<unsigned char, uzuki::BOOLEAN> DefaultBooleanVector;
typedef DefaultTypedVector<std::string, uzuki::DATE> DefaultDateVector;

template<uzuki::Type tt>
struct DefaultSomeFactorVector : public uzuki::SomeFactorVector<tt> {
    DefaultSomeFactorVector(size_t l, size_t ll) : base(l), levels(ll) {}

    size_t size() const { 
        return base.size(); 
    }

    void set(size_t i, size_t l) {
        base.set(i, l);
        return;
    }

    void use_names(bool u) {
        base.use_names(u);
        return;
    }

    void set_missing(size_t i) {
        base.set_missing(i);
        return;
    }

    void set_name(size_t i, std::string name) {
        base.set_name(i, std::move(name));
        return;
    }

    void set_level(size_t i, std::string l) {
        levels[i] = std::move(l);
        return;
    }

    VectorBase<size_t> base;
    std::vector<std::string> levels;
};

typedef DefaultSomeFactorVector<uzuki::FACTOR> DefaultFactorVector;
typedef DefaultSomeFactorVector<uzuki::ORDERED> DefaultOrderedVector;

/** Defining arrays. **/

template<typename T>
struct ArrayBase {
    ArrayBase(std::vector<size_t> d) : dimensions(std::move(d)), has_names(dimensions.size()) {
        size_t prod = 1;
        for (auto d_ : dimensions) { prod *= d_; }
        values.resize(prod);
    }

    size_t first_dim() const { 
        return dimensions.front();
    }

    void set(size_t i, T val) {
        values[i] = std::move(val);
        return;
    }

    void set_missing(size_t i) {
        set_missing_internal(values, i);
        return;
    }

    void use_names(size_t d, bool u) {
        has_names[d] = u;
        if (u) {
            names.resize(dimensions.size());
            names[d].resize(dimensions[d]);
        } else {
            names[d].clear();
        }
        return;
    }

    void set_name(size_t d, size_t i, std::string name) {
        names[d][i] = std::move(name);
        return;
    }

    std::vector<size_t> dimensions;
    std::vector<T> values;
    std::vector<unsigned char> has_names;
    std::vector<std::vector<std::string> > names;
};

template<typename T, uzuki::Type tt>
struct DefaultTypedArray : public uzuki::TypedArray<T, tt> {
    DefaultTypedArray(std::vector<size_t> d) : base(std::move(d)) {}

    size_t first_dim() const { 
        return base.first_dim();
    }

    void set(size_t i, T val) {
        base.set(i, std::move(val));
        return;
    }

    void set_missing(size_t i) {
        base.set_missing(i);
        return;
    }

    void use_names(size_t d, bool u) {
        base.use_names(d, u);
        return;
    }

    void set_name(size_t d, size_t i, std::string name) {
        base.set_name(d, i, std::move(name));
        return;
    }

    ArrayBase<T> base;
};

typedef DefaultTypedArray<int32_t, uzuki::INTEGER> DefaultIntegerArray; 
typedef DefaultTypedArray<double, uzuki::NUMBER> DefaultNumberArray;
typedef DefaultTypedArray<std::string, uzuki::STRING> DefaultStringArray;
typedef DefaultTypedArray<unsigned char, uzuki::BOOLEAN> DefaultBooleanArray;
typedef DefaultTypedArray<std::string, uzuki::DATE> DefaultDateArray;

template<uzuki::Type tt>
struct DefaultSomeFactorArray : public uzuki::SomeFactorArray<tt> {
    DefaultSomeFactorArray(std::vector<size_t> d, size_t ll) : base(std::move(d)), levels(ll) {}

    size_t first_dim() const { 
        return base.first_dim();
    }

    void set(size_t i, size_t val) {
        base.set(i, std::move(val));
        return;
    }

    void set_missing(size_t i) {
        base.set_missing(i);
        return;
    }

    void use_names(size_t d, bool u) {
        base.use_names(d, u);
        return;
    }

    void set_name(size_t d, size_t i, std::string name) {
        base.set_name(d, i, std::move(name));
        return;
    }

    void set_level(size_t i, std::string l) {
        levels[i] = std::move(l);
        return;
    }

    ArrayBase<size_t> base;
    std::vector<std::string> levels;
};

typedef DefaultSomeFactorArray<uzuki::FACTOR> DefaultFactorArray;
typedef DefaultSomeFactorArray<uzuki::ORDERED> DefaultOrderedArray;

/** Defining the structural elements. **/

struct DefaultNothing : public uzuki::Nothing {};

struct DefaultOther : public uzuki::Other {
    DefaultOther(void *p) : ptr(p) {}
    void* ptr;
};

struct DefaultUnnamedList : public uzuki::UnnamedList {
    DefaultUnnamedList(size_t n) : values(n) {}

    size_t size() const { 
        return values.size(); 
    }

    void set(size_t i, std::shared_ptr<uzuki::Base> ptr) {
        values[i] = std::move(ptr);
        return;
    }

    std::vector<std::shared_ptr<uzuki::Base> > values;
};

struct DefaultNamedList : public uzuki::NamedList {
    DefaultNamedList(size_t n) : values(n), names(n) {}

    size_t size() const { 
        return values.size(); 
    }

    void set(size_t i, std::shared_ptr<uzuki::Base> ptr) {
        values[i] = std::move(ptr);
        return;
    }

    void set_name(size_t i, std::string name) {
        names[i] = std::move(name);
        return;
    }

    std::vector<std::shared_ptr<uzuki::Base> > values;
    std::vector<std::string> names;
};

struct DefaultDataFrame : public uzuki::DataFrame {
    DefaultDataFrame(size_t r, size_t c) : nrows(r), columns(c), colnames(c) {}

    void set(size_t i, std::string n, std::shared_ptr<uzuki::Base> ptr) {
        columns[i] = std::move(ptr);
        colnames[i] = std::move(n);
        return;
    }

    void use_names(bool u) {
        has_names = u;
        if (has_names) {
            rownames.resize(nrows);
        }
        return;
    }

    void set_name(size_t i, std::string name) {
        rownames[i] = std::move(name);
        return;
    }

    std::vector<std::string> colnames;
    std::vector<std::shared_ptr<uzuki::Base> > columns;

    size_t nrows;
    bool has_names = false;
    std::vector<std::string> rownames;
};

/** Provisioner. **/

struct DefaultProvisioner {
    static uzuki::Nothing* new_Nothing() { return (new DefaultNothing); }

    static uzuki::Other* new_Other(void* p) { return (new DefaultOther(p)); }

    static uzuki::DataFrame* new_DataFrame(size_t r, size_t c) { return (new DefaultDataFrame(r, c)); }

    static uzuki::UnnamedList* new_UnnamedList(size_t l) { return (new DefaultUnnamedList(l)); }

    static uzuki::NamedList* new_NamedList(size_t l) { return (new DefaultNamedList(l)); }

    struct Vector {
        static uzuki::IntegerVector* new_Integer(size_t l) { return (new DefaultIntegerVector(l)); }

        static uzuki::NumberVector* new_Number(size_t l) { return (new DefaultNumberVector(l)); }

        static uzuki::StringVector* new_String(size_t l) { return (new DefaultStringVector(l)); }

        static uzuki::BooleanVector* new_Boolean(size_t l) { return (new DefaultBooleanVector(l)); }

        static uzuki::DateVector* new_Date(size_t l) { return (new DefaultDateVector(l)); }

        static uzuki::FactorVector* new_Factor(size_t l, size_t ll) { return (new DefaultFactorVector(l, ll)); }

        static uzuki::OrderedVector* new_Ordered(size_t l, size_t ll) { return (new DefaultOrderedVector(l, ll)); }
    };

    struct Array {
        static uzuki::IntegerArray* new_Integer(std::vector<size_t> d) { return (new DefaultIntegerArray(std::move(d))); }

        static uzuki::NumberArray* new_Number(std::vector<size_t> d) { return (new DefaultNumberArray(std::move(d))); }

        static uzuki::BooleanArray* new_Boolean(std::vector<size_t> d) { return (new DefaultBooleanArray(std::move(d))); }

        static uzuki::StringArray* new_String(std::vector<size_t> d) { return (new DefaultStringArray(std::move(d))); }

        static uzuki::DateArray* new_Date(std::vector<size_t> d) { return (new DefaultDateArray(std::move(d))); }

        static uzuki::FactorArray* new_Factor(std::vector<size_t> d, size_t ll) { return (new DefaultFactorArray(std::move(d), ll)); }

        static uzuki::OrderedArray* new_Ordered(std::vector<size_t> d, size_t ll) { return (new DefaultOrderedArray(std::move(d), ll)); }
    };
};

struct DefaultExternals {
    DefaultExternals(size_t n) : number(n) {}

    void* get(size_t i) {
        return reinterpret_cast<void*>(static_cast<uintptr_t>(i + 1));
    }

    size_t size() const {
        return number;
    }

    size_t number;
};

#endif
