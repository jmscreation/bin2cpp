# Bin 2 CPP
 Convert binary files into *.cpp files instantly.

# Build
It's as simple as using your favorite compiler, or simply running the build script if you already have MinGW64-gcc configured.

`g++ -std=c++20 -O3 -static bin2cpp.cpp -o bin2cpp`

# Resources In Code
When importing the resources, you can access the file data by simply including the header file generated which gives the externed declaration for your resource data. Your resource is now accessible as a `const char[]` array.
```cpp
#include "my_resource.h"

void LoopResourceData() {
    for(
        const char* pos = my_resource, // decay pointer for start position
                  * end = pos + sizeof(my_resource); // get a pointer to the end of the resource data
        pos < end; ++pos // iterate from start position to end position - 1
    ){
        const char& c = *pos; // get reference to character in resource data
        /*
            Do something here
        */
    }
}
```

Obviously this is just an example, but the idea here is you ultimately get a `const char*` pointer and a `sizeof(resource)` length; with this information you can use this to gain access to any embedded resource data, and use to your hearts content.

# Generate Script
For Windows there is an included `generate.bat` batch script which will iterate over the current directory, and run the bin2cpp application for a group of files with matching file extentions. This is ideal if you have a project that has many files that need to be directly embedded in your application. Once configured with your designated list of file extentions, this script will search the current working directory for all matching files, use `bin2cpp` to convert the files into `*.cpp` files, and generate a finalized header file. This lets you include just one header file which automatically includes all header files for all the resource files embedded.
### Project
This is not a complicated program; however, as some have inquired about it I have decided to publish it here on github. There are many simple applications already written that do similar things. This is simply another one of those programs.