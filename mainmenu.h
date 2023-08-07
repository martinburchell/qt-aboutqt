#pragma once
#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>

class QListWidget;
class QListWidgetItem;

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    MainMenu();
    QWidget* rowWidget() const;

public slots:
    void menuItemClicked(QListWidgetItem* item);

protected:
    QPointer<QListWidget> m_p_listwidget;
};
