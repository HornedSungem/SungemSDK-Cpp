<div align="center">
    <a href="http://hornedsungem.org/">
        <img src="https://raw.githubusercontent.com/HornedSungem/SungemSDK/master/logo/logo.png" style="max-height:220px"/>
    </a>
    <p style="font-size:xx-large">Horned Sungem</p>
</div>

# SungemSDK-Cpp

[SungemSDK] | [Documentation] | [简体中文](README.zh_CN.md)

## Installation Guide

Clone this repository and all its submodules with:

```bash
git clone --recursive https://github.com/HornedSungem/SungemSDK-Cpp.git
```

For installation, you should go to the `installer` folder, and run the `install.sh` corresponding to your operating system.

```bash
# take the Ubuntu for an example
cd installer/Linux/Ubuntu
./install.sh
```

## Getting Started

This repository contains the high-level APIs and some examples for `C++` developers. 

### Build the `High-Level APIs`
    
```bash
cd libhs_highapi
mkdir build && cd build
cmake .. && make
```

The built target named `libhs_highapi` will written to the `libhs_highapi/lib` folder.

### Run the examples

Before running the example programs, you must download and copy required files from [SungemSDK-GraphModels][] to the corresponding folders under the `examples`.

```
SungemSDK-GraphModels/graphs -> examples/graphs
SungemSDK-GraphModels/misc   -> examples/misc
```

And then, you can go to the `apps` folder and run the example code.

```bash
# take the MobileNet-SSD for an example
cd examples/apps/MobileNet-SSD
mkdir build && cd build
cmake .. && make
./MobileNet-SSD
```

## Support

If you need any help, please post us an issue on [GitHub Issues][] or send us an email (support@hornedsungem.org).

You are welcome to contact us for your suggestions, and model request.  


[GitHub Issues]: https://github.com/HornedSungem/SungemSDK-Cpp/issues
[SungemSDK]: https://github.com/HornedSungem/SungemSDK
[Documentation]: https://hornedsungem.github.io/Docs
[SungemSDK-GraphModels]: https://github.com/HornedSungem/SungemSDK-GraphModels