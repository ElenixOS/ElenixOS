# Services

## 简述

`Services`模块提供了一套统一的传感器访问与管理机制，通过异步事件驱动方式读取各类传感器数据。
该机制在采样请求与数据上报之间采用非阻塞操作，能够有效避免阻塞主线程或影响 UI 刷新性能。
同时，系统会在传感器数据更新后自动触发回调事件，将最新测量结果分发给上层模块，实现数据的实时同步与界面的平滑刷新。

## 使用方法

1. **服务启动**
   - 在系统初始化阶段调用 `eos_<sensor>_service_start()` 启动指定传感器服务。
   - 启动后，内部会创建一个 `lv_timer` 定时器，用于周期性触发 `eos_sensor_read_async()`。
   - 每次定时器到期时，系统将自动读取一次传感器数据（如电池电量、温度、加速度等）。
   - 读取结果会通过异步事件 `EOS_EVENT_SENSOR_REPORT_<SENSOR>` 广播到上层。

   ```c
   eos_battery_service_start();    // 启动电池服务
   eos_temp_service_start();       // 启动温度服务
   ```

2. **同步获取传感器值**
   - 调用 `eos_<sensor>_service_get_<value>()` 可在任意时刻获取最近一次缓存的传感器数据。
   - 该接口直接返回上一次测量结果，不会触发新的采样，适合在算法或UI刷新中快速访问。

   ```c
   uint8_t level = eos_battery_service_get_level();
   printf("Battery: %d%%\n", level);
   ```

3. **UI 事件绑定（异步更新）**
   - 若需要在界面上实时显示传感器变化，可在控件创建时注册全局广播事件回调。
   - 当底层传感器采样完成后，会通过 `EOS_EVENT_SENSOR_REPORT_<SENSOR>` 通知所有监听者。
   - 回调中可直接更新显示内容，实现传感器数据的动态刷新。

   ```c
   static void _on_battery_update(lv_event_t *e)
   {
       eos_sensor_t *sensor = lv_event_get_param(e);
       lv_label_set_text_fmt(battery_label, "%d%%", sensor->data.bat.level);
   }
   // TODO 更新文档
   eos_event_add_cb(
       eos_screen_active(),
       _on_battery_update,
       EOS_EVENT_SENSOR_REPORT_BAT,
       NULL);
   ```
## 扩展说明
   - 所有传感器服务（电池、温度、光线、IMU 等）均遵循相同的接口命名与行为模式。
   - 对于高速或流式传感器（如 IMU），可采用独立线程或 DMA + FIFO 机制采样，上层接口仍保持一致。
