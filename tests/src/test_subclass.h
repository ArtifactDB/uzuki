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
struct DefaultVectorBase { 
    DefaultVectorBase(size_t n) : values(n) {}

    size_t size() const { 
        return values.size(); 
    }

    void set(size_t i, T val) {
        values[i] = std::move(val);
        return;
    }

    void use_names() {
        has_names = true;
        names.resize(values.size());
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

    void use_names() {
        base.use_names();
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

    DefaultVectorBase<T> base;
};

typedef DefaultTypedVector<int32_t, uzuki::INTEGER> DefaultIntegerVector; 
typedef DefaultTypedVector<double, uzuki::NUMBER> DefaultNumberVector;
typedef DefaultTypedVector<std::string, uzuki::STRING> DefaultStringVector;
typedef DefaultTypedVector<unsigned char, uzuki::BOOLEAN> DefaultBooleanVector;
typedef DefaultTypedVector<std::string, uzuki::DATE> DefaultDateVector;

struct DefaultFactorBase {
    DefaultFactorBase(size_t ll) : levels(ll) {}

    void set_level(size_t i, std::string l) {
        levels[i] = std::move(l);
        return;
    }

    void is_ordered() {
        ordered = true;
        return;
    }

    std::vector<std::string> levels;
    bool ordered = false;
};

struct DefaultFactor : public uzuki::Factor {
    DefaultFactor(size_t l, size_t ll) : vbase(l), fbase(ll) {}

    size_t size() const { 
        return vbase.size(); 
    }

    void set(size_t i, size_t l) {
        vbase.set(i, l);
        return;
    }

    void use_names() {
        vbase.use_names();
        return;
    }

    void set_missing(size_t i) {
        vbase.set_missing(i);
        return;
    }

    void set_name(size_t i, std::string name) {
        vbase.set_name(i, std::move(name));
        return;
    }

    void set_level(size_t i, std::string l) {
        fbase.set_level(i, std::move(l));
        return;
    }

    void is_ordered() {
        fbase.is_ordered();
        return;
    }

    DefaultVectorBase<size_t> vbase;
    DefaultFactorBase fbase;
};

/** Defining arrays. **/

template<typename T>
struct DefaultArrayBase {
    DefaultArrayBase(std::vector<size_t> d) : dimensions(std::move(d)), has_names(dimensions.size()) {
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

    void use_names(size_t d) {
        has_names[d] = true;
        names.resize(dimensions.size());
        names[d].resize(dimensions[d]);
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

    void use_names(size_t d) {
        base.use_names(d);
        return;
    }

    void set_name(size_t d, size_t i, std::string name) {
        base.set_name(d, i, std::move(name));
        return;
    }

    DefaultArrayBase<T> base;
};

typedef DefaultTypedArray<int32_t, uzuki::INTEGER_ARRAY> DefaultIntegerArray; 
typedef DefaultTypedArray<double, uzuki::NUMBER_ARRAY> DefaultNumberArray;
typedef DefaultTypedArray<std::string, uzuki::STRING_ARRAY> DefaultStringArray;
typedef DefaultTypedArray<unsigned char, uzuki::BOOLEAN_ARRAY> DefaultBooleanArray;
typedef DefaultTypedArray<std::string, uzuki::DATE_ARRAY> DefaultDateArray;

struct DefaultFactorArray : public uzuki::FactorArray {
    DefaultFactorArray(std::vector<size_t> d, size_t ll) : abase(std::move(d)), fbase(ll) {}

    size_t first_dim() const { 
        return abase.first_dim();
    }

    void set(size_t i, size_t val) {
        abase.set(i, std::move(val));
        return;
    }

    void set_missing(size_t i) {
        abase.set_missing(i);
        return;
    }

    void use_names(size_t d) {
        abase.use_names(d);
        return;
    }

    void set_name(size_t d, size_t i, std::string name) {
        abase.set_name(d, i, std::move(name));
        return;
    }

    void is_ordered() {
        fbase.is_ordered();
        return;
    }

    void set_level(size_t i, std::string l) {
        fbase.set_level(i, std::move(l));
        return;
    }

    DefaultArrayBase<size_t> abase;
    DefaultFactorBase fbase;
};

/** Defining the structural elements. **/

struct DefaultNothing : public uzuki::Nothing {};

struct DefaultOther : public uzuki::Other {
    DefaultOther(void *p) : ptr(p) {}
    void* ptr;
};

struct DefaultList : public uzuki::List {
    DefaultList(size_t n) : values(n) {}

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

    void use_names() {
        has_names = true;
        names.resize(values.size());
        return;
    }

    std::vector<std::shared_ptr<uzuki::Base> > values;
    bool has_names = false;
    std::vector<std::string> names;
};

struct DefaultDataFrame : public uzuki::DataFrame {
    DefaultDataFrame(size_t r, size_t c) : nrows(r), columns(c), colnames(c) {}

    void set(size_t i, std::string n, std::shared_ptr<uzuki::Base> ptr) {
        columns[i] = std::move(ptr);
        colnames[i] = std::move(n);
        return;
    }

    void use_names() {
        has_names = true;
        rownames.resize(nrows);
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

    static uzuki::List* new_List(size_t l) { return (new DefaultList(l)); }

    static uzuki::IntegerVector* new_Integer(size_t l) { return (new DefaultIntegerVector(l)); }

    static uzuki::NumberVector* new_Number(size_t l) { return (new DefaultNumberVector(l)); }

    static uzuki::StringVector* new_String(size_t l) { return (new DefaultStringVector(l)); }

    static uzuki::BooleanVector* new_Boolean(size_t l) { return (new DefaultBooleanVector(l)); }

    static uzuki::DateVector* new_Date(size_t l) { return (new DefaultDateVector(l)); }

    static uzuki::Factor* new_Factor(size_t l, size_t ll) { return (new DefaultFactor(l, ll)); }

    static uzuki::IntegerArray* new_Integer(std::vector<size_t> d) { return (new DefaultIntegerArray(std::move(d))); }

    static uzuki::NumberArray* new_Number(std::vector<size_t> d) { return (new DefaultNumberArray(std::move(d))); }

    static uzuki::BooleanArray* new_Boolean(std::vector<size_t> d) { return (new DefaultBooleanArray(std::move(d))); }

    static uzuki::StringArray* new_String(std::vector<size_t> d) { return (new DefaultStringArray(std::move(d))); }

    static uzuki::DateArray* new_Date(std::vector<size_t> d) { return (new DefaultDateArray(std::move(d))); }

    static uzuki::FactorArray* new_Factor(std::vector<size_t> d, size_t ll) { return (new DefaultFactorArray(std::move(d), ll)); }
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
