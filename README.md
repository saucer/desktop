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
    VERSION        4.0.0
    GIT_REPOSITORY "https://github.com/saucer/desktop"
  )
  ```

* Using FetchContent
  ```cmake
  include(FetchContent)

  FetchContent_Declare(saucer-desktop GIT_REPOSITORY "https://github.com/saucer/desktop" GIT_TAG v4.0.0)
  FetchContent_MakeAvailable(saucer-desktop)
  ```

Finally, link against target:

```cmake
target_link_libraries(<target> saucer::desktop)
```

## 📃 Usage

For a usage example see the [desktop-example](https://github.com/saucer/saucer/blob/master/examples/desktop/main.cpp)
