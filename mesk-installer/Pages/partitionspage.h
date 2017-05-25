#ifndef PARTITIONSPAGE_H
#define PARTITIONSPAGE_H

#include "drive.h"
#include <QWidget>
#include "ui_diskPage.h"

#include <QTableWidgetItem>

class paritionSizeDelegate : public QAbstractItemDelegate
{
    public:
	paritionSizeDelegate( QObject *parent = 0 );
	const void paint(   QPainter *painter, const QStyleOptionViewItem &option,
			    const QModelIndex &index );
	const QSize sizeHint(	const QStyleOptionViewItem &option,
				const QModelIndex &index );
};

class partitionView
{
    private:
	QTableWidgetItem* pNumber;
	QTableWidgetItem* pLabel;
	QTableWidgetItem* pFS;
	QTableWidgetItem* pSize;
	QTableWidgetItem* pMountPoint;
    public:
	partitionView();
	int setNumber(QString);
	int setLabel(QString);
	int setFS(QString);
	int setSize(QString);
	int setMountPoint(QString);
	int setFormat(QString);
};


class partitionspage : public QWidget,  private Ui::diskPage
{
    Q_OBJECT

public:
    explicit partitionspage(QWidget *parent = 0);
    ~partitionspage();
    QList<Drive> getDisks();
    QList<QList<partitionView> > getPartitionViews();

private:
    int initdList();
    int initPartitionViews();
    QList<Drive> disks;
    QList<QList<partitionView> > partitionsList;
private slots:
    int refreshPartitionsViews(int);
};

#endif // PARTITIONSPAGE_H
