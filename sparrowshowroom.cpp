#include "sparrowshowroom.h"
#include "ui_sparrowshowroom.h"
#include "logger.h"

#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>

SparrowShowroom::SparrowShowroom(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SparrowShowroom)
{
    server = new QLocalServer(this);
    socket = nullptr;
    if (!server->listen("sparrow-showroom")) {
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
    socketDataStream.setVersion(QDataStream::Qt_5_9);
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

//ToDo: Refactor the duplicated code.
void SparrowShowroom::showImageOnView(QString imagePath, int index)
{
    QImage image = QImage(imagePath);
    if (!image.isNull())
    {
        QPixmap pixmap = QPixmap::fromImage(image);
        if (index == 0)
        {
            graphicsScene1->clear();
            graphicsScene1->addPixmap(pixmap);
            ui->graphicsView_1->setScene(graphicsScene1);
            ui->graphicsView_1->fitInView(graphicsScene1->sceneRect(), Qt::KeepAspectRatio);
        }
        else if (index == 1)
        {
            graphicsScene2->clear();
            graphicsScene2->addPixmap(pixmap);
            ui->graphicsView_2->setScene(graphicsScene2);
            ui->graphicsView_2->fitInView(graphicsScene2->sceneRect(), Qt::KeepAspectRatio);
        }
        else if (index == 2)
        {
            graphicsScene3->clear();
            graphicsScene3->addPixmap(pixmap);
            ui->graphicsView_3->setScene(graphicsScene3);
            ui->graphicsView_3->fitInView(graphicsScene3->sceneRect(), Qt::KeepAspectRatio);
        }
        else if (index == 3)
        {
            graphicsScene4->clear();
            graphicsScene4->addPixmap(pixmap);
            ui->graphicsView_4->setScene(graphicsScene4);
            ui->graphicsView_4->fitInView(graphicsScene4->sceneRect(), Qt::KeepAspectRatio);
        }
    }
}

void SparrowShowroom::readSocketData()
{
    if (blockSize == 0) {
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

    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
    QJsonObject jObject = doc.object();
    foreach(QString key, jObject.keys()) {
        if (key == "Sparrow_Message") {
            QJsonValue sparrow_msg_obj = jObject.take("Sparrow_Message");
            QJsonObject obj = sparrow_msg_obj.toObject();
            QString cmd = obj["cmd"].toString();
            QString path = obj["path"].toString();
            int display_id = obj["display_id"].toInt();
            showImageOnView(path, display_id);
        }
    }
}
