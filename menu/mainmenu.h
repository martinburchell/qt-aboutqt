#pragma once
#include <QPointer>
#include <QSharedPointer>
#include <QVector>
#include <QVBoxLayout>
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
    QWidget* rowWidget() const;

protected:
    void makeLayout();

    void aboutQt();
    void build() override;

public slots:
    void menuItemClicked(QListWidgetItem* item);

protected:
    QPointer<QVBoxLayout> m_mainlayout;
    QPointer<QListWidget> m_p_listwidget;
};
