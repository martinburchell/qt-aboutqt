#define SHOW_TASK_TIMING

#include "menuwindow.h"
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include "common/cssconst.h"
#include "common/uiconst.h"
#include "menulib/menuheader.h"

const int BAD_INDEX = -1;


MenuWindow::MenuWindow(App& app, const bool top) :
    m_app(app),
    m_top(top)
{
    setEscapeKeyCanAbort(!top, true);

    loadStyleSheet();
    setObjectName(cssconst::MENU_WINDOW_OUTER_OBJECT);

    // ------------------------------------------------------------------------
    // Layout
    // ------------------------------------------------------------------------

    /*
    For no clear reason, I have been unable to set the background colour
    of the widget that goes inside the QStackedLayout, either by class name
    or via setObjectName(), or with setAutoFillBackground(true).

    However, it works perfectly well to set the  background colour of inner
    widgets. So instead of this:

        QStackedLayout (main app)
            QWidget (MainWindow or Questionnaire)  <-- can't set bg colour
                m_mainlayout
                    widgets of interest

    it seems we have to do this:

        QStackedLayout (main app)
            QWidget (MenuWindow or Questionnaire)
                dummy_layout
                    dummy_widget  <-- set background colour of this one
                        m_mainlayout
                            widgets of interest
    */

    auto dummy_layout = new QVBoxLayout();
    dummy_layout->setContentsMargins(uiconst::NO_MARGINS);
    setLayout(dummy_layout);
    auto dummy_widget = new QWidget();  // doesn't need to be BaseWidget; contains scrolling list
    dummy_widget->setObjectName(cssconst::MENU_WINDOW_BACKGROUND);
    dummy_layout->addWidget(dummy_widget);

    m_mainlayout = new QVBoxLayout();
    m_mainlayout->setContentsMargins(uiconst::NO_MARGINS);
    dummy_widget->setLayout(m_mainlayout);
}


void MenuWindow::setIcon(const QString& icon)
{
    m_icon = icon;
    m_p_header->setIcon(icon);
}


void MenuWindow::loadStyleSheet()
{
}


void MenuWindow::rebuild(bool rebuild_header)
{
    if (rebuild_header) {
        makeLayout();
    }
    makeItems();
    build();
}


void MenuWindow::makeLayout()
{
    // ------------------------------------------------------------------------
    // Header
    // ------------------------------------------------------------------------

    const bool offer_debug_layout = false;
    m_p_header = new MenuHeader(this, m_app, m_top, "", m_icon,
                                offer_debug_layout);
    // ... we'll set its title later in build()
    m_mainlayout->addWidget(m_p_header);


    // ------------------------------------------------------------------------
    // List
    // ------------------------------------------------------------------------

    m_p_listwidget = new QListWidget();
    m_mainlayout->addWidget(m_p_listwidget);

    connect(m_p_listwidget, &QListWidget::itemClicked,
            this, &MenuWindow::menuItemClicked,
            Qt::UniqueConnection);
    connect(m_p_listwidget, &QListWidget::itemActivated,
            this, &MenuWindow::menuItemClicked,
            Qt::UniqueConnection);

    // ------------------------------------------------------------------------
    // Subclass specialization of layout
    // ------------------------------------------------------------------------

    extraLayoutCreation();
}


void MenuWindow::build()
{
    // qDebug() << Q_FUNC_INFO;

    if (m_items.isEmpty()) {  // First time through
        makeLayout();
        makeItems();
    }

    m_p_header->setTitle(title());

    m_p_listwidget->clear();

    // Method 1: QListWidget, QListWidgetItem
    // Size hints: https://forum.qt.io/topic/17481/easiest-way-to-have-a-simple-list-with-custom-items/4
    // Note that the widgets call setSizePolicy.
    for (int i = 0; i < m_items.size(); ++i) {
        MenuItem item = m_items.at(i);
        QWidget* row = item.rowWidget(m_app);
        auto listitem = new QListWidgetItem("", m_p_listwidget);
        listitem->setData(Qt::UserRole, QVariant(i));
        listitem->setSizeHint(row->sizeHint());
        m_p_listwidget->setItemWidget(listitem, row);
    }

    // Method 2: QListView, QStandardItemModel, custom delegate
    // http://doc.qt.io/qt-5/qlistview.html
    // argh!

    // Stretch not necessary, even if the menu is short (the QListWidget
    // seems to handle this fine).

    afterBuild();
}


QString MenuWindow::subtitle() const
{
    return "";
}


QString MenuWindow::icon() const
{
    return m_icon;
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
    qInfo().noquote().nospace() << "Clicked: " << m.info();
    // ACT ON IT. And clear the selection.
    m.act(m_app);
    m_p_listwidget->clearSelection();
}




bool MenuWindow::event(QEvent* e)
{
    const bool result = OpenableWidget::event(e);  // call parent
    const QEvent::Type type = e->type();
    if (type == QEvent::Type::LanguageChange) {
        rebuild();  // including rebuilding the header
    }
    return result;
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
