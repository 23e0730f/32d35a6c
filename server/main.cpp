#include <QApplication>
#include <QMainWindow>
#include <QStatusBar>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextBrowser>
#include <QFile>
#include <QTextStream>
#include <QStringList>

int
main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QMainWindow *w = new QMainWindow();
    w->resize(600, 600);
    QStatusBar *statusBar = new QStatusBar(w);
    QTcpServer *server = new QTcpServer(w);
    QTextBrowser *textBrowser = new QTextBrowser(w);
    textBrowser->setFixedSize(450, 450);
    textBrowser->move(75, 50);
    QObject::connect(server, &QTcpServer::newConnection, w, [=] {
        QTcpSocket *socket = server->nextPendingConnection();
        if(socket != nullptr) {
            QObject::connect(socket, &QTcpSocket::readyRead, w, [=] {
                if(socket->canReadLine()) {
                    QString msg = QString(socket->readLine());
                    quint64 amount = 0;
                    QStringList msgList = msg.split(":");
                    if(msgList.size() == 3) {
                        QFile *file = new QFile("accounts.txt");
                        if(file->open(QIODevice::ReadWrite | QIODevice::Text)) {
                            QTextStream *textStream = new QTextStream(file);
                            QStringList lineList;
                            quint64 linePos = 0;
                            QString line;
                            bool ok;
                            do {
                                line = textStream->readLine();
                                if(line.isNull()) {
                                    break;
                                }
                                linePos = textStream->pos() - line.size() - 1;
                                lineList = line.split(":");
                            } while(msgList[0] != lineList[0] || msgList[1] != lineList[1] || msgList[2].toInt() > lineList[2].toInt(&ok) || !ok);
                            if(!line.isNull()) {
                                amount = msgList[2].toInt();
                                textBrowser->append("Username: " + msgList[0] + ", password: " + msgList[1] + ", amount: " + msgList[2]);
                                file->seek(linePos);
                                QString newLine = msgList[0] + ":" + msgList[1] + ":" + QString::number(amount - lineList[2].toInt()) + "\n";
                                file->write(newLine.toUtf8());
                            }
                            file->close();
                        }
                    }
                    socket->write(reinterpret_cast<const char *>(&amount), sizeof(amount));
                    socket->waitForBytesWritten();
                }
            });
        }
    });
    if(server->listen(QHostAddress::Any, 1337)) {
        statusBar->showMessage("Connected.");
    }
    w->setStatusBar(statusBar);
    w->show();
    return a.exec();
}
