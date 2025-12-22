#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>

class ConfigManager {
public:
    static void setApiKey(const QString& key);
    static QString getApiKey();

    static void setBaseUrl(const QString& url);
    static QString getBaseUrl();

    static void setModel(const QString& model);
    static QString getModel();

    static void setSystemPrompt(const QString& prompt);
    static QString getSystemPrompt();

    static void setTemperature(double temp);
    static double getTemperature();

private:
    static QSettings& settings() {
        static QSettings s(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
        return s;
    }
};

#endif // CONFIGMANAGER_H
