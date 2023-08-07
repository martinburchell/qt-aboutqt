#pragma once
#include <QPointer>
#include <QSharedPointer>
#include <QVector>
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
    QPointer<QVBoxLayout> m_mainlayout;
    QPointer<QListWidget> m_p_listwidget;
};
