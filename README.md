- # 算法课程设计代码包说明

  ## 1. 文件结构

  本项目主要包含四个文件夹：

  - `data`：存放测试数据
  - `src`：存放 C++ 源代码
  - `result`：存放程序运行结果

  其中，`data` 文件夹中包含：

  - `small_case.txt`：小规模测试数据
  - `medium_case.txt`：中规模测试数据
  - `large_case.txt`：大规模测试数据

  `src` 文件夹中包含：

  - `基线模型.cpp`：基线算法程序
  - `迪杰斯特拉二分.cpp`：改进算法程序

  `result` 文件夹中包含：

  - `small_baseline_log.txt`：小规模数据下基线算法运行结果
  - `small_ours_log.txt`：小规模数据下改进算法运行结果
  - `muedium_baseline_log.txt`：中规模数据下基线算法运行结果
  - `muedium_ours_log.txt`：中规模数据下改进算法运行结果
  - `large_baseline_log.txt`：大规模数据下基线算法运行结果
  - `large_ours_log.txt`：大规模数据下改进算法运行结果
  - `result.xlsx`：实验结果汇总表

  ## 2. 使用方法

  1. 打开 Dev-C++。
  2. 在 Dev-C++ 中打开 `src` 文件夹下的 `.cpp` 源代码文件。
  3. 点击编译运行。
  4. 打开 `data` 文件夹中的测试数据文件。
  5. 将 `small_case.txt`、`medium_case.txt` 或 `large_case.txt` 中的内容全部复制。
  6. 粘贴到程序运行窗口中，按回车运行。
  7. 查看程序输出的调度结果和统计指标。

  ## 3. 测试数据说明

  一般可以按下面顺序测试：

  - 先使用 `small_case.txt`，查看小规模数据下的完整调度过程；
  - 再使用 `medium_case.txt`，查看中规模数据下的统计结果；
  - 最后使用 `large_case.txt`，查看大规模数据下的统计结果。

  ## 4. 输出结果说明

  程序运行后会输出调度结果。

  小规模数据会输出较详细的调度过程，包括事件编号、救护车编号、接诊医院、响应时间、送医时间和完成时间。

  中规模和大规模数据主要输出统计指标，包括：

  - 成功调度事件数
  - 任务失败率
  - 平均响应时间
  - 高危平均响应时间
  - 救护车总行驶时间

  对应的运行结果已整理在 `result` 文件夹中。其中，`baseline_log` 表示基线算法结果，`ours_log` 表示改进算法结果，`result.xlsx` 为实验结果汇总表。