<div align="center"> 
    <img src="assets/logo.svg" height=312>
</div>

<br/>

<p align="center">
    A C++20 pipewire-api wrapper
</p>


## 📃 Description

_rohrkabel_ is a wrapper around the pipewire-api that embraces RAII and tries to simplify working with pipewire.

> [!NOTE]  
> As of writing only a portion of the pipewire-api has been wrapped.  
> In case we're missing something you need feel free to [open an issue](https://github.com/Curve/rohrkabel/issues/new).  
> More portions of the api might be covered in the future depending on personal demand as well as demand from [Soundux](https://github.com/Soundux/Soundux). 

## 📦 Installation

* Using [CPM](https://github.com/cpm-cmake/CPM.cmake)
  ```cmake
  CPMFindPackage(
    NAME           rohrkabel
    VERSION        4.0
    GIT_REPOSITORY "https://github.com/Curve/rohrkabel"
  )
  ```

* Using FetchContent
  ```cmake
  include(FetchContent)

  FetchContent_Declare(rohrkabel GIT_REPOSITORY "https://github.com/Curve/rohrkabel" GIT_TAG v4.0)
  FetchContent_MakeAvailable(rohrkabel)

  target_link_libraries(<target> cr::rohrkabel)
  ```

## 🧵 Thread-Safety

The `pw_main_loop` is not thread-safe, but supplies a [special loop implementation](https://docs.pipewire.org/page_thread_loop.html) meant for threaded-usage. However said special loop is cumbersome to deal with, especially from a RAII perspective, thus _rohrkabel_ uses a different, [channel-based](https://github.com/Curve/channel) approach inspired by [pipewire-rs](https://pipewire.pages.freedesktop.org/pipewire-rs/pipewire/channel/index.html).

The channel based approach greatly simplifies working with _rohrkabel_ in multi-threaded environments.

For more information check out [this example](examples/channels), or feel free to [open a discussion](https://github.com/Curve/rohrkabel/discussions/new/choose).

## 🌎 Who's using rohrkabel?

<div align="center">
<br>

<img src="https://avatars.githubusercontent.com/u/74979035?s=200&v=4" width=30/>

[Soundux](https://github.com/Soundux)

<img src="https://raw.githubusercontent.com/maltejur/discord-screenaudio/master/assets/de.shorsh.discord-screenaudio.png" width=30/>

[Discord Screenaudio](https://github.com/maltejur/discord-screenaudio)

<img src="https://avatars.githubusercontent.com/u/113042587?s=200&v=4" width=30/>

[Vencord](https://github.com/Vencord/linux-virtmic)

<img src="https://raw.githubusercontent.com/PolisanTheEasyNick/Crescendo/main/icon.svg" width=30/>

[Crescendo](https://github.com/PolisanTheEasyNick/Crescendo)

</div>
