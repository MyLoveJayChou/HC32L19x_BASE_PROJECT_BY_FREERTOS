# HC32 嵌入式软件架构规范

## 分层顺序

依赖方向必须严格单向向下：

`App -> Service -> BSP wrapper -> BSP port -> BSP handler -> BSP driver -> HAL -> 厂商 SDK`

## 分层规则

- `App` 层只能调用 `Service` 层接口。
- `App` 层禁止包含 `BSP` 层头文件。
- `Service` 层可以调用 `BSP wrapper` 和 `Common`。
- `BSP wrapper` 是 `BSP` 层对上层暴露的唯一稳定接口。
- `BSP port` 是对象装配层，负责实例化、绑定和注入。
- `BSP handler` 负责控制逻辑、注册管理和线程入口。
- `BSP driver` 负责设备对象模型，是 `BSP` 内部最底层。
- `HAL` 层只抽象 MCU 片上外设。
- `Common` 层必须保持平台无关。
- `Middlewares` 层只存放第三方库和必要的移植文件。

## BSP 对象模型

- `bsp_<device>_driver_t` 表示一个具体设备对象实例。
- `driver` 结构体内部保存对象属性和操作函数指针。
- `driver` 对外只暴露 `bsp_<device>_driver_inst()`。
- `driver` 层的每个函数都必须有返回值。
- `driver` 层函数的返回类型必须是本层专用枚举，例如
  `bsp_<device>_driver_status_t`。
- `handler` 负责管理和注册一个或多个 `driver` 对象。
- `handler` 可以暴露 `inst` 函数、控制接口和线程函数。
- `handler` 层的每个函数都必须有返回值。
- `handler` 层函数的返回类型必须是本层专用枚举，例如
  `bsp_<device>_handler_status_t`。
- `port` 负责把 `handler`、`driver`、MCU 操作和 OS 操作装配起来。
- `wrapper` 负责向 `Service` 提供稳定接口，不暴露内部实现细节。

## 状态类型规则

- 禁止在 `driver` 和 `handler` 的公开接口中使用裸 `int`、`int32_t`
  表示状态。
- `driver` 必须定义并返回 `bsp_<device>_driver_status_t`。
- `handler` 必须定义并返回 `bsp_<device>_handler_status_t`。
- `handler` 不应直接向上透传 `driver` 的状态枚举。
- `handler` 应把 `driver` 状态转换为本层语义明确的状态。
- 不同层的状态枚举不要混用，避免打穿层边界。

## 函数指针注入规则

- MCU 操作和 OS 操作必须通过 `inst()` 参数注入。
- `driver` 和 `handler` 不允许直接绑定具体板级代码。
- 能通过注入解决的问题，不要使用隐藏全局变量。
- 设备自身的基础动作放在 `driver`。
- 注册、管理、状态机、缓升缓降、重试等控制逻辑放在 `handler`。
- 平台绑定、引脚映射、OS 适配和对象装配放在 `port`。
- 面向上层的稳定业务接口放在 `wrapper`。

## 生成代码时的原则

1. 优先把代码放到能够承担该职责的最低层。
2. 每层只向上一层暴露必要接口。
3. `App` 不允许知道具体硬件、设备、引脚和通信总线。
4. `Service` 表达业务能力，不负责对象装配。
5. `port` 只做平台适配和对象装配，不写业务规则。
6. 优先使用结构体和函数指针表达对象边界，避免隐藏耦合。

## 禁止的反模式

- `App` 直接调用 `BSP`。
- `App` 包含 `bsp_xxx.h`。
- `handler` 直接操作 MCU 寄存器或厂商 SDK。
- `driver` 直接依赖具体 OS 对象。
- `port` 实现业务规则。
- `wrapper` 泄漏 `handler` 或 `driver` 的内部结构。
- 为了分层而增加空转发函数。

## 命名规则

- `bsp_<device>_driver.c/.h`
- `bsp_<device>_handler.c/.h`
- `bsp_<device>_port.c/.h`
- `bsp_<device>_wrapper.c/.h`
