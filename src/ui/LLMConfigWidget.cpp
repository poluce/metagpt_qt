#include "LLMConfigWidget.h"
#include "core/utils/ConfigManager.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGroupBox>
#include <QSplitter>
#include <QTextCursor>
#include <QTextDocument>

LLMConfigWidget::LLMConfigWidget(QWidget *parent) : QWidget(parent) {
    m_agent = new LLMAgent(this);
    
    setupUI();
    loadConfig();

    connect(m_agent, &LLMAgent::chunkReceived, this, &LLMConfigWidget::onChunkReceived);
    connect(m_agent, &LLMAgent::finished, this, &LLMConfigWidget::onFinished);
    connect(m_agent, &LLMAgent::errorOccurred, this, &LLMConfigWidget::onError);
}

void LLMConfigWidget::setupUI() {
    setWindowTitle("DeepSeek LLM 配置与验证");
    resize(1200, 600);  // 扩大窗口宽度以容纳三列

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // --- 左侧：配置面板 ---
    QWidget *leftContainer = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0); // 消除内边距

    QGroupBox *configGroup = new QGroupBox("LLM 配置", this);
    QFormLayout *formLayout = new QFormLayout(configGroup);

    m_baseUrlEdit = new QLineEdit(this);
    m_apiKeyEdit = new QLineEdit(this);
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    m_modelEdit = new QLineEdit(this);
    m_systemPromptEdit = new QTextEdit(this);
    m_systemPromptEdit->setPlaceholderText("请输入智能体的人格设定...");
    m_systemPromptEdit->setMinimumHeight(150);

    formLayout->addRow("Base URL:", m_baseUrlEdit);
    formLayout->addRow("API Key:", m_apiKeyEdit);
    formLayout->addRow("Model:", m_modelEdit);
    formLayout->addRow("Agent Role:", m_systemPromptEdit);

    m_saveBtn = new QPushButton("保存配置 (Save)", this);
    connect(m_saveBtn, &QPushButton::clicked, this, &LLMConfigWidget::onSaveClicked);
    formLayout->addRow(m_saveBtn);

    leftLayout->addWidget(configGroup);
    leftLayout->addStretch();
    
    splitter->addWidget(leftContainer);

    // --- 右侧：交流面板 ---
    QWidget *centerContainer = new QWidget(this);
    QVBoxLayout *centerLayout = new QVBoxLayout(centerContainer);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    
    m_chatDisplay = new QTextBrowser(this);
    m_chatDisplay->setPlaceholderText("交流内容显示区...");
    centerLayout->addWidget(m_chatDisplay, 1);

    // 输入区
    QHBoxLayout *inputLayout = new QHBoxLayout();
    m_inputEdit = new QTextEdit(this);
    m_inputEdit->setMaximumHeight(100);
    m_inputEdit->setPlaceholderText("在此输入问题，按“发送”开始交流...");
    
    QVBoxLayout *btnLayout = new QVBoxLayout();
    m_sendBtn = new QPushButton("发送 (Send)", this);
    m_abortBtn = new QPushButton("停止 (Abort)", this);
    m_abortBtn->setEnabled(false);
    
    btnLayout->addWidget(m_sendBtn);
    btnLayout->addWidget(m_abortBtn);
    
    inputLayout->addWidget(m_inputEdit);
    inputLayout->addLayout(btnLayout);
    
    centerLayout->addLayout(inputLayout);
    
    splitter->addWidget(centerContainer);

    // --- 右侧:对话历史面板 ---
    QWidget *historyContainer = new QWidget(this);
    QVBoxLayout *historyLayout = new QVBoxLayout(historyContainer);
    historyLayout->setContentsMargins(0, 0, 0, 0);
    
    m_historyLabel = new QLabel("对话历史 (共 0 轮)", this);
    QFont labelFont = m_historyLabel->font();
    labelFont.setBold(true);
    m_historyLabel->setFont(labelFont);
    historyLayout->addWidget(m_historyLabel);
    
    m_historyDisplay = new QTextBrowser(this);
    m_historyDisplay->setPlaceholderText("对话历史将在此显示...");
    historyLayout->addWidget(m_historyDisplay, 1);
    
    m_clearHistoryBtn = new QPushButton("清空历史", this);
    historyLayout->addWidget(m_clearHistoryBtn);
    
    splitter->addWidget(historyContainer);

    // 设置初始比例：左侧 300px，右侧自适应
    splitter->setStretchFactor(0, 0); // 左侧不拉伸
    splitter->setStretchFactor(1, 1); // 右侧拉伸
    splitter->setSizes(QList<int>() << 320 << 580);

    mainLayout->addWidget(splitter);

    connect(m_sendBtn, &QPushButton::clicked, this, &LLMConfigWidget::onSendClicked);
    connect(m_abortBtn, &QPushButton::clicked, this, &LLMConfigWidget::onAbortClicked);
    connect(m_clearHistoryBtn, &QPushButton::clicked, this, &LLMConfigWidget::onClearHistoryClicked);
}

void LLMConfigWidget::loadConfig() {
    m_baseUrlEdit->setText(ConfigManager::getBaseUrl());
    m_apiKeyEdit->setText(ConfigManager::getApiKey());
    m_modelEdit->setText(ConfigManager::getModel());
    m_systemPromptEdit->setPlainText(ConfigManager::getSystemPrompt());
}

void LLMConfigWidget::onSaveClicked() {
    ConfigManager::setBaseUrl(m_baseUrlEdit->text().trimmed());
    ConfigManager::setApiKey(m_apiKeyEdit->text().trimmed());
    ConfigManager::setModel(m_modelEdit->text().trimmed());
    ConfigManager::setSystemPrompt(m_systemPromptEdit->toPlainText().trimmed());
    QMessageBox::information(this, "成功", "配置已成功保存至 config.ini");
}

void LLMConfigWidget::onSendClicked() {
    QString prompt = m_inputEdit->toPlainText().trimmed();
    if (prompt.isEmpty()) return;

    // 更新 Agent 的角色设定(不保存到配置文件)
    m_agent->setSystemPrompt(m_systemPromptEdit->toPlainText().trimmed());

    // 清空累积内容
    m_currentAssistantReply.clear();

    // 显示用户消息
    m_chatDisplay->append("<br>");
    m_chatDisplay->append("<b style='color: #2196F3;'>User:</b>");
    m_chatDisplay->append("<p>" + prompt.toHtmlEscaped() + "</p>");
    m_chatDisplay->append("<b style='color: #4CAF50;'>Assistant:</b>");
    
    m_sendBtn->setEnabled(false);
    m_abortBtn->setEnabled(true);
    
    m_agent->ask(prompt);
}

void LLMConfigWidget::onAbortClicked() {
    m_agent->abort();
    m_chatDisplay->append("<br><i>[已中断]</i>");
    onFinished("");
}

void LLMConfigWidget::onChunkReceived(const QString& chunk) {
    // 累积文本片段
    m_currentAssistantReply += chunk;
    
    // 实时显示纯文本(流式效果)
    m_chatDisplay->insertPlainText(chunk);
    m_chatDisplay->ensureCursorVisible();
}

void LLMConfigWidget::onFinished(const QString& fullContent) {
    Q_UNUSED(fullContent);
    
    // 将累积的纯文本替换为 Markdown 渲染
    if (!m_currentAssistantReply.isEmpty()) {
        QTextCursor cursor = m_chatDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        
        // 向前删除刚才插入的纯文本
        for (int i = 0; i < m_currentAssistantReply.length(); i++) {
            cursor.deletePreviousChar();
        }
        
        // 使用 QTextDocument 渲染 Markdown
        QTextDocument doc;
        doc.setMarkdown(m_currentAssistantReply);
        
        // 插入渲染后的 HTML
        cursor.insertHtml(doc.toHtml());
        m_chatDisplay->setTextCursor(cursor);
    }
    
    m_sendBtn->setEnabled(true);
    m_abortBtn->setEnabled(false);
    m_inputEdit->clear();
    
    // 更新历史显示
    updateHistoryDisplay();
}

void LLMConfigWidget::onError(const QString& errorMsg) {
    QMessageBox::critical(this, "API 错误", errorMsg);
    onFinished("");
}

void LLMConfigWidget::updateHistoryDisplay() {
    QJsonArray history = m_agent->getHistory();
    int count = m_agent->getConversationCount();
    
    m_historyLabel->setText(QString("对话历史 (共 %1 轮)").arg(count));
    
    if (history.isEmpty()) {
        m_historyDisplay->clear();
        return;
    }
    
    QString htmlContent;
    int roundNum = 0;
    
    for (int i = 0; i < history.size(); i++) {
        QJsonObject msg = history[i].toObject();
        QString role = msg["role"].toString();
        QString content = msg["content"].toString();
        
        if (role == "user") {
            roundNum++;
            htmlContent += QString("<p><b>第 %1 轮:</b></p>").arg(roundNum);
            htmlContent += QString("<p style='color: blue;'><b>User:</b> %1</p>").arg(content.toHtmlEscaped());
        } else if (role == "assistant") {
            htmlContent += QString("<p style='color: green;'><b>Assistant:</b> %1</p><br>").arg(content.toHtmlEscaped());
        }
    }
    
    m_historyDisplay->setHtml(htmlContent);
}

void LLMConfigWidget::onClearHistoryClicked() {
    m_agent->clearHistory();
    m_historyDisplay->clear();
    m_historyLabel->setText("对话历史 (共 0 轮)");
    m_chatDisplay->append("<br><i>[对话历史已清空]</i>");
}
