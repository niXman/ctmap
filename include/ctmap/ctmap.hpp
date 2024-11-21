
// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2024-2024 niXman (github dot nixman at pm dot me)
// This file is part of ctmap(github.com/niXman/ctmap) project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ----------------------------------------------------------------------------

#ifndef __CTMAP__CTMAP_HPP
#define __CTMAP__CTMAP_HPP

#include <cstdint>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <array>

namespace ctmap {
namespace details {

// based on QuickSort from https://github.com/serge-sans-paille/frozen
// https://github.com/serge-sans-paille/frozen/blob/master/include/frozen/bits/algorithms.h
/*************************************************************************************************/

template<typename Iter, typename Compare>
constexpr auto min_element(Iter begin, const Iter end, Compare const &compare) {
    auto result = begin;
    while (begin != end) {
        if (compare(*begin, *result)) {
            result = begin;
        }
        ++begin;
    }
    return result;
}

template <class T>
constexpr void cswap(T &a, T &b) {
    auto tmp = a;
    a = b;
    b = tmp;
}

template <class T, class U>
constexpr void cswap(std::pair<T, U> & a, std::pair<T, U> & b) {
    cswap(a.first, b.first);
    cswap(a.second, b.second);
}

template <class... Tys, std::size_t... Is>
constexpr void cswap(std::tuple<Tys...> &a, std::tuple<Tys...> &b, std::index_sequence<Is...>) {
    using swallow = int[];
    (void) swallow{(cswap(std::get<Is>(a), std::get<Is>(b)), 0)...};
}

template <class... Tys>
constexpr void cswap(std::tuple<Tys...> &a, std::tuple<Tys...> &b) {
    cswap(a, b, std::make_index_sequence<sizeof...(Tys)>());
}

template <typename Iter>
constexpr void iter_swap(Iter a, Iter b) {
    cswap(*a, *b);
}

template <typename Iterator, class Compare>
constexpr Iterator _partition(Iterator left, Iterator right, Compare const &compare) {
    auto pivot = left + (right - left) / 2;
    iter_swap(right, pivot);
    pivot = right;
    for (auto it = left; 0 < right - it; ++it) {
        if (compare(*it, *pivot)) {
            iter_swap(it, left);
            left++;
        }
    }
    iter_swap(pivot, left);
    pivot = left;
    return pivot;
}

template<typename Iterator, class Compare>
constexpr void quicksort(Iterator left, Iterator right, Compare const &compare) {
    while (0 < right - left) {
        auto new_pivot = _partition(left, right, compare);
        quicksort(left, new_pivot, compare);
        left = new_pivot + 1;
    }
}

template<typename Container, class Compare>
constexpr Container quicksort(Container const &array, Compare const &compare) {
    Container res = array;
    quicksort(res.begin(), res.end() - 1, compare);
    return res;
}

/*************************************************************************************************/

template<std::size_t N, typename T, typename CmpLess = std::less<T>>
struct sorted_vector {
private:
    using StorageType = std::array<T, N>;
    StorageType m_data;

    constexpr sorted_vector(const StorageType &arr)
        :m_data{quicksort(arr, CmpLess{})}
    {}

public:
    template<typename ...U>
    constexpr sorted_vector(U ...elems)
        :sorted_vector(StorageType{elems...})
    {}

    constexpr auto  size()  const noexcept { return N; }
    constexpr auto* begin() const noexcept { return m_data.begin(); }
    constexpr auto* end  () const noexcept { return m_data.end(); }

    constexpr auto& operator[](std::size_t i) const noexcept { return m_data[i]; }
};

template<typename T, typename CmpLess>
struct sorted_vector<1, T, CmpLess> {
private:
    const T m_data[1];

public:
    constexpr sorted_vector(T x)
        :m_data{x}
    {}

    constexpr auto  size()  const noexcept { return 1; }
    constexpr auto* begin() const noexcept { return m_data.begin(); }
    constexpr auto* end  () const noexcept { return m_data.end(); }

    constexpr auto& operator[](std::size_t i) const noexcept { return m_data[i]; }
};

template<typename T, typename CmpLess>
struct sorted_vector<0, T, CmpLess> {
private:

public:
    constexpr sorted_vector()
    {}

    constexpr auto  size()  const noexcept { return 0; }
    constexpr auto* begin() const noexcept { return nullptr; }
    constexpr auto* end  () const noexcept { return nullptr; }

    constexpr auto& operator[](std::size_t /*i*/) const { throw std::invalid_argument("zero size vector!"); }
};

/*************************************************************************************************/

template<typename T, typename ...Ts>
constexpr auto make_sorted_vector(T && t, Ts && ...ts) {
    return details::sorted_vector<1 + sizeof...(Ts), T>(
        std::forward<T>(t), std::forward<Ts>(ts)...);
}

template<typename T, typename CmpLess, typename ...Ts>
constexpr auto make_sorted_vector_cmp(CmpLess, T && t, Ts && ...ts) {
    return details::sorted_vector<1 + sizeof...(Ts), T, CmpLess>(
        std::forward<T>(t), std::forward<Ts>(ts)...);
}

/*************************************************************************************************/

} // ns details

/*************************************************************************************************/

template<typename T>
using optional_t = std::pair<bool, T>;

template<
     std::size_t N
    ,typename K
    ,typename V
    ,typename CmpLess = std::less<std::pair<K, V>>
    ,typename Storage = details::sorted_vector<N, std::pair<K, V>, CmpLess>
>
struct map {
    template<typename... Ts>
    constexpr map(Ts && ...ts)
        :vec{std::forward<Ts>(ts)...}
    {}

    constexpr auto* begin() const noexcept { return vec.begin(); }
    constexpr auto* end  () const noexcept { return vec.end  (); }
    constexpr auto  size () const noexcept { return vec.size();  }

    constexpr optional_t<V> find(const K &k) const noexcept { return lower_bound(k); }
    constexpr bool contains(const K &k)      const noexcept { return lower_bound(k).first; }

    constexpr auto& operator[](std::size_t i) const noexcept { return vec[i]; }

private:
    constexpr optional_t<V> lower_bound(const K &k) const noexcept {
        const auto *beg = vec.begin();
        const auto *end = vec.end();
        std::size_t count = N;

        while ( count > 0 ) {
            if ( (beg+count/2)->first < k ) {
                beg = beg+count/2+1;
                count -= count/2+1;
            } else {
                count = count/2;
            }
        }
        if ( beg != end ) {
            return {(beg->first == k), beg->second};
        }
        return {false, V{}};
    }

private:
    Storage vec;
};

template<
     typename K
    ,typename V
    ,typename CmpLess
    ,typename Storage
>
struct map<1, K, V, CmpLess, Storage> {
    template<typename... Ts>
    constexpr map(Ts && ...ts)
        :vec{std::forward<Ts>(ts)...}
    {}

    constexpr auto& operator[](std::size_t i) const noexcept { return vec[i]; }

    constexpr auto* begin() const noexcept { return vec.begin(); }
    constexpr auto* end  () const noexcept { return vec.end  (); }
    constexpr auto  size () const noexcept { return vec.size (); }

    constexpr optional_t<V> find(const K &k) const noexcept {
        return vec[0].first == k
            ? optional_t<V>{true, vec[0].second}
            : optional_t<V>{false, V{}}
        ;
    }
    constexpr bool contains(const K &k) const noexcept { return find(k).first; }

private:
    Storage vec;
};

/***********************************************************************************/

template<typename K, typename V, template<typename, typename> class ...Pairs>
constexpr auto make_map(Pairs<K, V> && ...ts) {
    using mapped_type = std::pair<K, V>;
    using cmp_less = std::less<mapped_type>;
    return map<sizeof...(Pairs), K, V, cmp_less>(std::forward<Pairs<K, V>>(ts)...);
}

template<typename CmpLess, typename K, typename V, template<typename, typename> class ...Pairs>
constexpr auto make_map_cmp(CmpLess, Pairs<K, V> && ...ts) {
    return map<sizeof...(Pairs), K, V, CmpLess>(std::forward<Pairs<K, V>>(ts)...);
}

/*************************************************************************************************/

} // ns ctmap

/*************************************************************************************************/

#endif // __CTMAP__CTMAP_HPP
