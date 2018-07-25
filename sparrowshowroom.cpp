#include "sparrowshowroom.h"
#include "ui_sparrowshowroom.h"
#include "logger.h"

#include <QLocalServer>
#include <QLocalSocket>

SparrowShowroom::SparrowShowroom(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SparrowShowroom)
{
    server = new QLocalServer(this);
    socket = nullptr;
    if (!server->listen("sparrow-showroom")) {
        /*QMessageBox::critical(this, tr("Sparrow showroom"),
                              tr("Unable to start the server: %1.")
                              .arg(server->errorString()));*/
        close();
        return;
    }
    connect(this, SIGNAL(add_log(QString)), this, SLOT(on_log_added(QString)));
    ui->setupUi(this);
    log_ui = this;

    connect(server, &QLocalServer::newConnection, this, &SparrowShowroom::clientConnected);
}

SparrowShowroom::~SparrowShowroom()
{
    if (socket != nullptr) {
        qInfo("Closing client socket.");
        socket->disconnectFromServer();
    }
    server->close();
    delete ui;
}

void SparrowShowroom::on_log_added(QString log)
{
    //ui->logEdit->append(log);
    ui->logPlainTextEdit->appendPlainText(log);
}


void SparrowShowroom::clientConnected()
{
    if (socket != nullptr) {
        qInfo("One client is already connecting/connected.");
        return;
    }
    qInfo("Incoming client");
    QLocalSocket *clientConnection = server->nextPendingConnection();
    connect(clientConnection, QOverload<QLocalSocket::LocalSocketState>::of(&QLocalSocket::stateChanged),
            this, &SparrowShowroom::handleSocketStateChanged);
    connect(clientConnection, &QLocalSocket::readyRead, this, &SparrowShowroom::readSocketData);
    socket = clientConnection;
    blockSize = 0;
    socketDataStream.setDevice(socket);
    socketDataStream.setVersion(QDataStream::Qt_5_10);
}

void SparrowShowroom::handleSocketStateChanged(QLocalSocket::LocalSocketState socketState)
{
    switch (socketState) {
    case QLocalSocket::UnconnectedState:
        qInfo("The showroom socket is now unconnected.");
        break;
    case QLocalSocket::ConnectingState:
        qInfo("The showroom socket is connecting...");
        break;
    case QLocalSocket::ConnectedState:
        qInfo("The showroom socket is connected.");
        break;
    case QLocalSocket::ClosingState:
        qInfo("The showroom socket is closing.");
        socket = nullptr;
        break;
    default:
        qInfo("Unhandled showroom socket state");
    }
}

void SparrowShowroom::readSocketData()
{
    if (blockSize == 0) {
        // Relies on the fact that QDataStream serializes a quint32 into
        // sizeof(quint32) bytes
        if (socket->bytesAvailable() < (int)sizeof(quint32))
            return;
        socketDataStream >> blockSize;
    }

    if (socket->bytesAvailable() < blockSize || socketDataStream.atEnd())
        return;

    QString msg;
    socketDataStream >> msg;

    ui->sampleTextEdit->append(msg);

    blockSize = 0;

}
