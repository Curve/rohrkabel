<hr/>

<div align="center"> 
    <img src="assets/logo.svg" height=312>
</div>

<br/>

<p align="center">
    A C++ RAII PipeWire-API Wrapper
</p>

<hr/>

## Description

<div align="left">

_Rohrkabel_ is a RAII wrapper around the pipewire-api that aims to simplify work with it.

**Currently only a portion of the pipewire-api is supported.**  

Depending on personal demand from Soundux or the community more portions of the API can be covered, simply open an issue if you'd like something to be added.

</div>

## Installation

<div align="left">

- With FetchContent
  ```cmake
  include(FetchContent)

  FetchContent_Declare(rohrkabel GIT_REPOSITORY "https://github.com/Soundux/rohrkabel" GIT_TAG v1.3)
  FetchContent_MakeAvailable(rohrkabel)

  target_link_libraries(<target> rohrkabel)
  ```
- As Git-Submodule
  ```bash
  $ git submodule add https://github.com/Soundux/rohrkabel
  $ cd rohrkabel
  $ git checkout v1.3
  ```
  ```cmake
  add_subdirectory(rohrkabel)
  target_link_libraries(<target> rohrkabel)
  ```


</div>

## Problematic Distributions

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

*Rohrkabel* supports `channel`s.  
Channels can be used for inter-thread communication and should be used if you want to communicate with a thread that is executing the `main_loop`.

For more information check out the [example](examples/channels/main.cpp).

> ² https://docs.pipewire.org/page_thread_loop.html  

> The idea for the `channel` mechanism was borrowed from [`pipewire-rs`](https://pipewire.pages.freedesktop.org/pipewire-rs/pipewire/channel/index.html#examples) 

</div>

<hr/>

## Documentation

Documentation can be found [here](https://curve.github.io/rohrkabel.docs/).

---

<div align="center">

### Projects using _Rohrkabel_
<img src="https://avatars.githubusercontent.com/u/74979035?s=200&v=4" width=15/> <b>[Soundux](https://github.com/Soundux)</b>

<img src="https://raw.githubusercontent.com/maltejur/discord-screenaudio/master/assets/de.shorsh.discord-screenaudio.png" width=15/> <b>[discord-screenaudio](https://github.com/maltejur/discord-screenaudio)</b>

</div>

---