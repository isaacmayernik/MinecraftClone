# minecraft_clone

Move the "minecraft_clone" directory to your desktop for things to function.

Have CMake installed, open it up, set the 
"Where is the source code:" to the Desktop/minecraft_clone
and the "Where to build the binaries:" to Desktop/minecraft_clone/build

Click Configure then Generate in the bottom-left

Navigate to "Desktop/minecraft_clone/build" and open the minecraft_clone.sln
In the Solution Explorer on the left, right-click "minecraft_clone" and click "Set as Startup Project"
Right click "minecraft_clone" again, go to Properties, go to Debugging, and change Working Directory to $(OutDir), then click Apply then Ok.
