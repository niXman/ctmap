
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
#include <cstring>
#include <string_view>

/*************************************************************************************************/

void func0(const char *arg) { std::cout << "hello from func0, arg=" << arg << std::endl; }
void func1(const char *arg) { std::cout << "hello from func1, arg=" << arg << std::endl; }
void func2(const char *arg) { std::cout << "hello from func2, arg=" << arg << std::endl; }
void func3(const char *arg) { std::cout << "hello from func3, arg=" << arg << std::endl; }

/*************************************************************************************************/

int main(int argc, char **argv) {
    constexpr auto func0_name = std::string_view{"func0"};
    constexpr auto func1_name = std::string_view{"func1"};
    constexpr auto func2_name = std::string_view{"func2"};
    constexpr auto func3_name = std::string_view{"func3"};

    static constexpr auto map = ctmap::make_map(
         std::make_pair(func0_name, func0)
        ,std::make_pair(func3_name, func3)
        ,std::make_pair(func1_name, func1)
        ,std::make_pair(func2_name, func2)
    );

    static_assert(map.size() == 4, "");
    constexpr auto *storage = map.begin();

    static_assert(storage[0].first == func0_name, "");
    assert(storage[0].first == func0_name);

    static_assert(storage[1].first == func1_name, "");
    assert(storage[1].first == func1_name);

    static_assert(storage[2].first == func2_name, "");
    assert(storage[2].first == func2_name);

    static_assert(storage[3].first == func3_name, "");
    assert(storage[3].first == func3_name);

    std::cout
        << func0_name << " -> " << (const void *)func0 << std::endl
        << func1_name << " -> " << (const void *)func1 << std::endl
        << func2_name << " -> " << (const void *)func2 << std::endl
        << func3_name << " -> " << (const void *)func3
    << std::endl;

    std::cout << "loop:" << std::endl;
    for ( const auto &it: map ) {
        std::cout << it.first << " -> " << (const void *)it.second << std::endl;
    }

    if ( argc != 2 ) {
        const char *p = std::strrchr(argv[0], '/');
        p = (p) ? (p+1) : argv[0];
        std::cout << "usage: ./" << p << " func0/func1/func2/func3" << std::endl;
        return EXIT_FAILURE;
    }

    std::string_view func_name{argv[1]};
    auto it = map.find(func_name);
    if ( !it.first ) {
        std::cout << "wrong function name: " << func_name << std::endl;
        return EXIT_FAILURE;
    }
    it.second(argv[1]);

    return EXIT_SUCCESS;
}

/*************************************************************************************************/
