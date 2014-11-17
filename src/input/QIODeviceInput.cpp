#include "QtAV/AVInput.h"
#include "QtAV/private/AVInput_p.h"
#include <QtCore/QIODevice>
#include "utils/Logger.h"

namespace QtAV {

class QIODeviceInput : public AVInput
{
public:
    void setIODevice(QIODevice *dev);
    QIODevice* device() const;
};

// qrc support
class QFileInput : public QIODeviceInput
{
public:
    void setFileName(const QString& name);
    QString fileName() const;
};

} //namespace QtAV
