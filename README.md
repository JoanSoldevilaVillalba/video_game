Video Game Project with a focus on logic, socket programming (client/server) and performance

(this is the first draft, it is not well written)


This is a learning project, where the main focus is not making a really fun videogame that everyone can enjoy,
but rather a learning experience on being proficient in c and have a somewhat understanding on how games are made at a fundemental level.

I understand that most games are written in c++, due to being in that interface between abstraction and system level usage which is what it makes a great language for making videogame and other 
softwares that require performance wiht some abstraction. I have decided to use c, the main reason being i don't want abstraction, i want to understand what is happening all of the time even if it makes 
the process of developing a videogame more frustrating and hard.

The main focus of this game will be the game engine (client and server side game engine), which will have a combination of c and SDL, specificly SDL3.
For those who don't know, Simple DirectMediaLayer(SDL) is an open source library that gives you access and control over low-level perphierals/systems such as 
mouse, joystick, keyboard, graphics ... by using OpenGL, Vulkan or Metal. This community driven open source project is natively written in c and abstracts low-level systems 
for software developers who want to write high-performace games and multimedia applications.

You might be asking why use SDL when you can use openGL or Vulkan, which allows you to learn more about how the computer actually deals with graphics and how your code effectivly communicates with drivers.
The reason being, is because i am fairly knew to graphics programming and althought SDL does have some level of abstraction, you still neet do understand for example what api calls interact with the cpu or gpu
to have better performance. Aswell, we are not just learning graphics programming.

As i have said in the beginning, there is going to be some network programming using sockets.


Aside from learning c, socket programming, sdl and game-engine architecture, this project is going to be a great way to learn tools that make me not just more proficient in c, but just in general as a software engineer.
After completing this project i want to be able to use:

Performance

gdb (debugger)

gcc (compiler)

git (control-version software)

valgrind 

tmux

linux terminal/wsl(windows subsystem for linux) terminal

perf (linux tool that allows us to profile our program)


The main goal is to be able to use this tools in an efficient way to enhance my capabilities as a programmer and as a software engineer.


This project is also only possible thanks to the sdl3 tutorials of Lazyfoo, they offer greate programming tutorials specialy for those who want to write code right away.

