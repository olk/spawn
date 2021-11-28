
// Copyright (c) 2021 Oliver Kowalke (oliver dot kowalke at gmail dot com)
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/context/detail/config.hpp>


#if defined(BOOST_CONTEXT_NO_CXX11)
#include <boost/spawn/cpp03/spawn.hpp>
#else
#include <boost/spawn/cpp11/spawn.hpp>
#endif
