#include <cstdio>
#include <memory>
#include <cstring>

#pragma mark - Definitions -

// Unique file type
typedef std::unique_ptr<FILE, decltype(&fclose)> ufile_t;

// Shared file type
typedef std::shared_ptr<FILE> sfile_t;

#pragma mark - Prototypes -

ufile_t fopen_unique(const char* fname, const char* mode);
sfile_t fopen_shared(const char* fname, const char* mode);
static int fclose_wrapper(FILE* file);

#pragma mark - Private Functions -

int fclose_wrapper(FILE* file)
{
	printf("Closing file: %p\n", file);
	return fclose(file);
}

/**
 * Generic Unique Pointer Container for C-style Resources
 *
 * Takes in an acquisiton function and release function (in this case fopen and fclose)
 * And forwards the remaining arguments to the acquisition function.
 *
 * The resulting resource is put into a unique_ptr container along with the release function.
 *
 * You template acquire_unique_resource off of the type: acquire_unique_resource<FILE>(...);
 */
template<typename T, typename AcquisitionFunc, typename ReleaseFunc, typename... Args>
std::unique_ptr<T, ReleaseFunc> acquire_unique_resource(AcquisitionFunc acquire,
														ReleaseFunc release, Args&&... args)
{
	return {acquire(std::forward<Args>(args)...), release};
}

/**
 * Similar function to acquire_unique_resource, except the shared_ptr type
 * doesn't need to know about the type of the release function
 */
template<typename T, typename AcquisitionFunc, typename ReleaseFunc, typename... Args>
std::shared_ptr<T> acquire_shared_resource(AcquisitionFunc acquire, ReleaseFunc release,
										   Args&&... args)
{
	return {acquire(std::forward<Args>(args)...), release};
}

#pragma mark - Public APIs -

ufile_t fopen_unique(const char* fname, const char* mode)
{
	// fclose_wrapper is supplied to provide printing for demonstration purposes
	// in deployment, you could just use fclose()
	return {fopen(fname, mode), fclose_wrapper};
}

sfile_t fopen_shared(const char* fname, const char* mode)
{
	return {fopen(fname, mode), fclose_wrapper};
}

int main(void)
{
	const char* test_content = "hello from Phillip!\n";
	const char* filename = "c_sbrm.1.testfile";
	auto ufile = fopen_unique(filename, "w");

	// Check if we opened the file.
	// If we failed to open, ufile will be NULL/nullptr
	if(ufile)
	{
		printf("Opened file %s. File handle pointer: %p\n", filename, ufile.get());
	}
	else
	{
		printf("Failed to open file %s\n", filename);
		return -1;
	}

	// Our API usage is the same - just add .get() to the ufile handle
	fwrite(test_content, 1, strlen(test_content), ufile.get());

	filename = "c_sbrm.2.testfile";
	auto sfile = fopen_shared(filename, "w");

	// Check if we opened the file.
	// If we failed to open, sfile will be NULL/nullptr
	if(sfile)
	{
		printf("Opened file %s. File handle pointer: %p\n", filename, sfile.get());
	}
	else
	{
		printf("Failed to open file %s\n", filename);
		return -1;
	}

	// Our API usage is the same - just add .get() to the sfile handle
	fwrite(test_content, 1, strlen(test_content), sfile.get());

	// Make a copy of the shared_ptr
	// The file will be closed when reference count reaches 0
	auto sfile2 = sfile;

	filename = "c_sbrm.3.testfile";
	ufile_t ufile2 = acquire_unique_resource<FILE>(fopen, fclose_wrapper, filename, "w");

	// Check if we opened the file.
	// If we failed to open, sfile will be NULL/nullptr
	if(ufile2)
	{
		printf("Opened file %s. File handle pointer: %p\n", filename, ufile2.get());
	}
	else
	{
		printf("Failed to open file %s\n", filename);
		return -1;
	}

	filename = "c_sbrm.4.testfile";
	sfile_t sfile3 = acquire_shared_resource<FILE>(fopen, fclose_wrapper, filename, "w");

	// Check if we opened the file.
	// If we failed to open, sfile will be NULL/nullptr
	if(sfile3)
	{
		printf("Opened file %s. File handle pointer: %p\n", filename, sfile3.get());
	}
	else
	{
		printf("Failed to open file %s\n", filename);
		return -1;
	}

	return 0;
}
