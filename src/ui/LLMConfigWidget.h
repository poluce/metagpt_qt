#ifndef LLMCONFIGWIDGET_H
#define LLMCONFIGWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include "core/agent/LLMAgent.h"

class LLMConfigWidget : public QWidget {
    Q_OBJECT
public:
    explicit LLMConfigWidget(QWidget *parent = nullptr);

private slots:
    void onSaveClicked();
    void onSendClicked();
    void onAbortClicked();
    void onClearHistoryClicked();           // 清空对话历史
    void onChunkReceived(const QString& chunk);
    void onFinished(const QString& fullContent);
    void onError(const QString& errorMsg);
    void updateHistoryDisplay();            // 更新历史显示

private:
    void setupUI();
    void loadConfig();

    // UI Widgets
    QLineEdit *m_baseUrlEdit;
    QLineEdit *m_apiKeyEdit;
    QLineEdit *m_modelEdit;
    QTextEdit *m_systemPromptEdit;
    
    QTextBrowser *m_chatDisplay;
    QTextEdit *m_inputEdit;
    
    QPushButton *m_saveBtn;
    QPushButton *m_sendBtn;
    QPushButton *m_abortBtn;
    
    // 对话历史显示
    QTextBrowser *m_historyDisplay;
    QPushButton *m_clearHistoryBtn;
    QLabel *m_historyLabel;

    LLMAgent *m_agent;
    QString m_currentAssistantReply;  // 当前助手回复的累积内容
};

#endif // LLMCONFIGWIDGET_H
