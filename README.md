# embedded-resources
Embedded Artistry Public Resources

Contains templates, documents, and source code examples referenced on https://embeddedartistry.com.

**Table of Contents**

1. [Requirements](#requirements)
	1. [git-lfs](#git-lfs)
	2. [meson](#meson)
2. [Structure](#structure)
3. [Building](#building)

# Requirements

This repository uses submodules. You can clone the repository recursively to automatically setup submodules:

```
$ git clone https://github.com/embeddedartistry/embedded-resources.git --recursive
```

You can also initialize submodules after cloning:

```
$ git submodule update --init --recursive
```

## git-lfs

This repository requires git-lfs.  If you do not have this installed, please visit https://git-lfs.github.com

If you cloned this repository before installing git-lfs, please run `git lfs pull`.  Otherwise clone will automatically perform a `git lfs pull`.

## meson

This repository builds with [meson](ttps://mesonbuild.com), which requires Python 3 and Ninja.

On Ubuntu these can be easily installed with the following command:

```
$ sudo apt-get install python3 python3-pip ninja-build
```

For OSX, you can use `brew`:

```
$ brew install python3 ninja
```

The best way to get Meson is through pip:

```
$ pip3 install meson
```

# Structure

* `build/`
	* Common build scripts and definitions
* `docs`
	* Open-source project templates and reference documentation
* `examples/`
	* `c/`
		* C examples for the Embedded Artistry website
	* `cpp/`
		* C++ examples for the Embedded Artistry website
	* `libc/`
		* Example libc implementations
	* `libcpp/`
		* Example libcpp implementations
* `interview/`
	* Example interview question implementations
* `manufacturing/`
	* Documents & templates that are useful for the manufacturing side of the embedded world.

# Building

You can run `make` from the top level to build all examples.  Output will be placed in a folder called `buildresults/` at the top level.

You can also use the proper meson syntax:

```
$ meson buildresults
$ cd buildresults
$ ninja
```

Targets can be built individually by using the ninja interface in the `buildresults/` directory.

```
$ cd buildresults
$ ninja interview/bad_c
```

To clean the builds, run `make clean` from the project root or `ninja clean` in the `buildresults/` directory.

Binaries will be stored under the `buildresults/` folder at the same hierarchical level as in the source tree.For example, the `bad_c` interview demo application will be in `buildresults/interview/`.

# Further Reading

* [Meson](https://www.mesonbuild.com)
* [Full Embedded Artistry libc implementation](https://github.com/embeddedartistry/libc)
* [Embedded Artistry libmemory](https://github.com/embeddedartistry/libmemory)
