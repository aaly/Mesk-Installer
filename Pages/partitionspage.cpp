#include "partitionspage.h"
//#include "ui_partitionspage.h"

partitionspage::partitionspage(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    populatePartedDrives(disks);
    initdList();

    connect(disksList, SIGNAL(currentIndexChanged(int)),
	    this, SLOT(refreshPartitionsViews(int)));
}

int partitionspage::initdList()
{
    Drive d = NULL;
    for ( int i =0; i < disks.length(); i++)
    {
	d = disks.at(i);
	disksList->addItem(d.getModel());

	QList<partitionView> plist;
	partitionView pview;
        for ( int l =0; l < d.getPartitions().count(); l++)
	{
	    Partition partition = d.getPartitions().at(l);

	    //pview = new partitionView();
	    pview.setFS(partition.getFSType());
	    pview.setFormat("NO");
	    pview.setLabel(partition.getLabel());
	    pview.setMountPoint("");
	    pview.setNumber(QString::number(l));
            pview.setSize(partition.getSize());

	    plist.push_back(pview);
	}
	partitionsList.push_back(plist);
    }
    return 0;
}

partitionspage::~partitionspage()
{
}


QList<Drive> partitionspage::getDisks()
{
    return disks;
}


QList<QList<partitionView> > partitionspage::getPartitionViews()
{
    return partitionsList;
}


/*int partitionspage::refreshPartitionViews(int x)
{

    return 0;
}*/
