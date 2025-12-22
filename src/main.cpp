#include <QApplication>
#include "ui/LLMConfigWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    LLMConfigWidget w;
    w.show();
    
    return a.exec();
}
