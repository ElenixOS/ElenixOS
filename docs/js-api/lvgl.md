# LVGL

## 概述

ElenaOS 通过绑定层将 LVGL UI 控件转换为 JS API，并绑定到 Realm 中。

## 基础控件

基础控件由脚本自动生成，接口与 LVGL UI 控件一致。

## 高级控件

### 定时器(lv_timer)

>**NOTE**
>脚本退出后，在JS中创建的所有定时器都会被删除。

#### 示例程序

```js
function timer_example() {
    let timer = lv.timer.create(function (t) {
        eos.print("Timer triggered");
        eos.print("Timer name: ", lv.timer.getUserData(timer).name);
    }, 1000);
    lv.timer.pause(timer);
    lv.timer.setUserData(timer, {
        "name": "My timer"
    });
    eos.print("Paused? ", lv.timer.getPaused(timer));

    let basicTimer = lv.timer.createBasic();
    lv.timer.setCb(basicTimer, function (t) {
        eos.print("Basic timer triggered");
        lv.timer.resume(timer);
        lv.timer.reset(timer);
    })
    lv.timer.setRepeatCount(basicTimer, 1);
    lv.timer.setPeriod(basicTimer, 1000);
}
```

#### API 列表

| API                                                                                     | 描述                                         |
| --------------------------------------------------------------------------------------- | -------------------------------------------- |
| [`lv.timer.create(cb: function, period: number, [user_data: any])`](#lv-timer-create)   | 创建一个定时器                               |
| [`lv.timer.createBasic()`](#lv-timer-createbasic)                                       | 创建一个基础定时器                           |
| [`lv.timer.delete(timer: object)`](#lv-timer-delete)                                    | 删除定时器                                   |
| [`lv.timer.setUserData(timer: object, data: any)`](#lv-timer-setuserdata)               | 设置定时器的用户数据                         |
| [`lv.timer.getUserData(timer: object)`](#lv-timer-getuserdata)                          | 获取定时器的用户数据                         |
| [`lv.timer.pause(timer: object)`](#lv-timer-pause)                                      | 暂停定时器                                   |
| [`lv.timer.resume(timer: object)`](#lv-timer-resume)                                    | 恢复定时器                                   |
| [`lv.timer.getPaused(timer: object)`](#lv-timer-getpaused)                              | 获取定时器的暂停状态                         |
| [`lv.timer.setPeriod(timer: object, period: number)`](#lv-timer-setperiod)              | 设置定时器的周期                             |
| [`lv.timer.setCb(timer: object, cb: finction)`](#lv-timer-setcb)                        | 设置定时器的回调函数                         |
| [`lv.timer.setRepeatCount(timer: object, count: number)`](#lv-timer-setrepeatcount)     | 设置定时器的重复次数，设置为 -1 表示无限重复 |
| [`lv.timer.ready(timer: object)`](#lv-timer-ready)                                      | 设置定时器就绪                               |
| [`lv.timer.setAutoDelete(timer: object, autoDelete: boolean)`](#lv-timer-setautodelete) | 设置定时器是否自动删除                       |
| [`lv.timer.reset(timer: object)`](#lv-timer-reset)                                      | 重置定时器                                   |
| [`lv.timer.del(timer: object)`](#lv-timer-delete)                                       | 删除定时器                                   |

#### API 详述

<a id="lv-timer-create"></a>

##### lv.timer.create(cb: function, period: number, [user_data: any])

创建一个定时器。

**参数说明：**

| 参数        | 类型     | 描述               |
| ----------- | -------- | ------------------ |
| `cb`        | Function | 定时器回调函数     |
| `period`    | number   | 定时器周期（毫秒） |
| `user_data` | any      | 可选，用户数据     |

**回调签名：**

```js
function(timer)
```

**返回值：**

* object：创建的定时器对象

---

<a id="lv-timer-createbasic"></a>

##### lv.timer.createBasic()

创建一个基础定时器，不需要指定回调和用户数据。

**返回值：**

* object：创建的基础定时器对象

---

<a id="lv-timer-delete"></a>

##### lv.timer.delete(timer: object)

删除定时器对象。

**参数说明：**

| 参数    | 类型   | 描述               |
| ------- | ------ | ------------------ |
| `timer` | object | 待删除的定时器对象 |

**返回值：**

* 无

---

<a id="lv-timer-setuserdata"></a>

##### lv.timer.setUserData(timer: object, data: any)

设置定时器的用户数据。

**参数说明：**

| 参数    | 类型   | 描述       |
| ------- | ------ | ---------- |
| `timer` | object | 定时器对象 |
| `data`  | any    | 用户数据   |

**返回值：**

* 无

---

<a id="lv-timer-getuserdata"></a>

##### lv.timer.getUserData(timer: object)

获取定时器的用户数据。

**参数说明：**

| 参数    | 类型   | 描述       |
| ------- | ------ | ---------- |
| `timer` | object | 定时器对象 |

**返回值：**

* any：定时器绑定的用户数据

---

<a id="lv-timer-pause"></a>

##### lv.timer.pause(timer: object)

暂停定时器。

**参数说明：**

| 参数    | 类型   | 描述       |
| ------- | ------ | ---------- |
| `timer` | object | 定时器对象 |

**返回值：**

* 无

---

<a id="lv-timer-resume"></a>

##### lv.timer.resume(timer: object)

恢复定时器。

**参数说明：**

| 参数    | 类型   | 描述       |
| ------- | ------ | ---------- |
| `timer` | object | 定时器对象 |

**返回值：**

* 无

---

<a id="lv-timer-getpaused"></a>

##### lv.timer.getPaused(timer: object)

获取定时器的暂停状态。

**参数说明：**

| 参数    | 类型   | 描述       |
| ------- | ------ | ---------- |
| `timer` | object | 定时器对象 |

**返回值：**

* boolean：是否处于暂停状态

---

<a id="lv-timer-setperiod"></a>

##### lv.timer.setPeriod(timer: object, period: number)

设置定时器的周期。

**参数说明：**

| 参数     | 类型   | 描述               |
| -------- | ------ | ------------------ |
| `timer`  | object | 定时器对象         |
| `period` | number | 定时器周期（毫秒） |

**返回值：**

* 无

---

<a id="lv-timer-setcb"></a>

##### lv.timer.setCb(timer: object, cb: function)

设置定时器的回调函数。

**参数说明：**

| 参数    | 类型     | 描述       |
| ------- | -------- | ---------- |
| `timer` | object   | 定时器对象 |
| `cb`    | function | 回调函数   |

**回调签名：**

```js
function(timer)
```

**返回值：**

* 无

---

<a id="lv-timer-setrepeatcount"></a>

##### lv.timer.setRepeatCount(timer: object, count: number)

设置定时器的重复次数。

**参数说明：**

| 参数    | 类型   | 描述                   |
| ------- | ------ | ---------------------- |
| `timer` | object | 定时器对象             |
| `count` | number | 重复次数，`-1`表示无限 |

**返回值：**

* 无

---

<a id="lv-timer-ready"></a>

##### lv.timer.ready(timer: object)

设置定时器就绪，立即触发回调。

**参数说明：**

| 参数    | 类型   | 描述       |
| ------- | ------ | ---------- |
| `timer` | object | 定时器对象 |

**返回值：**

* 无

---

<a id="lv-timer-setautodelete"></a>

##### lv.timer.setAutoDelete(timer: object, autoDelete: boolean)

设置定时器是否在回调执行完成后自动删除。

**参数说明：**

| 参数         | 类型    | 描述         |
| ------------ | ------- | ------------ |
| `timer`      | object  | 定时器对象   |
| `autoDelete` | boolean | 是否自动删除 |

**返回值：**

* 无

---

<a id="lv-timer-reset"></a>

##### lv.timer.reset(timer: object)

重置定时器，重新开始计时。

**参数说明：**

| 参数    | 类型   | 描述       |
| ------- | ------ | ---------- |
| `timer` | object | 定时器对象 |

**返回值：**

* 无

---

<a id="lv-timer-del"></a>

##### lv.timer.del(timer: object)

删除定时器（与 `lv.timer.delete` 等效）。

**参数说明：**

| 参数    | 类型   | 描述       |
| ------- | ------ | ---------- |
| `timer` | object | 定时器对象 |

**返回值：**

* 无

---


### 动画(lv_anim)

与C语言的LVGL动画不同，在JS中使用LVGL的动画只需要创建一个动画对象，然后直接设置它的属性即可。

>**NOTE**
>脚本退出后，在JS中创建的所有动画都会被删除。

#### 示例程序

```js
function animation_example() {
    let obj = lv.obj.create(scr);
    lv.obj.setSize(obj, 100, 100);
    lv.obj.setStyleBgColor(obj, lv.color.hex(0xFF0000), 0);
    lv.obj.setY(obj, 200);
    let anim = lv.anim.create();
    anim.duration = 1000;
    anim.var = obj;
    anim.startValue = 0;
    anim.endValue = 250;
    anim.repeatCnt = 1;
    anim.customExecCb = function (a, v) {
        eos.print("Value = " + v);
        lv.obj.setX(obj, v);
    };
    anim.startCb = function (a) {
        eos.print("Anim start");
    };
    anim.deletedCb = function (a) {
        eos.print("Anim deleted");
    };
    anim.completedCb = function (a) {
        eos.print("Anim completed");
    };
    anim.getValue = function (a) {
        eos.print("Get Value");
        return 100;
    };

    anim.pathCb = lv.ANIM_PATH_EASE_IN_OUT;

    lv.anim.start(anim);
}
```

#### 属性列表

| 属性                       | 描述                                                           |
| -------------------------- | -------------------------------------------------------------- |
| `var: any`                 | 要执行动画的目标对象，通常为某个控件或用户自定义对象           |
| `userData: any`            | 用户自定义数据，在动画回调中可通过该字段获取                   |
| `startValue: number`       | 动画起始值                                                     |
| `currentValue: number`     | 动画当前值，由动画系统在运行过程中自动更新                     |
| `endValue: number`         | 动画结束值                                                     |
| `duration: number`         | 动画持续时间，单位为毫秒                                       |
| `actTime: number`          | 动画当前已运行时间，单位为毫秒<br>设置为负值可实现延时启动动画 |
| `playbackDelay: number`    | 回放动画开始前的延时，单位为毫秒                               |
| `playbackDuration: number` | 回放动画的持续时间，单位为毫秒                                 |
| `repeatDelay: number`      | 动画重复执行前的延时，单位为毫秒                               |
| `repeatCnt: number`        | 动画重复次数，设置为 `lv.ANIM_REPEAT_INFINITE` 表示无限重复    |
| `bezier3: object`          | 三次贝塞尔曲线参数对象，用于控制动画的缓动效果                 |
| `bezier3.x1: number`       | 贝塞尔曲线第一个控制点的 X 坐标                                |
| `bezier3.y1: number`       | 贝塞尔曲线第一个控制点的 Y 坐标                                |
| `bezier3.x2: number`       | 贝塞尔曲线第二个控制点的 X 坐标                                |
| `bezier3.y2: number`       | 贝塞尔曲线第二个控制点的 Y 坐标                                |

#### 回调列表

| 回调名称                     | 描述                                           | 回调签名                        | 回调返回值 |
| ---------------------------- | ---------------------------------------------- | ------------------------------- | ---------- |
| `customExecCb: function`     | 动画执行回调，在动画运行过程中按当前值反复调用 | `(anim: object, value: number)` | `void`     |
| `startCb: function`          | 动画开始时触发的回调函数                       | `(anim: object)`                | `void`     |
| `completedCb: function`      | 动画正常播放完成时触发的回调函数               | `(anim: object)`                | `void`     |
| `deletedCb: function`        | 动画对象被删除时触发的回调函数                 | `(anim: object)`                | `void`     |
| `getValue: function`         | 获取动画目标对象的当前值                       | `(anim: object)`                | `number`   |
| `pathCb: function \| number` | 动画路径计算回调，用于根据时间计算插值结果     | `(anim: object)`                | `number`   |

##### pathCb

pathCb 支持自定义函数或枚举数值，枚举数值对应 LVGL 内置的动画路径函数。

| 枚举值                        | 含义               |
| ----------------------------- | ------------------ |
| `lv.ANIM_PATH_LINEAR`         | 线性路径           |
| `lv.ANIM_PATH_EASE_IN`        | 缓进路径           |
| `lv.ANIM_PATH_EASE_OUT`       | 缓出路径           |
| `lv.ANIM_PATH_EASE_IN_OUT`    | 缓进缓出路径       |
| `lv.ANIM_PATH_OVERSHOOT`      | 过冲路径           |
| `lv.ANIM_PATH_BOUNCE`         | 弹跳路径           |
| `lv.ANIM_PATH_STEP`           | 步进路径           |
| `lv.ANIM_PATH_CUSTOM_BEZIER3` | 三次贝塞尔曲线路径 |

**三次贝塞尔曲线路径**

三次贝塞尔曲线是 LVGL 提供的动画路径函数，用于实现动画的缓进缓出效果。
三次贝塞尔曲线的参数通过 `anim.bezier3` 对象设置，该对象包含以下属性：
- `x1`: 贝塞尔曲线第一个控制点的 X 坐标（0-1024）
- `y1`: 贝塞尔曲线第一个控制点的 Y 坐标（0-1024）
- `x2`: 贝塞尔曲线第二个控制点的 X 坐标（0-1024）
- `y2`: 贝塞尔曲线第二个控制点的 Y 坐标（0-1024）

示例：

```js
anim.pathCb = lv.ANIM_PATH_CUSTOM_BEZIER3;
anim.bezier3.x1 = 1024;
anim.bezier3.y1 = 0;
anim.bezier3.x2 = 594;
anim.bezier3.y2 = 1024;
```

#### API 列表

| API                                                                               | 描述                 |
| --------------------------------------------------------------------------------- | -------------------- |
| [`lv.anim.create(anim: object)`](#lv-anim-create)                                 | 创建动画对象         |
| [`lv.anim.start(anim: object)`](#lv-anim-start)                                   | 启动动画             |
| [`lv.anim.setCustomExecCb(anim: object, cb: function)`](#lv-anim-setcustomexeccb) | 设置自定义执行回调   |
| [`lv.anim.setPathCb(anim: object, cb: function)`](#lv-anim-setpathcb)             | 设置动画路径函数     |
| [`lv.anim.setStartCb(anim: object, cb: function)`](#lv-anim-setstartcb)           | 设置动画开始回调     |
| [`lv.anim.setGetValueCb(anim: object, cb: function)`](#lv-anim-setgetvaluecb)     | 设置获取当前值的回调 |
| [`lv.anim.setCompletedCb(anim: object, cb: function)`](#lv-anim-setcompletedcb)   | 设置动画完成回调     |
| [`lv.anim.setDeletedCb(anim: object, cb: function)`](#lv-anim-setdeletedcb)       | 设置动画删除回调     |
| [`lv.anim.setReadyCb(anim: object, cb: function)`](#lv-anim-setreadycb)           | 设置动画就绪回调     |
| [`lv.anim.setUserData(anim: object, data: any)`](#lv-anim-setuserdata)            | 设置用户数据         |
| [`lv.anim.getUserData(anim: object)`](#lv-anim-getuserdata)                       | 获取用户数据         |

---

#### API 详述

<a id="lv-anim-create"></a>
##### lv.anim.create(anim)

创建一个动画对象，并初始化其内部结构。

**参数说明：**

| 参数   | 类型   | 描述                             |
| ------ | ------ | -------------------------------- |
| `anim` | object | 动画对象，用于保存动画的所有配置 |

**返回值：**

* 无

**说明：**

- 该函数通常用于初始化一个新的动画对象
- 创建后需进一步设置回调、起止值、时长等参数
- 创建完成后需调用 `lv.anim.start()` 才会真正开始播放

---

<a id="lv-anim-start"></a>
##### lv.anim.start(anim)

启动动画，使其进入调度和播放流程。

**参数说明：**

| 参数   | 类型   | 描述                       |
| ------ | ------ | -------------------------- |
| `anim` | object | 已创建并配置完成的动画对象 |

**返回值：**

* 无

**说明：**

- 调用后动画将按照当前配置开始运行
- 若动画已在运行，重复调用行为由底层实现决定

---

<a id="lv-anim-setcustomexeccb"></a>
##### lv.anim.setCustomExecCb(anim, cb)

设置动画的自定义执行回调函数。

**参数说明：**

| 参数   | 类型     | 描述               |
| ------ | -------- | ------------------ |
| `anim` | object   | 动画对象           |
| `cb`   | Function | 自定义执行回调函数 |

**回调签名：**

```js
function(anim, value)
```

**返回值：**

* 无

**说明：**

* 在动画每次更新时调用
* `value` 为当前动画计算得到的值
* 通常用于直接操作对象属性

---

<a id="lv-anim-setpathcb"></a>

##### lv.anim.setPathCb(anim, cb)

设置动画路径函数，用于控制动画插值方式。

**参数说明：**

| 参数   | 类型     | 描述         |
| ------ | -------- | ------------ |
| `anim` | object   | 动画对象     |
| `cb`   | Function | 路径计算函数 |

**回调签名：**

```js
function(anim)
```

**返回值：**

* `int32`：计算后的动画值

---

<a id="lv-anim-setstartcb"></a>

##### lv.anim.setStartCb(anim, cb)

设置动画开始时的回调函数。

**参数说明：**

| 参数   | 类型     | 描述         |
| ------ | -------- | ------------ |
| `anim` | object   | 动画对象     |
| `cb`   | Function | 开始回调函数 |

**回调签名：**

```js
function(anim)
```

**返回值：**

* 无

---

<a id="lv-anim-setgetvaluecb"></a>

##### lv.anim.setGetValueCb(anim, cb)

设置用于获取当前对象值的回调函数。

**参数说明：**

| 参数   | 类型     | 描述         |
| ------ | -------- | ------------ |
| `anim` | object   | 动画对象     |
| `cb`   | Function | 获取值的回调 |

**回调签名：**

```js
function(anim)
```

**返回值：**

* `int32`：当前对象的值

---

<a id="lv-anim-setcompletedcb"></a>

##### lv.anim.setCompletedCb(anim, cb)

设置动画播放完成时的回调函数。

**参数说明：**

| 参数   | 类型     | 描述         |
| ------ | -------- | ------------ |
| `anim` | object   | 动画对象     |
| `cb`   | Function | 完成回调函数 |

**回调签名：**

```js
function(anim)
```

**返回值：**

* 无

---

<a id="lv-anim-setdeletedcb"></a>

##### lv.anim.setDeletedCb(anim, cb)

设置动画被删除时触发的回调函数。

**参数说明：**

| 参数   | 类型     | 描述         |
| ------ | -------- | ------------ |
| `anim` | object   | 动画对象     |
| `cb`   | Function | 删除回调函数 |

**回调签名：**

```js
function(anim)
```

**返回值：**

* 无

---

<a id="lv-anim-setreadycb"></a>

##### lv.anim.setReadyCb(anim, cb)

设置动画进入就绪状态时的回调函数。

**参数说明：**

| 参数   | 类型     | 描述         |
| ------ | -------- | ------------ |
| `anim` | object   | 动画对象     |
| `cb`   | Function | 就绪回调函数 |

**回调签名：**

```js
function(anim)
```

**返回值：**

* 无

---

<a id="lv-anim-setuserdata"></a>

##### lv.anim.setUserData(anim, data)

为动画对象设置用户自定义数据。

**参数说明：**

| 参数   | 类型   | 描述     |
| ------ | ------ | -------- |
| `anim` | object | 动画对象 |
| `data` | any    | 用户数据 |

**返回值：**

* 无

**说明：**

* 用户数据会随动画对象生命周期存在
* 可在回调函数中通过 `getUserData` 获取

---

<a id="lv-anim-getuserdata"></a>

##### lv.anim.getUserData(anim)

获取动画对象中存储的用户数据。

**参数说明：**

| 参数   | 类型   | 描述     |
| ------ | ------ | -------- |
| `anim` | object | 动画对象 |

**返回值：**

* 用户通过 `setUserData` 设置的数据
