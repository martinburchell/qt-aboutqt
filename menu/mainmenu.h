#pragma once
#include <QPointer>
#include <QSharedPointer>
#include <QVector>
#include <QVBoxLayout>
#include "menulib/menuitem.h"
#include "widgets/openablewidget.h"

class QListWidget;
class QListWidgetItem;

class MainMenu : public OpenableWidget
{
    Q_OBJECT

public:
    MainMenu();
    QString title() const;
    bool event(QEvent* e) override;

protected:
    void makeLayout();

    void makeItems();
    void aboutQt();
    void build() override;

public slots:
    void menuItemClicked(QListWidgetItem* item);

protected:
    QVector<MenuItem> m_items;
    QPointer<QVBoxLayout> m_mainlayout;
    QPointer<QListWidget> m_p_listwidget;
};
