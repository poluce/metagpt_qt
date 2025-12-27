#ifndef TREESITTERPARSER_H
#define TREESITTERPARSER_H

#include <QString>
#include <QByteArray>
#include <QVector>
#include <cstdint>

// 前向声明 tree-sitter 类型（仅在 .cpp 中包含 api.h）
struct TSNode;
struct TSTree;
struct TSParser;
struct TSLanguage;

class TreeSitterParser;

/**
 * @brief 表示语法树变化区域（Qt 友好类型）
 *
 * 用于增量解析后获取发生变化的代码区域。
 * 行号采用 1-based 约定，与 TreeSitterParser 其他接口一致。
 */
struct ChangedRange {
    uint32_t startLine;      ///< 起始行（1-based）
    uint32_t startColumn;    ///< 起始列（UTF-8 字节偏移）
    uint32_t endLine;        ///< 结束行（1-based）
    uint32_t endColumn;      ///< 结束列（UTF-8 字节偏移）
    uint32_t startByte;      ///< 起始字节偏移
    uint32_t endByte;        ///< 结束字节偏移
};

/**
 * @brief 语法节点封装类（Qt 友好类型）
 *
 * 完全封装 tree-sitter 的 TSNode，用户无需接触底层类型。
 * 
 * @warning 生命周期依赖于 TreeSitterParser：当 parser 调用
 *          parse()/reparse()/reset() 后，之前获取的 SyntaxNode 失效。
 */
class SyntaxNode {
public:
    SyntaxNode();  ///< 构造空节点

    // === 基本信息 ===
    
    QString type() const;           ///< 节点类型 (如 "function_definition")
    QString text() const;           ///< 节点源码文本
    bool isNull() const;            ///< 是否为空节点
    bool isNamed() const;           ///< 是否为命名节点
    bool hasError() const;          ///< 节点或子节点是否有语法错误
    bool isMissing() const;         ///< 是否由解析器插入的缺失节点

    // === 位置信息 ===
    
    uint32_t startLine() const;     ///< 起始行（1-based）
    uint32_t endLine() const;       ///< 结束行（1-based）
    uint32_t startColumn() const;   ///< 起始列（UTF-8 字节偏移）
    uint32_t endColumn() const;     ///< 结束列（UTF-8 字节偏移）
    uint32_t startByte() const;     ///< 起始字节偏移
    uint32_t endByte() const;       ///< 结束字节偏移

    // === 节点遍历 ===
    
    uint32_t childCount() const;                        ///< 子节点数量
    SyntaxNode child(uint32_t index) const;             ///< 获取第 i 个子节点
    uint32_t namedChildCount() const;                   ///< 命名子节点数量
    SyntaxNode namedChild(uint32_t index) const;        ///< 获取第 i 个命名子节点
    SyntaxNode childByFieldName(const QString& name) const;  ///< 按字段名获取子节点
    SyntaxNode parent() const;                          ///< 父节点
    SyntaxNode nextSibling() const;                     ///< 下一个兄弟节点
    SyntaxNode prevSibling() const;                     ///< 上一个兄弟节点
    SyntaxNode nextNamedSibling() const;                ///< 下一个命名兄弟节点
    SyntaxNode prevNamedSibling() const;                ///< 上一个命名兄弟节点

    // === 调试 ===
    
    QString sExpression() const;    ///< S-expression 表示

private:
    friend class TreeSitterParser;
    
    // 内部数据（与 TSNode 布局兼容，避免包含 api.h）
    uint32_t m_context[4];
    const void* m_id;
    const void* m_tree;
    
    const TreeSitterParser* m_parser;  ///< 用于获取源码
    
    SyntaxNode(const void* nodeData, const TreeSitterParser* parser);
    
    // 内部辅助：从 TSNode 创建 SyntaxNode（在 cpp 中实现）
    static SyntaxNode fromInternal(const void* nodeData, const TreeSitterParser* parser);
};

/**
 * @brief Qt 风格的 tree-sitter 封装类
 *
 * 提供代码解析、增量更新、节点遍历等功能。
 * 对外完全隐藏 tree-sitter 底层类型。
 *
 * @warning 线程安全：类实例不可跨线程并发使用。
 * @warning 节点生命周期：SyntaxNode 在 parse/reparse/reset 后失效。
 */
class TreeSitterParser {
public:
    TreeSitterParser();
    ~TreeSitterParser();

    // 禁止拷贝
    TreeSitterParser(const TreeSitterParser&) = delete;
    TreeSitterParser& operator=(const TreeSitterParser&) = delete;

    // === 解析器管理 ===

    /**
     * @brief 设置解析超时（微秒）
     * @note 暂不支持：tree-sitter 0.26 无此 API，调用后 lastError 会设置提示
     */
    void setTimeout(uint64_t microseconds);

    /**
     * @brief 重置所有状态
     */
    void reset();

    // === 解析操作 ===

    bool parse(const QString& source);
    bool parse(const QByteArray& utf8Source);

    // === 增量解析 ===

    /**
     * @brief 通知编辑范围
     *
     * @param startRow, oldEndRow, newEndRow 行号（1-based，内部转 0-based）
     * @param startCol, oldEndCol, newEndCol 列号（UTF-8 字节偏移，不转换）
     *
     * @warning 调用方必须确保 startByte/oldEndByte/newEndByte 与 row/col
     *          基于相同的 UTF-8 编码，否则树会损坏。
     * @note 调用后应使用 reparse(newSource) 而非 parse()
     */
    void applyEdit(uint32_t startByte, uint32_t oldEndByte, uint32_t newEndByte,
                   uint32_t startRow, uint32_t startCol,
                   uint32_t oldEndRow, uint32_t oldEndCol,
                   uint32_t newEndRow, uint32_t newEndCol);

    /**
     * @brief 增量解析（用新源码替换）
     * @note 未调用 applyEdit 时退化为全量解析
     */
    bool reparse(const QString& newSource);
    bool reparse(const QByteArray& newUtf8Source);

    // === 语法树 ===

    SyntaxNode rootNode() const;    ///< 获取根节点
    bool hasTree() const;           ///< 是否有已解析的树
    bool hasError() const;          ///< 树中是否有语法错误
    QString lastError() const;      ///< 最近 API 失败原因

    // === 节点定位 ===

    /**
     * @brief 按位置查找节点
     * @param line 行号（1-based）
     * @param column 列号（UTF-8 字节偏移）
     */
    SyntaxNode nodeAtPosition(uint32_t line, uint32_t column) const;

    // === 增量解析分析 ===

    /**
     * @brief 获取上次 reparse 的变化区域
     *
     * 在调用 reparse() 后，此方法返回旧树和新树之间的变化区域。
     * 如果未调用 reparse()（例如仅调用 parse()），则返回空列表。
     *
     * @return 变化区域列表，行号采用 1-based 约定
     */
    QVector<ChangedRange> getChangedRanges() const;

    // === 内部辅助 ===
    
    /**
     * @brief 获取源码（供 SyntaxNode 使用）
     */
    const QByteArray& source() const { return m_source; }

private:
    TSParser* m_parser = nullptr;
    TSTree* m_tree = nullptr;
    TSTree* m_oldTree = nullptr;   ///< 上次 reparse 前的旧树（用于 getChangedRanges）
    QByteArray m_source;
    QString m_lastError;
    bool m_hasParsed = false;
    bool m_hasEdit = false;
};

#endif // TREESITTERPARSER_H
