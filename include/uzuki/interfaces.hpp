#ifndef UZUKI_INTERFACES_HPP
#define UZUKI_INTERFACES_HPP

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

/**
 * @file interfaces.hpp
 * @brief Defines the interfaces to use in JSON parsing.
 */

namespace uzuki {

/**
 * Data type of an embedded R object.
 */
enum Type {
    INTEGER,
    NUMBER,
    STRING,
    BOOLEAN,
    FACTOR,
    ORDERED,
    DATE,
    STRUCTURAL 
};

/**
 * Structural type of an embedded R object.
 */
enum Structure {
    VECTOR,
    ARRAY,
    DATA_FRAME,
    UNNAMED_LIST,
    NAMED_LIST,
    NOTHING,
    OTHER
};

/**
 * @brief Base class for all embedded R objects.
 */
struct Base {
    /**
     * @cond
     */
    virtual ~Base() {}
    /**
     * @endcond
     */

    /**
     * @return The data type of the element.
     */
    virtual Type type() const = 0;

    /**
     * @return The structural type of the element.
     */
    virtual Structure structure() const = 0;
};

/**
 * @brief Base class for all embedded vector-like objects.
 */
struct Vector : public Base {
    Structure structure() const {
        return VECTOR;
    }

    /**
     * @return Length of the vector.
     */
    virtual size_t size () const = 0;

    /**
     * @param u Whether the elements of the vector are named.
     */
    virtual void use_names(bool u) = 0;

    /**
     * @param i Index of a vector element.
     * @param n Name for the vector element.
     */
    virtual void set_name(size_t i, std::string n) = 0;
};

/**
 * @brief Base class for embedded vectors with atomic types.
 *
 * @tparam T Atomic data type of the vector.
 * @tparam tt Data `Type` of the vector.
 */
template<typename T, Type tt>
struct TypedVector : public Vector {
    Type type() const {
        return tt;
    }

    /**
     * @param i Index of a vector element.
     * @param v Value of the vector element.
     */
    virtual void set(size_t i, T v) = 0;

    /**
     * @param i Index of a vector element to be marked as missing.
     */
    virtual void set_missing(size_t i) = 0;
};

/**
 * Virtual vector of integers.
 */
typedef TypedVector<int32_t, INTEGER> IntegerVector; 

/**
 * Virtual vector of doubles.
 */
typedef TypedVector<double, NUMBER> NumberVector;

/**
 * Virtual vector of strings.
 */
typedef TypedVector<std::string, STRING> StringVector;

/**
 * Virtual vector of booleans.
 */
typedef TypedVector<unsigned char, BOOLEAN> BooleanVector;

/**
 * Virtual vector of dates.
 */
typedef TypedVector<std::string, DATE> DateVector;

/**
 * @brief Base class for embedded factors.
 *
 * @tparam tt Data `Type` of the factor.
 */
template<Type tt>
struct SomeFactorVector : public Vector {
    Type type() const {
        return tt;
    }

    /**
     * @param i Index of the factor element.
     * @param v Value of the factor element, as an integer code that references the levels.
     */
    virtual void set(size_t i, size_t v) = 0;

    /**
     * @param i Index of a factor element to be marked as missing.
     */
    virtual void set_missing(size_t i) = 0;

    /**
     * @param il Index of the level element.
     * @param vl Value of the level element.
     */
    virtual void set_level(size_t il, std::string vl) = 0;
};

/**
 * Virtual vector of unordered factors.
 */
typedef SomeFactorVector<FACTOR> FactorVector;

/**
 * Virtual vector of ordered factors.
 */
typedef SomeFactorVector<ORDERED> OrderedVector;

/**
 * @brief Virtual class for an embedded multi-dimensional array-like object.
 */
struct Array : public Base {
    Structure structure() const {
        return ARRAY;
    }

    /**
     * @return Extent of the first dimension.
     * For matrices, this would be equivalent to the number of rows.
     */
    virtual size_t first_dim() const = 0;

    /**
     * @param d A dimension of the array.
     * @param u Whether the elements of the vector are named.
     */
    virtual void use_names(size_t d, bool u) = 0;

    /**
     * @param d A dimension of the array.
     * @param i An index along the dimension `d`.
     * @param n Name of entry `i` along dimension `d`.
     */
    virtual void set_name(size_t d, size_t i, std::string n) = 0;
};

/**
 * @brief Virtual class for an multi-dimensional array-like object with atomic types.
 *
 * @tparam T Atomic data type of the array.
 * @tparam tt Data `Type` of the array.
 */
template<typename T, Type tt>
struct TypedArray : public Array {
    Type type() const {
        return tt;
    }

    /**
     * @param i Index of the element in the multi-dimensional array, after treating it as a contiguous 1-dimensional array.
     * The first dimension can be assumed to be the fastest, followed by the second, etc.
     * @param v Value of the array element.
     */
    virtual void set(size_t i, T v) = 0;

    /**
     * @param i Index of the element in the multi-dimensional array (see `set()`) to be marked as missing.
     */
    virtual void set_missing(size_t) = 0;
};

/**
 * Virtual multi-dimensional array of integers.
 */
typedef TypedArray<int32_t, INTEGER> IntegerArray; 

/**
 * Virtual multi-dimensional array of doubles.
 */
typedef TypedArray<double, NUMBER> NumberArray;

/**
 * Virtual multi-dimensional array of strings.
 */
typedef TypedArray<std::string, STRING> StringArray;

/**
 * Virtual multi-dimensional array of booleans.
 */
typedef TypedArray<unsigned char, BOOLEAN> BooleanArray;

/**
 * Virtual multi-dimensional array of dates.
 */
typedef TypedArray<std::string, DATE> DateArray;

/**
 * @brief Base class for embedded multi-dimensional factor arrays.
 *
 * @tparam tt Data `Type` of the factor.
 */
template<Type tt>
struct SomeFactorArray : public Array {
    Type type() const {
        return tt;
    }

    /**
     * @param i Index of the element in the multi-dimensional array, after treating it as a contiguous 1-dimensional array.
     * The first dimension can be assumed to be the fastest, followed by the second, etc.
     * @param v Value of the array element, as an integer code that references the levels.
     */
    virtual void set(size_t i, size_t v) = 0;

    /**
     * @param i Index of the element in the multi-dimensional array (see `set()`) to be marked as missing.
     */
    virtual void set_missing(size_t i) = 0;

    /**
     * @param il Index of the level element.
     * @param vl Value of the level element.
     */
    virtual void set_level(size_t il, std::string vl) = 0;
};

/**
 * Virtual multi-dimensional array of unordered factors.
 */
typedef SomeFactorArray<FACTOR> FactorArray;

/**
 * Virtual multi-dimensional array of ordered factors.
 */
typedef SomeFactorArray<ORDERED> OrderedArray;

/**
 * @brief Representation of R's `NULL`.
 */
struct Nothing : public Base {
    Structure structure() const {
        return NOTHING;
    }

    Type type() const {
        return STRUCTURAL;
    }
};

/**
 * @brief Representation for unsupported objects.
 *
 * This usually captures links to external sources that can provide more details on the unsupported object.
 */
struct Other : public Base {
    Structure structure() const {
        return OTHER;
    }

    Type type() const {
        return STRUCTURAL;
    }
};

/**
 * @brief Representation for lists.
 */
struct List : public Base {
    Type type() const {
        return STRUCTURAL;
    }

    /**
     * @return Length of the list.
     */
    virtual size_t size() const = 0;

    /**
     * @param i Index of the list element.
     * @param v Value of the list element.
     */
    virtual void set(size_t, std::shared_ptr<Base> v) = 0;
};

/**
 * @brief Representation for unnamed lists.
 */
struct UnnamedList : public List {
    Structure structure() const {
        return UNNAMED_LIST;
    }
};

/**
 * @brief Representation for named lists.
 */
struct NamedList : public List {
    Structure structure() const {
        return NAMED_LIST;
    }

    /**
     * @param i Index of a list element.
     * @param n Name for the list element.
     */
    virtual void set_name(size_t i, std::string n) = 0;
};

/**
 * @brief Representation for data frames.
 */
struct DataFrame : public Base {
    Structure structure() const {
        return DATA_FRAME;
    }

    Type type() const {
        return STRUCTURAL;
    }

    /**
     * @param i Index of the data frame column.
     * @param n Name of the column.
     * @param v Value of the column.
     */
    virtual void set(size_t i, std::string n, std::shared_ptr<Base> v) = 0;

    /**
     * @param u Whether the rows are named.
     */
    virtual void use_names(bool u) = 0;

    /**
     * @param ir Index of the data frame row.
     * @param nr Name of the row.
     */
    virtual void set_name(size_t ir, std::string nr) = 0;
};

}

#endif
