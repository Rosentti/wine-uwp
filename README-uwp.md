# Wine-UWP
This is a wine fork that implements various UWP APIs, allowing some UWP-based games to run.

## Building
Run `./conf.sh` to configure everything for the first time.
Run `./build.sh` to rebuild every time you make changes to the code.

## Usage
Run `./pfx.sh` to (re)create the wineprefix. This will create a wineprefix at `<srcdir>/prefix`, which you can then use with `WINEPREFIX="$PWD/prefix" ./build/64/wine /path/to/UWP/app`
This wineprefix will have a special version of DXVK installed, which supports UWP apps. Do not replace it.

## Crashes?
File an issue!
Run the app with `WINEDEBUG=+all WINEPREFIX="$PWD/prefix" ./build/64/wine /path/to/UWP/app > log.txt 2>&1`, and compress it so it can be attached to the issue. 
Also include the app's name, and what dependencies it has.

## Acquiring apps
1. Go to the microsoft store, or Xbox store
2. Find an interesting PC game
3. Go to [Adguard Store](https://store.rg-adguard.net/)
4. Download the app and it's dependencies
    - x64 marked packages
    - If there's a universal package, download it. Inside will be x64 package. 
5. Extract the app to some folder
6. Extract and copy all the dependency dlls to the same folder. Do not replace contents if prompted.
7. Run the app.

## TODO
- Input (mouse and keyboard)
- Controller support
- Some other APIs

## Tested apps
- Microsoft Mahjong
  - Not working, would require XAML implementation
- Marble Maze demo app
  - Gets to main menu, however input is not currently supported.
- Minecraft for Windows 10
  - Mysterious page fault on read access to 0000000000000000
- Asphalt 8
  - `Windows.UI.StartScreen.JumpList` unimplemented
- Xbox App
  - Would require XAML implementation
