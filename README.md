# R lists to JSON, safely

![Unit tests](https://github.com/LTLA/uzuki/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/LTLA/uzuki/actions/workflows/doxygenate.yaml/badge.svg)
[![codecov](https://codecov.io/gh/LTLA/uzuki/branch/master/graph/badge.svg?token=J3dxS3MtT1)](https://codecov.io/gh/LTLA/uzuki)

## Overview

The **uzuki** repository describes a format for safely serializing R lists into JSON.
While most of this is handled by packages like **jsonlite**, there are some subtleties with respect to preserving type, e.g., factors, dates, integers versus doubles.
Some mechanism is also required to handle multi-dimensional arrays, data frames, and external references to non-serializable objects.
The C++ library implements a portable validator for this specification. 

## Specification

### Concepts

The serialized JSON string should contain a single array or JSON object.
The array/object may contain any number of nested arrays/objects, which in turn may contain any number of nested arrays/objects.

All non-list R objects are represented as a JSON object with a `type` key.
The value of `type` should be a string that determines the structure of the rest of the object.
This ensures that the type of the R object can be recovered from the JSON.

As R does not have any concept of scalars, neither does **uzuki**.
All "scalars" are treated as atomic vectors of length 1.

List objects are represented as plain JSON arrays or objects - see [below](#lists) for more details.
These are considered as "structural" elements and so they do not require a `type` string.

### Atomic vectors

We expect a `values` array containing the contents of the atomic vector.
For example, an integer vector of `1:4` is represented as:

```json
{
    "type": "integer",
    "values": [1, 2, 3, 4]
}
```

This layout is supported for `type`s of `integer`, `string`, `number` and `boolean`,
where the elements in `values` should be of the corresponding JSON type.
(Integers should be integral values in the JSON numeric type.)
Missing values are supported by using `null`.

Optionally, we may have a `names` array of the same length as the `values` array.
This should contain strings with the names of each element.
No `nulls` are allowed.

```json
{
    "type": "integer",
    "values": [1, 2, 3, 4],
    "names": ["A", "B", "C", "D"]
}
```

As in R, there is no concept of scalars; the closest supported analog is a length-1 vector in `values`.

### Atomic arrays

We expect a `values` array containing the contents of the atomic array, and a `dimensions` array containing the length of each dimension as integers.
The product of values in `dimensions` should be equal to the length of `values`.

In `values`, we assume that the first dimension is the fastest changing, then the second, then the third, and so on.
For example, an integer matrix of

```
1 6
2 7
3 8
4 9
5 0
```

would be represented as

```json
{
    "type": "integer",
    "values": [1, 2, 3, 4, 5, 6, 7, 8, 9, 0]
    "dimensions": [5, 2]
}
```

This layout is supported for `type`s of `integer`, `string`, `number` and `boolean`,
where the elements in `values` should be of the corresponding JSON type.
(Integers should be integral values in the JSON numeric type.)
Missing values are supported by using `null`.

Optionally, we may have a `names` array of the same length as `dimensions` array.
This should contain arrays of strings with the names along each dimension, or `null` if the dimension is unnamed.
Each array should be of length equal to the corresponding element of `dimensions`.
No `nulls` are allowed within each string array.

```json
{
    "type": "integer",
    "values": [1, 2, 3, 4, 5, 6, 7, 8, 9, 0]
    "dimensions": [5, 2]
    "names": [["A", "B", "C", "D", "E"], null]
}
```

### Factors

For `factor` or `ordered` types, we require a `values` array containing the factor values as strings.
We also expect an additional `levels` array of strings containing the factor levels in an appropriate order.
`levels` must be unique and a superset of all entries in `values`, with one exception:
`values` may contain `null` but `levels` may not.

```json
{
    "type": "factor",
    "values": ["aaron", "mike", null],
    "levels": ["aaron", "mike"]
}
```

### Dates

For the `date` type, we require a `values` array containing YYYY-MM-DD dates as strings.
Missing values are allowed and are represented as `null`s.

```json
{
    "type": "date",
    "values": ["2021-02-31", "2121-03-11"]
}
```

### Data frames

For the `data.frame` type, we expect the following additional fields:

- `rows`: an integer specifying the number of rows in the data frame.
- `columns`: a JSON object containing the named fields.
  Each element can be any of the supported types.
  Vectors, factors and dates should have length equal to `rows`,
  while the first dimension of any arrays should be equal to `rows.`
- `names`: (optional) a string array of length equal to `rows`,
  containing the name of each row.
  `null` values are not allowed here.

```json
{
    "type": "data.frame",
    "rows": 5
    "columns": {
        "stuff": {
            "type": "integer",
            "values": [1,2,3,4,5]
        },
        "foobar": {
            "type": "string",
            "dimensions": [5, 2],
            "values": ["", "", "A", "", "", "B", "C", "", "", "D"]
        }
    },
    "names": [ "v", "w", "x", "y", "z"]
}
```

### Nothing

In some cases, it can be useful to set a placeholder "nothing" value inside the list.
For example, this maps to a `NULL` element in R or a `None` value in Python.
It is supported by specifying an object with `"type": "nothing"`, i.e.:

```json
{
    "type": "nothing"
}
```

### Other objects

All other R objects are accommodated by setting `type` to `"other"`.
This should be accompanied by an `index` integer that identifies this R object uniquely within the entire list.
`index` should start at zero and be incremented whenever an unknown R object is encountered. 

```json
{
    "type": "other",
    "index": 0
}
```

Each index is a reference to external metadata describing more complex objects.
By indexing this external metadata, we can restore the object in its appropriate location in the R list.
The exact mechanism by which this restoration occurs is implementation-defined.

### Lists

Unnamed lists are represented as JSON arrays.
For example, `list(1:4, LETTERS[1:2])` is represented as:

```json
[
    {
        "type": "integer",
        "values": [ 1, 2, 3, 4 ]
    },
    {
        "type": "string",
        "values": [ "A", "B" ]
    }
]
```

Named lists are represented as JSON objects where the keys are the names of the elements and the values are the elements themselves.
Names are expected to be unique.
For example, `list(X=1:4, Y=LETTERS[1:2])` is represented as:

```json
{
    "X": {
        "type": "integer",
        "values": [ 1, 2, 3, 4 ]
    },
    "Y": {
        "type": "string",
        "values": [ "A", "B" ]
    }
}
```

## Implementation

### Quick start

A reference implementation of the validator is provided as a header-only C++ library in [`include/uzuki`](include/uzuki).
This is useful for portable deployment in different frameworks like R, Python, etc.
Once a JSON file is loaded in using the [`nlohmann/json`](https://github.com/nlohmann/json) library, we can check that it complies with the **uzuki** specification:

```cpp
#include "uzuki/uzuki.hpp"

auto contents = nlohmann::json::parse(json_str);
uzuki::validate(contents);
```

This will raise an error if any violations of the specification are observed.
If the expected number of "other" objects is known, we can check that there are no more references than expected:

```cpp
uzuki::validate(contents, num_references);
```

Advanced users can also use the **uzuki** parser to transform the JSON content into more convenient representations.
This is achieved by calling `parse()` with custom provisioner and external reference classes.
For example, [`tests/src/test_subclass.h`](tests/src/test_subclass.h) defines the `DefaultProvisioner` and `DefaultExternals` classes, 
which can be used to load the JSON contents into `std::vector`s for easier downstream operations.

```cpp
DefaultExternals ext(nexpected);
auto ptr = uzuki::parse<DefaultProvisioner>(contents, ext);
```

Also see the [reference documentation](https://ltla.github.io/uzuki) for more details.

### Building projects 

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```
include(FetchContent)

FetchContent_Declare(
  libscran
  GIT_REPOSITORY https://github.com/LTLA/uzuki
  GIT_TAG master # or any version of interest 
)

FetchContent_MakeAvailable(uzuki)
```

Then you can link to **uzuki** to make the headers available during compilation:

```
# For executables:
target_link_libraries(myexe uzuki)

# For libaries
target_link_libraries(mylib INTERFACE uzuki)
```

## Links

I can't remember where the name comes from, but it was probably from my habit of falling back to **iDOLM@ster** characters when I can't think of a better name.
In this case, [Uzuki Shimamura](https://myanimelist.net/character/70883/Uzuki_Shimamura):

![Uzuki Shimamura](https://pa1.narvii.com/6088/154344d5baafcd9da3154511d868e8b3ee33f5a4_hq.gif)

