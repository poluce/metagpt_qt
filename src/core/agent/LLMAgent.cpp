#include "LLMAgent.h"
#include "core/utils/ConfigManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

LLMAgent::LLMAgent(QObject *parent) : QObject(parent) {
    m_manager = new QNetworkAccessManager(this);
}

void LLMAgent::setSystemPrompt(const QString& prompt) {
    m_systemPrompt = prompt;
}

void LLMAgent::ask(const QString& prompt) {
    if (m_currentReply) {
        abort();
    }

    m_fullContent.clear();
    
    QString apiKey = ConfigManager::getApiKey();
    QString baseUrl = ConfigManager::getBaseUrl();
    QString model = ConfigManager::getModel();

    if (apiKey.isEmpty()) {
        emit errorOccurred("API Key is empty! Please configure it first.");
        return;
    }

    // 将用户消息添加到对话历史
    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = prompt;
    m_conversationHistory.append(userMsg);

    QUrl url(baseUrl + "/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(apiKey).toUtf8());

    // 构造请求 Body
    QJsonObject root;
    root["model"] = model;
    root["stream"] = true; // 强制开启流式

    QJsonArray messages;
    
    // 注入角色设定 (System Prompt)
    if (!m_systemPrompt.isEmpty()) {
        QJsonObject sysObj;
        sysObj["role"] = "system";
        sysObj["content"] = m_systemPrompt;
        messages.append(sysObj);
    }

    // 添加所有历史对话
    for (const QJsonValue& msg : m_conversationHistory) {
        messages.append(msg);
    }
    
    root["messages"] = messages;

    m_currentReply = m_manager->post(request, QJsonDocument(root).toJson());

    connect(m_currentReply, &QNetworkReply::readyRead, this, &LLMAgent::onReadyRead);
    connect(m_currentReply, &QNetworkReply::finished, this, &LLMAgent::onFinished);
    connect(m_currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &LLMAgent::onError);
}

void LLMAgent::abort() {
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}

void LLMAgent::onReadyRead() {
    if (!m_currentReply) return;

    while (m_currentReply->canReadLine()) {
        QByteArray line = m_currentReply->readLine().trimmed();
        if (line.isEmpty()) continue;

        if (line.startsWith("data: ")) {
            QString data = QString::fromUtf8(line.mid(6));
            if (data == "[DONE]") {
                return;
            }

            QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
            if (!doc.isNull()) {
                QJsonObject obj = doc.object();
                QJsonArray choices = obj["choices"].toArray();
                if (!choices.isEmpty()) {
                    QJsonObject delta = choices[0].toObject()["delta"].toObject();
                    if (delta.contains("content")) {
                        QString content = delta["content"].toString();
                        m_fullContent += content;
                        emit chunkReceived(content);
                    }
                }
            }
        }
    }
}

void LLMAgent::onFinished() {
    if (m_currentReply) {
        if (m_currentReply->error() == QNetworkReply::NoError) {
            // 将助手回复添加到对话历史
            QJsonObject assistantMsg;
            assistantMsg["role"] = "assistant";
            assistantMsg["content"] = m_fullContent;
            m_conversationHistory.append(assistantMsg);
            
            emit finished(m_fullContent);
        }
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}

void LLMAgent::onError(QNetworkReply::NetworkError code) {
    if (code != QNetworkReply::OperationCanceledError) {
        emit errorOccurred(m_currentReply->errorString());
    }
}

void LLMAgent::clearHistory() {
    m_conversationHistory = QJsonArray();
}

QJsonArray LLMAgent::getHistory() const {
    return m_conversationHistory;
}

int LLMAgent::getConversationCount() const {
    int count = 0;
    for (const QJsonValue& msg : m_conversationHistory) {
        if (msg.toObject()["role"].toString() == "user") {
            count++;
        }
    }
    return count;
}
