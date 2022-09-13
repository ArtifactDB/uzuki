#ifndef UZUKI_VALIDATE_HPP
#define UZUKI_VALIDATE_HPP

#include "unpack.hpp"
#include "Dummy.hpp"
#include "parse.hpp"

#include <vector>
#include <algorithm>
#include <stdexcept>

/**
 * @file validate.hpp
 *
 * @brief Validate JSON file contents against the **uzuki** spec.
 */

namespace uzuki {

/**
 * Validate JSON file contents against the **uzuki** specification.
 * Any invalid representations will cause an error to be thrown.
 *
 * @tparam Json A [`nlohmann::json`](https://github.com/nlohmann/json)-compatible representation of JSON data.
 *
 * @param contents Parsed contents of the JSON file.
 * @param num_external Expected number of external references to "other" objects.
 */
template<class Json>
void validate(const Json& contents, int num_external = 0) {
    DummyExternals others(num_external);
    parse<DummyProvisioner>(contents, others);
    return;
}

}

#endif
