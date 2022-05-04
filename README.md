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

  FetchContent_Declare(rohrkabel GIT_REPOSITORY "https://github.com/Soundux/rohrkabel")
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

<hr/>

### Projects using _Rohrkabel_
<img src="https://avatars.githubusercontent.com/u/74979035?s=200&v=4" width=15/> <b>[Soundux](https://github.com/Soundux)</b>

</div>