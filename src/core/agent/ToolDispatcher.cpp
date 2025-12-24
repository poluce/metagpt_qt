#include "ToolDispatcher.h"
#include "core/tools/FileTool.h"
#include "core/tools/ShellTool.h"
#include <QDebug>

ToolDispatcher::ToolDispatcher(QObject *parent) : QObject(parent) {
}

void ToolDispatcher::registerTool(const Tool& schema, 
                                   const QString& description,
                                   std::function<QString(const QJsonObject&)> executor) {
    ToolEntry entry;
    entry.schema = schema;
    entry.description = description;
    entry.execute = executor;
    
    m_registry[schema.name] = entry;
    qDebug() << "[ToolDispatcher] 注册工具:" << schema.name << "-" << description;
}

void ToolDispatcher::registerDefaultTools() {
    // FileTool
    registerTool(FileTool::getCreateFileSchema(), "创建文件", FileTool::executeCreateFile);
    registerTool(FileTool::getViewFileSchema(), "读取文件", FileTool::executeViewFile);
    registerTool(FileTool::getReadFileLinesSchema(), "读取文件行", FileTool::executeReadFileLines);
    
    // ShellTool
    registerTool(ShellTool::getExecuteCommandSchema(), "执行命令", ShellTool::execute);
}

QList<Tool> ToolDispatcher::getAllToolSchemas() const {
    QList<Tool> schemas;
    for (const ToolEntry& entry : m_registry) {
        schemas.append(entry.schema);
    }
    return schemas;
}

QString ToolDispatcher::dispatch(const ToolCall& call) {
    const QString& toolName = call.name;
    const QJsonObject& input = call.input;
    QString inputStr = QString::fromUtf8(QJsonDocument(input).toJson(QJsonDocument::Compact));
    
    qDebug() << "[ToolDispatcher] 分发工具调用:" << toolName;
    
    if (m_registry.contains(toolName)) {
        const ToolEntry& entry = m_registry[toolName];
        emit toolStarted(entry.description, inputStr);
        return entry.execute(input);
    }
    
    return QString("错误: 未知的工具 %1").arg(toolName);
}
