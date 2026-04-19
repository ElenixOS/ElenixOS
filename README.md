<div style="display: flex; align-items: center;">
  <img src="./assets/logo-light.png#gh-light-mode-only" />
  <img src="./assets/logo-dark.png#gh-dark-mode-only" />
</div>

[![STATUS](https://img.shields.io/badge/Status-DEVELOPING-red)](#) [![License](https://img.shields.io/badge/Licence-Apache%202.0-brightgreen.svg)](LICENSE) [![GUI](https://img.shields.io/badge/GUI-LVGL-blue)](https://lvgl.io) [![Script Engine](https://img.shields.io/badge/Script_Engine-JerryScript-orange)](https://jerryscript.net) ![release](https://img.shields.io/github/v/tag/Sab1e-dev/ElenaOS)

<div>
  <a href="./README.md">English</a> | <a href="./README.zh-CN.md">中文</a>
</div>

---

## Overview

ElenaOS is an open-source smartwatch operating system. Its graphical interface is built on LVGL, while watch faces and applications run on a unified script engine powered by JerryScript.

The system adopts a script-driven app model: JavaScript describes UI structure and interaction logic, and native code handles rendering, event scheduling, and hardware resource management. This design enables a flexible and efficient development experience on resource-constrained embedded platforms.

ElenaOS is designed with portability and extensibility in mind. It provides clear abstraction layers and unified interfaces, making it easier to port across different MCUs and hardware platforms. Watch faces and applications share the same runtime environment and API system, reducing system complexity and improving overall consistency.

From a user experience perspective, ElenaOS draws inspiration from Apple Watch interaction patterns, with a focus on smooth animations, gesture operations, and interface hierarchy. By leveraging LVGL's animation system and event mechanism, ElenaOS aims to deliver a near-native smartwatch visual and interaction experience on embedded devices.

ElenaOS is suitable for personal projects and learning purposes, and also serves as a customizable foundational framework for wearable systems, offering developers a complete solution from low-level components to application-layer development.

## Demo (Work in Progress)

<img width="400" alt="demo" src="https://github.com/user-attachments/assets/c97ce713-a5a6-4146-a66a-a47eee03a787" />

<img width="400" alt="demo1" src="https://github.com/user-attachments/assets/b88671d4-87e4-4b55-bd67-223c431f4271" />

## Quick Start

See [Quick Start](./docs/quick_start.md).

## Documentation

1. [Build and Usage](./docs/build.md)
1. [Watch Faces and Applications (Script Engine)](./docs/script_engine.md)
1. [ElenaOS Developer Tools](./docs/dev_tools.md)

## License

This project is released under the Apache License 2.0. You may freely use, modify, and redistribute this software and derivative works, including for commercial use, provided that original copyright notices, license text, and related notices are retained. The software is provided "as is", without warranties of any kind.

## Third-Party Software

ElenaOS uses the following open-source projects during development. We sincerely thank their authors and communities:

- LVGL: https://lvgl.io
- JerryScript: https://jerryscript.net
- RemixIcon: https://remixicon.com
- cJSON: https://github.com/DaveGamble/cJSON
- uthash: https://troydhanson.github.io/uthash
