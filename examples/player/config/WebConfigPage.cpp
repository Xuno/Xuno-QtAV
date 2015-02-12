#include "WebConfigPage.h"
#include <QLayout>
#include <QCheckBox>
#include <QLabel>
#include <QTextEdit>
#include "common/Config.h"

WebConfigPage::WebConfigPage()
{
    setObjectName("web");
    QGridLayout *gl = new QGridLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetFixedSize);
    int r = 0;
    m_enable = new QCheckBox(tr("Enable"));
    m_enable->setChecked(Config::instance().avfilterEnable());
    gl->addWidget(m_enable, r++, 0);
    gl->addWidget(new QLabel(tr("Web Links")), r++, 0);
    m_options = new QTextEdit();
    m_options->setText(Config::instance().avfilterOptions());
    gl->addWidget(m_options, r++, 0);
}

QString WebConfigPage::name() const
{
    return "Web Links";
}


void WebConfigPage::apply()
{
    Config::instance().avfilterOptions(m_options->toPlainText())
            .avfilterEnable(m_enable->isChecked())
            ;
}

void WebConfigPage::cancel()
{
    m_enable->setChecked(Config::instance().avfilterEnable());
    m_options->setText(Config::instance().avfilterOptions());
}

void WebConfigPage::reset()
{
}
