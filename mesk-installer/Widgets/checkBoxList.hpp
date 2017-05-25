#ifndef CHECKBOXLIST_H
#define CHECKBOXLIST_H

#include <MPF/Delegates/checkBoxListDelegate.hpp>
#include <QCheckBox>
#include <QComboBox>
#include <QAbstractItemView>
#include <QEvent>
#include <QStylePainter>

class CheckBoxList: public QComboBox
{
        Q_OBJECT;

public:
        CheckBoxList(QWidget *widget = 0);
        virtual ~CheckBoxList();
        bool                        eventFilter(QObject *object, QEvent *event);
        virtual void                paintEvent(QPaintEvent *);
        void                        SetDisplayText(QString text);
        QString                     GetDisplayText() const;
        void                        addItem ( const QString &, const QVariant & );
	QVector<QString>	    getSelectedItems();
	QVector<QString>	    getunSelectedItems();
        int                         setAutoUpdateDisplayText(bool);
	bool                        getAutoUpdateDisplayText();
	int			    resetSelection();
private:
        CheckBoxListDelegate*       checkBoxListDelegate;
	QVector<QString>	    originalSelectedItems;
	QVector<QString>	    selectedItems;
	QVector<QString>	    unSelectedItems;
        QString                     m_DisplayText;
        bool                        autoUpdateDisplayText;
	int			    matchComboListItem(const QVector<QString>&, const QString&);
	bool			    itemsUpdated;
private slots:
	int			    updateCurrentSelection(QWidget*);
	int			    updateCurrentSelection();
	int			    updateDisplayText();
signals:
	void			    sendCurrentSelection(QStringList);
	void			    currentItemsUpdated();
};
#endif // CHECKBOXLIST_H
