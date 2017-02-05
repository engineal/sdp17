## Getting Up and Running

To run the code, you will need to download a copy of Microsoft Visual Studio 2015. When you install Visual Studio, select a "Customized Install", and install the (GitHub Extension for Visual Studio)[https://visualstudio.github.com/]. Alternatively, if you have already installed Visual Studio, you may download the GitHub extension here, and install it separately.

Once you have the extension installed, select the "Team Explorer Tab". Under the Github section, select Clone, and then select the Earbeamer repository.

Once the project has been successfully cloned, you may open up the .sln file located within the Earbeamer/Earbeamer directory. Under the Solution Explorer pane, you should see the various projects contained in the solution, such as Earbeamer, Tests, SoftwareFiltering, etc.

One of these projects, GTest, will be labeled as unavailable. I'm hoping to use Google's testing framework to develop some unit tests for our code. However, in order to use it, you will have to compile the code into a static library. Remove the GTest project from the solution, and follow the instructions on (this blog post)[https://usingcpp.wordpress.com/2016/07/23/google-test-and-visual-studio/] to compile the library 
