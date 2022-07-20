<hr/>

<div align="center"> 
    <img src="assets/logo.svg" height=312>
</div>

<br/>

<p align="center">
    A C++ RAII PipeWire-API Wrapper
</p>

<hr/>

<div align="center">

### Description

<div align="left">

_Rohrkabel_ is a RAII wrapper around the pipewire-api that aims to simplify work with it, at the moment _Rohrkabel_ is only used by Soundux to reduce LOC and simplify the work with pipewire.  

**Currently only a portion of the pipewire-api is supported.**  

Depending on personal demand from Soundux or the community more portions of the API can be covered, simply open an issue if you'd like something to be added.

</div>

### Installation

<div align="left">

- With FetchContent
  ```cmake
  include(FetchContent)

  FetchContent_Declare(rohrkabel GIT_REPOSITORY "https://github.com/Soundux/rohrkabel" GIT_TAG v1.1)
  FetchContent_MakeAvailable(rohrkabel)

  target_link_libraries(<target> rohrkabel)
  ```
- As Git-Submodule
  ```bash
  $ git clone https://github.com/Soundux/rohrkabel
  ```
  ```cmake
  add_subdirectory(rohrkabel)
  target_link_libraries(<target> rohrkabel)
  ```


</div>

### Problematic Distributions

<center>

| Distro                     | Supported OOTB | Notes                                              |
| -------------------------- | :------------: | -------------------------------------------------- |
| Ubuntu 20.04               |       ❌        | Supplied PipeWire-Version too old, works with PPA¹ |
| Ubuntu 21.10 _(and later)_ |       ✅        |                                                    |

<div align="left">

> ¹ `sudo add-apt-repository ppa:pipewire-debian/pipewire-upstream`

</div>

</center>

### Why `rohrkabel`?

<div align="left">

The name `rohrkabel` is a 1 to 1 translation of `pipewire` to german.  
I chose this name because I would occasionally rant about some things that I didn't like about pipewire to my german friends and thought it would be funny if I call the thing `rohrkabel` instead.

</div>

### Thread-Safety


<div align="left">

Since the pipewire-api is not thread-safe by default² , special mechanisms have to be used to ensure thread-safety.  

*Rohrkabel* supports *pipewire*s `pw_thread_loop`.  
All operations that can be traced back to a `context` created from the `thread_loop` should `lock()` and `unlock()` the `thread_loop`.  
`std::lock_guard` can be used to automate the locking and unlocking.  
For an example on how to use the `thread_loop` see [this example](examples/thread-loop/main.cpp).

> ² https://docs.pipewire.org/page_thread_loop.html

</div>

<hr/>

### Documentation

Documentation can be found [here](https://curve.github.io/rohrkabel.docs/).

### Projects using _Rohrkabel_
<img src="https://avatars.githubusercontent.com/u/74979035?s=200&v=4" width=15/> <b>[Soundux](https://github.com/Soundux)</b>

</div>