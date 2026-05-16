# sjtu_contender 项目文档

## 项目概览

- **项目名称**: sjtu_contender
- **定位**: 交我办（上海交通大学综合服务平台）第二课堂自动报名抢票软件
- **技术栈**: C++20 / Win32 原生桌面应用 + WebView2 浏览器内核
- **构建系统**: MSBuild (Visual Studio 2026+), 平台工具集 v145
- **平台**: x64 / Win32, Windows 10+

---

## 目录结构

```
sjtu_contender/
├── src/
│   ├── main.cpp                    # 程序入口
│   ├── TicketEngine.h              # 抢票引擎头文件（核心类）
│   ├── TicketEngine.cpp            # 抢票引擎实现
│   ├── Basic/
│   │   ├── pch.h                   # 预编译头
│   │   ├── pch.cpp                 # 预编译头编译单元
│   │   ├── CheckFailure.h          # HRESULT 错误检查宏
│   │   └── CheckFailure.cpp        # 错误弹窗 + 快速失败
│   └── Window/
│       ├── base_window.cpp         # Win32 窗口 + WinMain + WndProc
│       ├── window_messages.h       # 自定义窗口消息
│       ├── resource.h              # 资源 ID 定义
│       ├── Resource.rc             # 资源脚本（字符串表、图标）
│       ├── sjtu_contender.ico      # 应用图标
│       └── sjtu_contender.png      # 应用图标 (PNG)
├── packages/                       # NuGet 离线包
│   ├── Microsoft.Web.WebView2 1.0.3967.48
│   └── Microsoft.Windows.ImplementationLibrary 1.0.260126.7
├── packages.config                 # NuGet 依赖声明
├── sjtu_contender.vcxproj          # MSBuild 项目文件
├── sjtu_contender.vcxproj.filters  # VS 文件筛选器
├── sjtu_contender.slnx             # VS 解决方案文件（新版格式）
├── .gitignore
└── README.md
```

---

## 代码架构

```
┌─────────────────────────────────┐
│  main.cpp                       │  ← 入口
├─────────────────────────────────┤
│  Window/base_window.cpp         │  ← UI 层：窗口创建、消息循环、WndProc
│  Window/window_messages.h       │     自定义消息 WM_WEBVIEWINITIALIZED
├─────────────────────────────────┤
│  TicketEngine.h / .cpp          │  ← 业务层（单例）
│                                 │     WebView2 生命周期管理
│                                 │     导航、JS 注入、日志回调
├─────────────────────────────────┤
│  Basic/pch.h                    │  ← 基础设施
│  Basic/CheckFailure.h / .cpp    │     预编译头、错误处理宏
└─────────────────────────────────┘
```

---

## 核心类: TicketEngine（单例）

### 成员变量

| 成员 | 类型 | 说明 |
|---|---|---|
| `m_webviewEnvironment` | `wil::com_ptr<ICoreWebView2Environment>` | WebView2 环境（工厂） |
| `m_controller` | `wil::com_ptr<ICoreWebView2Controller>` | WebView2 控制器（外壳，绑定 HWND） |
| `m_webview` | `wil::com_ptr<ICoreWebView2>` | WebView2 核心视图（JS 执行、导航） |
| `m_logCallback` | `std::function<void(wstring)>` | 日志回调函数 |

### 方法列表

| 方法 | 功能 | 状态 |
|---|---|---|
| `instance()` | 获取单例 | 已完成 |
| `InitializeBrowser(HWND)` | 异步创建 WebView2 环境 → 控制器 → 视图 | 已完成 |
| `OnResize(RECT)` | 窗口大小变化时缩放浏览器 | 已完成 |
| `NavigateTo(wstring)` | 导航到指定 URL | 已完成 |
| `StartSnapping(time, interval)` | 注入硬编码点击脚本 | 占位实现 |
| `StartSnappingFromFile(path)` | 从外部文件读取 JS 并注入 | 未完成 |
| `StopSnapping()` | 注入刹车脚本 | 已完成 |
| `SetLogCallback(cb)` | 注册日志回调 | 已完成 |

### StartSnapping 当前占位代码

```js
document.querySelector('.buy-btn').click();
```

参数 `targetTime` 和 `retryIntervalMs` 已定义但未使用。

### StartSnappingFromFile 状态

已实现文件读取（`std::wifstream`），但 `ExecuteScript` 调用被注释掉，JS 代码读取后未注入。

### StopSnapping 刹车逻辑

```js
window.isSnapping = false;
if (window.snappingTimer) clearInterval(window.snappingTimer);
```

---

## WebView2 初始化流程

异步回调链，三层嵌套：

```
CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, options, callback)
  └─ 回调: 保存 m_webviewEnvironment
       └─ env->CreateCoreWebView2Controller(HWND, callback)
            └─ 回调: 保存 m_controller, 获取 m_webview
                 ├─ 配置 Settings（启用 JS / 对话框 / WebMessage）
                 ├─ 调整 Bounds 填满窗口
                 ├─ 注册 NavigationCompleted → 输出日志
                 ├─ 注册 WebMessageReceived → 回显消息
                 └─ PostMessage(HWND, WM_WEBVIEWINITIALIZED, 0, 0)
```

WebView2 层级关系：
- **Environment**（环境/工厂） → **Controller**（控制器/外壳） → **CoreWebView2**（核心视图/内核）

---

## 窗口行为

1. 创建 800×600 窗口，类名/标题来自字符串表（当前存在中文编码问题）
2. 加载图标 `IDI_ICON1`
3. 收到 `WM_WEBVIEWINITIALIZED` 后自动导航到 `https://my.sjtu.edu.cn/ui/task?by=history&from=kkframenew`
4. `WM_SIZE` 时自动调整 WebView 填满窗口
5. 日志通过 `OutputDebugString` 输出到调试器

### 自定义窗口消息

| 宏 | 值 | 用途 |
|---|---|---|
| `WM_WEBVIEWINITIALIZED` | `WM_USER + 1` | WebView2 初始化完成通知 |

---

## 错误处理

### CHECK_FAILURE 宏链

```cpp
CHECK_FAILURE(hr)          // → 包装 __FILE__ / __LINE__
CHECK_FAILURE_BOOL(value)   // → 转 BOOL 为 HRESULT
FAIL_WITH_MSG(msg)          // → 自定义消息
```

行为：失败时弹窗显示文件名、行号、错误码，然后调用 `FAIL_FAST()` 立即终止。

实现要点：使用 lambda 包装，避免多行函数调用被宏展开导致调试信息不准。

---

## NuGet 依赖

| 包 | 版本 | 用途 |
|---|---|---|
| `Microsoft.Web.WebView2` | 1.0.3967.48 | 嵌入式 Chromium 浏览器内核 |
| `Microsoft.Windows.ImplementationLibrary` (WIL) | 1.0.260126.7 | RAII COM 指针、资源管理 |

---

## 构建配置要点

- **C++ 标准**: C++20 (`stdcpp20`)
- **字符集**: Unicode
- **子系统**: Windows（非控制台）
- **预编译头**: `Basic/pch.h`，通过 `/FI` 强制包含或手动 `#include`
- **附加包含目录**: `$(ProjectDir)src`
- **编码**: `/utf-8`（仅 Debug|x64 配置）
- **预编译头文件**: `Basic\pch.h`（仅 x64 配置）
- **段堆**: 已启用 (`EnableSegmentHeap`)

### pch.h 包含内容

| 头文件 | 用途 |
|---|---|
| `Windows.h`, `windowsx.h` | Win32 API |
| `Unknwn.h`, `winrt/base.h` | COM 基础 |
| `wil/com.h`, `wil/resource.h` | WIL RAII 封装 |
| `wrl.h` | WRL COM 指针 + Callback |
| `WebView2.h` | WebView2 API |
| `<string>`, `<functional>` | STL |

---

## 待完成事项

1. **抢票 JS 脚本**: `StartSnapping` 中硬编码的 `.buy-btn` 点击需要替换为真正的抢票逻辑（倒计时、高频轮询、表单提交）
2. **`StartSnappingFromFile`**: 文件读取后 `ExecuteScript` 调用被注释，需要补全
3. **参数利用**: `targetTime` 和 `retryIntervalMs` 需要在 JS 脚本中实际使用
4. **WebMessage 处理**: `WebMessageReceived` 仅回显消息，`processMessage` 被注释
5. **资源编码**: 字符串表中文字符显示乱码，需修复
6. **`main.cpp`**: 当前仅 `#include "Basic/pch.h"`，实际入口逻辑在 `base_window.cpp` 的 `WinMain` 中，需确认链接是否正确
7. **Git 历史**: 存在多个无意义早期提交，建议在合适时机 squash

---

## 关键文件速查

| 文件 | 用途 |
|---|---|
| `src/main.cpp` | 程序入口 |
| `src/Window/base_window.cpp` | WinMain + WndProc |
| `src/TicketEngine.h` | 引擎接口定义 |
| `src/TicketEngine.cpp` | 引擎核心实现 |
| `src/Basic/pch.h` | 预编译头 |
| `src/Basic/CheckFailure.h` | 错误处理宏 |
| `src/Window/resource.h` | 资源 ID |
