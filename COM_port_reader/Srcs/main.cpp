#include "mainwindow.h"
#include <xlsxdocument.h>

void    readMetaData(QHash<QString, QString> &metadata);

int main(int argc, char *argv[])
{
    QHash<QString, QString> metadata;
    readMetaData(metadata);

    QHashIterator<QString, QString> it(metadata);
    while (it.hasNext())
    {
        it.next();
        qDebug() << it.key() << ":" << it.value();
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

void    readMetaData(QHash<QString, QString> &metadata)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/oqni/Recordings/metadata.xlsx";
    QXlsx::Document xlsx(path);
    xlsx.selectSheet("subjects");
    for(int row = 2; row <= xlsx.dimension().lastRow(); ++row)
        metadata.insert(xlsx.read(row, 1).toString(), xlsx.read(row, 2).toString());
}

