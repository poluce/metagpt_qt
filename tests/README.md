# TmAgent 测试套件

## 目录结构

```
tests/
├── parser/                           # 解析器测试模块
│   ├── TreeSitterParserTest.pro
│   ├── TreeSitterParserTest.cpp
│   ├── README.md
│   └── TEST_REPORT.md
├── agent/                            # Agent 测试 (待添加)
├── tools/                            # 工具测试 (待添加)
└── README.md                         # 本文件
```

## 测试模块

| 模块              | 状态     | 描述                      |
| ----------------- | -------- | ------------------------- |
| [parser](parser/) | ✅ 14/14 | TreeSitterParser 封装测试 |
| agent             | 🔜       | LLMAgent、ToolDispatcher  |
| tools             | 🔜       | FileTool、ShellTool       |

## 运行测试

```powershell
# Parser 模块
cd tests/parser
mkdir build; cd build; qmake ..; mingw32-make -j4
.\release\TreeSitterParserTest.exe
```

## 添加新模块

1. 创建 `tests/<module>/` 目录
2. 添加 `*.pro`、`*Test.cpp`、`README.md`
3. 更新本文件的模块表格
