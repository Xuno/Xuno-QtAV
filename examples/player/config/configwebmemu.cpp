#include "configwebmemu.h"
#include "common/Config.h"


ConfigWebMemu::ConfigWebMemu(QWidget *parent) :
    QMenu(parent)
{
    initMenuItems();
    init();
}

ConfigWebMemu::~ConfigWebMemu()
{
    if (mpXunoBrowser) {
        delete mpXunoBrowser;
        mpXunoBrowser = 0;
    }
}

void ConfigWebMemu::openurl(QAction * a){
    int i=a->data().toInt();
    qDebug() << i << menuitemList.at(i)->name << menuitemList.at(i)->url;
    onXunoBrowser(menuitemList.at(i)->url);
    //onXunoBrowser("");
}

void ConfigWebMemu::initMenuItems(){

    QMap<QString,QVariant> links = QMap<QString,QVariant>(Config::instance().WebLinks());
    QStringList names = links.keys();
    QList<QVariant> urls = links.values();

    menuitemList.append(new MenuItems({"Xuno fixed","http://www.xuno.com/playlist_8bit.php","www.xuno.net.ico"}));
    menuitemList.append(new MenuItems({"Google fixed","http://www.google.com","www.google.com.ico"}));

    for (int i = 0; i < names.size(); ++i){
        menuitemList.append(new MenuItems({names[i],urls[i].toString() ,""}));
    }



    XUNOserverUrl="http://www.xuno.com";
    XUNOpresetUrl=XUNOserverUrl+"/getpreset.php?";
}

void ConfigWebMemu::init(){
    for (int i=0;i<menuitemList.size();i++){
        QIcon* ic = new QIcon(menuitemList.at(i)->iconurl);
        QAction* qa = addAction(*ic,menuitemList.at(i)->name);
        qa->setData(i);
    }
    connect(this, SIGNAL(triggered(QAction *)), this, SLOT(openurl(QAction *)));
}


void ConfigWebMemu::onXunoBrowser(QString url){

    bool emp=url.isEmpty();

    if (!mpXunoBrowser){
        mpXunoBrowser = new XunoBrowser();
        mpXunoBrowser->setXUNOContentUrl(QString(XUNOserverUrl).append("/content/"));
        connect(mpXunoBrowser, SIGNAL(clicked()), SLOT(onClickXunoBrowser()));
    }
    if (mpXunoBrowser->isHidden()) mpXunoBrowser->show();

    if (emp) {
      mpXunoBrowser->setUrl(QUrl(QString(XUNOserverUrl).append("/playlist_8bit.php")));
    }else{
      mpXunoBrowser->setUrl(url);
    }
}

void ConfigWebMemu::onClickXunoBrowser(){
    QUrl url=mpXunoBrowser->getClikedUrl();
    emit onPlayXunoBrowser(url);
}


void ConfigWebMemu::onChanged(){
    qDebug()<<"ConfigWebMemu::onChanged";
    initMenuItems();
    init();
}

