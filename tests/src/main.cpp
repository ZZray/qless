#include "mainwindow.h"
#include <QApplication>

#include "../../qless/src/parser.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    const QString style = app.applicationDirPath() + "/styles/app.less";
    ray::qless::Parser parser(style);
    parser.parse();
    MainWindow w;
    w.show();
    return app.exec();
}
