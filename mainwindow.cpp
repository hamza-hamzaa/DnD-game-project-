// Main window: builds the stacked pages (menu → hero → dungeon → ending), draws the map from GameController,
// and updates HP / log text when the game state changes.

#include "mainwindow.h"
#include <QFont>
#include <QPalette>
#include <QMessageBox>
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <algorithm>

// Resource path for the hero picture that matches race + class (see resources.qrc under /enemies/forms).
QString MainWindow::findPlayerSpritePath() const
{
    if (!player) {
        return QString();
    }
    const QString race = player->getRace().trimmed();
    const QString style = player->getStyle().trimmed();
    if (race == "Human" && style == "Warrior")
        return ":/enemies/forms/human warrior.png";
    else if (race == "Human" && style == "Fire Mage")
        return ":/enemies/forms/human fire mage.png";
    else if (race == "Human" && style == "Ice Mage")
        return ":/enemies/forms/human ice mage.png";
    else if (race == "Elf" && style == "Warrior")
        return ":/enemies/forms/elf warrior.png";
    else if (race == "Elf" && style == "Fire Mage")
        return ":/enemies/forms/elf fire mage.png";
    else if (race == "Elf" && style == "Ice Mage")
        return ":/enemies/forms/elf ice mage.png";
    else if (race == "Dwarf" && style == "Warrior")
        return ":/enemies/forms/dwarf warrior.png";
    else if (race == "Dwarf" && style == "Fire Mage")
        return ":/enemies/forms/dwarf fire mage.png";
    else if (race == "Dwarf" && style == "Ice Mage")
        return ":/enemies/forms/dwarf ice mage.png";
    return QString();
}

// Resource path for this enemy's portrait (goblin / orc / skeleton).
QString MainWindow::findEnemySpritePath(const Enemy& enemy) const
{
    const QString type = enemy.getType().trimmed();

    if (type == "Goblin")
        return ":/enemies/goblin.png";
    else if (type == "Orc")
        return ":/enemies/orc.png";
    else if (type == "Skeleton")
        return ":/enemies/skeleton.png";

    return QString();
}


    // ─────────────────────────────────────────────
    //  Constructor / Destructor
    // ─────────────────────────────────────────────

    MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),animTimer(new QTimer(this))
{
    setWindowTitle("Dungeon Realms");
    setFixedSize(800, 650);

    // dark background for the whole window
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(15, 12, 20));
    setPalette(pal);

    // stacked widget holds all pages
    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    buildStartPage();
    buildSelectPage();
    buildGamePage();
    buildEndPage();

    stack->addWidget(startPage);   // index 0
    stack->addWidget(selectPage);  // index 1
    stack->addWidget(gamePage);    // index 2
    stack->addWidget(endPage);     // index 3

    stack->setCurrentIndex(0);

    // enemy animation tick: every 600 ms enemies "pulse"
    connect(animTimer, &QTimer::timeout, this, &MainWindow::tickEnemyAnim);
    animTimer->start(600);
}

MainWindow::~MainWindow()
{
    delete gc;
    delete player;
}

// ─────────────────────────────────────────────
//  Page builders
// ─────────────────────────────────────────────

void MainWindow::buildStartPage()
{
    startPage = new QWidget;
    startPage->setStyleSheet("background-color: #0f0c14;");

    QVBoxLayout* root = new QVBoxLayout(startPage);
    root->setAlignment(Qt::AlignCenter);
    root->setSpacing(24);

    // title
    QLabel* title = new QLabel("⚔  DUNGEON & DRAGONS  ⚔");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "color: #e8c96a;"
        "font-size: 36px;"
        "font-weight: bold;"
        "font-family: 'Georgia', serif;"
        "letter-spacing: 4px;"
        );
    root->addWidget(title);

    QLabel* sub = new QLabel("A turn-based dungeon adventure");
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color: #9a8060; font-size: 14px; font-family: 'Georgia', serif;");
    root->addWidget(sub);

    root->addSpacing(20);

    // name input
    QLabel* nameLabel = new QLabel("Enter your name:");
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("color: #c8b88a; font-size: 16px;");
    root->addWidget(nameLabel);

    nameInput = new QLineEdit;
    nameInput->setFixedWidth(260);
    nameInput->setFixedHeight(40);
    nameInput->setAlignment(Qt::AlignCenter);
    nameInput->setPlaceholderText("Adventurer name...");
    nameInput->setStyleSheet(
        "background: #1e1828;"
        "color: #e8c96a;"
        "border: 1px solid #6a5030;"
        "border-radius: 6px;"
        "font-size: 16px;"
        "padding: 4px 12px;"
        );

    QHBoxLayout* inputRow = new QHBoxLayout;
    inputRow->setAlignment(Qt::AlignCenter);
    inputRow->addWidget(nameInput);
    root->addLayout(inputRow);

    root->addSpacing(10);

    // start button
    startBtn = new QPushButton("BEGIN ADVENTURE");
    startBtn->setFixedSize(220, 48);
    startBtn->setCursor(Qt::PointingHandCursor);
    startBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #6a3a10;"
        "  color: #e8c96a;"
        "  border: 1px solid #c8902a;"
        "  border-radius: 8px;"
        "  font-size: 15px;"
        "  font-weight: bold;"
        "  font-family: 'Georgia', serif;"
        "  letter-spacing: 2px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #8a4e18;"
        "  border-color: #e8c96a;"
        "}"
        "QPushButton:pressed { background-color: #4a2808; }"
        );
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    QHBoxLayout* btnRow = new QHBoxLayout;
    btnRow->setAlignment(Qt::AlignCenter);
    btnRow->addWidget(startBtn);
    root->addLayout(btnRow);
}

void MainWindow::buildSelectPage()
{
    selectPage = new QWidget;
    selectPage->setStyleSheet("background-color: #0f0c14;");

    QVBoxLayout* root = new QVBoxLayout(selectPage);
    root->setAlignment(Qt::AlignCenter);
    root->setSpacing(20);

    QLabel* title = new QLabel("Choose Your Hero");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "color: #e8c96a; font-size: 28px; font-weight: bold;"
        "font-family: 'Georgia', serif; letter-spacing: 3px;"
        );
    root->addWidget(title);

    root->addSpacing(16);

    // race row
    QLabel* raceLabel = new QLabel("Race:");
    raceLabel->setStyleSheet("color: #c8b88a; font-size: 15px;");
    raceBox = new QComboBox;
    raceBox->addItems({"Human", "Elf", "Dwarf"});
    raceBox->setFixedWidth(200);
    raceBox->setFixedHeight(36);
    raceBox->setStyleSheet(
        "QComboBox {"
        "  background: #1e1828; color: #e8c96a;"
        "  border: 1px solid #6a5030; border-radius: 6px;"
        "  font-size: 14px; padding-left: 10px;"
        "}"
        "QComboBox QAbstractItemView { background: #1e1828; color: #e8c96a; }"
        );

    QHBoxLayout* raceRow = new QHBoxLayout;
    raceRow->setAlignment(Qt::AlignCenter);
    raceRow->setSpacing(12);
    raceRow->addWidget(raceLabel);
    raceRow->addWidget(raceBox);
    root->addLayout(raceRow);

    // style row
    QLabel* styleLabel = new QLabel("Class:");
    styleLabel->setStyleSheet("color: #c8b88a; font-size: 15px;");
    styleBox = new QComboBox;
    styleBox->addItems({"Warrior", "Fire Mage", "Ice Mage"});
    styleBox->setFixedWidth(200);
    styleBox->setFixedHeight(36);
    styleBox->setStyleSheet(raceBox->styleSheet());

    QHBoxLayout* styleRow = new QHBoxLayout;
    styleRow->setAlignment(Qt::AlignCenter);
    styleRow->setSpacing(12);
    styleRow->addWidget(styleLabel);
    styleRow->addWidget(styleBox);
    root->addLayout(styleRow);

    // class description hint
    QLabel* hint = new QLabel("Warrior: tanky melee  |  Fire Mage: high damage  |  Ice Mage: balanced");
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet("color: #6a5840; font-size: 12px;");
    root->addWidget(hint);

    root->addSpacing(16);

    selectBtn = new QPushButton("ENTER THE DUNGEON");
    selectBtn->setFixedSize(240, 48);
    selectBtn->setCursor(Qt::PointingHandCursor);
    selectBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #1a3a6a;"
        "  color: #8ac8ff;"
        "  border: 1px solid #3a70c0;"
        "  border-radius: 8px;"
        "  font-size: 15px; font-weight: bold;"
        "  font-family: 'Georgia', serif; letter-spacing: 2px;"
        "}"
        "QPushButton:hover { background-color: #224888; border-color: #8ac8ff; }"
        "QPushButton:pressed { background-color: #102040; }"
        );
    connect(selectBtn, &QPushButton::clicked, this, &MainWindow::onSelectClicked);

    QHBoxLayout* btnRow = new QHBoxLayout;
    btnRow->setAlignment(Qt::AlignCenter);
    btnRow->addWidget(selectBtn);
    root->addLayout(btnRow);
}

void MainWindow::buildGamePage()
{
    gamePage = new QWidget;
    gamePage->setStyleSheet("background-color: #0a0810;");
    gamePage->setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout* root = new QVBoxLayout(gamePage);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    // ── HUD bar at top ──
    QHBoxLayout* hud = new QHBoxLayout;
    hud->setSpacing(16);

    levelLabel = new QLabel("Level 1");
    levelLabel->setStyleSheet(
        "color: #e8c96a; font-size: 15px; font-weight: bold;"
        "font-family: 'Georgia', serif;"
        );
    hud->addWidget(levelLabel);

    hud->addStretch();

    QLabel* hpTitle = new QLabel("HP:");
    hpTitle->setStyleSheet("color: #c8b88a; font-size: 14px;");
    hud->addWidget(hpTitle);

    hpBar = new QProgressBar;
    hpBar->setFixedWidth(160);
    hpBar->setFixedHeight(18);
    hpBar->setRange(0, 100);
    hpBar->setValue(100);
    hpBar->setTextVisible(false);
    hpBar->setStyleSheet(
        "QProgressBar { background: #2a1818; border: 1px solid #6a2020; border-radius: 4px; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "  stop:0 #c03020, stop:1 #e84030); border-radius: 3px; }"
        );
    hud->addWidget(hpBar);

    hpLabel = new QLabel("100 / 100");
    hpLabel->setStyleSheet("color: #e87060; font-size: 13px;");
    hud->addWidget(hpLabel);

    root->addLayout(hud);

    // ── Graphics view (the dungeon) ──
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QBrush(QColor(10, 8, 16)));

    view = new QGraphicsView(scene, gamePage);
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setStyleSheet(
        "QGraphicsView {"
        "  border: 2px solid #3a2a50;"
        "  background-color: #0a0810;"
        "}"
        );
    root->addWidget(view, 1);

    // ── Log label ──
    logLabel = new QLabel("Use arrow keys or WASD to move.");
    logLabel->setAlignment(Qt::AlignCenter);
    logLabel->setWordWrap(true);
    logLabel->setFixedHeight(38);
    logLabel->setStyleSheet(
        "color: #c8b88a; font-size: 13px;"
        "background: #12101a;"
        "border: 1px solid #2a2040;"
        "border-radius: 4px;"
        "padding: 4px 8px;"
        );
    root->addWidget(logLabel);

    // ── Restart button ──
    restartBtn = new QPushButton("Restart Level");
    restartBtn->setFixedHeight(32);
    restartBtn->setCursor(Qt::PointingHandCursor);
    restartBtn->setStyleSheet(
        "QPushButton {"
        "  background: #2a1818; color: #e87060;"
        "  border: 1px solid #6a2020; border-radius: 5px;"
        "  font-size: 13px;"
        "}"
        "QPushButton:hover { background: #3a2020; border-color: #e87060; }"
        );
    connect(restartBtn, &QPushButton::clicked, this, &MainWindow::onRestartClicked);
    root->addWidget(restartBtn);
}

void MainWindow::buildEndPage()
{
    endPage = new QWidget;
    endPage->setStyleSheet("background-color: #0f0c14;");

    QVBoxLayout* root = new QVBoxLayout(endPage);
    root->setAlignment(Qt::AlignCenter);
    root->setSpacing(24);

    endMsg = new QLabel("GAME OVER");
    endMsg->setAlignment(Qt::AlignCenter);
    endMsg->setWordWrap(true);
    endMsg->setStyleSheet(
        "color: #e8c96a; font-size: 34px; font-weight: bold;"
        "font-family: 'Georgia', serif; letter-spacing: 4px;"
        );
    root->addWidget(endMsg);

    endRestartBtn = new QPushButton("PLAY AGAIN");
    endRestartBtn->setFixedSize(200, 48);
    endRestartBtn->setCursor(Qt::PointingHandCursor);
    endRestartBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #1e3a1e; color: #68e068;"
        "  border: 1px solid #3a7a3a; border-radius: 8px;"
        "  font-size: 15px; font-weight: bold;"
        "  font-family: 'Georgia', serif; letter-spacing: 2px;"
        "}"
        "QPushButton:hover { background-color: #284e28; border-color: #68e068; }"
        );
    connect(endRestartBtn, &QPushButton::clicked, [this]() {
        if (gc && player && gc->checkLose()) {
            gc->restartLevel();
            drawGrid();
            redrawEntities();
            updateHUD();
            showLog("Level restarted.");
            stack->setCurrentIndex(2);
            gamePage->setFocus();
            return;
        }

        delete gc;
        gc = nullptr;
        delete player;
        player = nullptr;
        stack->setCurrentIndex(0);
    });

    QHBoxLayout* btnRow = new QHBoxLayout;
    btnRow->setAlignment(Qt::AlignCenter);
    btnRow->addWidget(endRestartBtn);
    root->addLayout(btnRow);
}

// ────────────────────────────────────────────
//  Slot: onStartClicked (Start page)
// ────────────────────────────────────────────

void MainWindow::onStartClicked()
{
    QString name = nameInput->text().trimmed();
    if (name.isEmpty()) {
        nameInput->setPlaceholderText("Please enter a name first!");
        return;
    }
    stack->setCurrentIndex(1); // go to character select
}

// ─────────────────────────────────────────────
//  Slot: onSelectClicked (Character select page)
// ─────────────────────────────────────────────

void MainWindow::onSelectClicked()
{
    QString name  = nameInput->text().trimmed();
    QString race  = raceBox->currentText();
    QString style = styleBox->currentText();

    // create the correct player subclass based on chosen style
    delete player;
    if (style == "Warrior") {
        player = new Warrior(name, race);
    } else if (style == "Fire Mage") {
        player = new FireMage(name, race);
    } else {
        player = new IceMage(name, race);
    }

    // create game controller and start
    delete gc;
    gc = new GameController(player, this);
    gc->startGame();
    gamePage->installEventFilter(gc);

    // build the visual grid for level 1
    drawGrid();
    redrawEntities();
    updateHUD();
    showLog("You enter the dungeon.  Move with arrow keys or WASD.");

    stack->setCurrentIndex(2); // show game page
    gamePage->setFocus();
}

// ─────────────────────────────────────────────
//  Slot: onRestartClicked (in-game restart)
// ─────────────────────────────────────────────

void MainWindow::onRestartClicked()
{
    if (!gc || !player) return;
    gc->restartLevel();
    drawGrid();
    redrawEntities();
    updateHUD();
    showLog("Level restarted.");
}

// ─────────────────────────────────────────────
//  Grid rendering
// ─────────────────────────────────────────────

void MainWindow::drawGrid()
{
    if (!gc || !gc->getLevel()) return;

    scene->clear();
    cellItems.clear();
    enemySprites.clear();
    playerSprite  = nullptr;
    playerIcon    = nullptr;

    Grid& grid = gc->getLevel()->getGrid();      // NOTE: expose getLevel() in GameController
    int rows = grid.getRows();
    int cols = grid.getCols();

    cellItems.resize(rows);

    for (int r = 0; r < rows; r++) {
        cellItems[r].resize(cols);
        for (int c = 0; c < cols; c++) {
            Cell& cell = grid.getCell(r, c);

            // pick tile color
            QColor tileColor = QColor(55, 45, 80);     // normal floor
            if (cell.hasPotion) {
                tileColor = QColor(35, 120, 55);       // potion -- green
            }
            if (cell.hasTrap) {
                tileColor = QColor(60, 20, 10);        // trap -- dark red
            }
            else if(cell.hasEquipment){
                tileColor = QColor(0, 0, 100);
            }

            QGraphicsRectItem* rect = new QGraphicsRectItem(
                c * cellSize, r * cellSize, cellSize - 2, cellSize - 2
                );
            rect->setBrush(QBrush(tileColor));
            rect->setPen(QPen(QColor(100, 80, 130), 1));
            scene->addItem(rect);
            cellItems[r][c] = rect;

            if (cell.hasTrap) {
                QGraphicsTextItem* lbl = scene->addText("⚠");
                lbl->setDefaultTextColor(QColor(220, 80, 40));
                lbl->setFont(QFont("Segoe UI Emoji", 16));
                lbl->setPos(c * cellSize + 14, r * cellSize + 10);
            }
        }
    }
    int wt = 6;
    QColor wallColor = QColor(180, 150, 220);

    const QVector<GameController::WallSegment> segments = gc->wallSegmentsForGrid(rows, cols);
    for (int i = 0; i < segments.size(); i++) {
        const GameController::WallSegment& segment = segments[i];
        const bool verticalDivider = segment.rowA == segment.rowB;
        const int minRow = std::min(segment.rowA, segment.rowB);
        const int minCol = std::min(segment.colA, segment.colB);

        QGraphicsRectItem* wall = nullptr;
        if (verticalDivider) {
            const int x = (minCol + 1) * cellSize - wt / 2;
            const int y = minRow * cellSize;
            wall = new QGraphicsRectItem(x, y, wt, cellSize);
        } else {
            const int x = minCol * cellSize;
            const int y = (minRow + 1) * cellSize - wt / 2;
            wall = new QGraphicsRectItem(x, y, cellSize, wt);
        }

        wall->setBrush(QBrush(wallColor));
        wall->setPen(Qt::NoPen);
        scene->addItem(wall);
    }

    // fit scene in view
    scene->setSceneRect(0, 0, cols * cellSize, rows * cellSize);
    view->setScene(scene);
    view->centerOn(0, 0);
}

// ─────────────────────────────────────────────
//  Entity rendering (player + enemies)
// ─────────────────────────────────────────────

void MainWindow::redrawEntities()
{
    if (!gc || !gc->getLevel() || !player) return;

    // remove old enemy sprites
    for (int i = 0; i < enemySprites.size(); i++) {
        scene->removeItem(enemySprites[i]);
    }
    enemySprites.clear();

    // remove old player visuals
    if (playerSprite) {
        scene->removeItem(playerSprite);
        playerSprite = nullptr;
    }
    if (playerIcon) {
        scene->removeItem(playerIcon);
        playerIcon = nullptr;
    }

    const int spriteSize = cellSize - 16;
    const int spriteOffset = (cellSize - spriteSize) / 2;

    // draw enemies
    std::vector<Enemy>& enemies = gc->getLevel()->getEnemies();

    for (size_t i = 0; i < enemies.size(); i++) {
        int er = enemies[i].getRow();
        int ec = enemies[i].getCol();

        QString enemyPath = findEnemySpritePath(enemies[i]);

        if (!enemyPath.isEmpty()) {
            QPixmap sprite(enemyPath);

            if (!sprite.isNull()) {
                QPixmap scaledSprite = sprite.scaled(
                    spriteSize,
                    spriteSize,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    );

                QGraphicsPixmapItem* enemyItem = scene->addPixmap(scaledSprite);

                qreal offsetX = (cellSize - scaledSprite.width()) / 2.0;
                qreal offsetY = (cellSize - scaledSprite.height()) / 2.0;

                enemyItem->setPos(ec * cellSize + offsetX,
                                  er * cellSize + offsetY);

                enemySprites.push_back(enemyItem);
            }
        }
    }

    // draw player
    int pr = player->getRow();
    int pc = player->getCol();

    const QString spritePath = findPlayerSpritePath();

    if (!spritePath.isEmpty()) {
        QPixmap sprite(spritePath);

        if (!sprite.isNull()) {
            QPixmap scaledSprite = sprite.scaled(
                spriteSize,
                spriteSize,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );

            playerSprite = scene->addPixmap(scaledSprite);

            qreal offsetX = (cellSize - scaledSprite.width()) / 2.0;
            qreal offsetY = (cellSize - scaledSprite.height()) / 2.0;

            playerSprite->setPos(pc * cellSize + offsetX,
                                 pr * cellSize + offsetY);
            return;
        }
    }

    // fallback player icon if sprite missing
    QString playerEmoji;
    if (player->getStyle() == "Warrior")        playerEmoji = "⚔";
    else if (player->getStyle() == "Fire Mage") playerEmoji = "🔥";
    else                                        playerEmoji = "❄";

    playerIcon = scene->addText(playerEmoji);
    playerIcon->setFont(QFont("Segoe UI Emoji", 32));
    playerIcon->setPos(pc * cellSize + spriteOffset / 2, pr * cellSize + 2);
}
// ─────────────────────────────────────────────
//  HUD update
// ─────────────────────────────────────────────

void MainWindow::updateHUD()
{
    if (!player || !gc) return;

    int hp    = player->getHealth();
    int maxHp = player->getMaxHealth();

    hpBar->setMaximum(maxHp);
    hpBar->setValue(qMax(0, hp));
    hpLabel->setText(QString::number(hp) + " / " + QString::number(maxHp));

    levelLabel->setText("Level " + QString::number(gc->getLevelNumber()));
}

// ─────────────────────────────────────────────
//  Log message
// ─────────────────────────────────────────────

void MainWindow::showLog(const QString& msg)
{
    logLabel->setText(msg);
}

// ─────────────────────────────────────────────
//  Win / Lose check
// ─────────────────────────────────────────────

void MainWindow::checkEndConditions()
{
    if (!gc || !player) return;
    if (!gc->isLevelStarted()) return;  // don't check until player has moved

    if (gc->checkLose()) {
        endMsg->setText("💀  DEFEATED  💀\n\n" + player->getName() + " fell in the darkness...");
        endRestartBtn->setText("RETRY LEVEL");
        endMsg->setStyleSheet(
            "color: #e85050; font-size: 28px; font-weight: bold;"
            "font-family: 'Georgia', serif; letter-spacing: 3px;"
            );
        stack->setCurrentIndex(3);
        return;
    }


    if (gc->checkWin()) {
        endMsg->setText("⚔  VICTORY!  ⚔\n\nWell done, " + player->getName() + "!\nYou cleared the dungeon and reached the exit.");
        endRestartBtn->setText("PLAY AGAIN");
        endMsg->setStyleSheet(
            "color: #e8c96a; font-size: 28px; font-weight: bold;"
            "font-family: 'Georgia', serif; letter-spacing: 3px;"
            );
        stack->setCurrentIndex(3);
    }
}
// ─────────────────────────────────────────────
//  Enemy animation tick (visual pulse effect)
// ─────────────────────────────────────────────

void MainWindow::tickEnemyAnim()
{
    static bool visible = true;

    for (int i = 0; i < enemySprites.size(); i++) {
        enemySprites[i]->setOpacity(visible ? 1.0 : 0.7);
    }

    visible = !visible;
}
