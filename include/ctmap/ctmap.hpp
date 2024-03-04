
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

namespace ctmap {
namespace details {

/*************************************************************************************************/

template<std::size_t...>
struct integral_holder
{};

template<std::size_t A, std::size_t B, std::size_t... Accum>
struct integral_range_: integral_range_<A+1, B, Accum..., A>
{};

template<std::size_t A, std::size_t... Accum>
struct integral_range_<A, A, Accum...> {
    using type = integral_holder<Accum...>;
};

template<std::size_t A, std::size_t B>
using integral_range = typename integral_range_<A, B>::type;

/*************************************************************************************************/

template<std::size_t N, typename T, typename CmpLess = std::less<T>>
struct sorted_vector {
private:
    template<typename U>
    static constexpr U mymin(U i) { return i; }
    template<typename U, typename... UU>
    static constexpr U mymin(U i, UU... tt) { return CmpLess{}(i, mymin(tt...)) ? i : mymin(tt...); }

    const T data[N];

    template<std::size_t... II, typename... U>
    constexpr sorted_vector(integral_holder<II...> ii, T minval, T a, U... tt)
        :data{
             (a == minval ? a : sorted_vector<N, T, CmpLess>(ii, minval, tt..., a).data[0])
            ,(a == minval ? sorted_vector<N-1, T, CmpLess>(tt...)[II] : sorted_vector<N, T, CmpLess>(ii, minval, tt..., a).data[II+1])...
        }
    {}

public:
    using cmp_less = CmpLess;

    template<typename... U>
    constexpr sorted_vector(U... u)
        :sorted_vector(integral_range<0, sizeof...(u)-1>(), mymin(u...), u...)
    {}

    constexpr auto  size()  const noexcept { return N; }
    constexpr auto* begin() const noexcept { return std::begin(data); }
    constexpr auto* end  () const noexcept { return std::end  (data); }

    constexpr auto& operator[](std::size_t i) const noexcept { return data[i]; }
};

template<typename T, typename CmpLess>
struct sorted_vector<1, T, CmpLess> {
private:
    const T data[1];

public:
    using cmp_less = CmpLess;

    constexpr sorted_vector(T x)
        :data{x}
    {}

    constexpr auto  size()  const noexcept { return 1; }
    constexpr auto* begin() const noexcept { return std::begin(data); }
    constexpr auto* end  () const noexcept { return std::end  (data); }

    constexpr auto& operator[](std::size_t i) const noexcept { return data[i]; }
};

template<typename T, typename CmpLess>
struct sorted_vector<0, T, CmpLess> {
private:

public:
    using cmp_less = CmpLess;

    constexpr sorted_vector(T x)
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

template<typename CmpLess, typename T, typename ...Ts>
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
    ,typename CmpLess = std::less<std::pair<const K, V>>
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
    return map<sizeof...(Pairs), K, V, std::less<std::pair<K, V>>>(std::forward<Pairs<K, V>>(ts)...);
}

template<typename CmpLess, typename K, typename V, template<typename, typename> class ...Pairs>
constexpr auto make_map_cmp(CmpLess, Pairs<K, V> && ...ts) {
    return map<sizeof...(Pairs), K, V, CmpLess>(std::forward<Pairs<K, V>>(ts)...);
}

/*************************************************************************************************/

} // ns ctmap

/*************************************************************************************************/

#endif // __CTMAP__CTMAP_HPP
