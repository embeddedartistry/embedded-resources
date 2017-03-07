# embedded-resources
Embedded Artistry Public Resources

Contains templates, documents, and source code examples referenced on https://embeddedartistry.com.

# Requirements
This repository requires git-lfs.  If you do not have this installed, please visit https://git-lfs.github.com

If you cloned this repository before installing git-lfs, please run `git lfs pull`.  Otherwise clone will automatically perform a `git lfs pull`.

# Structure

* build/
	* Common build definitions used in the various Makefiles throughout the repository
* examples/
	* c
		* C examples for the Embedded Artistry website
	* cpp/
		* C++ examples for the Embedded Artistry website
	* libc/
		* Example libc implementation
* interview/
	* Example interview question implementations
* manufacturing/
	* Documents & templates that are useful for the manufacturing side of the embedded world.

# Building

You can run `make` from the top level to build all examples.  You can also run `make interview`, `make c`, `make cpp`, `make libc` to build only specific examples.  Output will be placed in a folder called `buildresults/` at the top level.

Each of the source directories contains a Makefile that can be used independently.  Each of these Makefiles has separate targets for the individual examples.  Running `make` on any of the source folders will build all the contained examples.  Results will be placed in a folder called `buildresults` within the particular source directory that was built.

