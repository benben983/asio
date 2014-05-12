//
// impl/system_executor.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IMPL_SYSTEM_EXECUTOR_HPP
#define ASIO_IMPL_SYSTEM_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/executor_op.hpp"
#include "asio/detail/global.hpp"
#include "asio/detail/task_io_service_allocator.hpp"
#include "asio/detail/type_traits.hpp"
#include "asio/execution_context.hpp"

#include "asio/detail/push_options.hpp"

namespace asio {

inline execution_context& system_executor::context()
{
  return detail::global<context_impl>();
}

template <typename Function>
void system_executor::dispatch(ASIO_MOVE_ARG(Function) f)
{
  typename decay<Function>::type tmp(ASIO_MOVE_CAST(Function)(f));
  tmp();
}

template <typename Function>
void system_executor::post(ASIO_MOVE_ARG(Function) f)
{
  // Make a local, non-const copy of the function.
  typedef typename decay<Function>::type function_type;
  function_type tmp(ASIO_MOVE_CAST(Function)(f));

  // Allocate and construct an operation to wrap the function.
  typedef detail::task_io_service_allocator<void> allocator_type;
  typedef detail::executor_op<function_type, allocator_type> op;
  typename op::ptr p = { allocator_type(), 0, 0 };
  p.v = p.a.allocate(1);
  p.p = new (p.v) op(tmp, allocator_type());

  ASIO_HANDLER_CREATION((p.p, "system_executor", this, "post"));

  context_impl& ctx = detail::global<context_impl>();
  ctx.scheduler_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}

template <typename Function>
void system_executor::defer(ASIO_MOVE_ARG(Function) f)
{
  // Make a local, non-const copy of the function.
  typedef typename decay<Function>::type function_type;
  function_type tmp(ASIO_MOVE_CAST(Function)(f));

  // Allocate and construct an operation to wrap the function.
  typedef detail::task_io_service_allocator<void> allocator_type;
  typedef detail::executor_op<function_type, allocator_type> op;
  typename op::ptr p = { allocator_type(), 0, 0 };
  p.v = p.a.allocate(1);
  p.p = new (p.v) op(tmp, allocator_type());

  ASIO_HANDLER_CREATION((p.p, "system_executor", this, "defer"));

  context_impl& ctx = detail::global<context_impl>();
  ctx.scheduler_.post_immediate_completion(p.p, true);
  p.v = p.p = 0;
}

} // namespace asio

#include "asio/detail/pop_options.hpp"

#endif // ASIO_IMPL_SYSTEM_EXECUTOR_HPP