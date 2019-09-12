#include <QNetworkReply>
#include <QTimer>
#include <QDebug>

#include "qhttp.h"

#define QHTTP_CB "QHTTPCB"

Q_DECLARE_METATYPE(QHttp::RequestCallback)

QHttp *QHttp::getInstance()
{
    static QHttp q12306Network;
    return &q12306Network;
}

QHttp::QHttp(QObject *parent) : QObject(parent)
{
    // auto redirect
    m_networkManage.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    connect(&m_networkManage, &QNetworkAccessManager::finished, this, &QHttp::onHttpFinished);
}

QHttp::~QHttp()
{

}

void QHttp::testHttp()
{
    RequestParam data;
    data.url = "https://www.baidu.com/";

    //HeaderPair pair;
    //pair.first = "Accept-Encoding";
    //pair.second = "gzip, deflate";
    //data.headerList.append(pair);
    sendRequest(data, [](bool success, const QByteArray& data, const QString &error){
        if (success) {
            qDebug() << data;
        } else {
            qDebug() << error;
        }
    });
}

void QHttp::onHttpFinished(QNetworkReply *reply)
{        
    if (!reply) {
        Q_ASSERT(false);
        return;
    }
    reply->deleteLater();
    QHttp::RequestCallback cb = nullptr;
    QVariant var = reply->property(QHTTP_CB);
    if (var.canConvert<QHttp::RequestCallback>()) {
        cb = var.value<QHttp::RequestCallback>();
    }
    if (!cb) {
        Q_ASSERT(false);
        return;
    }
    if (QNetworkReply::NoError != reply->error()) {        
        cb(false, QByteArray(), reply->errorString());
        return;
    }

    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (200 != variant.toInt()) {
        cb(false, QByteArray(), QString("http state code %1").arg(variant.toInt()));
    }

    QByteArray byte = reply->readAll();
    //qDebug() << QString::fromUtf8(byte).left(1024);
    cb(true, byte, "");
}

bool QHttp::sendRequest(const RequestParam& param, const RequestCallback &cb)
{    
    if (!checkUrl(param.url)) {
        return false;
    }

    QNetworkRequest request;
    setDefaultHeader(request);

    HeadersList::ConstIterator it = param.headerList.constBegin();
    HeadersList::ConstIterator end = param.headerList.constEnd();
    for ( ; it != end; ++it) {
        request.setRawHeader(it->first.toUtf8(), it->second.toUtf8());
    }

    request.setUrl(QUrl(param.url));
    QNetworkReply* reply = Q_NULLPTR;
    if (param.postData.isEmpty()) {
        reply = m_networkManage.get(request);
    } else {
        QByteArray byte = param.postData.toUtf8();
        request.setHeader(QNetworkRequest::ContentLengthHeader, byte.size());
        reply = m_networkManage.post(request, byte);
    }
    if (reply) {
        reply->setProperty(QHTTP_CB, QVariant::fromValue(cb));

        int timeout = param.timeout;
        QTimer* timer = new QTimer(reply);
        connect(timer, &QTimer::timeout, reply, &QNetworkReply::abort);
        timer->setSingleShot(true);
        if (0 > timeout) {
            timeout = 5000;
        }
        timer->start(timeout);
    }
    return true;
}

void QHttp::setDefaultHeader(QNetworkRequest &request)
{
    // default ssl config
    QSslConfiguration sslConfig;
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);

    // 这里不要设置，qt会自动设置"gzip, deflate"并后面自动解压，如果这里手动设置了"gzip, deflate"后面反而需要自己手动解压
    //https://stackoverflow.com/questions/2340548/does-qnetworkmanager-get-accept-compressed-replies-by-default

    // request.setRawHeader(QByteArray("Accept-Encoding"), QByteArray("gzip, deflate"));
    request.setRawHeader(QByteArray("Content-Type"), QByteArray("application/x-www-form-urlencoded; charset=UTF-8"));
    request.setRawHeader(QByteArray("User-Agent"), QByteArray("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.119 Safari/537.36"));
}

bool QHttp::checkUrl(const QString &url)
{
    if (url.isEmpty()) {        
        return false;
    }

    const QUrl newUrl = QUrl::fromUserInput(url);
    if (!newUrl.isValid()) {        
        return false;
    }
    return true;
}
