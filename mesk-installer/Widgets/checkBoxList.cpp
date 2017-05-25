#include "Widgets/checkBoxList.hpp"

CheckBoxList::CheckBoxList(QWidget *widget )
:QComboBox(widget),m_DisplayText("")
{
        autoUpdateDisplayText = false;

         checkBoxListDelegate = new CheckBoxListDelegate(this);
        // set delegate items view
        view()->setItemDelegate(checkBoxListDelegate);
        // Enable editing on items view
        view()->setEditTriggers(QAbstractItemView::CurrentChanged);
        // set "CheckBoxList::eventFilter" as event filter for items view
        view()->viewport()->installEventFilter(this);
        // it just cool to have it as defualt ;)
        view()->setAlternatingRowColors(true);

	connect(checkBoxListDelegate, SIGNAL(itemStatusChanged(QWidget*)), this, SLOT(updateCurrentSelection(QWidget*)));
	connect(checkBoxListDelegate, SIGNAL(closeEditor(QWidget*)), this, SLOT(updateDisplayText()));
}

// make the item vectors strings and not QComboBox* :D

CheckBoxList::~CheckBoxList()
{
        ;
}

int CheckBoxList::setAutoUpdateDisplayText(bool cond)
{
    autoUpdateDisplayText = cond;

    if(autoUpdateDisplayText)
    {
	updateDisplayText();
    }

    return 0;
}

bool CheckBoxList::getAutoUpdateDisplayText()
{
    return autoUpdateDisplayText;
}

bool CheckBoxList::eventFilter(QObject *object, QEvent *event)
{
        // don't close items view after we release the mouse button
        // by simple eating MouseButtonRelease in viewport of items view
        if(event->type() == QEvent::MouseButtonRelease && object==view()->viewport())
        {
                return true;
        }
        return QComboBox::eventFilter(object,event);
}


void CheckBoxList::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);

        // if no display text been set , use "..." as default
        if(m_DisplayText.isNull())
                opt.currentText = "...";
        else
                opt.currentText = m_DisplayText;
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}


void CheckBoxList::SetDisplayText(QString text)
{
        m_DisplayText = text;
}

QString CheckBoxList::GetDisplayText() const
{
        return m_DisplayText;
}

void CheckBoxList::addItem ( const QString & text, const QVariant & userData = QVariant() )
{
    QCheckBox* cbox = new QCheckBox(text);
    cbox->setChecked(userData.toBool());

    if (matchComboListItem(selectedItems, text) == -1 &&
	matchComboListItem(unSelectedItems, text) == -1)
    {
	QComboBox::addItem(text, userData);

	//QCheckBox* cbox = new QCheckBox(text);
	//cbox->setChecked(userData.toBool());

	if(userData.toBool())
	{
	    originalSelectedItems.push_back(text);
	}
    }
    else
    {
	int row = matchComboListItem(selectedItems, text);
	if(row == -1)
	{
	    row = matchComboListItem(unSelectedItems, text);
	}
	setItemData(row, userData);
    }


    updateCurrentSelection(static_cast<QWidget*>(cbox));
    itemsUpdated = true;
    updateDisplayText();
}

int CheckBoxList::updateCurrentSelection()
{
    return 0;
}

int CheckBoxList::updateCurrentSelection(QWidget* widget)
{
    QCheckBox* cbox = static_cast<QCheckBox*>(widget);
    QString text = cbox->text();

    if (cbox->isChecked())
    {
	if(matchComboListItem(selectedItems, text) == -1)
	{
	    selectedItems.push_back(text);
	    itemsUpdated = true;
	}

	if (matchComboListItem(unSelectedItems, text) != -1)
	{
	    unSelectedItems.remove(matchComboListItem(unSelectedItems, text));
	    itemsUpdated = true;
	}
    }
    else
    {
	if(matchComboListItem(unSelectedItems, text) == -1)
	{
	    unSelectedItems.push_back(text);
	    itemsUpdated = true;
	}

	if (matchComboListItem(selectedItems, text) != -1)
	{
	    selectedItems.remove(matchComboListItem(selectedItems, text));
	    itemsUpdated = true;
	}
    }
    return 0;
}


QVector<QString> CheckBoxList::getSelectedItems()
{
    return selectedItems;
}

QVector<QString> CheckBoxList::getunSelectedItems()
{
    return unSelectedItems;
}

int CheckBoxList::matchComboListItem(const QVector<QString>& list, const QString& item)
{
    int size = list.size();

    for (int i =0; i < size; i++)
    {
	//if (list.at(i)->text() == item)
	if (list.at(i) == item)
	{
	    return i;
	}
    }

    return -1;
}

int CheckBoxList::resetSelection()
{
    selectedItems = QVector<QString>();
    unSelectedItems = QVector<QString>();
    int itemsCount = count();

    for (int i =0; i< itemsCount; i++)
    {
	if(matchComboListItem(originalSelectedItems, itemText(i)) != -1)
	{
	    selectedItems.push_back(itemText(i));
	    setItemData(i, true);
	}
	else
	{
	    unSelectedItems.push_back(itemText(i));
	    setItemData(i, false);
	}
    }

    return 0;
}

int CheckBoxList::updateDisplayText()
{
    if(!itemsUpdated || !autoUpdateDisplayText)
    {
	return 1;
    }

    QString _text ("");
    for ( int i =0; i < selectedItems.count(); i++)
    {
	if(selectedItems.at(i).length() > 0)
	{
	    _text.append(selectedItems.at(i));
	}

	if (i != selectedItems.count()-1)
	{
	    _text += ",";
	}
    }

    if(_text.length() != 0)
    {
	m_DisplayText = _text;
    }
    else
    {
	m_DisplayText = tr("None");
    }

    emit currentItemsUpdated();
    itemsUpdated = false;
    return 0;
}
