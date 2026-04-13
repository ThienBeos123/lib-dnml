# Dynamol

> [!CAUTION]
> We do **not** have a website, The only official place of this project is in this repository

<div align="center">


[![libdnml logo]](images/random.png)

[![github actions ci badge]][githu actions ci]
[![][latest-release-shield]][latest-release-link]
[![][downloads-shield]][downloads-link]
<br>
An open-source, high-performance, multi-purpose, arbitrary-precision library in C
</div>

<!----- -------------------------------------------------------------------------------- --->
## Table of Contents
- [Key Features](#key-features)
- [Platform Compatibility](#platform-compatibiltiy)
- [Architecture Goals](#design--architecture-goals)
- [Example Usage](#example-usage)
- [Build & Installation](#build--installation)
- [Documentation](#documentation)
- [Project Status](#project-status)

<!----- -------------------------------------------------------------------------------- --->
## Key Features
- **Arbitrary-Precision**: Integers (bigInt) & Floating-point numbers (bigFloat - W.I.P) 
- **Cryptographically secure**: Constant-time operations against timed-based attacks (W.I.P)
- **Cross-platform Compability**: Supported on nearly all 64 bit platform, (see more in [Platform Compatibily](#platform-compatibiltiy))
- **Integration Friendly**: Seamlessly integrate with other projects with a user-friendly API, while maintaining performance and control

<!----- -------------------------------------------------------------------------------- --->
## Architecture Goals
- **Predictable** memory behavior
- Explicit control over **performance-critical** operations
- **Explicit memory-usage** for temporary objects (Crypto and Embedded)

<!----- -------------------------------------------------------------------------------- --->
## Platform Compatibiltiy
- **Supported Architectures**:
  - *x86_64  / x86 64 bit* 
  - *AARCH64 / ARM64*
  - *RV64 / RISC-V 64 bit*
- **Supported Platform/OS**:
  - *Windows* (MinGW, MSVC - x64, ARM64)
  - *MacOS* (x64, ARM64)
  - *Linux* (GNU, non-GNU - x64, ARM64, RV64)
  - *Android* and *iOS* (ARM64)
  - *BSD-based* (OpenBSD, FreeBSD - x64, ARM64, RV64)

<!----- -------------------------------------------------------------------------------- --->
## Example Usage
~~~c
#include "dynamol.h"
// OR #include <dynamol.h>

int main(void) {
    bigInt a, b, res;

    bigInt_init(&res);
    bigInt_init(&a, "1234989689842");
    bigInt_init(&b, "1923895849860");

    bigInt_add(&a, &b, &res);
    bigInt_print(&res);
    return 0;
}
~~~

<!----- -------------------------------------------------------------------------------- --->
## Build & Installation
After the acquisition of the source code, have a look at INSTALLATION.md for detailed instructions at building, compiling, and integrating libDNML into your projects. For more platform-specific instructions:

* [Instruction for UNIX-like Platforms](/notice/UNIX-BUILD.md)
* [Instruction for Windows](/notice/WIN64-BUILD.md)
* [Instruction for Androids](/notice/ANDROID-BUILD.md)
* [Instruction for iOS](/notice/IOS-BUILD.md)
* [Instruction for BSD-based platforms](/notice/BSD-BUILD.md)

<!----- -------------------------------------------------------------------------------- --->
## Documentation
If you are a developer who wants to either contribute to our project, or just wants better integration quality by reading ducomentation, here are some valuable documentation for specific types:

* [BigNumbers Utilization Details](/adynamol/BIGNUM_CONTRACT.md)
* [FixedNumbers Utilization Details](/adynamol/FSNUM_CONTRACT.md)
* Future documentation incoming..

<!----- -------------------------------------------------------------------------------- --->
## Project Status
~~~md
## Status
- Version: v0.2.1 (pre-stable)
- API is subject to change

- Mathematic Module is maturing                 [#####     ] 50%
- Testing & benchmarking in progress            [###       ] 30%
- Cryptography features in progress             [          ] 0%
- Embedded-development features in progress     [          ] 0%
- Generic I/O in progress                       [          ] 0%
~~~

<!----- -------------------------------------------------------------------------------- --->
## License
libDNML is licensed under the Apache License 2.0, enabling for free acquisition and utilization for both commercial and non-commercial purposes as long you obey the project's requirements

<!----- -------------------------------------------------------------------------------- --->
## Contributing
Please have a look at [CONTRIBUTING.md](CONTRIBUTING.md) if you're interest in developing, maintaining, or overall just contributing in our project.
