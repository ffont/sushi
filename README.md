# AFTER-SUSHI

This repository contains a customized version of [ELK Audio's `sushi`](https://github.com/elk-audio/sushi) (v0.10.3) which adds support for channel pressure and polyphonic aftertouch MIDI messages (which are currently being filtered out because of a bug in the official release).  I tested it with ELK Pi and cross-compile it from a macOS which success. 

In this fork I include scripts to facilitate Docker-based cross-compilation from a mac (and generaly from any OS if using the Dockerized sdk). Here are the instructions to cross compile:

- **Prepare ELK development SDK**: The first thing to do is to prepare the ELK development SDK Docker image following the [instrucitons here](https://github.com/elk-audio/elkpi-sdk/blob/master/running_docker_container_on_macos.md). You need to run steps 1 to 3, no need to run the toolchain when everything installed.


- **Preparing some code dependencies**: In the compilation scripts `sushi` is configured to be built with VST2 supper, meaning that you need the VST2 SDK installed in your computer. The script used to cross-compile `sushi` will try to mount the VST2 SDK in the Docker container so it can be used for compilation. The script will search for a folder named `VST_SDK` (the SDK) at the same directory level where the `sushi` folder is (if needed, you can edit this in `fabfile.py`). You should find a copy of the VST2 SDK and place it in the corresponding directory.


- **Do the cross-compilation**: With all this in place, you should be able to cross-compile `sushi` by simply running `fab compile-elk`

*NOTE*: in the compilation scripts `sushi` is [configured](https://github.com/ffont/sushi/blob/31730e3a5478ca712527bbf384de501eb665ffdc/custom-esdk-launch.py#L123) with a number of CMAKE options turned off. Make sure that none of this is needed for yout build.

What follows is the original readme file for SUSHI:



# SUSHI
Headless plugin host for ELK Audio OS.

## Usage

See `sushi -h` for a complete list of options.
Common use cases are:

Test in offline mode with I/O from audio file:

    $ sushi -o -i input_file.wav -c config_file.json

Use JACK for realtime audio:

    $ sushi -j -c config_file.json

With JACK, sushi creates 8 virtual input and output ports that you can connect to other programs or system outputs.

## Configuration file examples

See directory `example_configs` for the JSON-schema definition and some example configurations.
Configuration files are used for global host configs, track and plugins configuration, MIDI routing and mapping, events sequencing.

## Building Sushi
Sushi uses CMake as its build system. A generate script is also provided for convenient setup. Simply running `./generate` with no arguments in the root of Sushi will setup a build folder containing a Release configuration and a Debug configuration. CMake arguments can be passed through the generate script using the `--cmake-args` flag. Those arguments will then be added to both configurations.

Make sure that Sushi is cloned with the `--recursive` flag to fetch all required submodules for building. Alternatively run `git submodule update --init --recursive` after cloning.

Sushi requires a compiler with support for C++17 features. The recommended compiler is GCC version 7.3 or higher.

### Building for native Linux
As all options are on by default, to build Sushi for a regular, non Elk Powered Linux distribution, the Xenomai options need to be turned off. In addition the Vst 2.x SDK needs to be provided.

    $ ./generate --cmake-args="-DWITH_XENOMAI=off -DVST2_SDK_PATH=/home/elk/sdks/vstsdk2.4" -b

If the Vst 2.4 SDK is not available, Sushi can still be built without Vst 2.x support using the command below:

    $ ./generate --cmake-args="-DWITH_XENOMAI=off -DWITH_VST2=off" -b

It is also possible to skip the `-b` flag and build by calling `make` directly in build/debug or build/release.

### Useful CMake build options
Various options can be passed to CMake directly, or through the generate script using the `--cmake-args` flag. Both general CMake options and Sushi-specific options that control which features Sushi is built with can be passed. Note that all options need to be prefixed with `-D` when passing them. All options are on be default as that is the most common use case.

Option                          | Value    | Default | Notes
--------------------------------|----------|---------|------------------------------------------------------------------------------------------------------
AUDIO_BUFFER_SIZE               | 8 - 512  | 64      | The buffer size used in the audio processing. Needs to be a power of 2 (8, 16, 32, 64, 128...).
WITH_XENOMAI                    | on / off | on      | Build Sushi with Xenomai RT-kernel support, only for ElkPowered hardware.
WITH_JACK                       | on / off | on      | Build Sushi with Jack Audio support, only for standard Linux distributions.
WITH_VST2                       | on / off | on      | Include support for loading Vst 2.x plugins in Sushi.
VST2_SDK_PATH                   | path     | empty   | Path to external Vst 2.4 SDK. Not included and required if WITH_VST2 is enabled.
WITH_VST3                       | on / off | on      | Include support for loading Vst 3.x plugins in Sushi.
WITH_LV2                        | on / off | on      | Include support for loading LV2 plugins in Sushi. 
WITH_LV2_MDA_TESTS              | on / off | on      | Include LV2 unit tests which depends on the LV2 drobilla port of the mda plugins being installed. 
WITH_RPC_INTERFACE              | on / off | on      | Build gRPC external control interface, requires gRPC development files.
WITH_TWINE                      | on / off | on      | Build and link with the included version of TWINE, tries to link with system wide TWINE if option is disabled.
WITH_UNIT_TESTS                 | on / off | on      | Build and run unit tests together with building Sushi.

### Dependecies
Sushi carries most dependencies as submodules and will build and link with them automatically. A couple of dependencies are not included however and must be provided or installed system-wide. See the list below:

  * libsndfile

  * alsa-lib

  * Jack2

  * Vst 2.4 SDK - Needs to be provided externally as it is not available from Steinberg anymore.

  * Raspa - Only required if building for an Elk Powered board and is included in the Elk cross compiling SDK.

  * gRPC - Needs to be built from source and installed system wide. See [https://github.com/grpc/grpc/blob/master/src/cpp/README.md] for instructions. The current version of gRPC used by sushi is 1.10.1. Other versions can not be guaranteed to work.

  * For LV2:

      * For Sushi:

          * liblilv-dev - at least version 0.24.2. Lilv is an official wrapper for LV2.
          * lilv-utils - at least version 0.24.5.
          * lv2-dev - at least version 1.16.2. The main LV2 library.

        The official Ubuntu repositories do not have these latest versions at the time of writing. The best source for them is instead the [KX Studio repositories, which you need to enable manually](https://kx.studio/Repositories).

      * For LV2 unit tests:

          * lv2-examples - at least version 1.16.2.
          * mda-lv2 - at least version 1.2.2 of [drobilla's port](http://drobilla.net/software/mda-lv2/) - not that from Mod Devices or others.

## License

Sushi is licensed under Affero General Public License (“AGPLv3”). See [LICENSE](LICENSE.md) document for the full details of the license. For contributing code to Sushi, see [CONTRIBUTING.md](CONTRIBUTING.md).



Copyright 2017-2020 Modern Ancient Instruments Networked AB, dba Elk, Stockholm, Sweden.
