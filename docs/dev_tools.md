# Dev tools

ElenaOS 在开发过程中使用了很多脚本工具用于辅助开发，下面将介绍如何使用这些工具。

## 使用方法

请自行创建Python3的虚拟环境，并安装依赖包：

```bash
pip install -r requirements.txt
```

依赖包安装完成后，即可使用脚本工具。

## 应用打包工具

应用打包工具（eos_pkg_builder.py）用于将应用打包成 .eapk 或 .ewpk 文件，该文件包含应用所需的所有资源，如图标、脚本、资源文件等。打包时不会压缩文件，因此打包后的文件大小可能会比原始文件大。

### 使用方法

```bash
python eos_pkg_builder.py <input_path> <output file> --type app
```

**参数说明**
`<input_path>`为应用目录，`<output file>`为输出文件路径。
`--type app`表示打包的是应用，可选项有`app`、`watchface`。

## 绑定层生成工具

绑定层生成工具（gen_lvgl_bindings.py）用于生成绑定层代码，该工具会根据指定的目录生成绑定层代码。

>**注意**
>生成的绑定层代码不能保证全部可用，因此一般用户不需要使用该工具，直接使用官方提供的绑定层代码即可。

### 使用方法

```bash
python gen_lvgl_bindings.py <参数>
```
**参数说明**

- --json-file=<lvgl.json文件路径>
- --output-c-path=<输出C文件路径>
- --extract-funcs-from=<提取函数的目录>
- --cfg-path=<配置文件目录的路径>
    - 配置文件目录内包含：
      - export_functions.txt
      - export_macros.txt
      - blacklist_functions.txt
      - blacklist_macros.txt
