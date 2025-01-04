<hr>

<div align="center"> 
    <img src="https://raw.githubusercontent.com/saucer/saucer.github.io/master/static/img/logo.png" height="312" />
</div>

<p align="center"> 
    Desktop module for <a href="https://github.com/saucer/saucer">saucer</a>
</p>

---

## 📦 Installation

* Using [CPM](https://github.com/cpm-cmake/CPM.cmake)
  ```cmake
  CPMFindPackage(
    NAME           saucer-desktop
    VERSION        2.0.0
    GIT_REPOSITORY "https://github.com/saucer/desktop"
  )
  ```

* Using FetchContent
  ```cmake
  include(FetchContent)

  FetchContent_Declare(saucer-desktop GIT_REPOSITORY "https://github.com/saucer/desktop" GIT_TAG v2.0.0)
  FetchContent_MakeAvailable(saucer-desktop)
  ```

Finally, link against target:

```cmake
target_link_libraries(<target> saucer::desktop)
```

## 📃 Usage

```cpp
using saucer::modules::desktop::picker::type;

auto app      = saucer::application::acquire(/*...*/);
auto& desktop = app->add_module<saucer::modules::desktop>();

desktop.open("https://google.com");
auto file = desktop.pick<type::file>({.filters = {"*.cpp"}});
```
