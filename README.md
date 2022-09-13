# R lists to JSON, safely

## Overview

This repository describes a format for safely serializing R lists (duh) into JSON.
While most of this is handled by packages like **jsonlite**, there are some subtleties with respect to some types, e.g., factors, dates, integers versus doubles.
Some mechanism is also required to handle non-serializable S3/S4 classes.

## Layout

Unnamed lists are represented as JSON arrays.

Named lists are represented as JSON objects where the keys are the names of the elements and the values are the elements themselves.
Names are expected to be unique.

All non-list R objects are represented as a JSON object with a `type` key.
The value of `type` should be a string that determines the structure of the rest of the object.

## Allowed types

### Atomic vectors

We expect a `values` array containing the contents of the atomic vector.
For example, an integer vector of `1:4` is represented as:

```
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

```
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

```
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

```
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

```
{
    "type": "factor",
    "values": ["aaron", "mike", null],
    "levels": ["aaron", "mike"]
}
```

### Dates

For the `date` type, we require a `values` array containing YYYY-MM-DD dates as strings.
Missing values are allowed and are represented as `null`s.

```
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

```
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

```
{
    "type": "nothing"
}
```

### Other objects

All other R objects are accommodated by setting `type` to `"other"`.
This should be accompanied by an `index` integer that identifies this R object uniquely within the entire list.

```
{
    "type": "other",
    "index": 0
}
```

`index` should start at zero and be incremented whenever an unknown R object is encountered. 
The idea is that the JSON file containing the R list is associated with additional metadata describing more complex objects;
by indexing this additional metadata, we can restore the object in its appropriate location in the R list.
