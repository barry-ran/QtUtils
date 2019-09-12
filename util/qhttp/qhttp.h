#ifndef QHTTP_H
#define QHTTP_H
#include <QObject>
#include <QNetworkAccessManager>

class QHttp : public QObject
{
    Q_OBJECT
public:
    typedef QPair<QString, QString> HeaderPair;
    typedef QList<HeaderPair> HeadersList;    
    typedef std::function< void(bool, const QByteArray&, const QString &)> RequestCallback;

    typedef struct RequestParam{
        QString url = "";
        QString postData = "";
        int timeout = 5000;
        HeadersList headerList;
    } RequestParam;

    static QHttp* getInstance();
    bool sendRequest(const RequestParam& param, const RequestCallback &cb);
    void testHttp();

private:
    explicit QHttp(QObject *parent = nullptr);
    virtual ~QHttp();

private slots:
    void onHttpFinished(QNetworkReply *reply);

private:    
    void setDefaultHeader(QNetworkRequest &request);
    bool checkUrl(const QString& url);     

private:
    QNetworkAccessManager m_networkManage;
};

#endif // QHTTP_H
