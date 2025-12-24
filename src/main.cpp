#include <QApplication>
#include "ui/AgentChatWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    AgentChatWidget w;
    w.show();
    
    return a.exec();
}
