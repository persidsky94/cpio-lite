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

# TODO
  - Export library interface properly (current 'extern "C" ' workaround is very, very dirty)
  - Export ONLY interface functions
  - Support creation of intermediate directories while unpacking
  - Support unpacking device files
  - Assure portability
  - Make samples launch more intuitive and automated
