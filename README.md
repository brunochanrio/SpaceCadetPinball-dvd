# 3D Pinball - Space Cadet for 3DS

This is a port of 3D Pinball - Space Cadet for Nintendo 3DS. It's originally a game that came bundled with Windows from Windows 95 up to Windows XP. This is the current state of the project:

- No menus, options, or results screen.
- There are still some bugs here and there.
- It runs slow.

It is based on the PC decompilation made by [k4zmu2a](https://github.com/k4zmu2a): https://github.com/k4zmu2a/SpaceCadetPinball

The PC decompilation uses SDL2 to render the game. This 3DS port has been changed to use native GPU rendering with the Citro3D library.

## How to build

The main requirement is to have [devkitPro](https://devkitpro.org).

Follow the instructions to install devkitPro here: https://devkitpro.org/wiki/Getting_Started
You will also need the 3DS development package, and also the libraries 3ds-sdl and 3ds-sdl_mixer.

If you use Windows or Ubuntu, here are more detailed instructions.

### Windows

Even though devkitPro offers a Windows installer, I've had some issues setting it up. It's easier to use WSL. If you want to use the Windows installer anyway, check the link above for instructions.

1. Install [WSL](https://docs.microsoft.com/en-us/windows/wsl/install). By default it will install Ubuntu, which is fine.
2. Open a WSL terminal and just follow the Ubuntu instructions below. With the difference that, if you want to clone the project into, for example, the `C:\` folder, you will need move to that folder inside the terminal with the command `cd /mnt/c/`.

### Ubuntu and other Debian based linux distros

1. Open the terminal in the folder where you want to clone the project.
2. Clone it with the command `git clone --branch 3ds https://github.com/MaikelChan/SpaceCadetPinball`. A subfolder called `SpaceCadetPinball` will be created containing the project.
3. Move to that subfolder with `cd SpaceCadetPinball`.
4. Download the latest version of the [custom devkitPro pacman](https://github.com/devkitPro/pacman/releases/tag/v1.0.2), that will be used to download the compilers and libraries to build the project. Once downloaded, put it in the `SpaceCadetPinball` folder.
5. Install devkitPro pacman with this command: `sudo gdebi devkitpro-pacman.amd64.deb`. If gdebi is not found, install it with `sudo apt install gdebi-core`, and then try again installing pacman.
6. Use the following command to sync pacman databases: `sudo dkp-pacman -Sy`.
7. Now update packages with `sudo dkp-pacman -Syu`.
8. Install the 3DS development tools with `sudo dkp-pacman -S 3ds-dev`.
9. Install SDL with `sudo dkp-pacman -S 3ds-sdl`.
10. Install SDL_mixer with `sudo dkp-pacman -S 3ds-sdl_mixer`.
11. Set the DEVKITPRO environment variables so the system knows where the compilers and libraries are installed with these commands:
    - `export DEVKITPRO=/opt/devkitpro`.
    - `export DEVKITPPC=/opt/devkitpro/devkitARM`.
12. Build the project with the command `make -j4`.

After a successful build, you will get a file called `SpaceCadetPinball.3dsx`, which is the main executable.

## How to run

### Citra

1. Get the [Citra emulator](https://citra-emu.org/download/) if you don't have it.
2. Open it and go to the menu `File/Open Citra Folder`. This will open the folder where Citra's configuration is stored.
3. Go to the `sdmc` folder and create a new folder there named `SpaceCadetPinball`.
4. For legal reasons, you will need to get the original PC game on your own to obtain the assets like graphics and sound effects. Those are not contained in this repository.
5. Copy all PC game's assets to to the `SpaceCadetPinball` folder that was created earlier.
6. Optionally, since this port doesn't play MIDI files, you'll need to convert the music to ogg format, and call the file `PINBALL.ogg`, and put it along the other assets in the `SpaceCadetPinball` folder.
7. If everything went fine, you should be able to run the game.

## How to play

```
A                    :  Launch the ball
L                    :  Move the left paddle
R                    :  Move the right paddle
DPAD Left, Right, Up :  Bump table
X                    :  Start a new game
Y                    :  Exit game
Start                :  Pause
```

## Screenshots

<p align="center">
  <img title="3D Pinball - Space Cadet for 3DS running on Citra Emulator" src="/screenshot00.png">
</p>