/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2012-2015 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include "AVFormatConfigPage.h"
#include <limits>
#include <QLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QDebug>

#include "common/Config.h"

AVFormatConfigPage::AVFormatConfigPage(QWidget *parent) :
    ConfigPageBase(parent)
{
    setObjectName(QString::fromLatin1("avformat"));
    QVBoxLayout *vl= new QVBoxLayout(this);
    QGroupBox *video = new QGroupBox(this);
    video->setTitle(tr(" Video"));
    QGridLayout *gl = new QGridLayout(video);
    gl->setContentsMargins(6, 6, 6, 6);
    //gl->setSizeConstraint(QLayout::SetFixedSize);
    int r = 0;
    m_on = new QCheckBox(QString::fromLatin1("%1 avformat %2").arg(tr("Enable")).arg(tr("options")),video);
    gl->addWidget(m_on, r++, 0);
    m_direct = new QCheckBox(tr("Reduce buffering"),video);
    gl->addWidget(m_direct, r++, 0);
    gl->addWidget(new QLabel(tr("Probe size"),video), r, 0, Qt::AlignRight);
    m_probeSize = new QSpinBox(video);
    m_probeSize->setMaximum(std::numeric_limits<int>::max());
    m_probeSize->setMinimum(0);
    m_probeSize->setToolTip(tr("0: auto"));
    gl->addWidget(m_probeSize, r++, 1, Qt::AlignLeft);
    gl->addWidget(new QLabel(tr("Max analyze duration"),video), r, 0, Qt::AlignRight);
    m_analyzeDuration = new QSpinBox(video);
    m_analyzeDuration->setMaximum(std::numeric_limits<int>::max());
    m_analyzeDuration->setToolTip(tr("0: auto. how many microseconds are analyzed to probe the input"));
    gl->addWidget(m_analyzeDuration, r++, 1, Qt::AlignLeft);

    gl->addWidget(new QLabel(tr("Extra"),video), r, 0, Qt::AlignRight);
    m_extra = new QLineEdit(video);
    m_extra->setToolTip(QString::fromLatin1("key1=value1 key2=value2 ..."));
    gl->addWidget(m_extra, r++, 1, Qt::AlignLeft);
    vl->addWidget(video, 1, Qt::AlignTop);


    QGroupBox *imgSeq = new QGroupBox(this);
    imgSeq->setTitle(tr(" Image Sequence"));
    QGridLayout *ggli = new QGridLayout(imgSeq);
    ggli->setContentsMargins(6, 6, 6, 6);
    r = 0;
    m_Ion = new QCheckBox(tr("Enable") + " " + "avformat " + tr("options"),imgSeq);
    ggli->addWidget(m_Ion, r++, 0);
    m_Idirect = new QCheckBox(tr("Reduce buffering"),imgSeq);
    ggli->addWidget(m_Idirect, r++, 0);
    ggli->addWidget(new QLabel(tr("Probe size"),imgSeq), r, 0, Qt::AlignRight);
    m_IprobeSize = new QSpinBox(imgSeq);
    m_IprobeSize->setMaximum(std::numeric_limits<int>::max());
    m_IprobeSize->setMinimum(0);
    m_IprobeSize->setToolTip("0: auto");
    ggli->addWidget(m_IprobeSize, r++, 1, Qt::AlignLeft);
    ggli->addWidget(new QLabel(tr("Max analyze duration"),imgSeq), r, 0, Qt::AlignRight);
    m_IanalyzeDuration = new QSpinBox(imgSeq);
    m_IanalyzeDuration->setMaximum(std::numeric_limits<int>::max());
    m_IanalyzeDuration->setToolTip("0: auto. how many microseconds are analyzed to probe the input");
    ggli->addWidget(m_IanalyzeDuration, r++, 1, Qt::AlignLeft);

    ggli->addWidget(new QLabel(tr("Extra"),imgSeq), r, 0, Qt::AlignRight);
    m_Iextra = new QLineEdit(imgSeq);
    m_Iextra->setToolTip("key1=value1 key2=value2 ...");
    ggli->addWidget(m_Iextra, r++, 1, Qt::AlignLeft);
    vl->addWidget(imgSeq, 1, Qt::AlignTop);

    setLayout(vl);

    applyToUi();
}

QString AVFormatConfigPage::name() const
{
    return tr("AVFormat");
}

void AVFormatConfigPage::applyFromUi()
{
    Config::instance()
            .setAvformatOptionsEnabled(m_on->isChecked())
            .probeSize(m_probeSize->value())
            .analyzeDuration(m_analyzeDuration->value())
            .reduceBuffering(m_direct->isChecked())
            .avformatExtra(m_extra->text())
            .setAvformatOptionsEnabledI(m_Ion->isChecked())
            .probeSizeI(m_IprobeSize->value())
            .analyzeDurationI(m_IanalyzeDuration->value())
            .reduceBufferingI(m_Idirect->isChecked())
            .avformatExtraI(m_Iextra->text());
}

void AVFormatConfigPage::applyToUi()
{
    m_on->setChecked(Config::instance().avformatOptionsEnabled());
    m_direct->setChecked(Config::instance().reduceBuffering());
    m_probeSize->setValue(Config::instance().probeSize());
    m_analyzeDuration->setValue(Config::instance().analyzeDuration());
    m_extra->setText(Config::instance().avformatExtra());

    m_Ion->setChecked(Config::instance().avformatOptionsEnabledI());
    m_Idirect->setChecked(Config::instance().reduceBufferingI());
    m_IprobeSize->setValue(Config::instance().probeSizeI());
    m_IanalyzeDuration->setValue(Config::instance().analyzeDurationI());
    m_Iextra->setText(Config::instance().avformatExtraI());
}
