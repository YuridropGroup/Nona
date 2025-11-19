# Introducing Nona Version 4.0.0
![Redis++](https://img.shields.io/badge/redis--plus--plus-v1.3.11-blue?logo=redis&logoColor=white&style=for-the-badge)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)



![Nona from Death Parade](https://images.steamusercontent.com/ugc/424818087612270798/328B81716D2AC2CA91B15BC4D38B57C9C6E08243/?imw=5000&imh=5000&ima=fit&impolicy=Letterbox&imcolor=%23000000&letterbox=false)

#### >> Created by CodedCodesCode on Github <<

# What is Nona?

Nona is a Discord nuke bot created in C++ using technologies such as Redis to allow for more efficient memory-based data structures, faster workers and efficient task queues.

# What is the purpose of Nona?

The purpose of Nona is simple: to be the best Discord nuke bot to exist. This is meant in terms of efficiency in resources, and speed thus not allowing a server administrator or moderator to be able to process anything that is happening. In future updates it will also support other features such as automatic doxing, DLL hooking (for our Windows users) and per-server malware.

# How to install Nona?

## 🪟 Windows Installation

### **1. Download the prerequisits**

Download the GCC/G++ compiler and Git if you wish to use it for the vcpkg install.

**[Download the GCC compiler via MSYS2 here.](https://www.msys2.org/#installation)**

**[Download Git here](https://git-scm.com/downloads/win)**

Redis is not officially supported by Windows anymore, so you must look at one of three alternatives.

**1: WSL ( recommended )**

If you go via the WSL ( Windows Subsystem For Linux ) route, then you can simply follow the instructions for your Linux distribution in the set-up for Linux below.

**2: Docker**

You can instead use Docker and have it pull the Redis image when you run the following command.

```
docker run -d --name redis -p 6379:6379 redis
```

You will have to run this each time you want to use Nona though since the run.bat does not include starting Redis due to the number of different methods.

You can install Docker at the following link:

**[Download Docker here](https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe?utm_source=docker&utm_medium=webreferral&utm_campaign=dd-smartbutton&utm_location=module)**

**3. Memurai**

Memurai is a Windows Native port of Redis, follow the instructions to install and run it. You can download it at the below link:

**[Memurai](https://www.memurai.com/)**

### **2. Download and install vcpkg**

Download vcpkg from Github either using Git or manually downloading and extracting. Install it to a directory such as `C:\vcpkg`

**The following Git command can be used:**

```
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
```

or alternatively visit the URL and then press the big green download button and download it as a .zip file and extract it.

After the repository is downloaded, please run the following commands:

```
cd C:\vcpkg
.\bootstrap-vcpkg.bat
```

This will install vcpkg for you to use and download packages that are required for Nona to function.

### **3. Add vcpkg to PATH (optional but recomended)**

Firstly, open the search box on your Windows task bar, type in "System Properties" and press enter.

After that naviate into "Advanced" and then "Environment Variables".

Edit the `Path` variable and type in the path to your vcpkg directory, in this case you would type in `C:\vcpkg`. After this you can safely close out of System Properties.

### **4. Install the dependencies for Nona via vcpkg**

If you have added vcpkg to PATH, then you can simply run the following command, otherwise you will have to navigate into the directory where your vcpkg is installed and run it there locally.

```
vcpkg install dpp redis-plus-plus nlohmann-json openssl
```

In `CMakeLists.txt`, you will want to change the setting of CMAKE_TOOLCHAIN_FILE to where yours is. In this case it will most likely be at C:\vcpkg\scripts\buildsystems\vcpkg.cmake

### **5. Build and run Nona**

Assuming that every other step has completed successfully, it should now be fine to build and run Nona.

Firstly, we recommend giving variables to Nona in the `config/stickers` and `config/emojis` directory for Nona to provide image for sticker and emoji related commands.

It is also wise to provide variables for Nona in `config/variables.json` for things such as channel and role creation.

After this you will want to build Nona. You can do this by running the `compile.bat` file. This may require administator permissions so press allow if prompted to do so.

You can do this simply by typing `compile.bat` into your Command Prompt.

After that, to run Nona simply type `Nona.exe` in the local directory to run Nona, it will prompt you to enter the guild ID and then after that you can enter commands as you want.

## 🐧 Linux Installation ( Ubuntu And Others)

### **1. Download the prerequisits ( Ubuntu )**

Firstly, update your system to make sure everything will run fine and up-to-date.

Git and a C++ compiler should already be configured in Linux by default so you do not need to bother installing these.

You can use the following command:

```
sudo apt update
sudo apt install build-essential git cmake pkg-config
sudo apt install redis-server 
```

### **1. Download the prerequisits ( Arch )**

The same applies as above, but the commands will be slightly different. Please use the following commands instead:

```
sudo pacman -Syu
sudo pacman -S base-devel
sudo pacman -S redis
```

### **2. Download and install vcpkg**

You can use the following command to install vcpkg using Git:

```
sudo git clone https://github.com/microsoft/vcpkg.git /usr/local/vcpkg
```

This will install it system-wide, though this is not recommended for everyone (or those who share a Linux machine with other users). If you do not wish to install it system wide and just be able to use vcpkg at a defined location, install it at the root instead by replacing `/usr/local/vcpkg` with `~/home`.

After this, change directory to where your installation directory is and bootstrap vcpkg to install it, in our case run the following commands:

```
cd /usr/local/vcpkg
sudo ./bootstrap-vcpkg.sh
```

### **3. Install the dependencies for Nona via vcpkg**

If you have added vcpkg globally then you can run the following command as-is, else if you installed it to a local directory add a `./` infront of the vcpkg.

```
sudo vcpkg install dpp redis-plus-plus nlohmann-json openssl
```

In `CMakeLists.txt`, you will want to change the setting of CMAKE_TOOLCHAIN_FILE to where yours is. In this case it will most likely be at /home/USER/vcpkg/scripts/buildsystems/vcpkg.cmake

### **4. Build and run Nona**

Assuming that every other step has completed successfully, it should now be fine to build and run Nona.

Firstly, we recommend giving variables to Nona in the `config/stickers` and `config/emojis` directory for Nona to provide image for sticker and emoji related commands.

It is also wise to provide variables for Nona in `config/variables.json` for things such as channel and role creation.

After this you will want to build Nona. You can do this by running the `compile.sh` file.

You can do this simply by typing `compile.sh` into your terminal.

After that, to run Nona simply type `run.sh` in your terminal. This will start the redis service, clear out any left-over objects in Redis' local database and give you a smooth experience. Doing this will run Nona in the CLI mode since our Linux release does not yet support a GUI.

# What are the commands?

To view the commands you can simply run the executable itself (not the run script) with the flag `-h` or `--help`. An alternative to this is running the command `help` after inputting the guild ID when prompted and waiting for it to finish precaching all objects and guild data. If you want to find help for a specific command you can type in the command name after any one of these methods. An example being `./Nona -h deletechannels` and it will display the command name, the command description and the command usage in your terminal.