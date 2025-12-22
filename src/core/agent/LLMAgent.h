#ifndef LLMAGENT_H
#define LLMAGENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

class LLMAgent : public QObject {
    Q_OBJECT
public:
    explicit LLMAgent(QObject *parent = nullptr);
    
    // 发送消息，支持多轮对话上下文（后续可加）
    void ask(const QString& prompt);
    
    // 设置 Agent 的角色 (System Prompt)
    void setSystemPrompt(const QString& prompt);
    QString systemPrompt() const { return m_systemPrompt; }

    // 对话历史管理
    void clearHistory();                    // 清空对话历史
    QJsonArray getHistory() const;          // 获取对话历史
    int getConversationCount() const;       // 获取对话轮数

    // 中断请求
    void abort();

signals:
    void chunkReceived(const QString& chunk);    // 收到文本片段
    void finished(const QString& fullContent);   // 请求圆满结束
    void errorOccurred(const QString& errorMsg); // 发生错误

private slots:
    void onReadyRead();
    void onFinished();
    void onError(QNetworkReply::NetworkError code);

private:
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_currentReply = nullptr;
    QString m_fullContent;
    QString m_systemPrompt;
    QJsonArray m_conversationHistory;  // 对话历史
};

#endif // LLMAGENT_H
