#define SHOW_TASK_TIMING

#include "menuwindow.h"
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include "menulib/menuheader.h"

const int BAD_INDEX = -1;


MenuWindow::MenuWindow(App& app) :
    m_app(app)
{
    auto dummy_layout = new QVBoxLayout();
    dummy_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(dummy_layout);
    auto dummy_widget = new QWidget();  // doesn't need to be BaseWidget; contains scrolling list
    dummy_layout->addWidget(dummy_widget);

    m_mainlayout = new QVBoxLayout();
    m_mainlayout->setContentsMargins(0, 0, 0, 0);
    dummy_widget->setLayout(m_mainlayout);
}


void MenuWindow::makeLayout()
{
    m_p_listwidget = new QListWidget();
    m_mainlayout->addWidget(m_p_listwidget);

    connect(m_p_listwidget, &QListWidget::itemClicked,
            this, &MenuWindow::menuItemClicked,
            Qt::UniqueConnection);
    connect(m_p_listwidget, &QListWidget::itemActivated,
            this, &MenuWindow::menuItemClicked,
            Qt::UniqueConnection);
}


void MenuWindow::build()
{
    if (m_items.isEmpty()) {
        makeLayout();
        makeItems();
    }

    m_p_listwidget->clear();

    for (int i = 0; i < m_items.size(); ++i) {
        MenuItem item = m_items.at(i);
        QWidget* row = item.rowWidget(m_app);
        auto listitem = new QListWidgetItem("", m_p_listwidget);
        listitem->setData(Qt::UserRole, QVariant(i));
        listitem->setSizeHint(row->sizeHint());
        m_p_listwidget->setItemWidget(listitem, row);
    }
}



void MenuWindow::menuItemClicked(QListWidgetItem* item)
{
    // Act on a click

    const QVariant v = item->data(Qt::UserRole);
    const int i = v.toInt();
    if (i < 0 || i >= m_items.size()) {
        qWarning() << Q_FUNC_INFO << "Selection out of range:" << i
                   << "(vector size:" << m_items.size() << ")";
        return;
    }
    MenuItem& m = m_items[i];
    m.act();
    m_p_listwidget->clearSelection();
}




bool MenuWindow::event(QEvent* e)
{
    return OpenableWidget::event(e);
}


int MenuWindow::currentIndex() const
{
    QListWidgetItem* item = m_p_listwidget->currentItem();
    if (!item) {
        return BAD_INDEX;
    }
    const QVariant v = item->data(Qt::UserRole);
    const int i = v.toInt();
    if (i >= m_items.size() || i <= -1) {
        // Out of bounds; coerce to -1
        return BAD_INDEX;
    }
    return i;
}
