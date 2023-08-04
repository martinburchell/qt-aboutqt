#include "menuitem.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScopedPointer>
#include <QSize>
#include <QUrl>
#include "core/app.h"
#include "widgets/basewidget.h"

MenuItem::MenuItem(const QString& title, const MenuItem::ActionFunction& func) :
    m_title(title)
{
    m_func = func;
}


QString MenuItem::title() const
{
    return m_title;
}


QWidget* MenuItem::rowWidget(App& app) const
{
    Q_UNUSED(app)
    Qt::Alignment text_align = Qt::AlignLeft | Qt::AlignVCenter;

    auto row = new QWidget();
    auto rowlayout = new QHBoxLayout();
    row->setLayout(rowlayout);

    auto textlayout = new QVBoxLayout();

    auto title = new QLabel(m_title);
    title->setAlignment(text_align);
    textlayout->addWidget(title);
    rowlayout->addLayout(textlayout);
    rowlayout->addStretch();

    return row;
}


void MenuItem::act() const
{
    if (m_func) {
        m_func();
        return;
    }
}
