#include "mainwindow.h"
#include <QFont>
#include <QPalette>
#include <QMessageBox>
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QPixmap>
#include <QTimer>
#include <QSettings>
#include <QUrl>
#include <algorithm>

QString MainWindow::formsDirectory() const
{
    const QString appForms = QDir(QCoreApplication::applicationDirPath()).filePath("forms");
    if (QFileInfo::exists(appForms)) {
        return appForms;
    }
    const QString cwdForms = QDir(QDir::currentPath()).filePath("forms");
    if (QFileInfo::exists(cwdForms)) {
        return cwdForms;
    }
    return appForms;
}

QString MainWindow::defaultSaveFilePath() const
{
    return QDir(QDir::currentPath()).absoluteFilePath(QStringLiteral("dungeon_save.txt"));
}

void MainWindow::playUiBlip()
{
    if (uiBlip.isLoaded()) {
        uiBlip.play();
    }
}

QString MainWindow::findPlayerSpritePath() const
{
    if (!player) {
        return QString();
    }

    const QString formsDir = formsDirectory();
    const QString race = player->getRace().trimmed();
    const QString style = player->getStyle().trimmed();

    QString fileName;

    if (race == "Human" && style == "Warrior") {
        fileName = "human warrior.png";
    } else if (race == "Human" && style == "Fire Mage") {
        fileName = "human fire mage.png";
    } else if (race == "Human" && style == "Ice Mage") {
        fileName = "human ice mage.png";
    } else if (race == "Elf" && style == "Warrior") {
        fileName = "elf warrior.png";
    } else if (race == "Elf" && style == "Fire Mage") {
        fileName = "elf fire mage.png";
    } else if (race == "Elf" && style == "Ice Mage") {
        fileName = "elf ice mage.png";
    } else if (race == "Dwarf" && style == "Warrior") {
        fileName = "dwarf warrior.png";
    } else if (race == "Dwarf" && style == "Fire Mage") {
        fileName = "dwarf fire mage.png";
    } else if (race == "Dwarf" && style == "Ice Mage") {
        fileName = "dwarf ice mage.png";
    } else if (race == "Half-Orc" && style == "Warrior") {
        fileName = "half-orc warrior.png";
    } else if (race == "Half-Orc" && style == "Fire Mage") {
        fileName = "half-orc fire mage.png";
    } else if (race == "Half-Orc" && style == "Ice Mage") {
        fileName = "half-orc ice mage.png";
    }

    if (fileName.isEmpty()) {
        return QString();
    }

    const QString qrcPath = QStringLiteral(":/forms/") + fileName;
    if (!QPixmap(qrcPath).isNull()) {
        return qrcPath;
    }

    const QString spritePath = QDir(formsDir).absoluteFilePath(fileName);
    if (QFileInfo::exists(spritePath)) {
        return spritePath;
    }

    return QString();
}
QString MainWindow::findEnemySpritePath(const Enemy& enemy) const
{
    const QString type = enemy.getType().trimmed();

    if (type == QStringLiteral("Goblin")) {
        return QStringLiteral(":/forms/goblin.png");
    }
    if (type == QStringLiteral("Orc")) {
        return QStringLiteral(":/forms/orc.png");
    }
    if (type == QStringLiteral("Skeleton")) {
        return QStringLiteral(":/forms/skeleton.png");
    }
    if (type == QStringLiteral("Lich")) {
        return QStringLiteral(":/forms/skeleton.png");
    }

    return QString();
}

// ─────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    uiBlip.setSource(QUrl(QStringLiteral("qrc:/sounds/step.wav")));
    uiBlip.setVolume(0.35f);

    connect(&hudTimer, &QTimer::timeout, this, &MainWindow::onHudTick);
    hudTimer.start(200);

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
    QLabel* hint = new QLabel(
        "Warrior / Fire Mage / Ice Mage — each plays differently.\n"
        "Racial ability (R): Human heals, Elf reveals hidden traps, Dwarf gains temporary armor. "
        "It recharges after a random number of moves (shown in HUD after you start).\n"
        "Find the hidden key, defeat all enemies, then reach the exit. Lower run time = better score.");
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet("color: #6a5840; font-size: 11px; max-width: 520px;");
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

    keyLabel = new QLabel("Key: No");
    keyLabel->setStyleSheet(
        "color: #c8b88a; font-size: 15px; font-weight: bold;"
        "font-family: 'Georgia', serif;"
        );
    hud->addWidget(keyLabel);

    timeLabel = new QLabel("Time: 0.0s");
    timeLabel->setStyleSheet(
        "color: #9ad8ff; font-size: 15px; font-weight: bold;"
        "font-family: 'Georgia', serif;"
        );
    hud->addWidget(timeLabel);

    enemiesLabel = new QLabel("Enemies: 0");
    enemiesLabel->setStyleSheet(
        "color: #c8b0e8; font-size: 15px; font-weight: bold;"
        "font-family: 'Georgia', serif;"
        );
    hud->addWidget(enemiesLabel);

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

    saveGameBtn = new QPushButton("Save");
    saveGameBtn->setFixedHeight(32);
    saveGameBtn->setCursor(Qt::PointingHandCursor);
    saveGameBtn->setStyleSheet(restartBtn->styleSheet());
    connect(saveGameBtn, &QPushButton::clicked, this, &MainWindow::onSaveGameClicked);

    loadGameBtn = new QPushButton("Load");
    loadGameBtn->setFixedHeight(32);
    loadGameBtn->setCursor(Qt::PointingHandCursor);
    loadGameBtn->setStyleSheet(restartBtn->styleSheet());
    connect(loadGameBtn, &QPushButton::clicked, this, &MainWindow::onLoadGameClicked);

    QHBoxLayout* bottomRow = new QHBoxLayout;
    bottomRow->addWidget(restartBtn);
    bottomRow->addWidget(saveGameBtn);
    bottomRow->addWidget(loadGameBtn);
    root->addLayout(bottomRow);
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

    delete player;
    if (style == "Warrior") {
        player = new Warrior(name, race);
    } else if (style == "Fire Mage") {
        player = new FireMage(name, race);
    } else {
        player = new IceMage(name, race);
    }

    delete gc;
    gc = new GameController(player);
    gc->startGame();

    QSettings qs;
    if (!qs.value(QStringLiteral("tutorial/dungeon_v1"), false).toBool()) {
        QMessageBox::information(
            this,
            QStringLiteral("How to play"),
            QStringLiteral(
                "Defeat every enemy, find the hidden key (same tile as a normal floor), then stand on the exit.\n"
                "Press R to use your race ability when the HUD shows R: READY (recharges after N moves; N is random each new run).\n"
                "Enemy turns resolve right after yours. Save / Load uses dungeon_save.txt in the working directory."));
        qs.setValue(QStringLiteral("tutorial/dungeon_v1"), true);
    }

    drawGrid();
    redrawEntities();
    updateHUD();
    showLog(QStringLiteral("You enter the dungeon. WASD / arrows to move. R = racial when ready. "
                           "Racial cooldown this run: %1 moves.")
                .arg(player->getRacialMovePeriod()));

    stack->setCurrentIndex(2);
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
//  Keyboard input
// ─────────────────────────────────────────────
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (stack->currentIndex() != 2 || !gc || !player) {
        QMainWindow::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_R) {
        const QString msg = gc->tryRacialAbility();
        if (!msg.isEmpty()) {
            drawGrid();
            redrawEntities();
            updateHUD();
            showLog(msg);
            playUiBlip();
        }
        QTimer::singleShot(800, this, &MainWindow::checkEndConditions);
        return;
    }

    int dx = 0;
    int dy = 0;
    QString dir;

    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        dx = -1;
        dir = QStringLiteral("north");
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        dx = 1;
        dir = QStringLiteral("south");
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        dy = -1;
        dir = QStringLiteral("west");
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        dy = 1;
        dir = QStringLiteral("east");
        break;
    default:
        QMainWindow::keyPressEvent(event);
        return;
    }

    const QString msg = gc->resolvePlayerTurn(dx, dy);
    drawGrid();
    redrawEntities();
    updateHUD();

    if (gc->hasPendingEnemyPhase()) {
        showLog(QStringLiteral("Enemy turn..."));
        QTimer::singleShot(320, this, [this, msg, dir]() {
            const QString enemyLog = gc->resolveEnemyTurn();
            drawGrid();
            redrawEntities();
            updateHUD();
            QString merged = msg;
            if (!enemyLog.isEmpty()) {
                if (!merged.isEmpty()) {
                    merged += QLatin1Char('\n');
                }
                merged += enemyLog;
            }
            if (!merged.isEmpty()) {
                showLog(merged);
                playUiBlip();
            } else {
                showLog(QStringLiteral("You move ") + dir + QLatin1Char('.'));
                playUiBlip();
            }
            QTimer::singleShot(800, this, &MainWindow::checkEndConditions);
        });
        return;
    }

    if (!msg.isEmpty()) {
        if (msg == QStringLiteral("Blocked!")) {
            showLog(QStringLiteral("A wall blocks your path."));
        } else {
            showLog(msg);
            playUiBlip();
        }
    } else {
        showLog(QStringLiteral("You move ") + dir + QLatin1Char('.'));
        playUiBlip();
    }

    QTimer::singleShot(800, this, &MainWindow::checkEndConditions);
}
// ─────────────────────────────────────────────
//  Grid rendering
// ─────────────────────────────────────────────

void MainWindow::drawGrid()
{
    if (!gc) return;

    scene->clear();
    cellItems.clear();
    enemySprites.clear();
    playerSprite  = nullptr;
    playerIcon    = nullptr;

    Grid& grid = gc->getLevel()->getGrid();
    const int rows = grid.getRows();
    const int cols = grid.getCols();

    cellSize = std::clamp(560 / std::max(rows, cols), 28, 60);

    cellItems.resize(rows);

    for (int r = 0; r < rows; r++) {
        cellItems[r].resize(cols);
        for (int c = 0; c < cols; c++) {
            Cell& cell = grid.getCell(r, c);

            const bool isExit = (r == rows - 1 && c == cols - 1);

            QColor tileColor = QColor(55, 45, 80);
            if (isExit) {
                tileColor = QColor(90, 70, 30);
            }
            if (cell.hasPotion) {
                tileColor = QColor(35, 120, 55);
            }
            if (cell.hasVisibleTrap) {
                tileColor = QColor(60, 20, 10);
            }

            QGraphicsRectItem* rect = new QGraphicsRectItem(
                c * cellSize, r * cellSize, cellSize - 2, cellSize - 2
                );
            rect->setBrush(QBrush(tileColor));
            rect->setPen(QPen(QColor(100, 80, 130), 1));
            scene->addItem(rect);
            cellItems[r][c] = rect;

            if (cell.hasVisibleTrap) {
                QGraphicsTextItem* lbl = scene->addText(QStringLiteral("⚠"));
                lbl->setDefaultTextColor(QColor(220, 80, 40));
                lbl->setFont(QFont("Segoe UI Emoji", 16));
                lbl->setPos(c * cellSize + 14, r * cellSize + 10);
            }
            if (isExit) {
                QGraphicsTextItem* ex = scene->addText(QStringLiteral("⛩"));
                ex->setDefaultTextColor(QColor(255, 210, 120));
                ex->setFont(QFont("Segoe UI Emoji", 14));
                ex->setPos(c * cellSize + 16, r * cellSize + 10);
            }
        }
    }

    const int wt = 6;
    const QColor wallColor(180, 150, 220);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            const Cell& cell = grid.getCell(r, c);
            if (r == 0 && cell.wallTop) {
                auto* wall = new QGraphicsRectItem(
                    c * cellSize, r * cellSize - wt / 2.0, cellSize, wt);
                wall->setBrush(QBrush(wallColor));
                wall->setPen(Qt::NoPen);
                scene->addItem(wall);
            }
            if (c == 0 && cell.wallLeft) {
                auto* wall = new QGraphicsRectItem(
                    c * cellSize - wt / 2.0, r * cellSize, wt, cellSize);
                wall->setBrush(QBrush(wallColor));
                wall->setPen(Qt::NoPen);
                scene->addItem(wall);
            }
            if (cell.wallRight) {
                auto* wall = new QGraphicsRectItem(
                    (c + 1) * cellSize - wt / 2.0, r * cellSize, wt, cellSize);
                wall->setBrush(QBrush(wallColor));
                wall->setPen(Qt::NoPen);
                scene->addItem(wall);
            }
            if (cell.wallBottom) {
                auto* wall = new QGraphicsRectItem(
                    c * cellSize, (r + 1) * cellSize - wt / 2.0, cellSize, wt);
                wall->setBrush(QBrush(wallColor));
                wall->setPen(Qt::NoPen);
                scene->addItem(wall);
            }
        }
    }

    scene->setSceneRect(0, 0, cols * cellSize, rows * cellSize);
    view->setScene(scene);
    view->centerOn(scene->sceneRect().center());
}

// ─────────────────────────────────────────────
//  Entity rendering (player + enemies)
// ─────────────────────────────────────────────

void MainWindow::redrawEntities()
{
    if (!gc || !player) return;

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

    if (player->getHasLevelKey()) {
        keyLabel->setText("Key: Yes");
        keyLabel->setStyleSheet(
            "color: #ffd878; font-size: 15px; font-weight: bold;"
            "font-family: 'Georgia', serif;"
            );
    } else {
        keyLabel->setText("Key: No");
        keyLabel->setStyleSheet(
            "color: #c8b88a; font-size: 15px; font-weight: bold;"
            "font-family: 'Georgia', serif;"
            );
    }

    const double sec = gc->elapsedMs() / 1000.0;
    timeLabel->setText(QStringLiteral("Time: %1s").arg(QString::number(sec, 'f', 1)));

    int enemyCount = 0;
    if (gc->getLevel()) {
        enemyCount = static_cast<int>(gc->getLevel()->getEnemies().size());
    }
    QString rLine;
    if (player->isRacialAbilityReady()) {
        rLine = QStringLiteral("R: READY");
    } else {
        rLine = QStringLiteral("R: %1/%2").arg(player->getMovesTowardRacial()).arg(player->getRacialMovePeriod());
    }
    enemiesLabel->setText(QStringLiteral("Enemies: %1   %2").arg(enemyCount).arg(rLine));
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
        const double scoreSec = gc->victoryTimeMs() > 0
            ? gc->victoryTimeMs() / 1000.0
            : gc->elapsedMs() / 1000.0;
        endMsg->setText(
            QStringLiteral("⚔  VICTORY!  ⚔\n\nWell done, ") + player->getName()
            + QStringLiteral("!\nFinal run time (score): ") + QString::number(scoreSec, 'f', 1)
            + QStringLiteral(" s\n(Lower is better.)"));
        endRestartBtn->setText("PLAY AGAIN");
        endMsg->setStyleSheet(
            "color: #e8c96a; font-size: 28px; font-weight: bold;"
            "font-family: 'Georgia', serif; letter-spacing: 3px;"
            );
        stack->setCurrentIndex(3);
    }
}

void MainWindow::onHudTick()
{
    if (stack->currentIndex() == 2 && gc && player) {
        updateHUD();
    }
}

void MainWindow::onSaveGameClicked()
{
    if (!gc || !player) {
        return;
    }
    const QString path = defaultSaveFilePath();
    if (gc->saveGameToFile(path)) {
        showLog(QStringLiteral("Game saved to ") + path);
        QMessageBox::information(this, QStringLiteral("Saved"), QStringLiteral("Progress written to:\n") + path);
    } else {
        QMessageBox::warning(this, QStringLiteral("Save failed"), QStringLiteral("Could not write save file."));
    }
}

void MainWindow::onLoadGameClicked()
{
    QString err;
    Player* newPlayer = player;
    GameController* newGc = gc;
    const QString path = defaultSaveFilePath();
    if (!GameController::loadGameFromFile(path, newPlayer, newGc, err)) {
        QMessageBox::warning(this, QStringLiteral("Load failed"), err);
        return;
    }
    player = newPlayer;
    gc = newGc;
    drawGrid();
    redrawEntities();
    updateHUD();
    showLog(QStringLiteral("Loaded game from ") + path);
    gamePage->setFocus();
}
