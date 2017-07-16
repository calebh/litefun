# litefun

litefun is a small library that offers a lightweight implementation of the C++ std::function wrapper. This is useful for embedded systems that do not offer the C++ standard library.

There are two function wrappers defined in this library:
* litefun::function provides a basic wrapper. The closure captured by a lambda passed to this wrapper will be cloned every time the wrapper is copied to a new variable, passed by value to a function, etc.
* litefun::smartfun provides a more advanced wrapper. The lifetime of the closure captured by a lambda passed to this wrapper is controlled by a smart pointer (reference counting). A lightweight implementation of a smart pointer is included in this library as well.