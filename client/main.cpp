#include <QApplication>
#include <QMainWindow>
#include <QStatusBar>
#include <QTcpSocket>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>

int
main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QMainWindow *w = new QMainWindow();
    w->resize(600, 600);
    QStatusBar *statusBar = new QStatusBar(w);
    QTcpSocket *socket = new QTcpSocket(w);
    QLabel *amountLabel = new QLabel(w);
    amountLabel->setText("Amount:");
    amountLabel->move(50, 100);
    QLineEdit *amountEdit = new QLineEdit(w);
    amountEdit->setFixedSize(300, 100);
    amountEdit->setAlignment(Qt::AlignTop);
    amountEdit->move(150, 100);
    QLabel *usernameLabel = new QLabel(w);
    usernameLabel->setText("Username:");
    usernameLabel->move(50, 200);
    QLineEdit *usernameEdit = new QLineEdit(w);
    usernameEdit->setFixedSize(300, 100);
    usernameEdit->setAlignment(Qt::AlignTop);
    usernameEdit->move(150, 200);
    QLabel *passwordLabel = new QLabel(w);
    passwordLabel->setText("Password:");
    passwordLabel->move(50, 300);
    QLineEdit *passwordEdit = new QLineEdit(w);
    passwordEdit->setFixedSize(300, 100);
    passwordEdit->setAlignment(Qt::AlignTop);
    passwordEdit->move(150, 300);
    QPushButton *pushButton = new QPushButton(w);
    pushButton->move(250, 400);
    pushButton->setText("Send");
    statusBar->showMessage("Disconnected.");
    QObject::connect(socket, &QTcpSocket::disconnected, w, [=] {
        statusBar->showMessage("Disconnected.");
    });
    QObject::connect(socket, &QTcpSocket::connected, w, [=] {
        statusBar->showMessage("Connected.");
        QObject::connect(pushButton, &QPushButton::clicked, w, [=] {
            QString msg = usernameEdit->text() + ":" + passwordEdit->text() + ":" + amountEdit->text() + "\n";
            socket->write(msg.toUtf8());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();
            quint64 amount;
            socket->read(reinterpret_cast<char *>(&amount), sizeof(amount));
            bool ok;
            if(amount == amountEdit->text().toUInt(&ok) && ok) {
                QMessageBox::information(w, "QTCPClient", "Successfully sent amount: " + QString::number(amount));
            } else {
                QMessageBox::warning(w, "QTCPClient", "Either account balance is not enough or the account is invalid.");
            }
        });
    });
    socket->connectToHost(QHostAddress::LocalHost, 1337);
    socket->waitForConnected();
    w->setStatusBar(statusBar);
    w->show();
    return a.exec();
}
