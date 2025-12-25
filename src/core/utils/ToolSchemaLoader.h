#ifndef TOOLSCHEMALOADER_H
#define TOOLSCHEMALOADER_H

#include <QString>
#include <QVector>
#include <QMap>
#include "core/agent/ToolTypes.h"

/**
 * @brief 工具 Schema 加载器
 * 
 * 从 YAML 文件加载工具定义，将其转换为 Tool 对象。
 * 支持运行时加载，无需重新编译即可更新工具定义。
 */
class ToolSchemaLoader {
public:
    /**
     * @brief 从 YAML 文件加载所有工具定义
     * @param yamlPath YAML 文件的绝对路径
     * @return 加载的工具列表
     */
    static QVector<Tool> loadFromFile(const QString& yamlPath);
    
    /**
     * @brief 获取指定名称的工具 Schema
     * @param name 工具名称
     * @return 工具定义（如果未找到返回空 Tool）
     */
    static Tool getToolSchema(const QString& name);
    
    /**
     * @brief 获取所有已加载的工具
     * @return 工具列表
     */
    static QVector<Tool> getAllTools();
    
    /**
     * @brief 刷新工具定义（重新从文件加载）
     * @param yamlPath YAML 文件路径
     */
    static void reload(const QString& yamlPath);

private:
    // 缓存已加载的工具
    static QMap<QString, Tool> s_toolCache;
    static QString s_lastLoadedPath;
};

#endif // TOOLSCHEMALOADER_H
