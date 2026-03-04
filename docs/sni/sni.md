# 脚本原生接口（SNI）

脚本原生接口（**Script Native Interface**，简称SNI）是一种用于在脚本中调用 C 函数的接口。

## 类型桥阶层（SNI Type Bridge Layer）

### 基本数据类型

基本数据类型有：
- 数值（Number）
- 布尔（Boolean）
- 字符串（String）

SNI 直接使用 JerryScript 的 API 来处理基本数据类型。

### 结构体

SNI 将结构体分为两类：Value Object 与 Handle Object。

#### Value Object

**Value Object** 是一种不独立拥有底层资源的运行时对象，用于表示纯数据或配置值。

Value Object 不需要显式创建或销毁，其生命周期通常受限于一次函数调用或表达式求值过程。它在桥接层会被直接封装为 JS 对象，其成员变量会被直接映射为 JS 对象的属性。
它们不会被运行时登记或追踪，也不会参与 Realm 级别的资源管理。

Value Object 的生命周期为**栈生命周期（Stack-Scoped）**。

栈生命周期：由 JS 引擎自动管理，当函数调用结束时，栈上的 Value Object 会被自动销毁。

#### Handle Object

**Handle Object** 是一种表示底层资源引用的运行时对象，其本身并不包含资源数据，而是作为对底层分配实体的间接访问入口。

Handle Object 必须通过显式创建获得，并支持显式销毁。
其生命周期由脚本逻辑直接控制，但运行时在 Realm 结束时提供统一的兜底回收，以防止资源泄漏。

Handle Object 的生命周期为**堆生命周期（Heap-Scoped）**。

堆生命周期：由脚本逻辑显式创建，通过调用特定的销毁函数来释放底层资源。当所有对 Handle Object 的引用都被释放时，运行时会自动触发其销毁过程。在本系统中，堆生命周期的对象会在 Realm 结束时被统一销毁。

## API 暴露层（SNI API Exposure Layer）

负责构建并维护 API 对象，包括：

- 函数注册
- 枚举注册
- 常量注册
- 子命名空间注册
- 类型构造器注册

该层主要负责 API 结构组织。
