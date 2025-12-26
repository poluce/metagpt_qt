# TreeSitterParser 测试报告

## 测试概述

**测试日期**: 2025-12-26  
**测试环境**: Windows + MinGW 7.3 + Qt 5.14.2  
**测试结果**: ✅ 14/14 通过

## 环境配置要点

### MinGW snprintf C99 兼容性修复

在 MinGW 环境下，需要在编译 tree-sitter 代码时定义 `__USE_MINGW_ANSI_STDIO=1`：

```qmake
# tree-sitter.pri
win32-g++: DEFINES += __USE_MINGW_ANSI_STDIO=1
```

**原因**: 老版本 MSVCRT 的 `snprintf` 在截断时返回 `-1`（而非 C99 规定的"本应写入的长度"），导致 `ts_node_string()` 内部长度计算错误。

## 测试覆盖范围

### 1. 基础解析测试

- ✅ **测试 1**: 基础解析 - 简单函数

### 2. 增量解析测试

- ✅ **测试 2**: 增量解析 - 插入代码

### 3. 错误处理测试

- ✅ **测试 3**: 错误处理 - 语法错误代码

### 4. 节点遍历测试

- ✅ **测试 4**: 节点遍历 - childCount 和 child
- ✅ **测试 5**: 节点遍历 - namedChildCount

### 5. 位置信息测试

- ✅ **测试 6**: 位置信息 - startLine, endLine, startByte, endByte

### 6. 节点定位测试

- ✅ **测试 7**: 节点定位 - nodeAtPosition

### 7. 环境验证测试

- ✅ **测试 8**: 环境验证 - snprintf C99 语义
  - 验证 `__USE_MINGW_ANSI_STDIO=1` 生效
  - `snprintf(buf, 1, "Hello, World!")` 返回 13（C99 标准）

### 8. S-expression 调试输出

- ✅ **测试 9**: 调试功能 - sExpression
  - 修复前: 只返回 `")"`
  - 修复后: 返回完整 S-expression

### 9. 多次编辑测试

- ✅ **测试 10**: 多次编辑 - 连续增量解析

### 10. 边界情况测试

- ✅ **测试 11**: 边界情况 - 空文件

### 11. 节点属性测试

- ✅ **测试 12**: 节点属性 - isNamed, isMissing, nodeHasError

### 12. 节点导航测试

- ✅ **测试 13**: 节点导航 - nextSibling, prevSibling

### 13. 解析器管理测试

- ✅ **测试 14**: 解析器管理 - reset

## 已修复问题

### sExpression 在 MinGW 下返回错误结果 (已修复 ✅)

**问题描述**: `ts_node_string()` 只返回 `")"`  
**根本原因**: MinGW 老版 MSVCRT 的 `snprintf` 在截断时返回 -1，导致 tree-sitter 内部长度计算下溢  
**修复方案**: 编译时定义 `__USE_MINGW_ANSI_STDIO=1`

#### 技术细节

```c
// tree-sitter/lib/src/subtree.c:934-946
char *ts_subtree_string(...) {
  char scratch_string[1];
  size_t size = ts_subtree__write_to_string(
    self, scratch_string, 1,  // limit=1 导致截断
    ...
  ) + 1;
  // 如果 snprintf 返回 -1，size 会变成 0
  char *result = ts_malloc(size * sizeof(char));
  // 第二次调用写入不完整
  ...
}
```

## 测试统计

| 类别       | 测试数 | 通过   | 失败  | 跳过  |
| ---------- | ------ | ------ | ----- | ----- |
| 基础功能   | 3      | 3      | 0     | 0     |
| 节点操作   | 5      | 5      | 0     | 0     |
| 增量解析   | 2      | 2      | 0     | 0     |
| 环境验证   | 1      | 1      | 0     | 0     |
| 调试功能   | 1      | 1      | 0     | 0     |
| 边界情况   | 1      | 1      | 0     | 0     |
| 解析器管理 | 1      | 1      | 0     | 0     |
| **总计**   | **14** | **14** | **0** | **0** |

## 结论

TreeSitterParser 所有功能已通过测试：

- ✅ 初次解析功能完整
- ✅ 增量解析功能正常
- ✅ 节点遍历和查询功能稳定
- ✅ 错误处理机制健壮
- ✅ 位置信息准确
- ✅ S-expression 调试功能正常（需正确配置编译环境）

**总体评价**: 解析器实现质量高，可以投入使用。

## 配置清单

确保以下配置正确：

1. **主工程** (`TmAgent.pro`): `include(3rdparty/tree-sitter.pri)`
2. **测试工程** (`TreeSitterParserTest.pro`): `include(../3rdparty/tree-sitter.pri)`
3. **tree-sitter.pri**: 包含 `win32-g++: DEFINES += __USE_MINGW_ANSI_STDIO=1`
