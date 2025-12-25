#include "ToolSchemaLoader.h"
#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <yaml-cpp/yaml.h>

// 静态成员初始化
QMap<QString, Tool> ToolSchemaLoader::s_toolCache;
QString ToolSchemaLoader::s_lastLoadedPath;

QVector<Tool> ToolSchemaLoader::loadFromFile(const QString& yamlPath) {
    QVector<Tool> tools;
    
    // 检查文件是否存在
    QFile file(yamlPath);
    if (!file.exists()) {
        qWarning() << "[ToolSchemaLoader] YAML 文件不存在:" << yamlPath;
        return tools;
    }
    
    try {
        // 使用 yaml-cpp 加载文件
        YAML::Node root = YAML::LoadFile(yamlPath.toStdString());
        
        if (!root["tools"]) {
            qWarning() << "[ToolSchemaLoader] YAML 文件缺少 'tools' 节点";
            return tools;
        }
        
        YAML::Node toolsNode = root["tools"];
        
        for (const auto& toolNode : toolsNode) {
            Tool tool;
            
            // 解析基本字段
            tool.name = QString::fromStdString(toolNode["name"].as<std::string>());
            tool.description = QString::fromStdString(toolNode["description"].as<std::string>());
            
            // 构建 inputSchema (JSON Schema 格式)
            QJsonObject properties;
            QJsonArray required;
            
            if (toolNode["parameters"]) {
                for (const auto& paramNode : toolNode["parameters"]) {
                    QString paramName = QString::fromStdString(paramNode["name"].as<std::string>());
                    QString paramType = QString::fromStdString(paramNode["type"].as<std::string>());
                    QString paramDesc = QString::fromStdString(paramNode["description"].as<std::string>(""));
                    bool isRequired = paramNode["required"].as<bool>(false);
                    
                    QJsonObject paramSchema;
                    paramSchema["type"] = paramType;
                    if (!paramDesc.isEmpty()) {
                        paramSchema["description"] = paramDesc;
                    }
                    
                    // 处理数组类型的 items 定义
                    if (paramType == "array" && paramNode["items"]) {
                        YAML::Node itemsNode = paramNode["items"];
                        QJsonObject itemsSchema;
                        itemsSchema["type"] = QString::fromStdString(itemsNode["type"].as<std::string>("object"));
                        
                        // 如果有嵌套 properties
                        if (itemsNode["properties"]) {
                            QJsonObject nestedProps;
                            for (const auto& nestedProp : itemsNode["properties"]) {
                                QString nestedName = QString::fromStdString(nestedProp["name"].as<std::string>());
                                QJsonObject nestedSchema;
                                nestedSchema["type"] = QString::fromStdString(nestedProp["type"].as<std::string>("string"));
                                if (nestedProp["description"]) {
                                    nestedSchema["description"] = QString::fromStdString(nestedProp["description"].as<std::string>());
                                }
                                nestedProps[nestedName] = nestedSchema;
                            }
                            itemsSchema["properties"] = nestedProps;
                        }
                        paramSchema["items"] = itemsSchema;
                    }
                    
                    properties[paramName] = paramSchema;
                    
                    if (isRequired) {
                        required.append(paramName);
                    }
                }
            }
            
            // 组装完整的 inputSchema
            QJsonObject inputSchema;
            inputSchema["type"] = "object";
            inputSchema["properties"] = properties;
            inputSchema["required"] = required;
            tool.inputSchema = inputSchema;
            
            tools.append(tool);
            s_toolCache[tool.name] = tool;
            
            qDebug() << "[ToolSchemaLoader] 加载工具:" << tool.name;
        }
        
        s_lastLoadedPath = yamlPath;
        qInfo() << "[ToolSchemaLoader] 成功加载" << tools.size() << "个工具定义";
        
    } catch (const YAML::Exception& e) {
        qCritical() << "[ToolSchemaLoader] YAML 解析错误:" << e.what();
    }
    
    return tools;
}

Tool ToolSchemaLoader::getToolSchema(const QString& name) {
    if (s_toolCache.contains(name)) {
        return s_toolCache[name];
    }
    
    qWarning() << "[ToolSchemaLoader] 未找到工具:" << name;
    return Tool();
}

QVector<Tool> ToolSchemaLoader::getAllTools() {
    QVector<Tool> tools;
    for (const Tool& tool : s_toolCache) {
        tools.append(tool);
    }
    return tools;
}

void ToolSchemaLoader::reload(const QString& yamlPath) {
    s_toolCache.clear();
    loadFromFile(yamlPath);
}
