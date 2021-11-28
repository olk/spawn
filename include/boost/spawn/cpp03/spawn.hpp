
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2017,2021 Oliver Kowalke (oliver dot kowalke at gmail dot com)
// Copyright (c) 2019 Casey Bodley (cbodley at redhat dot com)
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SPAWN_SPAWN_H
#define BOOST_SPAWN_SPAWN_H

#include <boost/asio/detail/config.hpp>
#include <boost/context/fixedsize_stack.hpp>
#include <boost/context/segmented_stack.hpp>
#include <boost/core/enable_if.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/system_error.hpp>
#include <boost/type_traits.hpp>

#include <boost/spawn/cpp03/detail/net.hpp>
#include <boost/spawn/cpp03/detail/is_stack_allocator.hpp>

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
            boost::weak_ptr< detail::spawn_context > const& callee,
            detail::spawn_context & caller,
            Handler & handler) :
        callee_( callee),
        caller_( caller),
        handler_( handler),
        ec_( 0) {
    }

    // Construct a yield context from another yield context type.
    // Requires that OtherHandler be convertible to Handler.
    template< typename OtherHandler >
    basic_yield_context(
            basic_yield_context< OtherHandler > const& other) :
        callee_( other.callee_),
        caller_( other.caller_),
        handler_( other.handler_),
        ec_( other.ec_) {
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
    boost::weak_ptr< detail::spawn_context >    callee_;
    detail::spawn_context &                     caller_;
    Handler                                     handler_;
    boost::system::error_code *                 ec_;
};

typedef basic_yield_context<
        detail::net::executor_binder< void(*)(), detail::net::executor >
    > yield_context;

}

// The spawn_fiber() function is a high-level wrapper over the Boost.Context
// library (spawn_context). This function enables programs to
// implement asynchronous logic in a synchronous manner.
template< typename Function, typename StackAllocator = boost::context::default_stack >
void spawn_fiber( BOOST_ASIO_MOVE_ARG(Function) fn,
        StackAllocator const& salloc = StackAllocator(),
        boost::enable_if<
            boost::spawn::detail::is_stack_allocator< typename boost::decay< StackAllocator >::type >::value
        >::type = 0);

template< typename Handler, typename Function, typename StackAllocator = boost::context::default_stack >
void spawn_fiber( BOOST_ASIO_MOVE_ARG(Handler) hndlr,
        BOOST_ASIO_MOVE_ARG(Function) fn,
        StackAllocator const& salloc = StackAllocator(),
        boost::enable_if<
            ! boost::spawn::detail::net::is_executor< typename boost::decay< Handler >::type >::value &&
            ! boost::is_convertible< Handler &, boost::spawn::detail::net::execution_context & >::value &&
            ! boost::spawn::detail::is_stack_allocator< typename boost::decay< Function >::type >::value &&
            boost::spawn::detail::is_stack_allocator< typename boost::decay< StackAllocator >::type >::value
        >::type = 0);

template< typename Handler, typename Function, typename StackAllocator = boost::context::default_stack >
void spawn_fiber( boost::spawn::basic_yield_context< Handler > ctx,
        BOOST_ASIO_MOVE_ARG(Function) fn,
        StackAllocator const& salloc = StackAllocator(),
        boost::enable_if<
            boost::spawn::detail::is_stack_allocator< typename boost::decay< StackAllocator >::type >::value
        >::type = 0);

template< typename Function, typename Executor, typename StackAllocator = boost::context::default_stack >
void spawn_fiber( Executor const& ex,
        BOOST_ASIO_MOVE_ARG(Function) function,
        StackAllocator const& salloc = StackAllocator(),
        boost::enable_if<
            boost::spawn::detail::net::is_executor< Executor >::value &&
            boost::spawn::detail::is_stack_allocator< typename boost::decay< StackAllocator >::type >::value
        >::type = 0);

template< typename Function, typename Executor, typename StackAllocator = boost::context::default_stack >
void spawn_fiber( boost::spawn::detail::net::strand< Executor > const& ex,
        BOOST_ASIO_MOVE_ARG(Function) function,
        StackAllocator const& salloc = StackAllocator(),
        boost::enable_if<
            boost::spawn::detail::is_stack_allocator< typename boost::decay< StackAllocator >::type >::value
        >::type = 0);

#if !defined(BOOST_ASIO_NO_TS_EXECUTORS)
template< typename Function, typename StackAllocator = boost::context::default_stack >
void spawn_fiber( boost::asio::io_context::strand const& s,
        BOOT_ASIO_MOVE_ARG(Function) function,
        StackAllocator const& salloc = StackAllocator(),
        boost::enable_if<
            boost::spawn::detail::is_stack_allocator< typename boost::decay< StackAllocator >::type >::value
        >::type = 0);
#endif

template< typename Function, typename ExecutionContext, typename StackAllocator = boost::context::default_stack >
void spawn_fiber( ExecutionContext & ctx,
        BOOT_ASIO_MOVE_ARG(Function) function,
        StackAllocator const& salloc = StackAllocator(),
        boost::enable_if<
            boost::is_convertible< ExecutionContext &, boost::spawn::detail::net::execution_context & >::value &&
            boost::spawn::detail::is_stack_allocator< typename boost::decay< StackAllocator >::type >::value
        >::type = 0);

}

#include <boost/spawn/cpp03/impl/spawn.hpp>

#endif // BOOST_SPAWN_SPAWN_H
