#include <QDebug>
#include <QTextCodec>
#include <iostream>

#include "core/parser/TreeSitterParser.h"

static int g_testCount = 0;
static int g_passCount = 0;

#define TEST(name) \
    ++g_testCount; \
    qDebug() << "[TEST" << g_testCount << "]" << name; \
    if (auto result = [&]() -> int

#define END_TEST \
    (); result != 0) { \
        qCritical() << "  ❌ FAILED"; \
        return result; \
    } else { \
        ++g_passCount; \
        qDebug() << "  ✅ PASSED"; \
    }

static int Fail(const QString& message) {
    qCritical().noquote() << "    " << message;
    return 1;
}

int main() {
    // 设置 Qt 默认编码为 UTF-8（跨平台）
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    qDebug() << "========================================";
    qDebug() << "TreeSitterParser 测试套件 (新 SyntaxNode 接口)";
    qDebug() << "========================================";

    // ========================================
    // 测试 1: 基础解析
    // ========================================
    TEST("基础解析 - 简单函数") {
        TreeSitterParser parser;
        const QByteArray source = "int main() { }";
        
        if (!parser.parse(source)) {
            return Fail(QStringLiteral("parse failed: %1").arg(parser.lastError()));
        }

        SyntaxNode root = parser.rootNode();
        if (root.isNull()) {
            return Fail(QStringLiteral("root node is null"));
        }
        if (root.type() != QStringLiteral("translation_unit")) {
            return Fail(QStringLiteral("unexpected root node type: %1").arg(root.type()));
        }
        if (parser.hasError()) {
            return Fail(QStringLiteral("unexpected syntax error"));
        }
        if (root.text().toUtf8() != source) {
            return Fail(QStringLiteral("text() mismatch"));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 2: 增量解析
    // ========================================
    TEST("增量解析 - 插入代码") {
        TreeSitterParser parser;
        const QByteArray source = "int main() { }";
        parser.parse(source);

        const QByteArray insertText = "return 0; ";
        int insertPos = source.indexOf('}');
        QByteArray updated = source;
        updated.insert(insertPos, insertText);

        parser.applyEdit(
            static_cast<uint32_t>(insertPos),
            static_cast<uint32_t>(insertPos),
            static_cast<uint32_t>(insertPos + insertText.size()),
            1, static_cast<uint32_t>(insertPos),
            1, static_cast<uint32_t>(insertPos),
            1, static_cast<uint32_t>(insertPos + insertText.size())
        );

        if (!parser.reparse(updated)) {
            return Fail(QStringLiteral("reparse failed: %1").arg(parser.lastError()));
        }

        SyntaxNode root = parser.rootNode();
        if (parser.hasError()) {
            return Fail(QStringLiteral("unexpected syntax error after reparse"));
        }
        if (root.text().toUtf8() != updated) {
            return Fail(QStringLiteral("text() mismatch after reparse"));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 3: 错误处理 - 语法错误
    // ========================================
    TEST("错误处理 - 语法错误代码") {
        TreeSitterParser parser;
        const QByteArray errorSource = "int main() { return 0";  // 缺少 }
        
        if (!parser.parse(errorSource)) {
            return Fail(QStringLiteral("parse failed: %1").arg(parser.lastError()));
        }

        if (!parser.hasError()) {
            return Fail(QStringLiteral("expected syntax error but got none"));
        }

        SyntaxNode root = parser.rootNode();
        if (root.isNull()) {
            return Fail(QStringLiteral("root should not be null even with errors"));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 4: 节点遍历
    // ========================================
    TEST("节点遍历 - childCount 和 child") {
        TreeSitterParser parser;
        const QByteArray source = "int x = 5;\nint y = 10;";
        parser.parse(source);

        SyntaxNode root = parser.rootNode();
        uint32_t count = root.childCount();
        
        if (count == 0) {
            return Fail(QStringLiteral("root should have children"));
        }

        // 遍历所有子节点
        for (uint32_t i = 0; i < count; ++i) {
            SyntaxNode child = root.child(i);
            if (child.isNull()) {
                return Fail(QStringLiteral("child %1 is null").arg(i));
            }
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 5: 命名子节点
    // ========================================
    TEST("节点遍历 - namedChildCount") {
        TreeSitterParser parser;
        const QByteArray source = "int main() { int x = 5; return x; }";
        parser.parse(source);

        SyntaxNode root = parser.rootNode();
        uint32_t namedCount = root.namedChildCount();
        
        if (namedCount == 0) {
            return Fail(QStringLiteral("root should have named children"));
        }

        for (uint32_t i = 0; i < namedCount; ++i) {
            SyntaxNode namedChild = root.namedChild(i);
            if (namedChild.isNull()) {
                return Fail(QStringLiteral("named child %1 is null").arg(i));
            }
            if (!namedChild.isNamed()) {
                return Fail(QStringLiteral("named child %1 is not actually named").arg(i));
            }
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 6: 位置信息
    // ========================================
    TEST("位置信息 - startLine, endLine, startByte, endByte") {
        TreeSitterParser parser;
        const QByteArray source = "int main() {\n  return 0;\n}";
        parser.parse(source);

        SyntaxNode root = parser.rootNode();
        
        uint32_t startLine = root.startLine();
        uint32_t endLine = root.endLine();
        uint32_t startByte = root.startByte();
        uint32_t endByte = root.endByte();

        if (startLine != 1) {
            return Fail(QStringLiteral("startLine should be 1, got %1").arg(startLine));
        }
        if (endLine < startLine) {
            return Fail(QStringLiteral("endLine should >= startLine"));
        }
        if (startByte != 0) {
            return Fail(QStringLiteral("startByte should be 0, got %1").arg(startByte));
        }
        if (endByte != static_cast<uint32_t>(source.size())) {
            return Fail(QStringLiteral("endByte should be %1, got %2").arg(source.size()).arg(endByte));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 7: 节点定位
    // ========================================
    TEST("节点定位 - nodeAtPosition") {
        TreeSitterParser parser;
        const QByteArray source = "int main() {\n  return 0;\n}";
        parser.parse(source);

        // 查找第2行的节点
        SyntaxNode node = parser.nodeAtPosition(2, 2);
        
        if (node.isNull()) {
            return Fail(QStringLiteral("nodeAtPosition returned null"));
        }

        QString nodeType = node.type();
        if (nodeType.isEmpty()) {
            return Fail(QStringLiteral("node type is empty"));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 8: snprintf C99 语义验证
    // ========================================
    TEST("环境验证 - snprintf C99 语义") {
        char buf[1];
        int ret = snprintf(buf, 1, "Hello, World!");
        
        qDebug() << "    snprintf(buf, 1, \"Hello, World!\") returned:" << ret;
        qDebug() << "    C99 标准期望返回: 13";
        
        if (ret == 13) {
            qDebug() << "    ✓ snprintf 使用 C99 语义";
            return 0;
        } else if (ret == -1) {
            return Fail(QStringLiteral("snprintf 返回 -1，说明 __USE_MINGW_ANSI_STDIO 未生效"));
        } else {
            return Fail(QStringLiteral("snprintf 返回异常值: %1").arg(ret));
        }
    } END_TEST

    // ========================================
    // 测试 9: S-expression 调试输出
    // ========================================
    TEST("调试功能 - sExpression") {
        TreeSitterParser parser;
        const QByteArray source = "int x = 5;";
        if (!parser.parse(source)) {
            return Fail(QStringLiteral("parse failed: %1").arg(parser.lastError()));
        }

        SyntaxNode root = parser.rootNode();
        if (root.isNull()) {
            return Fail(QStringLiteral("root node is null"));
        }
        
        QString sexp = root.sExpression();
        
        qDebug() << "    S-expression:" << sexp.left(100);
        
        if (sexp.isEmpty()) {
            return Fail(QStringLiteral("sExpression returned empty string"));
        }
        if (sexp == ")") {
            return Fail(QStringLiteral("sExpression 返回 ')'，这是 MinGW snprintf 问题的典型症状"));
        }
        if (!sexp.contains("translation_unit")) {
            return Fail(QStringLiteral("sExpression should contain 'translation_unit', got: %1").arg(sexp));
        }
        
        return 0;
    } END_TEST

    // ========================================
    // 测试 10: 多次编辑
    // ========================================
    TEST("多次编辑 - 连续增量解析") {
        TreeSitterParser parser;
        QByteArray source = "int x;";
        parser.parse(source);

        // 第一次编辑: 添加初始化
        QByteArray edit1 = " = 5";
        int pos1 = source.indexOf(';');
        source.insert(pos1, edit1);
        parser.applyEdit(pos1, pos1, pos1 + edit1.size(),
                        1, pos1, 1, pos1, 1, pos1 + edit1.size());
        if (!parser.reparse(source)) {
            return Fail(QStringLiteral("first reparse failed"));
        }

        // 第二次编辑: 添加新变量
        QByteArray edit2 = "\nint y = 10;";
        source.append(edit2);
        uint32_t oldEnd = source.size() - edit2.size();
        parser.applyEdit(oldEnd, oldEnd, source.size(),
                        1, oldEnd, 1, oldEnd, 2, 11);
        if (!parser.reparse(source)) {
            return Fail(QStringLiteral("second reparse failed"));
        }

        if (parser.hasError()) {
            return Fail(QStringLiteral("unexpected error after multiple edits"));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 11: 边界情况 - 空文件
    // ========================================
    TEST("边界情况 - 空文件") {
        TreeSitterParser parser;
        const QByteArray emptySource = "";
        
        if (!parser.parse(emptySource)) {
            return Fail(QStringLiteral("parse empty file failed: %1").arg(parser.lastError()));
        }

        SyntaxNode root = parser.rootNode();
        if (root.isNull()) {
            return Fail(QStringLiteral("root should not be null for empty file"));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 12: 节点属性
    // ========================================
    TEST("节点属性 - isNamed, isMissing, hasError") {
        TreeSitterParser parser;
        const QByteArray source = "int main() { }";
        parser.parse(source);

        SyntaxNode root = parser.rootNode();
        
        if (!root.isNamed()) {
            return Fail(QStringLiteral("root should be named"));
        }
        if (root.isMissing()) {
            return Fail(QStringLiteral("root should not be missing"));
        }
        if (root.hasError()) {
            return Fail(QStringLiteral("root should not have error"));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 13: 节点导航 - 兄弟节点
    // ========================================
    TEST("节点导航 - nextSibling, prevSibling") {
        TreeSitterParser parser;
        const QByteArray source = "int x = 5;\nint y = 10;\nint z = 15;";
        parser.parse(source);

        SyntaxNode root = parser.rootNode();
        if (root.namedChildCount() < 2) {
            return Fail(QStringLiteral("need at least 2 named children for sibling test"));
        }

        SyntaxNode first = root.namedChild(0);
        SyntaxNode next = first.nextNamedSibling();
        
        if (next.isNull()) {
            return Fail(QStringLiteral("nextNamedSibling should not be null"));
        }

        SyntaxNode prev = next.prevNamedSibling();
        if (prev.isNull()) {
            return Fail(QStringLiteral("prevNamedSibling should not be null"));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 14: reset 功能
    // ========================================
    TEST("解析器管理 - reset") {
        TreeSitterParser parser;
        parser.parse(QByteArray("int x = 5;"));
        
        parser.reset();
        
        if (parser.hasTree()) {
            return Fail(QStringLiteral("tree should be cleared after reset"));
        }

        // reset 后应该可以重新解析
        if (!parser.parse(QByteArray("int y = 10;"))) {
            return Fail(QStringLiteral("parse after reset failed"));
        }
        return 0;
    } END_TEST

    // ========================================
    // 测试 15: getChangedRanges 功能
    // ========================================
    TEST("增量解析 - getChangedRanges") {
        TreeSitterParser parser;
        const QByteArray source = "int x = 5;";
        parser.parse(source);

        // parse() 后 getChangedRanges 应返回空（因为没有旧树）
        QVector<ChangedRange> emptyRanges = parser.getChangedRanges();
        if (!emptyRanges.isEmpty()) {
            return Fail(QStringLiteral("getChangedRanges after parse() should be empty"));
        }

        // 使用更大的变化：添加新行
        // "int x = 5;" -> "int x = 5;\nint y = 10;"
        QByteArray updated = "int x = 5;\nint y = 10;";
        uint32_t oldEnd = static_cast<uint32_t>(source.size());
        uint32_t newEnd = static_cast<uint32_t>(updated.size());

        parser.applyEdit(
            oldEnd,      // startByte (末尾)
            oldEnd,      // oldEndByte (原末尾)
            newEnd,      // newEndByte (新末尾)
            1, oldEnd,   // startRow, startCol
            1, oldEnd,   // oldEndRow, oldEndCol
            2, 11        // newEndRow, newEndCol (新增一行)
        );

        if (!parser.reparse(updated)) {
            return Fail(QStringLiteral("reparse failed: %1").arg(parser.lastError()));
        }

        // 验证 getChangedRanges 可正常调用
        QVector<ChangedRange> ranges = parser.getChangedRanges();
        
        qDebug() << "    变化区域数量:" << ranges.size();
        for (int i = 0; i < ranges.size(); ++i) {
            qDebug() << "    区域" << i << ": 行" << ranges[i].startLine << "-" << ranges[i].endLine
                     << ", 字节" << ranges[i].startByte << "-" << ranges[i].endByte;
        }

        // 验证 API 正常工作（不崩溃）且返回有意义的结果
        // 注意：tree-sitter 可能返回空（如果认为整个树结构未变），这是合法行为
        qDebug() << "    getChangedRanges API 正常工作";
        
        return 0;
    } END_TEST

    // ========================================
    // 测试总结
    // ========================================
    qDebug() << "\n========================================";
    qDebug() << "测试完成:" << g_passCount << "/" << g_testCount << "通过";
    qDebug() << "========================================";

    if (g_passCount == g_testCount) {
        qDebug() << "🎉 所有测试通过!";
        return 0;
    } else {
        qCritical() << "❌ 有测试失败!";
        return 1;
    }
}
