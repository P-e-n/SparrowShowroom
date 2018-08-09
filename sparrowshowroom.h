#ifndef SPARROWSHOWROOM_H
#define SPARROWSHOWROOM_H

#include <QMainWindow>

#include <QLocalSocket>
#include <QDataStream>
#include <QGraphicsScene>

class QLocalServer;

namespace Ui {
class SparrowShowroom;
}

class SparrowShowroom : public QMainWindow
{
    Q_OBJECT

public:
    explicit SparrowShowroom(QWidget *parent = 0);
    ~SparrowShowroom();

public slots:
    void on_log_added(QString log);

signals:
    void add_log(QString log);

private:
    void clientConnected();
    void handleSocketStateChanged(QLocalSocket::LocalSocketState socketState);
    void readSocketData();
    void showImageOnView(QString imagePath, int index);

    Ui::SparrowShowroom *ui;
    QLocalServer* server;
    QLocalSocket* socket;
    QDataStream socketDataStream;
    quint32 blockSize;

    //ToDo: Use QList to make this easy extentable
    QGraphicsScene * graphicsScene1 = new QGraphicsScene();
    QGraphicsScene * graphicsScene2 = new QGraphicsScene();
    QGraphicsScene * graphicsScene3 = new QGraphicsScene();
    QGraphicsScene * graphicsScene4 = new QGraphicsScene();

};

#endif // SPARROWSHOWROOM_H
