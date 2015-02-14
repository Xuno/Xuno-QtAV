#ifndef WebConfigPage_H
#define WebConfigPage_H

#include "ConfigPageBase.h"
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QAbstractButton>
#include <QDialogButtonBox>

class StringListModel : public QStringListModel
{
};



class QCheckBox;
class QListView;
class QPushButton;
class QLineEdit;

class WebConfigPage : public ConfigPageBase
{
      Q_OBJECT
public:
    WebConfigPage();
    virtual QString name() const;


public slots:
    virtual void apply(); //store the values on ui. call Config::xxx
    virtual void cancel(); //cancel the values on ui. values are from Config
    virtual void reset(); //reset to default

private slots:
     void onInsert();
     void onChange();
     void onDelete();
     void onSelected(QModelIndex);

private:
    QCheckBox *m_enable;
    QListView *m_options;
    QDialogButtonBox *mpButtonBox;
    QLineEdit *ename;
    QLineEdit *eurl;
    StringListModel *model = 0;
    QMap<QString,QVariant> links;
    void initModelData();
    void saveLinks(QString name, QString urls);
    void deleteLinks(QString name);
    bool ValidURL(const QString &url);
};


#endif // WebConfigPage_H
