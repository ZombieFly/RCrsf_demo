# RCrsf 例程

这是一个使用 RCrsf 库的示例程序，展示了如何使用 RCrsf 进行简单的功能实现。本例程采用达妙MC02开发板，MCU为 STM32H723VGT6。

支持VSCode EIDE插件进行开发与编译，也支持使用Makefile进行编译。

## 构建

### 获取仓库

#### clone（推荐）

- 克隆仓库

```bash
git clone https://github.com/ZombieFly/RCrsf_demo.git
```
或
```bash
git clone git@github.com:ZombieFly/RCrsf_demo.git
```

- 拉取子模块

```bash
cd RCrsf_demo
git submodule update --init --recursive
```

#### 下载源码（备选）

由于 GitHub 不支持直接下载包含子模块的仓库，需要先下载[本仓库代码](https://github.com/ZombieFly/RCrsf_demo/archive/main.zip)，解压到本地，随后下载[RCrsf代码](https://github.com/ZombieFly/RCrsf/archive/main.zip)并解压到本地的 `lib/` 目录下。

此时，你的目录结构应该如下：

```
RCrsf_demo
├── Libs
│   ├── rcrsf
│   │     ├──rcrsf.c
│   │     ├──rcrsf.h
│   │     └──...
│   └── ...
└── ...
```

### 工具链

- arm-none-eabi-gcc

可前往[GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)下载。

> [!NOTE]
> - 在Windows上，如果本机安装过CubeMX，在版本较新的CubeMX中，通常自带了arm-none-eabi-gcc，位置位于`%USERPROFILE%\AppData\Local\stm32cube\bundles\gnu-tools-for-stm32`，将其`bin`文件夹添加到系统环境变量中即可
> - 在Windows上，如果采用VSCode EIDE插件进行开发，可以通过`实用工具`功能进行安装 arm-none-eabi-gcc

Linux通常自带make。针对Windows，通常安装minGW。

推荐使用[msys2](https://github.com/msys2/msys2-installer/releases)([阿里云镜像](https://mirrors.aliyun.com/msys2/distrib/msys2-x86_64-latest.exe))进行辅助安装，安装完成后使用管理员权限运行“msys2 msys”(msys2.exe)，在打开的终端中执行以下命令进行minGW安装：
```bash
pacman -S mingw-w64-x86_64-make
```
完成安装后，在msys2安装目录下的`mingw64\bin`目录中找到`mingw32-make.exe`，将其重命名为`make.exe`，并将该目录添加到系统环境变量中。

> [!NOTE]
> - 如果采用VSCode EIDE进行开发，该插件集成Make，仅需要配置arm-none-eabi-gcc路径即可，无需额外配置Make。

### 编译

#### 直接编译
在项目根目录下，执行以下命令:

```bash
make -f Makefile.user -j6
```

编译产物位于`build/`目录下，包含`rcrsf.elf`、`rcrsf.hex`等文件。可以使用JLink-flash、CubeProgrammer等工具将`rcrsf.hex`烧录到开发板上。

#### 使用VSCode EIDE插件编译

<div align="center">
    <a href="https://code.visualstudio.com/Download">
        <img src="https://raw.githubusercontent.com/Aikoyori/ProgrammingVTuberLogos/refs/heads/main/VSCode/VSCode-Thick.png" alt="请支持vscode喵，请支持vscode谢谢喵" width="50%"/>
    </a>
</div>

- 在VSCode中安装[Embedded IDE](https://marketplace.visualstudio.com/items?itemName=CL.eide)插件。
- 在EIDE的工具链设置中，添加`GNU Arm Embedded Toolchain (arm-none-eabi-gcc)`工具链，配置路径为arm-none-eabi-gcc所在目录。
- 在EIDE的项目管理中，配置合适的烧录配置，随后进行编译和烧录。

### 验证
- 将程序烧录到开发板后，正常运行态为绿灯闪烁
- 使用USB连接单片机与操作系统，启动/重启单片机，黄灯闪烁，进入储存模式，在操作系统中识别出一个新的USB 大容量存储设备（初次烧录后建议进行格式化）
- `UART7`连接到Elrs接收机，启用CRSF协议（默认启用），并完成配对，确保接收机提示灯常亮后，遥控器端进入Elrs lua脚本，在`Other Devices`中可见`Hello RCrsf!`设备（未见可刷新）