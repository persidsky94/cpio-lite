# cpio-lite
Small C++ library, implementing some of the cpio util functionality

# How to build
```sh
$ cmake CMakeLists.txt
$ make
```

# Special notes
  - ONLY for C++ client code usage (I know, 'extern "C" ' makes wrong impression)
  - Supports only old cpio archive format
  - Doesn't support creating intermediate directories while unpacking
  - Doesn't support unpacking device files
  - Most options (represented by flags in cpio util) are fixed in this implementation
