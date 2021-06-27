boost.spawn
=============

boost.spawn creates a new fiber and starts new stackful thread of execution.
The spawn() function is a high-level wrapper over the boost.context library. This function
enables programs to implement asynchronous logic in a synchronous manner.
Suspending/resuming of the spawned fiber is controlled by boost.asio.

In contrast to boost.asio that uses the deprecated boost.coroutine
library for its boost::asio::spawn() the implementation of boost.spawn is based only on boost.context.

boost.context requires C++11! 
