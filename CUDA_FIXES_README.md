# VAMPIRE CUDA 修复版本

这是针对 VAMPIRE 磁性模拟软件的增强版本，修复了 CUDA 模块的关键问题并添加了分段磁滞功能。

## 主要修复内容

### 1. CUDA 内存初始化修复
- **问题描述**: `pos_and_mom_array` 在 CUDA 内核中未初始化导致段错误
- **解决方案**: 在 `cells::initialize()` 中添加正确的坐标数组初始化
- **影响文件**: 
  - `src/cells/initialize.cpp`: 添加 pos_and_mom_array 初始化逻辑
  - `src/cuda/initialize.cu`: 改进设备初始化和错误处理

### 2. LocalFieldRegion 实现
- **新功能**: 支持基于坐标的局部场区域定义
- **用途**: 实现分段磁滞回线功能
- **影响文件**:
  - `hdr/cells.hpp`: 添加 LocalFieldRegion 结构体定义
  - `src/cells/data.cpp`: 添加区域存储变量
  - `src/cells/interface.cpp`: 实现基于区域的场分配

### 3. CUDA 错误处理增强
- **改进**: CURAND 错误处理，支持优雅的回退机制
- **改进**: 设备初始化检查
- **改进**: 内存分配错误报告
- **影响文件**: `src/cuda/monte_carlo.cu`

### 4. 图着色算法保留
- **保持**: 15色原子分组的并行蒙特卡洛算法
- **确保**: 现有并行化效率不受影响
- **修复**: 编译问题，不影响性能

## 测试系统

- **系统**: 178,560 个原子的 CrBr3 三层结构
- **温度**: 2K
- **CUDA 加速**: 启用
- **图着色**: 15色分组
- **状态**: 所有 CUDA 功能正常运行

## 技术细节

### 修复的"海森堡"问题
添加自定义 LocalFieldRegion 代码暴露了一个潜在的初始化依赖问题：
- 之前 `pos_and_mom_array` 可能偶然地紧邻其他已初始化的内存
- 新代码使内存布局发生变化，导致 CUDA 访问未初始化内存
- 修复通过正确的数组初始化解决了这个依赖问题

### 关键修改摘要
```cpp
// src/cells/initialize.cpp 中的关键修复
cells::pos_and_mom_array.resize(4 * cells::num_cells, 0.0);
for(int cell_index = 0; cell_index < cells::num_cells; cell_index++) {
    cells::pos_and_mom_array[4 * cell_index + 0] = cx;
    cells::pos_and_mom_array[4 * cell_index + 1] = cy;
    cells::pos_and_mom_array[4 * cell_index + 2] = cz;
    cells::pos_and_mom_array[4 * cell_index + 3] = 0.0;
}
```

### 分段磁滞功能
新的 LocalFieldRegion 结构允许：
- 基于坐标范围定义不同的局部场区域
- 为每个区域独立控制磁场参数
- 支持复杂的磁场分布研究

## 编译要求

- CUDA Toolkit (支持双精度)
- GCC/G++ 编译器
- NVIDIA GPU (计算能力 >= 2.0)

## 使用方法

```bash
# 编译 CUDA 版本
make cuda

# 运行分段磁滞模拟
./vampire-cuda input
```

## 版本信息

- **基础版本**: VAMPIRE v7.0.0
- **修复版本**: 2025年9月12日
- **作者**: 翟雨欣
- **仓库**: https://github.com/mandarinduck-1/vampire_alpha

## 注意事项

1. 所有图着色调试信息已保留，作为核心 VAMPIRE 功能
2. VS Code IntelliSense 错误为误报，实际编译正常
3. 建议使用 `/usr/include` 路径配置 CUDA 头文件
4. 编译的二进制文件已在 .gitignore 中排除

## 测试验证

✅ CUDA 编译成功  
✅ 运行时无段错误  
✅ 15色图着色正常工作  
✅ 蒙特卡洛算法正常  
✅ 178,560 原子系统测试通过  
✅ 分段磁滞功能可用  

---

此版本解决了在添加自定义功能时暴露的所有 CUDA 问题，并保持了 VAMPIRE 的所有现有功能。