# BlamBSP_GX

Halo map viewer for the GameCube. It loads `bloodgulch.map` from the disc
(`MAP_NAME` in `main.cpp`), whichever map that file actually is.

Build it with:

```sh
./cb build console:powerpc-eabi:cube/BlamBSP_GX
```

## 1. Prepare the map

Run the map through `MapTranscode`, which every `./cb build` produces as a host
tool. Xbox maps are recommended: they load faster and need no `bitmaps.map`.

```sh
T=multi_build/host-x64-linux-native/bin/MapTranscode

# Xbox (compressed or already inflated)
$T /mnt/blam/xbox/b30.map --target Gekko --gx-vertices --jobs 16 \
    --output-map out/bloodgulch.map

# PC: the transcoded bitmaps.map goes on the disc with the map
$T /mnt/blam/pc/b30.map /mnt/blam/pc/bitmaps.map \
    --target Gekko --gx-vertices --jobs 16 \
    --output-map out/bloodgulch.map --output-bitmaps out/bitmaps.map
```

- The output files must not exist yet.
- `--gx-vertices` makes loading faster; a map written with it can only be read
  by BlamBSP_GX.
- Use one PC map per disc: its `bitmaps.map` only has that map's images
  converted.

## 2. Put it on a disc

The build leaves a disc folder at
`multi_build/console-powerpc-eabi-cube/packaged/gamecube/BlamBSP_GX/`. Copy the
map (and `bitmaps.map` for PC) into its `files/`. Every build empties `files/`,
so copy after building.

```sh
P=multi_build/console-powerpc-eabi-cube/packaged/gamecube/BlamBSP_GX
cp out/bloodgulch.map $P/files/
```

Run it in Dolphin. MMU emulation is required:

```sh
DVD_DIR=$P/sys/main.dol .github/tests/dolphin/dolphin.sh \
    multi_build/console-powerpc-eabi-cube/bin/BlamBSP_GX.elf \
    -C Dolphin.Core.MMU=True
```

For a disc image to burn or boot directly, rebuild the `.gcm` with the maps
included:

```sh
toolchain/desktop/make_gcm.py \
    --dol $P/sys/main.dol --files $P/files \
    --apploader $P/sys/apploader.img \
    --out BlamBSP_GX.gcm
```

## 3. Join a server

The GameCube joins BlamGraphics servers through `BlamGCGateway`, which runs on
a PC and needs a broadband adapter on the console. The server must run the
same map, from the same version (PC or Xbox), before transcoding.

Start a server and the gateway (both built by the desktop target):

```sh
D=desktop:x86_64-buildroot-linux-gnu:multi
./cb build $D/BlamGraphics && ./cb build $D/BlamGCGateway

./cb run $D/BlamGraphics -- <assets> /mnt/blam/pc/bloodgulch.map \
    --listen 0.0.0.0:27105
./cb run $D/BlamGCGateway -- --port 27790 --server 127.0.0.1:27105
```

Add `gateway.txt` to `files/` next to the map. It names the gateway, as the
console reaches it, and optionally the server, as the gateway reaches it:

```
192.168.1.20:27790 127.0.0.1:27105
```

Without a server, the gateway's `--server` is used. Without `gateway.txt`, the
viewer stays offline.

The console gets its address over DHCP. In Dolphin, use the built-in adapter,
and point `gateway.txt` at the PC's LAN address rather than `127.0.0.1`:

```sh
DVD_DIR=$P/sys/main.dol .github/tests/dolphin/dolphin.sh \
    multi_build/console-powerpc-eabi-cube/bin/BlamBSP_GX.elf \
    -C Dolphin.Core.MMU=True -C Dolphin.Core.SerialPort1=12
```
