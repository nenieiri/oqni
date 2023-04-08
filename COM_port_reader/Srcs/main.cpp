#include "mainwindow.h"

#include <xlsxdocument.h>

int main(int argc, char *argv[])
{
    QXlsx::Document xlsx("C:/Users/george/Desktop/metadata.xlsx");
    xlsx.selectSheet("placements");
    qDebug() << xlsx.read("B2").toString();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
