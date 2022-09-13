#ifndef UZUKI_DUMMY_HPP
#define UZUKI_DUMMY_HPP

#include <vector>
#include <memory>
#include <string>
#include <cstdint>

#include "interfaces.hpp"

namespace uzuki {

/** Defining the simple vectors first. **/

template<typename T, Type tt>
struct DummyTypedVector : public TypedVector<T, tt> {
    DummyTypedVector(size_t s) : length(s) {}

    size_t size() const { return length; }

    void set(size_t, T) {}
    void set_missing(size_t) {}
   
    void use_names(bool) {}
    void set_name(size_t, std::string) {}

    size_t length;
};

typedef DummyTypedVector<int32_t, INTEGER> DummyIntegerVector; // storing as double to avoid headaches with limits. 
typedef DummyTypedVector<double, NUMBER> DummyNumberVector;
typedef DummyTypedVector<std::string, STRING> DummyStringVector;
typedef DummyTypedVector<unsigned char, BOOLEAN> DummyBooleanVector;
typedef DummyTypedVector<std::string, DATE> DummyDateVector;

template<Type tt>
struct SomeDummyFactorVector : public SomeFactorVector<tt> {
    SomeDummyFactorVector(size_t s, size_t) : length(s) {}

    size_t size() const { return length; }

    void set(size_t, size_t) {}
    void set_missing(size_t) {}
   
    void use_names(bool) {}
    void set_name(size_t, std::string) {}

    void set_level(size_t, std::string) {}

    size_t length;
};

typedef SomeDummyFactorVector<FACTOR> DummyFactorVector;
typedef SomeDummyFactorVector<ORDERED> DummyOrderedVector;

/** Defining arrays. **/

template<typename T, Type tt>
struct DummyTypedArray : public TypedArray<T, tt> {
    DummyTypedArray(std::vector<size_t> d) : dimensions(std::move(d)) { }

    size_t first_dim() const { return dimensions[0]; }

    void set(size_t, T) { }
    
    void set_missing(size_t) {}
   
    void use_names(size_t, bool) {}

    void set_name(size_t, size_t, std::string) {}

    std::vector<size_t> dimensions;
};

typedef DummyTypedArray<int32_t, INTEGER> DummyIntegerArray; 
typedef DummyTypedArray<double, NUMBER> DummyNumberArray;
typedef DummyTypedArray<std::string, STRING> DummyStringArray;
typedef DummyTypedArray<unsigned char, BOOLEAN> DummyBooleanArray;
typedef DummyTypedArray<std::string, DATE> DummyDateArray;

template<Type tt>
struct SomeDummyFactorArray : public SomeFactorArray<tt> {
    SomeDummyFactorArray(std::vector<size_t> d, size_t) : dimensions(std::move(d)) {}

    size_t first_dim() const { return dimensions[0]; }

    void set(size_t, size_t) {}
    void set_missing(size_t) {}
   
    void use_names(size_t, bool) {}
    void set_name(size_t, size_t, std::string) {}

    void set_level(size_t, std::string) {}

    std::vector<size_t> dimensions;
};

typedef SomeDummyFactorArray<FACTOR> DummyFactorArray;
typedef SomeDummyFactorArray<ORDERED> DummyOrderedArray;

/** Defining the structural elements. **/

struct DummyNothing : public Nothing {};

struct DummyOther : public Other {};

struct DummyUnnamedList : public UnnamedList {
    DummyUnnamedList(size_t n) : length(n) {}

    size_t size() const { return length; }

    void set(size_t, std::shared_ptr<Base>) {}

    size_t length;
};

struct DummyNamedList : public NamedList {
    DummyNamedList(size_t n) : length(n) {}

    size_t size() const { return length; }

    void set(size_t, std::shared_ptr<Base>) {}

    void use_names(bool) {}

    void set_name(size_t, std::string) {}

    size_t length;
};

struct DummyDataFrame : public DataFrame {
    DummyDataFrame(size_t r, size_t c) : nrows(r), ncols(c) {}

    void set(size_t, std::string, std::shared_ptr<Base>) {}

    void set_name(size_t, std::string) {}

    void use_names(bool) {}

    size_t nrows, ncols;
};

/** Dummy provisioner. **/

struct DummyProvisioner {
    static Nothing* new_Nothing() { return (new DummyNothing); }

    static Other* new_Other(void* p) { return (new DummyOther); }

    static DataFrame* new_DataFrame(size_t r, size_t c) { return (new DummyDataFrame(r, c)); }

    static UnnamedList* new_UnnamedList(size_t l) { return (new DummyUnnamedList(l)); }

    static NamedList* new_NamedList(size_t l) { return (new DummyNamedList(l)); }

    struct Vector {
        static IntegerVector* new_Integer(size_t l) { return (new DummyIntegerVector(l)); }

        static NumberVector* new_Number(size_t l) { return (new DummyNumberVector(l)); }

        static StringVector* new_String(size_t l) { return (new DummyStringVector(l)); }

        static BooleanVector* new_Boolean(size_t l) { return (new DummyBooleanVector(l)); }

        static DateVector* new_Date(size_t l) { return (new DummyDateVector(l)); }

        static FactorVector* new_Factor(size_t l, size_t ll) { return (new DummyFactorVector(l, ll)); }

        static OrderedVector* new_Ordered(size_t l, size_t ll) { return (new DummyOrderedVector(l, ll)); }
    };

    struct Array {
        static IntegerArray* new_Integer(std::vector<size_t> d) { return (new DummyIntegerArray(std::move(d))); }

        static NumberArray* new_Number(std::vector<size_t> d) { return (new DummyNumberArray(std::move(d))); }

        static BooleanArray* new_Boolean(std::vector<size_t> d) { return (new DummyBooleanArray(std::move(d))); }

        static StringArray* new_String(std::vector<size_t> d) { return (new DummyStringArray(std::move(d))); }

        static DateArray* new_Date(std::vector<size_t> d) { return (new DummyDateArray(std::move(d))); }

        static FactorArray* new_Factor(std::vector<size_t> d, size_t ll) { return (new DummyFactorArray(std::move(d), ll)); }

        static OrderedArray* new_Ordered(std::vector<size_t> d, size_t ll) { return (new DummyOrderedArray(std::move(d), ll)); }
    };
};

struct DummyExternals {
    DummyExternals(size_t n) : number(n) {}

    void* get(size_t i) const {
        return nullptr;
    }

    size_t size() const {
        return number;
    }

    size_t number;
};

}

#endif
