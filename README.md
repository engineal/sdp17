## Getting Up and Running





To run the code, you will need to download a copy of Microsoft Visual Studio 2015. When you install Visual Studio, select a "Customized Install", and install the [GitHub Extension for Visual Studio](https://visualstudio.github.com/). Alternatively, if you have already installed Visual Studio, you may download the GitHub extension here, and install it separately.

Once you have the extension installed, select the "Team Explorer Tab". Under the Github section, select Clone, and then select the Earbeamer repository.

Once the project has been successfully cloned, you may open up the .sln file located within the Earbeamer/Earbeamer directory. Under the Solution Explorer pane, you should see the various projects contained in the solution, such as Earbeamer, Tests, etc.

One of these projects, GTest, will be labeled as unavailable. I'm hoping to use Google's testing framework to develop some unit tests for our code. However, in order to use it, you will have to compile the code into a static library. Remove the GTest project from the solution, and follow the instructions on [this blog post](https://usingcpp.wordpress.com/2016/07/23/google-test-and-visual-studio/) to compile the library

## Installing Boost

Follow [the instructions on this page](http://www.boost.org/doc/libs/1_63_0/more/getting_started/windows.html#id13). However, when invoking ./b2, use the following commmand:

```
b2 toolset=msvc threading=multi --build-type=complete address-model=64 --with-regex --with-system --with-date_time
```
