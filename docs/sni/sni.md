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

## API 导出层（SNI API Export Layer）

API 导出层（**SNI API Export Layer**）负责将 API 导出给 JS Realm，使它们在脚本中可直接调用，API 是通过描述表（**API Description Table**）来定义的。

描述表内包括：
- 函数
- 枚举
- 常量
- 子命名空间

该层主要负责 API 结构组织。

### API 描述表（API Description Table）

API 描述表是一个 C 语言结构体数组，每个元素表示一个 API 条目。

每个 API 条目包含以下字段：
- 名称（Name）
- 类型（Type）
- 值（Value）

**API的类型**

目前 SNI 支持的 API 类型有：
- 函数：`jerry_external_handler_t` 类型的函数指针
- 常量：整数常量、浮点数常量、字符串常量
- 子条目：指向子条目的指针，用于实现命名空间的递归结构

#### 函数 API

函数类型实际上就是 `jerry_external_handler_t` 类型的函数指针，您需要实现该类型的函数，用于处理 JS 调用。

该函数的参数和返回值都需要符合 JerryScript 引擎的要求，具体可以参考 [jerry_external_handler_t - JerryScript 文档](https://jerryscript.net/api-reference/#jerry_external_handler_t)。

#### 常量 API

常量 API 是指在描述表中定义的整数值、浮点数常量和字符串常量，它们会被直接导出为 JS 中的常量。
它们在 C 代码中通常是枚举类型或由宏定义的常量。

#### 子条目 API

子条目 API 是指在描述表中定义的指向子条目的指针，用于实现命名空间的递归结构。

例如：

```c
jerry_value_t my_lv_obj_create(const jerry_call_info_t *call_info_p,
                               const jerry_value_t args_p[],
                               const jerry_length_t args_count)
{
    // 参数类型检查
    if (args_count != 1 || !jerry_value_is_object(args_p[0]))
    {
        return jerry_throw_value(jerry_error_sz(JERRY_ERROR_TYPE, "Invalid argument type"), true);
    }

    // 通过类型桥将 JS 参数转换为 C 类型
    lv_obj_t* obj = sni_tb_js2c(args_p[0], LVGL_OBJ_HANDLE);
    // obj 可以为 NULL

    // 创建 LVGL 对象
    lv_obj_t* new_obj = lv_obj_create(obj);
    if (new_obj == NULL)
    {
        return jerry_throw_value(jerry_error_sz(JERRY_ERROR_TYPE, "Failed to create LVGL object"), true);
    }

    // 将新创建的 LVGL 对象转换为 JS 对象
    jerry_value_t new_obj_handle = sni_tb_c2js(new_obj, LVGL_OBJ_HANDLE);
    if (jerry_value_is_exception(new_obj_handle))
    {
        return jerry_throw_value(new_obj_handle, true);
    }

    return new_obj_handle; // 返回创建的对象
}

const struct sni_api_entry_t lvgl_api_desc[] = {
    { "obj", SNI_ENTRY_NAMESPACE, { .sub_entries = lvgl_obj_api_desc } },
    // 其他子条目...
};

const struct sni_api_entry_t lvgl_obj_api_desc[] = {
    { "create", SNI_ENTRY_FUNCTION, { .function = my_lv_obj_create } },
    // 其他子条目...
};
```

这样，您就可以在 JS 中通过 `lv.obj.create()` 来调用 C 函数 `my_lv_obj_create()` 了。

### API 的导出过程

API 导出由以下过程组成：
1. 获取描述表代码
2. 描述表的解析
3. API 的挂载

#### 获取描述表代码

描述表代码一般通过 Python 脚本生成。

例如，LVGL API的描述表代码可以通过以下命令生成：

```bash
python3 generate_lvgl_desc.py
```

当然，您也可以根据需求自行编写描述表代码。

#### 描述表的解析

描述表的解析只需要调用`sni_api_build()`函数即可构建 API 描述表，若构建成功则会返回一个`jerry_value_t`类型的 JS 对象，此对象下挂载了所有 API 条目，此对象通常称为全局原生对象（Global Native Capability Object），例如`lv`就是一个全局原生对象。

#### API 的挂载

API 的挂载过程相对简单，只需要调用`sni_api_mount()`函数即可将 API 描述表挂载到指定的 JS Realm 中。

例如，挂载 LVGL API 描述表到指定 Realm 中可以通过以下代码实现：

```c
jerry_value_t lvgl_api_obj = sni_api_build(lvgl_api_desc);
sni_api_mount(jerry_realm, lvgl_api_obj, "lv");
```

挂载成功后，JS Realm 中就可以直接调用 LVGL API 了。

例如：

```js
lv.obj.create();
```
