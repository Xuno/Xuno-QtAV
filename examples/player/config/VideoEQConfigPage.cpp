/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2012-2014 Wang Bin <wbsecg1@gmail.com>

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


#include "VideoEQConfigPage.h"
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include "../Slider.h"
#include <QList>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtNetwork>
#include <QNetworkReply>

VideoEQConfigPage::VideoEQConfigPage(QWidget *parent) :
    QWidget(parent)
{
    mEngine = SWScale;
    mPresetID = 0;
    QGridLayout *gl = new QGridLayout();
    setLayout(gl);

    QLabel *label = new QLabel();
    label->setText(tr("Engine"));
    mpEngine = new QComboBox();
    setEngines(QVector<Engine>(1, SWScale));
    connect(mpEngine, SIGNAL(currentIndexChanged(int)), SLOT(onEngineChangedByUI()));

    int r = 0, c = 0;
    gl->addWidget(label, r, c);
    gl->addWidget(mpEngine, r, c+1);
    r++;


    struct {
        QSlider **slider;
        QString text;
        int init;
    } sliders[] = {
        { &mpBSlider, tr("Brightness"),0 },
        { &mpCSlider, tr("Constrast"),0},
        { &mpHSlider, tr("Hue"),0 },
        { &mpSSlider, tr("Saturation"),0},
        { &mpGSlider, tr("GammaRGB"),0},
        { &mpFSSlider, tr("Filter Sharp"),-100},
        { 0, "",0 }
    };
    for (int i = 0; sliders[i].slider; ++i) {
        QLabel *label = new QLabel(sliders[i].text);
        *sliders[i].slider = new Slider();
        QSlider *slider = *sliders[i].slider;
        slider->setOrientation(Qt::Horizontal);
        slider->setTickInterval(2);
        slider->setRange(-100, 100);
        slider->setValue((sliders[i].init)?sliders[i].init:0);

        gl->addWidget(label, r, c);
        gl->addWidget(slider, r, c+1);
        r++;
    }
    mpGlobal = new QCheckBox(tr("Global"));
    mpGlobal->setEnabled(false);
    mpGlobal->setChecked(false);
    mpResetButton = new QPushButton(tr("Reset"));

    gl->addWidget(mpGlobal, r, c, Qt::AlignLeft);
    gl->addWidget(mpResetButton, r, c+1, Qt::AlignRight);
    r++;

    mpLoadPreset = new QPushButton(tr("Load Preset"));
    gl->addWidget(mpLoadPreset, r, c, Qt::AlignLeft);
    mpListPreset = new QComboBox();

    setListPresets();
    initPresets();

    connect(mpListPreset, SIGNAL(currentIndexChanged(int)), SLOT(onListPresetChangedByUI()));
    gl->addWidget(mpListPreset, r, c+1, Qt::AlignRight);
    r++;
    mpSavePreset = new QPushButton(tr("Save Preset"));
    gl->addWidget(mpSavePreset, r, c, Qt::AlignLeft);

    connect(mpBSlider, SIGNAL(valueChanged(int)), SIGNAL(brightnessChanged(int)));
    connect(mpCSlider, SIGNAL(valueChanged(int)), SIGNAL(contrastChanged(int)));
    connect(mpHSlider, SIGNAL(valueChanged(int)), SIGNAL(hueChanegd(int)));
    connect(mpSSlider, SIGNAL(valueChanged(int)), SIGNAL(saturationChanged(int)));
    connect(mpGlobal, SIGNAL(toggled(bool)), SLOT(onGlobalSet(bool)));
    connect(mpResetButton, SIGNAL(clicked()), SLOT(onReset()));
    connect(mpGSlider, SIGNAL(valueChanged(int)), SIGNAL(gammaRGBChanged(int)));
    connect(mpFSSlider, SIGNAL(valueChanged(int)), SIGNAL(filterSharpChanged(int)));
    connect(mpSavePreset, SIGNAL(clicked()), SLOT(onSavePreset()));
    connect(mpLoadPreset, SIGNAL(clicked()), SLOT(onLoadPreset()));
}

void VideoEQConfigPage::onGlobalSet(bool g)
{
    Q_UNUSED(g);
}

void VideoEQConfigPage::setListPresets()
{
    const QList <QString> PresetName =  QList <QString>() << "Day" << "Nigth" << "User 1" << "User 2";
    ColorSpacePreset ctmp;
    mpListPreset->clear();
    for (int i=0;i<PresetName.size();i++) {
         mpListPreset->addItem(QString(" Preset [%1]").arg(PresetName.at(i)));
         ctmp.name=PresetName.at(i);
         mPreset.append(ctmp);
    }
}

void VideoEQConfigPage::initPresets()
{
    for (int i=0;i<mPreset.size();i++){
        fillPreset(i);
    }
}

void VideoEQConfigPage::loadPresets()
{
    for (int i=0;i<mPreset.size();i++){
        //fillPreset(i);
    }
}

void VideoEQConfigPage::savePresets()
{
    for (int i=0;i<mPreset.size();i++){
        //fillPreset(i);
    }
}

void VideoEQConfigPage::fillPreset(int id)
{
    mPreset[id].brightness=brightness_p();
    mPreset[id].contrast=contrast_p();
    mPreset[id].hue=hue_p();
    mPreset[id].saturation=saturation_p();
    mPreset[id].gammaRGB=gammaRGB_p();
    mPreset[id].filterSharp=filterSharp_p();
}

void VideoEQConfigPage::fillCurrentPreset()
{
    fillPreset(mPresetID);
}

void VideoEQConfigPage::readCurrentPreset()
{
    //qDebug("VideoEQConfigPage::readCurrentPreset brightness()  %f, ID: %d",mPreset[mPresetID].brightness, mPresetID);
    brightness(mPreset.at(mPresetID).brightness);
    contrast(mPreset.at(mPresetID).contrast);
    hue(mPreset.at(mPresetID).hue);
    saturation(mPreset.at(mPresetID).saturation);
    gammaRGB(mPreset.at(mPresetID).gammaRGB);
    filterSharp(mPreset.at(mPresetID).filterSharp);
}

void VideoEQConfigPage::setListPreset(int id)
{
    if (mpListPreset->count()) {
        mpListPreset->setCurrentIndex(id);
    }
    emit listPresetChanged();
}

void VideoEQConfigPage::setEngines(const QVector<Engine> &engines)
{
    mpEngine->clear();
    QVector<Engine> es(engines);
    qSort(es);
    mEngines = es;
    foreach (Engine e, es) {
        if (e == SWScale) {
            mpEngine->addItem("libswscale");
        } else if (e == GLSL) {
            mpEngine->addItem("GLSL");
        } else if (e == XV) {
            mpEngine->addItem("XV");
        }
    }
}

void VideoEQConfigPage::setEngine(Engine engine)
{
    if (engine == mEngine)
        return;
    mEngine = engine;
    if (!mEngines.isEmpty()) {
        mpEngine->setCurrentIndex(mEngines.indexOf(engine));
    }
    emit engineChanged();
}

VideoEQConfigPage::Engine VideoEQConfigPage::engine() const
{
    return mEngine;
}

qreal VideoEQConfigPage::brightness() const
{
    //return (qreal)mpBSlider->value()/100.0;
    return brightness_p()+mRemotePreset.brightness;
}

qreal VideoEQConfigPage::brightness_p() const
{
    return ((qreal)mpBSlider->value()/100.0);
}

void VideoEQConfigPage::brightness(qreal val)
{
    mpBSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::contrast() const
{
    //return (qreal)mpCSlider->value()/100.0;
    return contrast_p()+mRemotePreset.contrast;
}

qreal VideoEQConfigPage::contrast_p() const
{
    return ((qreal)mpCSlider->value()/100.0);
}

void VideoEQConfigPage::contrast(qreal val) const
{
    mpCSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::hue() const
{
   //return (qreal)mpHSlider->value()/100.0;
   return hue_p()+mRemotePreset.hue;
}

qreal VideoEQConfigPage::hue_p() const
{
    return ((qreal)mpHSlider->value()/100.0);
}

void VideoEQConfigPage::hue(qreal val) const
{
    mpHSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::saturation() const
{
    //return (qreal)mpSSlider->value()/100.0;
    return saturation_p()+mRemotePreset.saturation;
}

qreal VideoEQConfigPage::saturation_p() const
{
    return ((qreal)mpSSlider->value()/100.0);
}

void VideoEQConfigPage::saturation(qreal val) const
{
    mpSSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::gammaRGB() const
{
    return gammaRGB_p()+mRemotePreset.gammaRGB;
}

qreal VideoEQConfigPage::gammaRGB_p() const
{
    return ((qreal)mpGSlider->value()/100.0);
}

void VideoEQConfigPage::gammaRGB(qreal val) const
{
    mpGSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::filterSharp() const
{
    //qDebug("VideoEQConfigPage::filterSharp return bar:%f bar+remote:%f,remote: %f",filterSharp_p(),filterSharp_p()+mRemotePreset.filterSharp,mRemotePreset.filterSharp);
    return filterSharp_p()+mRemotePreset.filterSharp;
}

qreal VideoEQConfigPage::filterSharp_p() const
{
    return ((qreal)mpFSSlider->value()/100.0);
}

void VideoEQConfigPage::filterSharp(qreal val) const
{
    mpFSSlider->setValue(int(val*100));
}

void VideoEQConfigPage::onReset()
{
    mpBSlider->setValue(0);
    mpCSlider->setValue(0);
    mpHSlider->setValue(0);
    mpSSlider->setValue(0);
    mpGSlider->setValue(0);
    mpFSSlider->setValue(-100);
}

void VideoEQConfigPage::onLoadPreset()
{
  qDebug("VideoEQConfigPage::LoadPreset");
  readCurrentPreset();
  saveLocalPresets();
}

void VideoEQConfigPage::onSavePreset()
{
  qDebug("VideoEQConfigPage::SavePreset");
  fillCurrentPreset();
  saveLocalPresets();
}

void VideoEQConfigPage::setSaveFile(const QString &file){
     mFile = file;
}

QString VideoEQConfigPage::saveFile() const
{
    return mFile;
}

void VideoEQConfigPage::loadLocalPresets()
{
    //qDebug("************ Load config presets %s************", qPrintable(mFile));
    QSettings settings(mFile, QSettings::IniFormat);
    for (int i=0;i<mPreset.size();i++){
        settings.beginGroup(QString("preset_%1").arg(i));
        mPreset[i].name=settings.value("name", mPreset[i].name).toString();
        mPreset[i].brightness=settings.value("brightness", mPreset[i].brightness).toReal();
        mPreset[i].contrast=settings.value("contrast", mPreset[i].contrast).toReal();
        mPreset[i].hue=settings.value("hue",  mPreset[i].hue).toReal();
        mPreset[i].saturation=settings.value("saturation", mPreset[i].saturation).toReal();
        mPreset[i].gammaRGB=settings.value("gammaRGB", mPreset[i].gammaRGB).toReal();
        mPreset[i].filterSharp=settings.value("filterSharp", mPreset[i].filterSharp).toReal()-1.;
        settings.endGroup();
    }
    settings.beginGroup("preset");
    mPresetID=settings.value("last",mPresetID).toInt();
    settings.endGroup();
    readCurrentPreset();
    setListPreset(mPresetID);
}

void VideoEQConfigPage::saveLocalPresets()
{
    //qDebug("************ Save config presets %s************", qPrintable(mFile));
    QSettings settings(mFile, QSettings::IniFormat);
    for (int i=0;i<mPreset.size();i++){
        settings.beginGroup(QString("preset_%1").arg(i));
        settings.setValue("name", mPreset.at(i).name);
        settings.setValue("brightness", mPreset.at(i).brightness);
        settings.setValue("contrast", mPreset.at(i).contrast);
        settings.setValue("hue", mPreset.at(i).hue);
        settings.setValue("saturation", mPreset.at(i).saturation);
        settings.setValue("gammaRGB", mPreset.at(i).gammaRGB);
        settings.setValue("filterSharp", mPreset.at(i).filterSharp+qreal(1.));
        settings.endGroup();
    }
    settings.beginGroup("preset");
    settings.setValue("last", mPresetID);
    settings.endGroup();
}

QList <VideoEQConfigPage::ColorSpacePreset> VideoEQConfigPage::presetItems() const
{
    return mPreset;
}

void VideoEQConfigPage::onListPresetChangedByUI()
{
 //   if (mpListPreset->currentIndex() >= mpListPreset.count() || mpListPreset->currentIndex() < 0)
 //       return;
    mPresetID = mpListPreset->currentIndex();
    //qDebug("VideoEQConfigPage::onListPresetChangedByUI : %d",mPresetID);
    emit listPresetChanged();
}

void VideoEQConfigPage::setRemoteUrlPresset(const QString &file){
    if (!file.isEmpty()){
     mURL=file;
    }
}

void VideoEQConfigPage::getRemotePressets (){
    //qDebug("VideoEQConfigPage::getRemotePressets before: mURL %s",qPrintable(mURL));
    if (mURL.compare(presetUrl)==0 || mURL.isEmpty()) return; //check if pressets was loaded for this url
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(onPresetRequestFinished(QNetworkReply*)));
    QUrl url(mURL);
    QNetworkReply* reply = nam->get(QNetworkRequest(url));
    if (reply->error() != QNetworkReply::NoError) {
        qDebug("Error  getRemotePressets : %s, error %d",qPrintable(mURL),(int)reply->error());
    }
    //qDebug("VideoEQConfigPage::getRemotePressets after: mURL %s",qPrintable(mURL));
}

void VideoEQConfigPage::onPresetRequestFinished(QNetworkReply* reply){
    if(reply->error() == QNetworkReply::NoError) {
        QString strReply = (QString)reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        if (!jsonResponse.isEmpty()){
            QJsonObject jsonObject = jsonResponse.object();
            if (!jsonObject.isEmpty()){
                if (jsonObject.value("success").toInt()==1){
                    mRemotePreset.brightness=jsonObject.value("brightness").toDouble();
                    mRemotePreset.contrast=jsonObject.value("contrast").toDouble();
                    mRemotePreset.hue=jsonObject.value("hue").toDouble();
                    mRemotePreset.saturation=jsonObject.value("saturation").toDouble();
                    mRemotePreset.gammaRGB=jsonObject.value("gammaRGB").toDouble();
                    mRemotePreset.filterSharp=jsonObject.value("filterSharp").toDouble();
                    qDebug(" *********  mRemotePreset json loaded");
                    presetUrl=mURL;
                    emit engineChanged();
                }
            }
        }
    } else {
        qDebug("VideoEQConfigPage.cpp: ERROR, read fromJson VideoEQConfigPage::onPresetRequestFinished");
    }
    delete reply;
    qDebug("VideoEQConfigPage::onPresetRequestFinished");
}

void VideoEQConfigPage::onEngineChangedByUI()
{
    if (mpEngine->currentIndex() >= mEngines.size() || mpEngine->currentIndex() < 0)
        return;
    mEngine = mEngines.at(mpEngine->currentIndex());
    mpGSlider->setEnabled(mEngine==GLSL);
    mpFSSlider->setEnabled(mEngine==GLSL);
    emit engineChanged();
}
