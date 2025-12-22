#include "ConfigManager.h"

void ConfigManager::setApiKey(const QString& key) {
    settings().setValue("llm/api_key", key);
    settings().sync();
}

QString ConfigManager::getApiKey() {
    return settings().value("llm/api_key", "").toString();
}

void ConfigManager::setBaseUrl(const QString& url) {
    settings().setValue("llm/base_url", url);
    settings().sync();
}

QString ConfigManager::getBaseUrl() {
    // 默认预设 DeepSeek 官方地址
    return settings().value("llm/base_url", "https://api.deepseek.com").toString();
}

void ConfigManager::setModel(const QString& model) {
    settings().setValue("llm/model", model);
    settings().sync();
}

QString ConfigManager::getModel() {
    // 默认预设 DeepSeek Chat 模型
    return settings().value("llm/model", "deepseek-chat").toString();
}

void ConfigManager::setSystemPrompt(const QString& prompt) {
    settings().setValue("llm/system_prompt", prompt);
    settings().sync();
}

QString ConfigManager::getSystemPrompt() {
    return settings().value("llm/system_prompt", "你是一个专业的 Qt 高级开发工程师，旨在帮助用户解决 C++/Qt 相关的编程问题。").toString();
}

void ConfigManager::setTemperature(double temp) {
    settings().setValue("llm/temperature", temp);
    settings().sync();
}

double ConfigManager::getTemperature() {
    return settings().value("llm/temperature", 0.7).toDouble();
}
