#ifndef SRC_COMMON_FOUND_CONTAINERS_HPP_
#define SRC_COMMON_FOUND_CONTAINERS_HPP_

// Backend selection for container and owning-pointer primitives.
// Keep call sites behind domain aliases so the ETL/STL split stays localized.

#ifdef FOUND_USE_STACK_CONTAINERS
#include "etl/vector.h"
#include "etl/map.h"
#include "etl/set.h"
#include "etl/list.h"
#include "etl/deque.h"
#include "etl/array.h"
#include "etl/optional.h"
#include "etl/queue.h"
#include "etl/stack.h"
#include "etl/string.h"
#include "etl/bitset.h"
#include "etl/algorithm.h"
#include "etl/utility.h"
#include "etl/memory.h"
#define FOUND_VECTOR(T, N) etl::vector<T, N>
#define FOUND_MAP(K, V, N) etl::map<K, V, N>
#define FOUND_SET(T, N) etl::set<T, N>
#define FOUND_LIST(T, N) etl::list<T, N>
#define FOUND_DEQUE(T, N) etl::deque<T, N>
#define FOUND_ARRAY(T, N) etl::array<T, N>
#define FOUND_OPTIONAL(T) etl::optional<T>
#define FOUND_POOL(T, N) etl::pool<T, N>
// Usage: FOUND_UNIQUE_PTR(T, N, pool, args...) expands to etl::unique_ptr<T>(pool.create(args...))
#define FOUND_UNIQUE_PTR(T, N, pool, ...) etl::unique_ptr<T>((pool).create(__VA_ARGS__))
#define FOUND_UNIQUE_PTR_TYPE(T, N) etl::unique_ptr<T>
#else
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <array>
#include <optional>
#include <memory>
#define FOUND_VECTOR(T, N) std::vector<T>
#define FOUND_MAP(K, V, N) std::map<K, V>
#define FOUND_SET(T, N) std::set<T>
#define FOUND_LIST(T, N) std::list<T>
#define FOUND_DEQUE(T, N) std::deque<T>
#define FOUND_ARRAY(T, N) std::array<T, N>
#define FOUND_OPTIONAL(T) std::optional<T>
#define FOUND_POOL(T, N) int /* dummy placeholder so STL call sites keep the same shape */
// Usage: FOUND_UNIQUE_PTR(T, N, pool, args...) expands to std::make_unique<T>(args...)
#define FOUND_UNIQUE_PTR(T, N, pool, ...) ((void)(pool), std::make_unique<T>(__VA_ARGS__))
#define FOUND_UNIQUE_PTR_TYPE(T, N) std::unique_ptr<T>
#endif

#endif  // SRC_COMMON_FOUND_CONTAINERS_HPP_
