#include "mainwindow.h"

#include <QDebug>
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
    xlsx.selectSheet("DB");
    for(int row = 2; row <= xlsx.dimension().lastRow(); ++row)
//        metadata.insert(xlsx.read(row, 1).toString(), xlsx.read(row, 2).toString());
        qDebug() << xlsx.read(row, 1).toString() << " " <<  xlsx.read(row, 2).toString() << " " << xlsx.read(row, 3).toString() << " " << xlsx.read(row, 4).toString() << " " << xlsx.read(row, 5).toString() << " " << xlsx.read(row, 6).toString() << " " << xlsx.read(row, 7).toString() << " " << xlsx.read(row, 8).toString();
}

