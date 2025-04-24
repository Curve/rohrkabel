<div align="center"> 
    <img src="assets/logo.svg" height="256">
</div>


## 📃 Description

_Rohrkabel_ is a C++23 wrapper around the pipewire-api that embraces **RAII** and **Coroutines**.

As of writing this, only a portion of the pipewire-api has been wrapped.
However, this library easily allows to access the underlying wrapped objects and also allows to wrap existing pipewire objects without taking over their lifetime.

More portions of the pipewire-api will be wrapped based on user demand or (in-house) downstream ([Soundux](https://github.com/Soundux/Soundux), [venmic](https://github.com/Vencord/venmic)) demand.

## 📦 Installation

* Using [CPM](https://github.com/cpm-cmake/CPM.cmake)
  ```cmake
  CPMFindPackage(
    NAME           rohrkabel
    VERSION        9.0.0
    GIT_REPOSITORY "https://github.com/Curve/rohrkabel"
  )
  ```

* Using FetchContent
  ```cmake
  include(FetchContent)

  FetchContent_Declare(rohrkabel GIT_REPOSITORY "https://github.com/Curve/rohrkabel" GIT_TAG v9.0.0)
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
