# Filament glfw shared context example

This is a demonstration of using a shared context with Filament and GLFW.

It sets up two windows, one invisible to use as a GL share context.

It then creates a GL texture, and passes it in to Filament using Texture::import(), rendering it on a quad with Filament.

## Dependencies

Requires cmake, clang, libc++, lld, glfw, and a local installation of Filament

### Ubuntu 20.04
```bash
apt install clang-12 lld-12 libc++-12-dev libc++abi-12-dev
```

## Building

```bash
mkdir build
cd build
CC=clang CXX=clang++ cmake -DFILAMENT_INSTALL_PATH=<path to Filament install dir> ..
make
```

## Running

In the build dir:
```
./glfw-shared-gl-context
```

To reproduce the `BadMatch` error described at https://github.com/google/filament/issues/5617:
```
./glfw-shared-gl-context --badmatch
```