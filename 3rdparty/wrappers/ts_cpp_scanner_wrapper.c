// tree-sitter-cpp scanner 代理文件
// 通过 wrapper 文件解决与 yaml-cpp 的文件名冲突
// 编译输出: ts_cpp_scanner_wrapper.o

#include "../tree-sitter-cpp-0.23.4/src/scanner.c"
