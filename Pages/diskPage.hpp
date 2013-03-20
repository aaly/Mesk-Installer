/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef DISKPAGE_H
#define DISKPAGE_H

#include <MPF/System/drive.hpp>
#include <MPF/Pages/pageBase.hpp>
#include <QTableView>
#include "ui_diskPage.h"
#include <MPF/Delegates/partitionSizeDelegate.hpp>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <MPF/Delegates/comboBoxListDelegate.hpp>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QVector>


class mountPoint
{
public:
    QString partition;
    QString path;
    QString fs;
    QString opts;
};

enum PAction
{
    PFORMAT=0,
    PLABEL,
    PMOUNT
};

class partitionAction
{
    public:
        Partition      partition;
        int             Action;
        QString         Value;

};

class diskPage : public pageBase, private Ui::diskPage
{
    Q_OBJECT

public:
    explicit diskPage(QWidget *parent = 0);
    ~diskPage();

    QList<Drive>        getDisks();
    int                 setDisks(QList<Drive>);
    QString             rootPartition;
    QVector<mountPoint> installationMountPoints;
    QVector<mountPoint> getInstallationMountPoints() const;

    int     initAll();
private:
    enum Partitioncolumn
    {
        LABEL,
        PARTITION,
        MOUNTPOINT,
        FILESYSTEM,
        SIZE
    };

    ComboBoxListDelegate*	    mountPointDelegate;
    QList<Drive> disks;
    QVector<QStandardItemModel*> partitionsTreeModels;
    int                 currentHard;

    QString                               parseCommand(QString, QString, QString, QString);
    int                                   initFormatTools();
    int                                   initLabelTools();
    QString                               formatToolsFilePath;
    QString                               labelToolsFilePath;
    QMap<QString, QString>                formatTools;
    QMap<QString, QString>                labelTools;

    QVector<partitionAction>              diskActions;

    QProcess                              actionProcess;

    int                                   formatPartition(Partition, QString);
    int                                   labelPartition(Partition, QString);

    QString                               getText(int);
    Partition                             getCurrentPartition();

    QVector<QString>                        mountPoints;
    int                                   hasAction(Partition&, int);
     QVector< QPair<QString, QString> > availableFileSystems;

     void changeEvent(QEvent* event);

private slots:
    int refreshPartitionViews(int);
    int formatSelected(bool);
    int enableOperations(QModelIndex);
    int changeLabel(QString);
    int changeMountPoint(int);
    int changeFilesystem();
    int addAction();
    int removeAction();
    int applyActions();
    int reset();
};

#endif // DISKPAGE_H
