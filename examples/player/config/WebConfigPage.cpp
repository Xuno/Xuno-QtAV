#include "WebConfigPage.h"
#include <QLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
//#include <QTextEdit>
#include <QListView>
#include <QGroupBox>
#include "common/Config.h"
#include <QDebug>


WebConfigPage::WebConfigPage()
{
    setObjectName("web");
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setAlignment(Qt::AlignTop);
    QGroupBox *groupBox = new QGroupBox(tr("Edit list of Web links"));
    QVBoxLayout *gl = new QVBoxLayout();
    gl->setSizeConstraint(QLayout::SetFixedSize);

    QStringList numbers;
    numbers << "Xuno" << "Google" << "Three" << "Four" << "Five";

    model = new StringListModel();
    model->setStringList(numbers);


    m_options = new QListView();
    m_options->setModel(model);
    m_options->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_options, SIGNAL(clicked(QModelIndex)),this, SLOT(onSelected(QModelIndex)));

    //m_options->setText(Config::instance().avfilterOptions());
    gl->addWidget(m_options);

    QHBoxLayout *hb1 = new QHBoxLayout();
    QPushButton *bInsert = new QPushButton(tr("&Insert"), this);
    QPushButton *bChange = new QPushButton(tr("&Change"), this);
    QPushButton *bDelete = new QPushButton(tr("&Delete"), this);
    hb1->addWidget(bInsert);
    hb1->addWidget(bChange);
    hb1->addWidget(bDelete);

    QObject::connect(bInsert, SIGNAL(pressed()), this, SLOT(onInsert()));
    QObject::connect(bChange, SIGNAL(pressed()), this, SLOT(onChange()));
    QObject::connect(bDelete, SIGNAL(pressed()), this, SLOT(onDelete()));

    gl->addLayout(hb1);
    int r=0;
    QGridLayout *gbox = new QGridLayout();
    QLabel *lname = new QLabel(tr("&Name:"));
    ename = new QLineEdit();
    lname->setBuddy(ename);
    gbox->addWidget(lname,r,0);
    gbox->addWidget(ename,r,1);
    r++;
    QLabel *lurl = new QLabel(tr("&Url:"));
    eurl = new QLineEdit();
    lurl->setBuddy(eurl);
    gbox->addWidget(lurl,r,0);
    gbox->addWidget(eurl,r,1);
    gl->addLayout(gbox);
    groupBox->setLayout(gl);
    vbox->addWidget(groupBox);
    setLayout(vbox);
}

QString WebConfigPage::name() const
{
    return "Web Links";
}


void WebConfigPage::apply()
{
//    Config::instance().avfilterOptions(m_options->toPlainText())
//            .avfilterEnable(m_enable->isChecked())
//            ;
}

void WebConfigPage::cancel()
{
//    m_enable->setChecked(Config::instance().avfilterEnable());
//    m_options->setText(Config::instance().avfilterOptions());
}

void WebConfigPage::reset()
{
}

void WebConfigPage::onInsert()
{
    qDebug("WebConfigPage onInsert");
    QString name = ename->text();
    model->insertRow(model->rowCount(),QModelIndex());
    QModelIndex index = model->index(model->rowCount()-1, 0, QModelIndex());
    if (model->setData(index, name, Qt::EditRole)){
        m_options->setCurrentIndex(index);
        ename->clear();
    }

}

void WebConfigPage::onChange()
{
    qDebug("WebConfigPage onChange");
    QModelIndex index = m_options->currentIndex();
    QString name = ename->text();
    if (model->setData(index, name, Qt::EditRole)){
        m_options->setCurrentIndex(index);
        qDebug("WebConfigPage Changed");
    }
}

void WebConfigPage::onDelete()
{
    qDebug("WebConfigPage onDelete");
    QModelIndex index = m_options->currentIndex();
    qDebug("Index row %d:",index.row());
    if (model->removeRow(index.row(),QModelIndex())){
        qDebug("WebConfigPage Deleted");
    }


}

void WebConfigPage::onSelected(QModelIndex index)
{
    QString name = index.data().toString();
    qDebug() << "WebConfigPage onSelected :" << name ;
    ename->setText(name);
}
