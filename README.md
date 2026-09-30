# TMHotkey · 自定义按键隐藏界面

给《上古卷轴五·天际》重制版（SSE / AE）用的极简插件：**按一个你自己设定的键，就等同控制台 TM**——一键隐藏全部界面，再按一次恢复。不用开控制台、不用施放技能。

## 特点

- 按键完全自定义，写在 INI 里随时改。
- 支持 Ctrl / Alt / Shift 组合键（如 Ctrl+F8）。
- 调用引擎原生菜单切换，效果与控制台 TM 完全一致。
- 只依赖 SKSE64，不需要 SkyUI、ConsoleUtil 等其他插件。

## 前置要求

- 天际特别版 / 周年庆版（Steam）
- SKSE64（版本与游戏匹配，官网 skse.silverlock.org）

## 安装

1. 用 Mod Organizer 2 / Vortex 安装编译好的 `TMHotkey-*.zip`，或手动把 `SKSE/` 合并进游戏 `Data/`。
2. 通过 SKSE64 启动游戏。
3. 默认按 **F8** 隐藏 / 恢复全部界面。

## 自定义按键

编辑 `Data/SKSE/Plugins/TMHotkey.ini`：

```ini
[Hotkey]
Key = 66              ; DIK 十进制扫描码（F8=66）
RequireCtrl = false
RequireAlt = false
RequireShift = false
```

INI 里附有常用键位扫描码表。改完重新读档或重启游戏生效。

## 编译

本仓库已配置 GitHub Actions 云端 Windows 自动编译，无需本地安装 Visual Studio：

1. 新建 GitHub Public 仓库，把本工程全部文件（含 `.github` 隐藏文件夹）上传到 `main` 分支。
2. 打开 Actions 标签，等待 “Build TMHotkey” 跑完（首次约 20–40 分钟）。
3. 成功后在运行记录最下方 Artifacts 下载 `TMHotkey-compiled`，解压即得可安装 zip。

本地编译（需要 Windows + VS2022 + CMake + vcpkg）：

```bat
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build build --config Release
```

## 说明

- 隐藏后连准星、控制台都会消失，恢复就再按一次设定键（按键在界面隐藏时依然有效）。
- 仅 PC（依赖 SKSE）；不影响成就。
- 天际等为 Bethesda Softworks 商标，本项目为非官方学习项目。
