#include "mainmenu.h"
#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSharedPointer>
#include "core/app.h"
#include "menulib/menuitem.h"


MainMenu::MainMenu()
{
    auto dummy_layout = new QVBoxLayout();
    dummy_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(dummy_layout);
    auto dummy_widget = new QWidget();
    dummy_layout->addWidget(dummy_widget);

    m_mainlayout = new QVBoxLayout();
    m_mainlayout->setContentsMargins(0, 0, 0, 0);
    dummy_widget->setLayout(m_mainlayout);
}


QString MainMenu::title() const
{
    return tr("Testing");
}

void MainMenu::makeLayout()
{
    m_p_listwidget = new QListWidget();
    m_mainlayout->addWidget(m_p_listwidget);

    connect(m_p_listwidget, &QListWidget::itemClicked,
            this, &MainMenu::menuItemClicked,
            Qt::UniqueConnection);
    connect(m_p_listwidget, &QListWidget::itemActivated,
            this, &MainMenu::menuItemClicked,
            Qt::UniqueConnection);
}


void MainMenu::build()
{
    if (m_items.isEmpty()) {
        makeLayout();
        makeItems();
    }

    m_p_listwidget->clear();

    for (int i = 0; i < m_items.size(); ++i) {
        MenuItem item = m_items.at(i);
        QWidget* row = item.rowWidget();
        auto listitem = new QListWidgetItem("", m_p_listwidget);
        listitem->setData(Qt::UserRole, QVariant(i));
        listitem->setSizeHint(row->sizeHint());
        m_p_listwidget->setItemWidget(listitem, row);
    }
}



void MainMenu::menuItemClicked(QListWidgetItem* item)
{
    Q_UNUSED(item)

    aboutQt();
    m_p_listwidget->clearSelection();
}




bool MainMenu::event(QEvent* e)
{
    return OpenableWidget::event(e);
}

void MainMenu::makeItems()
{
    m_items = {
        MenuItem(tr("About Qt"),
                 std::bind(&MainMenu::aboutQt, this)),
    };
}

void MainMenu::aboutQt()
{
    QMessageBox::aboutQt(this);
}
