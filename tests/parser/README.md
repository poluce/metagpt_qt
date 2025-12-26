# TreeSitterParser 测试模块

## 概述

此模块测试 `TreeSitterParser` 类的 tree-sitter C++ 封装功能。

## 编译和运行

```powershell
mkdir build; cd build
qmake ..
mingw32-make -j4
.\release\TreeSitterParserTest.exe
```

## 测试用例 (14/14 通过)

| #   | 测试名称         | 描述                                |
| --- | ---------------- | ----------------------------------- |
| 1   | 基础解析         | 简单函数解析                        |
| 2   | 增量解析         | 插入代码后重新解析                  |
| 3   | 错误处理         | 语法错误代码检测                    |
| 4   | childCount/child | 子节点遍历                          |
| 5   | namedChildCount  | 命名子节点遍历                      |
| 6   | 位置信息         | startLine/endLine/startByte/endByte |
| 7   | nodeAtPosition   | 根据位置查找节点                    |
| 8   | snprintf C99     | 环境验证（MinGW 兼容性）            |
| 9   | sExpression      | S-expression 调试输出               |
| 10  | 多次编辑         | 连续增量解析                        |
| 11  | 空文件           | 边界情况处理                        |
| 12  | 节点属性         | isNamed/isMissing/nodeHasError      |
| 13  | 兄弟节点         | nextSibling/prevSibling             |
| 14  | reset            | 解析器重置                          |

## 环境配置

确保 `tree-sitter.pri` 包含：

```qmake
win32-g++: DEFINES += __USE_MINGW_ANSI_STDIO=1
```

详见 [TEST_REPORT.md](TEST_REPORT.md)
