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

//main window class that handles all the UI and rendering
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    //intercepts key presses so arrow keys dont get stolen by the scroll view
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    //called when user clicks begin adventure on the start page
    void onStartClicked();
    //called when user picks race and class and clicks enter dungeon
    void onSelectClicked();
    //restarts the current level
    void onRestartClicked();
    //fires every 200ms to keep the timer and hud updated
    void onHudTick();
    //writes current game state to a file
    void onSaveGameClicked();
    //loads game state from file and redraws everything
    void onLoadGameClicked();

private:
    //stacked widget that switches between the 4 pages
    QStackedWidget* stack;

    //page 0 start screen
    QWidget* startPage;
    QLineEdit* nameInput;
    QPushButton* startBtn;

    //page 1 character select
    QWidget* selectPage;
    QComboBox* raceBox;
    QComboBox* styleBox;
    QPushButton* selectBtn;

    //page 2 the game with the dungeon grid
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

    //page 3 win or lose screen
    QWidget* endPage;
    QLabel* endMsg;
    QPushButton* endRestartBtn;

    //game objects, both null until the user starts a game
    Player* player = nullptr;
    GameController* gc = nullptr;

    //timer that ticks every 200ms to update the hud while playing
    QTimer hudTimer;
    QSoundEffect uiBlip;

    //grid rendering stuff
    int cellSize = 60; //size of each tile in pixels, recalculated each draw
    QVector<QVector<QGraphicsRectItem*>> cellItems; //holds the tile rects so we can update them
    QGraphicsPixmapItem* playerSprite = nullptr;
    QVector<QGraphicsPixmapItem*> enemySprites;
    QGraphicsTextItem* playerIcon = nullptr; //fallback emoji if sprite is missing

    //tile images loaded once at startup to avoid lag
    QPixmap floorPix;
    QPixmap trapPix;
    QPixmap potionPix;
    QPixmap exitPix;
    QPixmap wallTopPix;
    QPixmap wallSidePix;
    QPixmap keyPix;
    QPixmap floorSkullPix; //floor decoration variants
    QPixmap floorWebPix;
    QPixmap floorBonesPix;
    bool pixmapsLoaded = false; //so we only load from disk once

    //builds each page layout, called once in the constructor
    void buildStartPage();
    void buildSelectPage();
    void buildGamePage();
    void buildEndPage();

    //redraws the full grid tiles and walls each turn
    void drawGrid();
    //redraws just the player and enemy sprites on top of the grid
    void redrawEntities();
    //updates hp bar, level label, key status, timer, enemy count
    void updateHUD();
    //shows a message in the log bar at the bottom
    void showLog(const QString& msg);
    //checks if player died or won and switches to the end page
    void checkEndConditions();

    //helper to find the forms folder regardless of working directory
    QString formsDirectory() const;
    //returns the default path for the save file
    QString defaultSaveFilePath() const;
    //picks the right sprite image based on player race and class combo
    QString findPlayerSpritePath() const;
    //picks the right sprite image for a given enemy type
    QString findEnemySpritePath(const Enemy& enemy) const;
    //plays the step sound if it loaded successfully
    void playUiBlip();
};

#endif
