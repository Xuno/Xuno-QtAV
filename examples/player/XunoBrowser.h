#ifndef XUNOBROWSER_H
#define XUNOBROWSER_H

#include <QtWidgets>
#include <QUrl>
#include <QLabel>
#include <QDialog>
#include <QWebEngineProfile>
#include <QWebEngineView>

class QWebEngineView;
class QLineEdit;
class QLabel;

//----------------------------------------------------------

class myWebEnginePage  : public QWebEnginePage
{
    Q_OBJECT
signals:
    void onClick(QUrl url);
public:
    explicit myWebEnginePage(QWebEngineProfile *profile, QObject *parent = Q_NULLPTR);
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

//-----------------------------------------------------------


class XunoBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit XunoBrowser(QWidget *parent = Q_NULLPTR, const QString &version="none");
    ~XunoBrowser();
    void setUrl(const QUrl& url);
    void setXUNOContentUrl(const QString &url);
    QUrl getClikedUrl();
    void setXunoVersion(const QString &value);

signals:
    void clicked();

protected slots:

    void adjustTitle();
    void adjustBrowserSize();
    void setProgress(int p);
    void finishLoading(bool);
    void linkClicked(QUrl url);

private:
    QWebEngineView *view;
    QWebEngineProfile* profile;
    myWebEnginePage *page;
    QUrl remove_fistsubdomain(QUrl url);
    bool same_site_domain(const QUrl &url1, const QUrl &url2);
    //    QLineEdit *locationEdit;
    //    QAction *rotateAction;
    QLabel *loading;
    int progress;
    QUrl clickedUrl;
    bool clickedMediaUrl=false;
    QString XUNOContentUrl;
    QString XunoVersion;
    virtual void hideEvent(QHideEvent* e);
    virtual void showEvent(QShowEvent* e);
    virtual void resizeEvent(QResizeEvent* e);

};






#endif // XUNOBROWSER_H
