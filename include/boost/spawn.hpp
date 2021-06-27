
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2017,2021 Oliver Kowalke (oliver dot kowalke at gmail dot com)
// Copyright (c) 2019 Casey Bodley (cbodley at redhat dot com)
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SPAWN_SPAWN_H
#define BOOST_SPAWN_SPAWN_H

#include <memory>

#include <boost/system/system_error.hpp>

#include <boost/context/fixedsize_stack.hpp>
#include <boost/context/segmented_stack.hpp>

#include <boost/spawn/detail/net.hpp>
#include <boost/spawn/detail/is_stack_allocator.hpp>

namespace boost {
namespace spawn {
namespace detail {

class spawn_context;

}

// Context object represents the current execution context.
// The basic_yield_context class is used to represent the current execution
// context. A basic_yield_context may be passed as a handler to an
// asynchronous operation. For example:
template< typename Handler >
class basic_yield_context {
public:
    // Construct a yield context to represent the specified execution context.
    // Most applications do not need to use this constructor. Instead, the
    // spawn_fiber() function passes a yield context as an argument to the fiber
    // function.
    basic_yield_context(
            std::weak_ptr< detail::spawn_context > const& callee,
            detail::spawn_context & caller,
            Handler & handler) :
        callee_{ callee },
        caller_{ caller },
        handler_{ handler },
        ec_{ 0 } {
    }

    // Construct a yield context from another yield context type.
    // Requires that OtherHandler be convertible to Handler.
    template< typename OtherHandler >
    basic_yield_context(
            basic_yield_context< OtherHandler > const& other) :
        callee_{ other.callee_ },
        caller_{ other.caller_ },
        handler_{ other.handler_ },
        ec_{ other.ec_ } {
    }

    // Return a yield context that sets the specified error_code.
    // By default, when a yield context is used with an asynchronous operation, a
    // non-success error_code is converted to system_error and thrown. This
    // operator may be used to specify an error_code object that should instead be
    // set with the asynchronous operation's result. For example:
    basic_yield_context operator[]( boost::system::error_code & ec) const {
        basic_yield_context tmp{ * this };
        tmp.ec_ = & ec;
        return tmp;
    }

//private:
    std::weak_ptr< detail::spawn_context >  callee_;
    detail::spawn_context &                 caller_;
    Handler                                 handler_;
    boost::system::error_code *             ec_;
};

using yield_context = basic_yield_context< detail::net::executor_binder< void(*)(), detail::net::executor > >;

}

// The spawn_fiber() function is a high-level wrapper over the Boost.Context
// library (spawn_context). This function enables programs to
// implement asynchronous logic in a synchronous manner.
template< typename Function, typename StackAllocator = boost::context::default_stack >
auto spawn_fiber( Function && fn, StackAllocator && salloc = StackAllocator() )
    -> typename std::enable_if<
            boost::spawn::detail::is_stack_allocator< typename std::decay< StackAllocator >::type >::value
        >::type;

template< typename Handler, typename Function, typename StackAllocator = boost::context::default_stack >
auto spawn_fiber( Handler && hndlr, Function && fn, StackAllocator && salloc = StackAllocator() )
    -> typename std::enable_if<
            ! boost::spawn::detail::net::is_executor< typename std::decay< Handler >::type >::value &&
            ! std::is_convertible< Handler &, boost::spawn::detail::net::execution_context & >::value &&
            ! boost::spawn::detail::is_stack_allocator< typename std::decay< Function >::type >::value &&
            boost::spawn::detail::is_stack_allocator< typename std::decay< StackAllocator >::type >::value
        >::type;

template< typename Handler, typename Function, typename StackAllocator = boost::context::default_stack >
auto spawn_fiber( boost::spawn::basic_yield_context< Handler > ctx, Function && fn, StackAllocator && salloc = StackAllocator() )
    -> typename std::enable_if<
            boost::spawn::detail::is_stack_allocator< typename std::decay< StackAllocator >::type >::value
        >::type;

template< typename Function, typename Executor, typename StackAllocator = boost::context::default_stack >
auto spawn_fiber( Executor const& ex, Function && function, StackAllocator && salloc = StackAllocator() )
    -> typename std::enable_if<
            boost::spawn::detail::net::is_executor< Executor >::value &&
            boost::spawn::detail::is_stack_allocator< typename std::decay< StackAllocator >::type >::value
        >::type;

template< typename Function, typename Executor, typename StackAllocator = boost::context::default_stack >
auto spawn_fiber( boost::spawn::detail::net::strand< Executor > const& ex, Function && function, StackAllocator && salloc = StackAllocator() )
    -> typename std::enable_if<
            boost::spawn::detail::is_stack_allocator< typename std::decay< StackAllocator >::type >::value
        >::type;

template< typename Function, typename ExecutionContext, typename StackAllocator = boost::context::default_stack >
auto spawn_fiber( ExecutionContext & ctx, Function && function, StackAllocator && salloc = StackAllocator() )
    -> typename std::enable_if<
            std::is_convertible< ExecutionContext &, boost::spawn::detail::net::execution_context & >::value &&
            boost::spawn::detail::is_stack_allocator< typename std::decay< StackAllocator >::type >::value
        >::type;

}

#include <boost/spawn/impl/spawn.hpp>

#endif // BOOST_SPAWN_SPAWN_H
