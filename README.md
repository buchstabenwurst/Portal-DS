# Portal-DS
Portal 2 fan port for the Nintendo ds


![Unbenannt](https://github.com/user-attachments/assets/45e6f7b1-6e04-40ff-8ea4-28adcebc1d89)

## Building
Requires [BlocksDS](https://blocksds.skylyrac.net/docs/) to build.
Requires some [Portal 2](https://store.steampowered.com/app/620/Portal_2) files.

### Linux
clone with submodules `git clone --recurse-submodules https://github.com/buchstabenwurst/Portal-DS`

Copy `[Portal 2]/scripts/vscripts/` to `[PortalDS]/assets/scripts/vscripts/`

Then run
```shell
cd ./libs/squirrel
cmake --fresh -DCMAKE_TOOLCHAIN_FILE=/opt/blocksds/core/cmake/BlocksDS.cmake -DDISABLE_DYNAMIC=ON -DSQ_DISABLE_INTERPRETER=ON ./CMakeLists.txt 
make
cd ../../
make
```
## Installation
[Download nightly Build](https://nightly.link/buchstabenwurst/Portal-DS/workflows/c-cpp/master/Portal%20DS.zip)

There are currently no Releases

### Optional - for mods
Copy the `_nds` folder to your SD card
