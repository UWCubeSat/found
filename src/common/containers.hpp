#ifndef SRC_COMMON_CONTAINERS_HPP_
#define SRC_COMMON_CONTAINERS_HPP_

#ifndef FOUND_MAX_IMAGE_WIDTH
#define FOUND_MAX_IMAGE_WIDTH 1024
#endif
#ifndef FOUND_MAX_IMAGE_HEIGHT
#define FOUND_MAX_IMAGE_HEIGHT 1024
#endif
#ifndef FOUND_MAX_IMAGE_PIXELS
#define FOUND_MAX_IMAGE_PIXELS (FOUND_MAX_IMAGE_WIDTH * FOUND_MAX_IMAGE_HEIGHT)
#endif
#ifndef FOUND_MAX_POINTS
#define FOUND_MAX_POINTS \
    ((FOUND_MAX_IMAGE_WIDTH > FOUND_MAX_IMAGE_HEIGHT) ? FOUND_MAX_IMAGE_WIDTH : FOUND_MAX_IMAGE_HEIGHT)
#endif
#ifndef FOUND_MAX_COMPONENTS
#define FOUND_MAX_COMPONENTS (((FOUND_MAX_IMAGE_WIDTH + 1) / 2) * ((FOUND_MAX_IMAGE_HEIGHT + 1) / 2))
#endif
#ifndef FOUND_MAX_EDGES
#define FOUND_MAX_EDGES FOUND_MAX_COMPONENTS
#endif
#ifndef FOUND_MAX_LOCATION_RECORDS
#define FOUND_MAX_LOCATION_RECORDS 4096
#endif

#ifdef FOUND_USE_ETL_CONTAINERS
#include <cstddef>
#include <memory>
#include <utility>

#include "etl/pool.h"
#include "etl/vector.h"
#else
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#endif

namespace found::cnt {

#ifdef FOUND_USE_ETL_CONTAINERS
template <typename T, size_t N>
using vector = etl::vector<T, N>;

template <typename T, size_t N = 1>
using pool = etl::pool<T, N>;
#else
struct dummy_pool {
};

template <typename T, size_t N>
using vector = std::vector<T>;

template <typename T, size_t N = 1>
using pool = dummy_pool;
#endif

template <typename T, size_t N = 1>
using unique_ptr = std::unique_ptr<T>;

template <typename T, size_t N = 1, typename... Args>
unique_ptr<T, N> make_unique(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename Base, typename Derived, size_t N = 1, typename... Args>
unique_ptr<Base, N> make_unique_as(Args &&...args) {
    return std::make_unique<Derived>(std::forward<Args>(args)...);
}

template <typename T, size_t N = 1, typename... Args>
unique_ptr<T, N> make_unique([[maybe_unused]] pool<T, N> &pool_ref, Args &&...args) {
    return make_unique<T, N>(std::forward<Args>(args)...);
}

template <typename Base, typename Derived, size_t N = 1, typename... Args>
unique_ptr<Base, N> make_unique_as([[maybe_unused]] pool<Derived, N> &pool_ref, Args &&...args) {
    return make_unique_as<Base, Derived, N>(std::forward<Args>(args)...);
}

}  // namespace found::cnt

#endif  // SRC_COMMON_CONTAINERS_HPP_
