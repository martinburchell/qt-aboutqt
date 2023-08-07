#include "mainmenu.h"
#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSharedPointer>
#include "app.h"


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

    m_p_listwidget = new QListWidget();
    m_mainlayout->addWidget(m_p_listwidget);

    connect(m_p_listwidget, &QListWidget::itemClicked,
            this, &MainMenu::menuItemClicked,
            Qt::UniqueConnection);
    connect(m_p_listwidget, &QListWidget::itemActivated,
            this, &MainMenu::menuItemClicked,
            Qt::UniqueConnection);

    QWidget* row = rowWidget();
    auto listitem = new QListWidgetItem("", m_p_listwidget);
    listitem->setSizeHint(row->sizeHint());
    m_p_listwidget->setItemWidget(listitem, row);

}


QWidget* MainMenu::rowWidget() const
{
    Qt::Alignment text_align = Qt::AlignLeft | Qt::AlignVCenter;

    auto row = new QWidget();
    auto rowlayout = new QHBoxLayout();
    row->setLayout(rowlayout);

    auto textlayout = new QVBoxLayout();

    auto title = new QLabel("About Qt");
    title->setAlignment(text_align);
    textlayout->addWidget(title);
    rowlayout->addLayout(textlayout);
    rowlayout->addStretch();

    return row;
}


void MainMenu::menuItemClicked(QListWidgetItem* item)
{
    Q_UNUSED(item)

    QMessageBox::aboutQt(this);
    m_p_listwidget->clearSelection();
}