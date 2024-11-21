
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

#include <ctmap/ctmap.hpp>

#include <iostream>
#include <cassert>

#ifdef NDEBUG
#   error "This file MUST be compiled with NDEBUG undefined!"
#endif

struct pair_cmp_less {
    template<typename T, typename U>
    constexpr bool operator() (const T &l, const U &r)
        const noexcept(noexcept(l.first < r.first))
    { return l.first < r.first; }
};

int func(int v) { return v; }

/***********************************************************************************/

int main(int argc, char **) {
    {
        constexpr auto vec = ctmap::details::make_sorted_vector(3, 6, 1, 8);
        static_assert(vec.size() == 4, "");

        static_assert(vec[0] == 1, "");
        static_assert(vec[1] == 3, "");
        static_assert(vec[2] == 6, "");
        static_assert(vec[3] == 8, "");
    }
    {
        constexpr auto vec = ctmap::details::make_sorted_vector_cmp(
             pair_cmp_less{}
            ,std::make_pair(3, 1)
            ,std::make_pair(6, 2)
            ,std::make_pair(1, 0)
            ,std::make_pair(8, 3)
        );
        static_assert(vec.size() == 4, "");

        static_assert(vec[0].first == 1, "");
        static_assert(vec[0].second == 0, "");

        static_assert(vec[1].first == 3, "");
        static_assert(vec[1].second == 1, "");

        static_assert(vec[2].first == 6, "");
        static_assert(vec[2].second == 2, "");

        static_assert(vec[3].first == 8, "");
        static_assert(vec[3].second == 3, "");
    }

    {
        constexpr auto m = ctmap::make_map_cmp(
             pair_cmp_less{}
            ,std::make_pair(1,0)
            ,std::make_pair(4,1)
            ,std::make_pair(2,2)
            ,std::make_pair(5,3)
            ,std::make_pair(3,4)
        );

        constexpr auto r0 = m.find(1);
        static_assert(r0.first == true, "");
        static_assert(r0.second == 0, "");

        constexpr auto r1 = m.find(4);
        static_assert(r1.first == true, "");
        static_assert(r1.second == 1, "");

        constexpr auto r2 = m.find(2);
        static_assert(r2.first == true, "");
        static_assert(r2.second == 2, "");

        constexpr auto r3 = m.find(5);
        static_assert(r3.first == true, "");
        static_assert(r3.second == 3, "");

        constexpr auto r4 = m.find(3);
        static_assert(r4.first == true, "");
        static_assert(r4.second == 4, "");

        constexpr auto r5 = m.find(6);
        static_assert(r5.first == false, "");
        static_assert(r5.second == 0, "");
    }
    {
        constexpr auto m = ctmap::make_map_cmp(
             pair_cmp_less{}
            ,std::make_pair(1, 0)
        );

        constexpr auto r0 = m.find(1);
        static_assert(r0.first == true, "");
        static_assert(r0.second == 0, "");

        constexpr auto r5 = m.find(6);
        static_assert(r5.first == false, "");
        static_assert(r5.second == 0, "");
    }
    {
        constexpr auto m0 = ctmap::make_map_cmp(
             pair_cmp_less{}
            ,std::make_pair(1, 0)
            ,std::make_pair(4, 1)
            ,std::make_pair(2, 2)
            ,std::make_pair(5, 3)
            ,std::make_pair(3, 4)
        );

        static_assert(m0[0].first == 1 && m0[0].second == 0, "");
        static_assert(m0[1].first == 2 && m0[1].second == 2, "");
        static_assert(m0[2].first == 3 && m0[2].second == 4, "");
        static_assert(m0[3].first == 4 && m0[3].second == 1, "");
        static_assert(m0[4].first == 5 && m0[4].second == 3, "");

        std::cout << "1:" << std::endl;
        for ( const auto &it: m0 )
            std::cout << it.first << " -> " << it.second << std::endl;

        std::tuple<int, char, long, short, unsigned> t{1, 2, 3, 4, 5};
        long res = 0;
        switch ( argc ) {
            case m0[0].first: res = std::get<m0[0].second>(t); break;
            case m0[1].first: res = std::get<m0[1].second>(t); break;
            case m0[2].first: res = std::get<m0[2].second>(t); break;
            case m0[3].first: res = std::get<m0[3].second>(t); break;
            case m0[4].first: res = std::get<m0[4].second>(t); break;
        }
        std::cout << "argc=" << argc << ", res=" << res << std::endl;

        switch ( argc ) {
            case 1: {assert(res == 1); break;}
            case 2: {assert(res == 3); break;}
            case 3: {assert(res == 5); break;}
            case 4: {assert(res == 2); break;}
            case 5: {assert(res == 4); break;}
            default: { assert(!"wrong number of cmdline args"); }
        }
    }
    {
        constexpr auto m0 = ctmap::make_map(
             std::make_pair(1, func)
            ,std::make_pair(4, func)
            ,std::make_pair(2, func)
            ,std::make_pair(5, func)
            ,std::make_pair(3, func)
        );
        constexpr auto m1 = ctmap::make_map_cmp(
             pair_cmp_less{}
            ,std::make_pair(1, func)
            ,std::make_pair(4, func)
            ,std::make_pair(2, func)
            ,std::make_pair(5, func)
            ,std::make_pair(3, func)
        );
        static_assert(m0.equal(m1, [](const auto &l, const auto &r){ return l.first == r.first; }));

        std::cout << "2:" << std::endl;
        for ( const auto &it: m0 ) {
            std::cout << it.first << " -> " << it.second(it.first) << std::endl;
        }
    }

    return 0;
}

/***********************************************************************************/
