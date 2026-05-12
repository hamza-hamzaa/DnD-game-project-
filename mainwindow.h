#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QStackedWidget>
#include <QVector>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QSoundEffect>
#include <QTimer>
#include "GameController.h"
#include "player.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onStartClicked();
    void onSelectClicked();
    void onRestartClicked();
    void onHudTick();
    void onSaveGameClicked();
    void onLoadGameClicked();

private:
    // -- pages --
    QStackedWidget* stack;

    // page 0: start screen
    QWidget* startPage;
    QLineEdit* nameInput;
    QPushButton* startBtn;

    // page 1: character select
    QWidget* selectPage;
    QComboBox* raceBox;
    QComboBox* styleBox;
    QPushButton* selectBtn;

    // page 2: game screen
    QWidget *gamePage;
    QGraphicsView* view;
    QGraphicsScene* scene;
    QLabel* hpLabel;
    QLabel* levelLabel;
    QLabel* keyLabel;
    QLabel* timeLabel;
    QLabel* enemiesLabel;
    QLabel* logLabel;
    QProgressBar* hpBar;
    QPushButton* restartBtn;
    QPushButton* saveGameBtn;
    QPushButton* loadGameBtn;
    // page 3: game over / victory
    QWidget* endPage;
    QLabel*endMsg;
    QPushButton* endRestartBtn;

    // -- game objects --
    Player* player = nullptr;
    GameController* gc = nullptr;
    QTimer hudTimer;
    QSoundEffect uiBlip;

    // -- grid rendering --
    int cellSize = 60;
    QVector<QVector<QGraphicsRectItem*>> cellItems; // visual tiles
    QGraphicsPixmapItem* playerSprite=nullptr;
    QVector<QGraphicsPixmapItem*> enemySprites;
    QGraphicsTextItem*playerIcon= nullptr;

    // -- helpers --
    void buildStartPage();
    void buildSelectPage();
    void buildGamePage();
    void buildEndPage();

    void drawGrid();
    void redrawEntities();
    void updateHUD();
    void showLog(const QString& msg);
    void checkEndConditions();
    QString formsDirectory() const;
    QString defaultSaveFilePath() const;
    QString findPlayerSpritePath() const;
    QString findEnemySpritePath(const Enemy& enemy) const;
    void playUiBlip();
};

#endif // MAINWINDOW_H
