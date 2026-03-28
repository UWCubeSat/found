#ifndef SRC_COMMON_CONTAINERS_HPP_
#define SRC_COMMON_CONTAINERS_HPP_

#if defined(FOUND_USE_ETL_CONTAINERS)
#include <cstddef>
#include <utility>

#include "etl/algorithm.h"
#include "etl/array.h"
#include "etl/bitset.h"
#include "etl/deque.h"
#include "etl/list.h"
#include "etl/map.h"
#include "etl/memory.h"
#include "etl/optional.h"
#include "etl/pool.h"
#include "etl/queue.h"
#include "etl/set.h"
#include "etl/stack.h"
#include "etl/string.h"
#include "etl/utility.h"
#include "etl/vector.h"
#else
#include <array>
#include <cstddef>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <utility>
#include <vector>
#endif

namespace found {

#if defined(FOUND_USE_ETL_CONTAINERS)
struct pool_deleter {
    void *pool = nullptr;
    void *object = nullptr;
    void (*destroy_fn)(void *, void *) = nullptr;

    template <typename PointerType>
    void operator()(PointerType *ptr) const {
        (void)ptr;
        if ((destroy_fn != nullptr) && (object != nullptr)) {
            destroy_fn(pool, object);
        }
    }

    template <typename PoolType, typename ObjectType>
    static pool_deleter Make(PoolType &pool_ref, ObjectType *object_ptr) {
        return {&pool_ref, object_ptr, [](void *pool_ptr, void *stored_object) {
                    static_cast<PoolType *>(pool_ptr)->destroy(static_cast<ObjectType *>(stored_object));
                }};
    }
};

template <typename T, size_t N>
using vector = etl::vector<T, N>;

template <typename K, typename V, size_t N>
using map = etl::map<K, V, N>;

template <typename T, size_t N>
using set = etl::set<T, N>;

template <typename T, size_t N>
using list = etl::list<T, N>;

template <typename T, size_t N>
using deque = etl::deque<T, N>;

template <typename T, size_t N>
using array = etl::array<T, N>;

template <typename T>
using optional = etl::optional<T>;

template <typename T, size_t N>
using pool = etl::pool<T, N>;

template <typename T, size_t N>
using unique_ptr = etl::unique_ptr<T, pool_deleter>;

template <typename T, size_t N, typename PoolType, typename... Args>
unique_ptr<T, N> make_unique(PoolType &pool_ref, Args &&...args) {
    T *object = pool_ref.create(std::forward<Args>(args)...);
    return unique_ptr<T, N>(object, pool_deleter::Make(pool_ref, object));
}
#else
struct dummy_pool {
};

template <typename T, size_t N>
using vector = std::vector<T>;

template <typename K, typename V, size_t N>
using map = std::map<K, V>;

template <typename T, size_t N>
using set = std::set<T>;

template <typename T, size_t N>
using list = std::list<T>;

template <typename T, size_t N>
using deque = std::deque<T>;

template <typename T, size_t N>
using array = std::array<T, N>;

template <typename T>
using optional = std::optional<T>;

template <typename T, size_t N>
using pool = dummy_pool;

template <typename T, size_t N>
using unique_ptr = std::unique_ptr<T>;

// GCOVR_EXCL_START

template <typename T, size_t N, typename PoolType, typename... Args>
unique_ptr<T, N> make_unique([[maybe_unused]] PoolType &pool_ref, Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// GCOVR_EXCL_STOP
#endif

}  // namespace found

#endif  // SRC_COMMON_CONTAINERS_HPP_
