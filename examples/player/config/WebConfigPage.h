#ifndef WebConfigPage_H
#define WebConfigPage_H

#include "ConfigPageBase.h"

class QCheckBox;
class QTextEdit;
class WebConfigPage : public ConfigPageBase
{
public:
    WebConfigPage();
    virtual QString name() const;
public slots:
    virtual void apply(); //store the values on ui. call Config::xxx
    virtual void cancel(); //cancel the values on ui. values are from Config
    virtual void reset(); //reset to default

private:
    QCheckBox *m_enable;
    QTextEdit *m_options;
};

#endif // WebConfigPage_H
