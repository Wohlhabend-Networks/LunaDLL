#include "networkutils.h"
#include "qreplytimeout.h"
#include "qnetworkreplytimeoutexception.h"
#include "qnetworkreplyexception.h"
#include "../Common/qurlinvalidexception.h"
#include <QEventLoop>

Q_CONSTEXPR char timeoutURL[] = "http://google.com";


QByteArray NetworkUtils::getString(const QUrl &url, int timeout)
{
    if(!url.isValid())
        throw QUrlInvalidException(url);

    QNetworkAccessManager downloader;
    downloader.setNetworkAccessible(QNetworkAccessManager::Accessible);

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent" , "SMBX Launcher");

    QNetworkReply* reply = downloader.get(req);

    QReplyTimeout* timeoutControl = new QReplyTimeout(reply, timeout);

    QEventLoop e;
    QObject::connect(&downloader, &QNetworkAccessManager::finished, &e, &QEventLoop::quit);
    e.exec();

    qDebug() << "Error output: " << reply->error() << reply->errorString();

    reply->deleteLater();
    if(timeoutControl->isAborted())
        throw QNetworkReplyTimeoutException();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode >= 400)
        throw QNetworkReplyException(reply->error(), reply->errorString());

    QByteArray replyContent = reply->readAll();
    if(replyContent.isEmpty())
        throw QNetworkReplyException(reply->error(), reply->errorString());

    qDebug() << "Received data from " << url << ":" << replyContent;

    return replyContent;
}

bool NetworkUtils::checkInternetConnection(int timeout)
{
    try {
        getString(QUrl(timeoutURL), timeout);
    } catch (const QNetworkReplyTimeoutException&) {
        return false;
    }
    return true;
}


