#pragma once
#include <SFML/Graphics.hpp>
#include "Level.h"
#include "CharacterManager.h"
#include "Sonic.h"
#include "Tails.h"
#include "Knuckles.h"
#include "HUD.h"
#include "Ring.h"
#include "Spike.h"
#include "BreakableWall.h"
#include "Enemy.h"
#include "Crab.h"
#include "Bee.h"
#include "BatBrain.h"
#include "MotoBug.h"
#include "Boss.h"
#include "ScoreManager.h"
#include "Leaderboard.h"
#include "ExtraLife.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
using namespace std;
using namespace sf;

class Game {
public:
    Game()
        : window(VideoMode(screen_x, screen_y), "Sonic the Hedgehog-OOP", Style::Close),
          camera(FloatRect(0, 0, screen_x, screen_y)),
          state(STATE_MENU),
          selectedOption(0),
          pauseSelectedOption(0),
          gameOverSelectedOption(0),
          currentLevel(1),
          sonic(nullptr),
          tails(nullptr),
          knuckles(nullptr),
          boss(nullptr),
          enemyCount(0),
          ringCount(0),
          spikeCount(0),
          wallCount(0),
          totalRings(0),
          playerName(""),
          nameInputIndex(0),
          pendingLevel(1)
    {
        for (int i = 0; i < MAX_ENEMIES; ++i) enemies[i] = nullptr;
        for (int i = 0; i < MAX_RINGS; ++i) rings[i] = nullptr;
        for (int i = 0; i < MAX_SPIKES; ++i) spikes[i] = nullptr;
        for (int i = 0; i < MAX_WALLS; ++i) walls[i] = nullptr;
        for (int i = 0; i < MAX_NAME_LENGTH; ++i) nameInput[i] = '\0';

        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);
        cout << "Starting Game initialization..." << endl;

        // Initialize pause background
        Image backgroundImage;
        backgroundImage.create(1, 1, Color(0, 0, 0, 150)); // Semi-transparent black
        pauseBackgroundTexture.loadFromImage(backgroundImage);
        pauseBackground.setTexture(pauseBackgroundTexture);
        pauseBackground.setScale(screen_x, screen_y);
        pauseBackground.setPosition(0, 0);

        // Initialize game over background
        gameOverBackground.setTexture(pauseBackgroundTexture);
        gameOverBackground.setScale(screen_x, screen_y);
        gameOverBackground.setPosition(0, 0);

        // Initialize victory background
        victoryBackground.setTexture(pauseBackgroundTexture);
        victoryBackground.setScale(screen_x, screen_y);
        victoryBackground.setPosition(0, 0);

        if (!loadAssets()) {
            cerr << "Asset loading failed. Exiting." << endl;
            exit(1);
        }
        cout << "Assets loaded successfully." << endl;

        if (!loadLevel(currentLevel)) {
            cerr << "Level loading failed. Exiting." << endl;
            exit(1);
        }
        cout << "Level " << currentLevel << " loaded successfully." << endl;

        initCharacters();
        cout << "Characters initialized." << endl;

        spawnEnemies();
        cout << "Enemies spawned." << endl;

        spawnRings();
        cout << "Rings spawned." << endl;

        initObstacles();
        cout << "Obstacles initialized." << endl;
    }

    ~Game() {
        for (int i = 0; i < ringCount; i++) {
            delete rings[i];
        }
        for (int i = 0; i < enemyCount; i++) {
            delete enemies[i];
        }
        for (int i = 0; i < spikeCount; i++) {
            delete spikes[i];
        }
        for (int i = 0; i < wallCount; i++) {
            delete walls[i];
        }
        delete sonic;
        delete tails;
        delete knuckles;
        delete boss;
    }

    void run() {
        Clock clock;
        while (window.isOpen()) {
            handleEvents();
            float deltaTime = clock.restart().asSeconds();
            update(deltaTime);
            render();
        }
    }

private:
    const int screen_x = 1280;
    const int screen_y = 720;

    static const int STATE_MENU = 0;
    static const int STATE_GAMEPLAY = 1;
    static const int STATE_INSTRUCTIONS = 2;
    static const int STATE_PAUSED = 3;
    static const int STATE_GAMEOVER = 4;
    static const int STATE_NAME_INPUT = 5;
    static const int STATE_LEADERBOARD = 6;
    static const int STATE_VICTORY = 7;
    int state;

    RenderWindow window;
    View camera;
    Level level;
    CharacterManager characterManager;
    HUD hud;
    ScoreManager scoreManager;
    Leaderboard leaderboard;

    Texture backgroundTextures[3];
    Sprite backgroundSprites[3];
    Texture menuBackgroundTexture;
    Sprite menuBackground;

    Texture wallTextures[3][3];
    Sprite wallSprites[3][3];
    Texture tempBlockTexture;
    Sprite tempBlockSprite;

    Texture texCrabIdle, texCrabRun, texBee, texBatBrain;
    Texture texBeeIdle, texBeeRun, texBatBrainIdle, texBatBrainRun;
    Texture texMotoBugIdle, texMotoBugRun;
    Texture texBossIdle, texBossAttack;
    Texture texProjectile;

    Texture texSonicIdle, texSonicRun, texSonicJump;
    Texture texTailsIdle, texTailsRun, texTailsJump, texTailsFly;
    Texture texKnucklesIdle, texKnucklesRun, texKnucklesJump;
    Texture texSonicInvincible, texTailsInvincible, texKnucklesInvincible, texSonicBoost;
    Texture texKnucklesPunch;

    static const int MAX_ENEMIES = 30;
    Enemy* enemies[MAX_ENEMIES];
    int enemyCount;

    Sonic* sonic;
    Tails* tails;
    Knuckles* knuckles;
    Boss* boss;

    static const int MAX_SPIKES = 10;
    Spike* spikes[MAX_SPIKES];
    int spikeCount;

    static const int MAX_EXTRALIVES = 10;
    ExtraLife* extraLives[MAX_EXTRALIVES];
    int extraLifeCount;

    static const int MAX_WALLS = 10;
    BreakableWall* walls[MAX_WALLS];
    int wallCount;

    static const int MAX_RINGS = 100;
    Ring* rings[MAX_RINGS];
    int ringCount;
    int totalRings;

    Font font;
    Text menuOptions[7];
    int selectedOption;
    static const int NUM_OPTIONS = 7;
    const char* optionStrings[7] = {"Continue", "Level 1", "Level 2", "Level 3", "Leaderboard", "Instructions", "Exit"};
    Text instructionsText;
    Text nameInputText;
    Text leaderboardText;

    Text pauseTitleText;
    Text pauseOptions[3];
    int pauseSelectedOption;
    static const int PAUSE_NUM_OPTIONS = 3;
    const char* pauseOptionStrings[3] = {"Resume Game", "Restart Level", "Main Menu"};
    Texture pauseBackgroundTexture;
    Sprite pauseBackground;

    Text gameOverTitleText;
    Text gameOverOptions[2];
    int gameOverSelectedOption;
    static const int GAMEOVER_NUM_OPTIONS = 2;
    const char* gameOverOptionStrings[2] = {"Restart Level", "Main Menu"};
    Sprite gameOverBackground;

    Text victoryTitleText;
    Text victoryScoreText;
    Text victoryOptionText;
    Sprite victoryBackground;

    int currentLevel;
    const char* levelFiles[3] = {"levels/level1.txt", "levels/level2.txt", "levels/level3.txt"};
    const int cell_size = 64;

    enum EnemyType { CRAB, BEE, MOTOBUG, BATBRAIN, BOSS };

    struct EnemyPosition {
        float x;
        float y;
        EnemyType type;
        float speed;
    };

    string playerName;
    static const int MAX_NAME_LENGTH = 11; // 10 chars + null terminator
    char nameInput[MAX_NAME_LENGTH];
    int nameInputIndex;
    int pendingLevel; // Store level to load after name input

    bool loadAssets() {
        if (!backgroundTextures[0].loadFromFile("assets/bg1.png")) {
            cerr << "Error loading background texture for level 1!" << endl;
            return false;
        }
        backgroundSprites[0].setTexture(backgroundTextures[0]);

        if (!backgroundTextures[1].loadFromFile("assets/bg2.png")) {
            cerr << "Warning: Failed to load background texture for level 2, using level 1 background." << endl;
            backgroundTextures[1] = backgroundTextures[0];
        }
        backgroundSprites[1].setTexture(backgroundTextures[1]);

        if (!backgroundTextures[2].loadFromFile("assets/bg3.png")) {
            cerr << "Warning: Failed to load background texture for level 3, using level 1 background." << endl;
            backgroundTextures[2] = backgroundTextures[0];
        }
        backgroundSprites[2].setTexture(backgroundTextures[2]);

        if (!menuBackgroundTexture.loadFromFile("assets/MenuBackground.png")) {
            cerr << "Warning: Failed to load background texture for Menu, using level 1 background." << endl;
            menuBackgroundTexture = backgroundTextures[0];
        }
        menuBackground.setTexture(menuBackgroundTexture);
        menuBackground.setPosition(0, 0);

        if (!wallTextures[0][0].loadFromFile("assets/brick1_level1.png") ||
            !wallTextures[0][1].loadFromFile("assets/brick2_level1.png") ||
            !wallTextures[0][2].loadFromFile("assets/brick3_level1.png")) {
            cerr << "Error loading wall textures for level 1!" << endl;
            return false;
        }
        wallSprites[0][0].setTexture(wallTextures[0][0]);
        wallSprites[0][1].setTexture(wallTextures[0][1]);
        wallSprites[0][2].setTexture(wallTextures[0][2]);

        if (!wallTextures[1][0].loadFromFile("assets/brick1_level2.png") ||
            !wallTextures[1][1].loadFromFile("assets/brick2_level2.png") ||
            !wallTextures[1][2].loadFromFile("assets/brick3_level2.png")) {
            cerr << "Warning: Failed to load wall textures for level 2, using level 1 textures." << endl;
            wallTextures[1][0] = wallTextures[0][0];
            wallTextures[1][1] = wallTextures[0][1];
            wallTextures[1][2] = wallTextures[0][2];
        }
        wallSprites[1][0].setTexture(wallTextures[1][0]);
        wallSprites[1][1].setTexture(wallTextures[1][1]);
        wallSprites[1][2].setTexture(wallTextures[1][2]);

        if (!wallTextures[2][0].loadFromFile("assets/brick1_level3.png") ||
            !wallTextures[2][1].loadFromFile("assets/brick2_level3.png") ||
            !wallTextures[2][2].loadFromFile("assets/brick3_level3.png")) {
            cerr << "Warning: Failed to load wall textures for level 3, using level 1 textures." << endl;
            wallTextures[2][0] = wallTextures[0][0];
            wallTextures[2][1] = wallTextures[0][1];
            wallTextures[2][2] = wallTextures[0][2];
        }
        wallSprites[2][0].setTexture(wallTextures[2][0]);
        wallSprites[2][1].setTexture(wallTextures[2][1]);
        wallSprites[2][2].setTexture(wallTextures[2][2]);

        if (!tempBlockTexture.loadFromFile("assets/temp_block.png")) {
            cerr << "Error loading temporary block texture!" << endl;
            return false;
        }
        tempBlockSprite.setTexture(tempBlockTexture);

        if (!texProjectile.loadFromFile("assets/projectile.png")) {
            cerr << "Error loading projectile texture!" << endl;
            return false;
        }

        if (!texCrabIdle.loadFromFile("assets/crab_idle.png") ||
            !texCrabRun.loadFromFile("assets/crab_run.png")) {
            cerr << "Error loading crab textures!" << endl;
            return false;
        }

        if (!texBeeIdle.loadFromFile("assets/bee_idle.png") ||
            !texBeeRun.loadFromFile("assets/bee_run.png")) {
            cerr << "Error loading bee textures!" << endl;
            return false;
        }

        if (!texBatBrainIdle.loadFromFile("assets/batbrain_idle.png") ||
            !texBatBrainRun.loadFromFile("assets/batbrain_run.png")) {
            cerr << "Error loading batbrain textures!" << endl;
            return false;
        }

        if (!texMotoBugIdle.loadFromFile("assets/motobug_idle.png") ||
            !texMotoBugRun.loadFromFile("assets/motobug_run.png")) {
            cerr << "Error loading motobug textures!" << endl;
            return false;
        }

        if (!texBossIdle.loadFromFile("assets/boss_idle.png") ||
            !texBossAttack.loadFromFile("assets/boss_attack.png")) {
            cerr << "Error loading boss textures!" << endl;
            return false;
        }

        if (!texSonicIdle.loadFromFile("assets/sonic_idle.png") ||
            !texSonicRun.loadFromFile("assets/sonic_run.png") ||
            !texSonicJump.loadFromFile("assets/sonic_jump.png") ||
            !texSonicInvincible.loadFromFile("assets/sonic_invincible.png") ||
            !texSonicBoost.loadFromFile("assets/sonic_boost.png")) {
            cerr << "Error loading Sonic textures!" << endl;
            return false;
        }

        if (!texKnucklesPunch.loadFromFile("assets/knuckles_punch.png")) {
            cerr << "Error loading Knuckles punch texture!" << endl;
            return false;
        }

        if (!texTailsIdle.loadFromFile("assets/tails_idle.png") ||
            !texTailsRun.loadFromFile("assets/tails_run.png") ||
            !texTailsJump.loadFromFile("assets/tails_jump.png") ||
            !texTailsFly.loadFromFile("assets/tails_fly.png") ||
            !texTailsInvincible.loadFromFile("assets/tails_invincible.png")) {
            cerr << "Error loading Tails textures!" << endl;
            return false;
        }

        if (!texKnucklesIdle.loadFromFile("assets/knuckles_idle.png") ||
            !texKnucklesRun.loadFromFile("assets/knuckles_run.png") ||
            !texKnucklesJump.loadFromFile("assets/knuckles_jump.png") ||
            !texKnucklesInvincible.loadFromFile("assets/knuckles_invincible.png")) {
            cerr << "Error loading Knuckles textures!" << endl;
            return false;
        }

        if (!font.loadFromFile("assets/font.ttf")) {
            cerr << "Error loading font!" << endl;
            return false;
        }

        for (int i = 0; i < NUM_OPTIONS; ++i) {
            menuOptions[i].setFont(font);
            menuOptions[i].setString(optionStrings[i]);
            menuOptions[i].setCharacterSize(40);
            menuOptions[i].setFillColor(i == selectedOption ? Color::Yellow : Color::White);
            menuOptions[i].setPosition(screen_x / 4 - menuOptions[i].getGlobalBounds().width / 2, 300 + i * 60);
        }

        instructionsText.setFont(font);
        instructionsText.setString(
            "Instructions:\n\n"
            "Left/Right: Move\n"
            "Up: Jump\n"
            "Space: Ability\n"
            "Z: Switch Character\n"
            "N: Next Level\n"
            "P: Pause\n"
            "S: Save Game\n"
            "Escape: Exit\n\n"
            "Collect rings (+100 points), defeat enemies (+1000 points), and complete levels (+5000 points)!\n"
            "Defeat the boss in Level 3 to win (+10000 points)!\n"
            "Score persists across levels for a high score.\n\n"
            "Press Enter to return to menu"
        );
        instructionsText.setCharacterSize(30);
        instructionsText.setFillColor(Color::White);
        instructionsText.setPosition(screen_x / 2 - instructionsText.getGlobalBounds().width / 2, 100);

        nameInputText.setFont(font);
        nameInputText.setString("Enter Name (Enter to confirm):\n");
        nameInputText.setCharacterSize(30);
        nameInputText.setFillColor(Color::White);
        nameInputText.setPosition(screen_x / 2 - nameInputText.getGlobalBounds().width / 2, screen_y / 2 - 50);

        leaderboardText.setFont(font);
        leaderboardText.setString("Leaderboard\n\nPress Enter to return to menu");
        leaderboardText.setCharacterSize(30);
        leaderboardText.setFillColor(Color::White);
        leaderboardText.setPosition(screen_x / 2 - leaderboardText.getGlobalBounds().width / 2, 100);

        pauseTitleText.setFont(font);
        pauseTitleText.setString("Paused");
        pauseTitleText.setCharacterSize(50);
        pauseTitleText.setFillColor(Color::Cyan);
        pauseTitleText.setPosition(screen_x / 2 - pauseTitleText.getGlobalBounds().width / 2, 100);

        for (int i = 0; i < PAUSE_NUM_OPTIONS; ++i) {
            pauseOptions[i].setFont(font);
            pauseOptions[i].setString(pauseOptionStrings[i]);
            pauseOptions[i].setCharacterSize(30);
            pauseOptions[i].setFillColor(i == pauseSelectedOption ? Color::Yellow : Color::White);
            pauseOptions[i].setPosition(screen_x / 2 - pauseOptions[i].getGlobalBounds().width / 2, 300 + i * 60);
        }

        gameOverTitleText.setFont(font);
        gameOverTitleText.setString("Game Over");
        gameOverTitleText.setCharacterSize(50);
        gameOverTitleText.setFillColor(Color::Red);
        gameOverTitleText.setPosition(screen_x / 2 - gameOverTitleText.getGlobalBounds().width / 2, 100);

        for (int i = 0; i < GAMEOVER_NUM_OPTIONS; ++i) {
            gameOverOptions[i].setFont(font);
            gameOverOptions[i].setString(gameOverOptionStrings[i]);
            gameOverOptions[i].setCharacterSize(30);
            gameOverOptions[i].setFillColor(i == gameOverSelectedOption ? Color::Yellow : Color::White);
            gameOverOptions[i].setPosition(screen_x / 2 - gameOverOptions[i].getGlobalBounds().width / 2, 300 + i * 60);
        }

        victoryTitleText.setFont(font);
        victoryTitleText.setString("Victory!");
        victoryTitleText.setCharacterSize(50);
        victoryTitleText.setFillColor(Color::Green);
        victoryTitleText.setPosition(screen_x / 2 - victoryTitleText.getGlobalBounds().width / 2, 100);

        victoryScoreText.setFont(font);
        victoryScoreText.setString("Score: 0");
        victoryScoreText.setCharacterSize(30);
        victoryScoreText.setFillColor(Color::White);
        victoryScoreText.setPosition(screen_x / 2 - victoryScoreText.getGlobalBounds().width / 2, 200);

        victoryOptionText.setFont(font);
        victoryOptionText.setString("Press Enter to return to Main Menu");
        victoryOptionText.setCharacterSize(30);
        victoryOptionText.setFillColor(Color::Yellow);
        victoryOptionText.setPosition(screen_x / 2 - victoryOptionText.getGlobalBounds().width / 2, 300);

        return true;
    }

    bool loadLevel(int levelNum) {
        if (levelNum < 1 || levelNum > 3) {
            cerr << "Invalid level number: " << levelNum << endl;
            return false;
        }
        currentLevel = levelNum;
        if (!level.loadFromFile(levelFiles[levelNum - 1])) {
            cerr << "Failed to load level file: " << levelFiles[levelNum - 1] << endl;
            return false;
        }
        if (level.width == 0 || level.height == 0) {
            cerr << "Level " << levelNum << " has invalid dimensions." << endl;
            return false;
        }

        //resetting only objects not score
        for (int i = 0; i < enemyCount; ++i) {
            if (enemies[i]) {
                if (enemies[i] == boss) {
                    boss = nullptr; //clears boss if its in enemies
                }
                delete enemies[i];
                enemies[i] = nullptr;
            }
        }
        enemyCount = 0;

        for (int i = 0; i < ringCount; ++i) {
            delete rings[i];
            rings[i] = nullptr;
        }
        ringCount = 0;
        totalRings = 0;

        for (int i = 0; i < spikeCount; ++i) {
            delete spikes[i];
            spikes[i] = nullptr;
        }
        spikeCount = 0;

        for (int i = 0; i < wallCount; ++i) {
            delete walls[i];
            walls[i] = nullptr;
        }
        wallCount = 0;

        for (int i = 0; i < extraLifeCount; ++i) {
            delete extraLives[i];
            extraLives[i] = nullptr;
        }
        extraLifeCount = 0;

        //ensure boss is deleted and null
        if (boss) {
            delete boss;
            boss = nullptr;
        }

        delete sonic;
        delete tails;
        delete knuckles;
        sonic = nullptr;
        tails = nullptr;
        knuckles = nullptr;

        initCharacters();

        //level physics
        if (sonic && tails && knuckles) {
            if (currentLevel == 1) {
                sonic->friction = 0.3f;
                sonic->accelRate = 0.5f;
                sonic->gravity = 0.4f;
                tails->friction = 0.3f;
                tails->accelRate = 0.5f;
                tails->gravity = 0.4f;
                knuckles->friction = 0.3f;
                knuckles->accelRate = 0.5f;
                knuckles->gravity = 0.4f;
            } else if (currentLevel == 2) {
                sonic->friction = 0.15f;
                sonic->accelRate = 0.75f;
                sonic->gravity = 0.4f;
                tails->friction = 0.15f;
                tails->accelRate = 0.75f;
                tails->gravity = 0.4f;
                knuckles->friction = 0.15f;
                knuckles->accelRate = 0.75f;
                knuckles->gravity = 0.4f;
            } else if (currentLevel == 3) {
                sonic->friction = 0.3f;
                sonic->accelRate = 0.5f;
                sonic->gravity = 0.3f;
                tails->friction = 0.3f;
                tails->accelRate = 0.5f;
                tails->gravity = 0.3f;
                knuckles->friction = 0.3f;
                knuckles->accelRate = 0.5f;
                knuckles->gravity = 0.3f;
            }
        }

        spawnEnemies();
        spawnRings();
        initObstacles();

        camera.setCenter(screen_x / 2.0f, screen_y / 2.0f);
        window.setView(camera);

        return true;
    }

    bool loadNextLevel() {
        int nextLevel = currentLevel % 3 + 1;
        if (nextLevel < 1 || nextLevel > 3) {
            cerr << "Invalid next level number: " << nextLevel << endl;
            return false;
        }
        currentLevel = nextLevel;
        if (!level.loadFromFile(levelFiles[nextLevel - 1])) {
            cerr << "Failed to load level file: " << levelFiles[nextLevel - 1] << endl;
            return false;
        }
        if (level.width == 0 || level.height == 0) {
            cerr << "Level " << nextLevel << " has invalid dimensions." << endl;
            return false;
        }

        for (int i = 0; i < enemyCount; ++i) {
            delete enemies[i];
            enemies[i] = nullptr;
        }
        enemyCount = 0;

        for (int i = 0; i < ringCount; ++i) {
            delete rings[i];
            rings[i] = nullptr;
        }
        ringCount = 0;
        totalRings = 0;

        for (int i = 0; i < spikeCount; ++i) {
            delete spikes[i];
            spikes[i] = nullptr;
        }
        spikeCount = 0;

        for (int i = 0; i < wallCount; ++i) {
            delete walls[i];
            walls[i] = nullptr;
        }
        wallCount = 0;

        for (int i = 0; i < extraLifeCount; ++i) {
            delete extraLives[i];
            extraLives[i] = nullptr;
        }
        extraLifeCount = 0;

        delete boss;
        boss = nullptr;

        //reset character positions
        if (sonic) {
            sonic->setPosition(4 * cell_size, 11 * cell_size);
            sonic->hp += 1;
        }
        if (tails) {
            tails->setPosition(4 * cell_size, 11 * cell_size);
            tails->hp += 1;
        }
        if (knuckles) {
            knuckles->setPosition(4 * cell_size, 11 * cell_size);
            knuckles->hp += 1;
        }

        //level based physics
        if (sonic && tails && knuckles) {
            if (currentLevel == 1) {
                sonic->friction = 0.3f;
                sonic->accelRate = 0.5f;
                sonic->gravity = 0.4f;
                tails->friction = 0.3f;
                tails->accelRate = 0.5f;
                tails->gravity = 0.4f;
                knuckles->friction = 0.3f;
                knuckles->accelRate = 0.5f;
                knuckles->gravity = 0.4f;
            } else if (currentLevel == 2) {
                sonic->friction = 0.15f;
                sonic->accelRate = 0.75f;
                sonic->gravity = 0.4f;
                tails->friction = 0.15f;
                tails->accelRate = 0.75f;
                tails->gravity = 0.4f;
                knuckles->friction = 0.15f;
                knuckles->accelRate = 0.75f;
                knuckles->gravity = 0.4f;
            } else if (currentLevel == 3) {
                sonic->friction = 0.3f;
                sonic->accelRate = 0.5f;
                sonic->gravity = 0.3f;
                tails->friction = 0.3f;
                tails->accelRate = 0.5f;
                tails->gravity = 0.3f;
                knuckles->friction = 0.3f;
                knuckles->accelRate = 0.5f;
                knuckles->gravity = 0.3f;
            }
        }

        spawnEnemies();
        spawnRings();
        initObstacles();

        camera.setCenter(screen_x / 2.0f, screen_y / 2.0f);
        window.setView(camera);

        return true;
    }

    void initCharacters() {
        sonic = new Sonic(texSonicIdle, texSonicRun, texSonicJump, texSonicInvincible, texSonicBoost, level, 10, 8, 10);
        tails = new Tails(texTailsIdle, texTailsRun, texTailsJump, texTailsFly, texTailsInvincible, level, 8, 8, 0, characterManager);
        knuckles = new Knuckles(texKnucklesIdle, texKnucklesRun, texKnucklesJump, texKnucklesPunch, texKnucklesInvincible, level, 8, 8, 0);

        characterManager.init(sonic, tails, knuckles);
    }

    void spawnEnemies() {
        //clear all
        for (int i = 0; i < enemyCount; ++i) {
            if (enemies[i]) {
                if (enemies[i] == boss) {
                    boss = nullptr;
                }
                delete enemies[i];
                enemies[i] = nullptr;
            }
        }
        enemyCount = 0;

        //clearing boss
        if (boss) {
            delete boss;
            boss = nullptr;
        }

        //hard coded locations
        const EnemyPosition level1Enemies[] = {
            {10, float(level.getGroundLevel() - 3), CRAB, 100.0f},
            {25, float(level.getGroundLevel() - 3), BEE, 120.0f},
            {70, float(level.getGroundLevel() - 3), MOTOBUG, 100.0f},
            {45, float(level.getGroundLevel() - 3), BATBRAIN, 80.0f},
            {35, float(level.getGroundLevel() - 1), MOTOBUG, 100.0f},
            {145, float(level.getGroundLevel() - 1), CRAB, 100.0f},
            {155, float(level.getGroundLevel() - 1), MOTOBUG, 100.0f},
            {165, float(level.getGroundLevel() - 1), MOTOBUG, 100.0f}
        };
        const EnemyPosition level2Enemies[] = {
            {15, float(level.getGroundLevel() - 3), MOTOBUG, 100.0f},
            {25, float(level.getGroundLevel() - 4), BEE, 120.0f},
            {40, float(level.getGroundLevel() - 1), CRAB, 100.0f},
            {40, float(level.getGroundLevel() - 3), BATBRAIN, 80.0f},
            {50, float(level.getGroundLevel() - 1), CRAB, 100.0f},
            {114, float(level.getGroundLevel() - 7), BEE, 120.0f},
            {97, float(level.getGroundLevel() - 5), CRAB, 100.0f},
            {207, float(level.getGroundLevel() - 4), CRAB, 100.0f},
            {221, float(level.getGroundLevel() - 6), BEE, 120.0f},
            {189, float(level.getGroundLevel() - 2), MOTOBUG, 100.0f}
        };
        const EnemyPosition level3Enemies[] = {
            {41, float(level.getGroundLevel() - 1), CRAB, 100.0f},
            {15, float(level.getGroundLevel() - 3), MOTOBUG, 100.0f},
            {25, float(level.getGroundLevel() - 4), BEE, 120.0f},
            {40, float(level.getGroundLevel() - 3), BATBRAIN, 80.0f},
            {50, float(level.getGroundLevel() - 1), CRAB, 100.0f},
            {97, float(level.getGroundLevel() - 5), CRAB, 100.0f},
            {114, float(level.getGroundLevel() - 7), BEE, 120.0f},
            {125, float(level.getGroundLevel() - 1), MOTOBUG, 100.0f},
            {135, float(level.getGroundLevel() - 3), BATBRAIN, 80.0f},
            {135, float(level.getGroundLevel() - 1), MOTOBUG, 80.0f},
            {218, float(level.getGroundLevel() - 2), CRAB, 100.0f},
            {216, float(level.getGroundLevel() - 8), BEE, 120.0f}
        };

        const int level1EnemyCount = sizeof(level1Enemies) / sizeof(level1Enemies[0]);
        const int level2EnemyCount = sizeof(level2Enemies) / sizeof(level2Enemies[0]);
        const int level3EnemyCount = sizeof(level3Enemies) / sizeof(level3Enemies[0]);

        if (currentLevel == 1 && enemyCount + level1EnemyCount <= MAX_ENEMIES) {
            for (int i = 0; i < level1EnemyCount; ++i) {
                createEnemy(level1Enemies[i]);
                enemyCount++;
            }
        }
        else if (currentLevel == 2 && enemyCount + level2EnemyCount <= MAX_ENEMIES) {
            for (int i = 0; i < level2EnemyCount; ++i) {
                createEnemy(level2Enemies[i]);
                enemyCount++;
            }
        }
        else if (currentLevel == 3 && enemyCount + level3EnemyCount <= MAX_ENEMIES) {
            for (int i = 0; i < level3EnemyCount; ++i) {
                createEnemy(level3Enemies[i]);
                enemyCount++;
            }

            if (enemyCount < MAX_ENEMIES && !boss) {
                boss = new Boss(texBossIdle, texBossAttack, texProjectile, 280 * cell_size, float(level.getGroundLevel() - 3) * cell_size, 5, 6);
                enemies[enemyCount++] = boss;
            }
        }
        else if (enemyCount < MAX_ENEMIES) {
            enemies[enemyCount++] = new Crab(texCrabIdle, texCrabRun, texProjectile, 10 * cell_size, (level.getGroundLevel() - 1) * cell_size, 5);
        }

        for (int i = 0; i < enemyCount; ++i) {
            if (enemies[i] && dynamic_cast<BatBrain*>(enemies[i])) {
                dynamic_cast<BatBrain*>(enemies[i])->setTarget(characterManager.getCurrentCharacter());
            }
        }
    }

    void createEnemy(const EnemyPosition& pos) {
        switch (pos.type) {
            case CRAB:
                enemies[enemyCount] = new Crab(texCrabIdle, texCrabRun, texProjectile, pos.x * cell_size, pos.y * cell_size, 5);
                break;
            case BEE:
                enemies[enemyCount] = new Bee(texBeeIdle, texBeeRun, texProjectile, pos.x * cell_size, pos.y * cell_size, 4, level);
                break;
            case MOTOBUG:
                enemies[enemyCount] = new MotoBug(texMotoBugIdle, texMotoBugRun, pos.x * cell_size, pos.y * cell_size, 5);
                break;
            case BATBRAIN:
                enemies[enemyCount] = new BatBrain(texBatBrainIdle, texBatBrainRun, pos.x * cell_size, pos.y * cell_size, 8);
                break;
        }
    }

    void initObstacles() {
        for (int i = 0; i < spikeCount; ++i) {
            delete spikes[i];
            spikes[i] = nullptr;
        }
        spikeCount = 0;

        for (int i = 0; i < wallCount; ++i) {
            delete walls[i];
            walls[i] = nullptr;
        }
        wallCount = 0;

        struct SpikePosition {
            float x;
            float y;
        };

        struct WallPosition {
            float x;
            float y;
        };

        //hardcoded spikes
        const SpikePosition level1Spikes[] = {
            {62, float(level.getGroundLevel() - 3)},
            {55, float(level.getGroundLevel() - 1)},
            {59, float(level.getGroundLevel() - 2)},
            {30, float(level.getGroundLevel() - 1)}
        };
        const SpikePosition level2Spikes[] = {
            {10, float(level.getGroundLevel() - 1)},
            {25, float(level.getGroundLevel() - 3)},
            {21, float(level.getGroundLevel() - 3)},
            {181, float(level.getGroundLevel() - 1)},
            {199, float(level.getGroundLevel() - 2)},
            {201, float(level.getGroundLevel() - 2)},
            {203, float(level.getGroundLevel() - 2)},
            {205, float(level.getGroundLevel() - 2)},
            {237, float(level.getGroundLevel() - 2)},
            {237, float(level.getGroundLevel() - 10)}
        };
        const SpikePosition level3Spikes[] = {
            {10, float(level.getGroundLevel() - 1)},
            {25, float(level.getGroundLevel() - 3)},
            {21, float(level.getGroundLevel() - 3)},
            {40, float(level.getGroundLevel() - 1)},
            {50, float(level.getGroundLevel() - 1)},
            {203, 5},
            {242, float(level.getGroundLevel())}
        };

        //hardcoded breakable walls
        const WallPosition level1Walls[] = {
            {9, float(level.getGroundLevel() - 1)},
            {10, float(level.getGroundLevel() - 1)},
            {11, float(level.getGroundLevel() - 1)},
            {12, float(level.getGroundLevel() - 1)}
        };
        const WallPosition level2Walls[] = {
            {30, float(level.getGroundLevel() - 3)},
            {30, float(level.getGroundLevel() - 4)},
            {30, float(level.getGroundLevel() - 5)},
            {203, float(level.getGroundLevel() - 10)},
            {202, float(level.getGroundLevel() - 10)},
            {203, float(level.getGroundLevel() - 11)},
            {202, float(level.getGroundLevel() - 11)}
        };
        const WallPosition level3Walls[] = {
            {30, float(level.getGroundLevel() - 3)},
            {30, float(level.getGroundLevel() - 4)},
            {30, float(level.getGroundLevel() - 5)},
            {237, float(level.getGroundLevel() - 11)}
        };

        const int level1SpikeCount = sizeof(level1Spikes) / sizeof(level1Spikes[0]);
        const int level2SpikeCount = sizeof(level2Spikes) / sizeof(level2Spikes[0]);
        const int level3SpikeCount = sizeof(level3Spikes) / sizeof(level3Spikes[0]);

        const int level1WallCount = sizeof(level1Walls) / sizeof(level1Walls[0]);
        const int level2WallCount = sizeof(level2Walls) / sizeof(level2Walls[0]);
        const int level3WallCount = sizeof(level3Walls) / sizeof(level3Walls[0]);

        if (currentLevel == 1 && spikeCount + level1SpikeCount <= MAX_SPIKES) {
            for (int i = 0; i < level1SpikeCount; ++i) {
                spikes[spikeCount++] = new Spike(level1Spikes[i].x * cell_size, level1Spikes[i].y * cell_size);
            }
        }
        else if (currentLevel == 2 && spikeCount + level2SpikeCount <= MAX_SPIKES) {
            for (int i = 0; i < level2SpikeCount; ++i) {
                spikes[spikeCount++] = new Spike(level2Spikes[i].x * cell_size, level2Spikes[i].y * cell_size);
            }
        }
        else if (currentLevel == 3 && spikeCount + level3SpikeCount <= MAX_SPIKES) {
            for (int i = 0; i < level3SpikeCount; ++i) {
                spikes[spikeCount++] = new Spike(level3Spikes[i].x * cell_size, level3Spikes[i].y * cell_size);
            }
        }
        else if (spikeCount < MAX_SPIKES) {
            spikes[spikeCount++] = new Spike(19 * cell_size, (level.getGroundLevel() - 1) * cell_size);
        }

        if (currentLevel == 1 && wallCount + level1WallCount <= MAX_WALLS) {
            for (int i = 0; i < level1WallCount; ++i) {
                walls[wallCount++] = new BreakableWall(level1Walls[i].x * cell_size, level1Walls[i].y * cell_size);
            }
        }
        else if (currentLevel == 2 && wallCount + level2WallCount <= MAX_WALLS) {
            for (int i = 0; i < level2WallCount; ++i) {
                walls[wallCount++] = new BreakableWall(level2Walls[i].x * cell_size, level2Walls[i].y * cell_size);
            }
        }
        else if (currentLevel == 3 && wallCount + level3WallCount <= MAX_WALLS) {
            for (int i = 0; i < level3WallCount; ++i) {
                walls[wallCount++] = new BreakableWall(level3Walls[i].x * cell_size, level3Walls[i].y * cell_size);
            }
        }
        else if (wallCount < MAX_WALLS) {
            walls[wallCount++] = new BreakableWall(20 * cell_size, (level.getGroundLevel() - 1) * cell_size);
        }
    }

    void spawnRings() {
        for (int i = 0; i < ringCount; ++i) {
            delete rings[i];
            rings[i] = nullptr;
        }
        ringCount = 0;

        for (int i = 0; i < extraLifeCount; ++i) {
            delete extraLives[i];
            extraLives[i] = nullptr;
        }
        extraLifeCount = 0;

        struct RingPosition {
            float x;
            float y;
        };

        struct ExtraLifePosition {
            float x;
            float y;
        };

        //hardcoded Rings
        const RingPosition level1Rings[] = {
            {10, float(level.getGroundLevel() - 3)},
            {12, float(level.getGroundLevel() - 3)},
            {14, float(level.getGroundLevel() - 3)},
            {16, float(level.getGroundLevel() - 3)},
            {17, float(level.getGroundLevel() - 1)},
            {15, float(level.getGroundLevel() - 1)},
            {13, float(level.getGroundLevel() - 1)},
            {56, float(level.getGroundLevel() - 2)},
            {57, float(level.getGroundLevel() - 2)},
            {58, float(level.getGroundLevel() - 2)},
            {60, float(level.getGroundLevel() - 3)},
            {61, float(level.getGroundLevel() - 3)},
            {70, float(level.getGroundLevel() - 6)},
            {71, float(level.getGroundLevel() - 6)},
            {72, float(level.getGroundLevel() - 6)},
            {73, float(level.getGroundLevel() - 6)},
            {74, float(level.getGroundLevel() - 6)},
            {77, float(level.getGroundLevel() - 8)},
            {78, float(level.getGroundLevel() - 8)},
            {79, float(level.getGroundLevel() - 8)},
            {80, float(level.getGroundLevel() - 8)},
            {22, float(level.getGroundLevel() - 1)},
            {22, float(level.getGroundLevel() - 2)},
            {22, float(level.getGroundLevel() - 3)},
            {135.00f, float(level.getGroundLevel() - 1)},
            {137.06f, float(level.getGroundLevel() - 1)},
            {141.18f, float(level.getGroundLevel() - 1)},
            {145.29f, float(level.getGroundLevel() - 1)},
            {149.41f, float(level.getGroundLevel() - 1)},
            {153.53f, float(level.getGroundLevel() - 1)},
            {157.65f, float(level.getGroundLevel() - 1)},
            {161.76f, float(level.getGroundLevel() - 1)},
            {163.82f, float(level.getGroundLevel() - 1)},
            {165.88f, float(level.getGroundLevel() - 1)},
            {167.94f, float(level.getGroundLevel() - 1)},
            {170.00f, float(level.getGroundLevel() - 1)},
            {51, float(level.getGroundLevel() - 1)},
            {52, float(level.getGroundLevel() - 1.5f)},
            {53, float(level.getGroundLevel() - 2)}
        };
        const RingPosition level2Rings[] = {
            {18, float(level.getGroundLevel() - 3)},
            {19, float(level.getGroundLevel() - 3)},
            {22, float(level.getGroundLevel() - 3)},
            {23, float(level.getGroundLevel() - 3)},
            {15, float(level.getGroundLevel() - 8)},
            {16, float(level.getGroundLevel() - 8)},
            {17, float(level.getGroundLevel() - 8)},
            {18, float(level.getGroundLevel() - 8)},
            {19, float(level.getGroundLevel() - 8)},
            {86, float(level.getGroundLevel() - 4)},
            {87, float(level.getGroundLevel() - 4)},
            {88, float(level.getGroundLevel() - 4)},
            {89, float(level.getGroundLevel() - 4)},
            {156, float(level.getGroundLevel() - 7)},
            {157, float(level.getGroundLevel() - 7)},
            {158, float(level.getGroundLevel() - 7)},
            {159, float(level.getGroundLevel() - 7)},
            {187.65f, float(level.getGroundLevel() - 10)},
            {191.76f, float(level.getGroundLevel() - 10)},
            {193.82f, float(level.getGroundLevel() - 10)},
            {195.88f, float(level.getGroundLevel() - 10)},
            {197.94f, float(level.getGroundLevel() - 10)},
            {200.00f, float(level.getGroundLevel() - 10)}
        };
        const RingPosition level3Rings[] = {
            {18, float(level.getGroundLevel() - 3)},
            {19, float(level.getGroundLevel() - 3)},
            {22, float(level.getGroundLevel() - 3)},
            {23, float(level.getGroundLevel() - 3)},
            {15, float(level.getGroundLevel() - 8)},
            {16, float(level.getGroundLevel() - 8)},
            {17, float(level.getGroundLevel() - 8)},
            {18, float(level.getGroundLevel() - 8)},
            {19, float(level.getGroundLevel() - 8)},
            {86, float(level.getGroundLevel() - 4)},
            {87, float(level.getGroundLevel() - 4)},
            {88, float(level.getGroundLevel() - 4)},
            {89, float(level.getGroundLevel() - 4)},
            {205, 5},
            {207, 5},
            {209, 5},
            {184, 4},
            {186, 4},
            {188, 4}
        };

        const ExtraLifePosition level1ExtraLives[] = {
            {20, float(level.getGroundLevel() - 3)},
            {75, float(level.getGroundLevel() - 6)}
        };
        const ExtraLifePosition level2ExtraLives[] = {
            {20, float(level.getGroundLevel() - 3)},
            {190, float(level.getGroundLevel() - 10)}
        };
        const ExtraLifePosition level3ExtraLives[] = {
            {20, float(level.getGroundLevel() - 3)},
            {238, float(level.getGroundLevel() - 11)}
        };

        const int level1RingCount = sizeof(level1Rings) / sizeof(level1Rings[0]);
        const int level2RingCount = sizeof(level2Rings) / sizeof(level2Rings[0]);
        const int level3RingCount = sizeof(level3Rings) / sizeof(level3Rings[0]);

        const int level1ExtraLifeCount = sizeof(level1ExtraLives) / sizeof(level1ExtraLives[0]);
        const int level2ExtraLifeCount = sizeof(level2ExtraLives) / sizeof(level2ExtraLives[0]);
        const int level3ExtraLifeCount = sizeof(level3ExtraLives) / sizeof(level3ExtraLives[0]);

        if (currentLevel == 1 && ringCount + level1RingCount <= MAX_RINGS) {
            for (int i = 0; i < level1RingCount; ++i) {
                rings[ringCount++] = new Ring(level1Rings[i].x * cell_size, level1Rings[i].y * cell_size);
            }
            if (extraLifeCount + level1ExtraLifeCount <= MAX_EXTRALIVES) {
                for (int i = 0; i < level1ExtraLifeCount; ++i) {
                    extraLives[extraLifeCount++] = new ExtraLife(level1ExtraLives[i].x * cell_size, level1ExtraLives[i].y * cell_size);
                }
            }
        }
        else if (currentLevel == 2 && ringCount + level2RingCount <= MAX_RINGS) {
            for (int i = 0; i < level2RingCount; ++i) {
                rings[ringCount++] = new Ring(level2Rings[i].x * cell_size, level2Rings[i].y * cell_size);
            }
            if (extraLifeCount + level2ExtraLifeCount <= MAX_EXTRALIVES) {
                for (int i = 0; i < level2ExtraLifeCount; ++i) {
                    extraLives[extraLifeCount++] = new ExtraLife(level2ExtraLives[i].x * cell_size, level2ExtraLives[i].y * cell_size);
                }
            }
        }
        else if (currentLevel == 3 && ringCount + level3RingCount <= MAX_RINGS) {
            for (int i = 0; i < level3RingCount; ++i) {
                rings[ringCount++] = new Ring(level3Rings[i].x * cell_size, level3Rings[i].y * cell_size);
            }
            if (extraLifeCount + level3ExtraLifeCount <= MAX_EXTRALIVES) {
                for (int i = 0; i < level3ExtraLifeCount; ++i) {
                    extraLives[extraLifeCount++] = new ExtraLife(level3ExtraLives[i].x * cell_size, level3ExtraLives[i].y * cell_size);
                }
            }
        }
        else if (ringCount < MAX_RINGS) {
            rings[ringCount++] = new Ring(10 * cell_size, (level.getGroundLevel() - 1) * cell_size);
            if (extraLifeCount < MAX_EXTRALIVES) {
                extraLives[extraLifeCount++] = new ExtraLife(12 * cell_size, (level.getGroundLevel() - 1) * cell_size);
            }
        }
    }

#include <fstream>

    bool saveGame() {
        std::ofstream outFile("savegame.txt");
        if (!outFile.is_open()) {
            std::cerr << "Failed to open savegame.txt for writing!" << std::endl;
            return false;
        }

        Character* focus = characterManager.getCurrentCharacter();
        if (!focus) {
            return false;
        }

        // Write player name with length limit
        std::string nameToSave = playerName.substr(0, MAX_NAME_LENGTH - 1);
        outFile << nameToSave << '\n';
        outFile << currentLevel << '\n';
        outFile << scoreManager.getScore() << '\n';
        outFile << focus->x << '\n';
        outFile << focus->y << '\n';
        outFile << focus->hp << '\n';
        outFile << totalRings << '\n';

        return true;
    }


    bool loadGame() {
        std::ifstream inFile("savegame.txt");
        if (!inFile.is_open()) {
            std::cerr << "No savegame.txt found!" << std::endl;
            return false;
        }

        std::string nameBuffer;
        int levelNum, score, hp, rings;
        float x, y;

        if (!std::getline(inFile, nameBuffer) ||
            !(inFile >> levelNum) ||
            !(inFile >> score) ||
            !(inFile >> x) ||
            !(inFile >> y) ||
            !(inFile >> hp) ||
            !(inFile >> rings)) {
            std::cerr << "Error reading savegame.txt!" << std::endl;
            return false;
        }

        playerName = nameBuffer;
        if (!loadLevel(levelNum)) {
            std::cerr << "Failed to load level " << levelNum << " from save!" << std::endl;
            return false;
        }

        scoreManager.setScore(score);
        totalRings = rings;

        Character* focus = characterManager.getCurrentCharacter();
        if (focus) {
            focus->setPosition(x, y);
            focus->hp = hp;
        }

        return true;
    }



    void handleEvents() {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::KeyPressed) {
                if (state == STATE_MENU) {
                    if (event.key.code == Keyboard::Up) {
                        selectedOption = (selectedOption - 1 + NUM_OPTIONS) % NUM_OPTIONS;
                        updateMenuSelection();
                    }
                    if (event.key.code == Keyboard::Down) {
                        selectedOption = (selectedOption + 1) % NUM_OPTIONS;
                        updateMenuSelection();
                    }
                    if (event.key.code == Keyboard::Enter) {
                        if (selectedOption == 0) {
                            if (loadGame()) {
                                state = STATE_GAMEPLAY;
                            }
                        }
                        else if (selectedOption == 1) {
                            scoreManager.reset();
                            pendingLevel = 1;
                            state = STATE_NAME_INPUT;
                            nameInputIndex = 0;
                            for (int i = 0; i < MAX_NAME_LENGTH; ++i) nameInput[i] = '\0';
                        }
                        else if (selectedOption == 2) {
                            scoreManager.reset();
                            pendingLevel = 2;
                            state = STATE_NAME_INPUT;
                            nameInputIndex = 0;
                            for (int i = 0; i < MAX_NAME_LENGTH; ++i) nameInput[i] = '\0';
                        }
                        else if (selectedOption == 3) {
                            scoreManager.reset();
                            pendingLevel = 3;
                            state = STATE_NAME_INPUT;
                            nameInputIndex = 0;
                            for (int i = 0; i < MAX_NAME_LENGTH; ++i) nameInput[i] = '\0';
                        }
                        else if (selectedOption == 4) {
                            state = STATE_LEADERBOARD;
                        }
                        else if (selectedOption == 5) {
                            state = STATE_INSTRUCTIONS;
                        }
                        else if (selectedOption == 6) {
                            window.close();
                        }
                    }
                }
                else if (state == STATE_NAME_INPUT) {
                    if (event.key.code >= Keyboard::A && event.key.code <= Keyboard::Z) {
                        if (nameInputIndex < MAX_NAME_LENGTH - 1) {
                            nameInput[nameInputIndex++] = 'A' + (event.key.code - Keyboard::A);
                            nameInput[nameInputIndex] = '\0';
                        }
                    }
                    else if (event.key.code == Keyboard::BackSpace) {
                        if (nameInputIndex > 0) {
                            nameInput[--nameInputIndex] = '\0';
                        }
                    }
                    else if (event.key.code == Keyboard::Enter && nameInputIndex > 0) {
                        playerName = string(nameInput);
                        if (!loadLevel(pendingLevel)) {
                            cerr << "Failed to load level " << pendingLevel << ". Exiting." << endl;
                            window.close();
                        }
                        state = STATE_GAMEPLAY;
                    }
                }
                else if (state == STATE_LEADERBOARD) {
                    if (event.key.code == Keyboard::Enter) {
                        state = STATE_MENU;
                    }
                }
                else if (state == STATE_INSTRUCTIONS) {
                    if (event.key.code == Keyboard::Enter) {
                        state = STATE_MENU;
                    }
                }
                else if (state == STATE_GAMEPLAY) {
                    if (event.key.code == Keyboard::Up)
                        characterManager.getCurrentCharacter()->jump();
                    if (event.key.code == Keyboard::Z)
                        characterManager.switchCharacter();
                    if (event.key.code == Keyboard::N) {
                        loadNextLevel();
                    }
                    if (event.key.code == Keyboard::P) {
                        state = STATE_PAUSED;
                        pauseSelectedOption = 0;
                        updatePauseMenuSelection();
                    }
                    if (event.key.code == Keyboard::S) {
                        saveGame();
                    }
                }
                else if (state == STATE_PAUSED) {
                    if (event.key.code == Keyboard::Up) {
                        pauseSelectedOption = (pauseSelectedOption - 1 + PAUSE_NUM_OPTIONS) % PAUSE_NUM_OPTIONS;
                        updatePauseMenuSelection();
                    }
                    if (event.key.code == Keyboard::Down) {
                        pauseSelectedOption = (pauseSelectedOption + 1) % PAUSE_NUM_OPTIONS;
                        updatePauseMenuSelection();
                    }
                    if (event.key.code == Keyboard::Enter) {
                        if (pauseSelectedOption == 0) {
                            state = STATE_GAMEPLAY;
                        }
                        else if (pauseSelectedOption == 1) {
                            scoreManager.reset();
                            loadLevel(currentLevel);
                            state = STATE_GAMEPLAY;
                        }
                        else if (pauseSelectedOption == 2) {
                            leaderboard.addScore(playerName, scoreManager.getScore());
                            scoreManager.reset();
                            state = STATE_MENU;
                        }
                    }
                    if (event.key.code == Keyboard::P) {
                        state = STATE_GAMEPLAY;
                    }
                }
                else if (state == STATE_GAMEOVER) {
                    if (event.key.code == Keyboard::Up) {
                        gameOverSelectedOption = (gameOverSelectedOption - 1 + GAMEOVER_NUM_OPTIONS) % GAMEOVER_NUM_OPTIONS;
                        updateGameOverMenuSelection();
                    }
                    if (event.key.code == Keyboard::Down) {
                        gameOverSelectedOption = (gameOverSelectedOption + 1) % GAMEOVER_NUM_OPTIONS;
                        updateGameOverMenuSelection();
                    }
                    if (event.key.code == Keyboard::Enter) {
                        if (gameOverSelectedOption == 0) {
                            scoreManager.reset();
                            loadLevel(currentLevel);
                            state = STATE_GAMEPLAY;
                        }
                        else if (gameOverSelectedOption == 1) {
                            leaderboard.addScore(playerName, scoreManager.getScore());
                            scoreManager.reset();
                            state = STATE_MENU;
                        }
                    }
                    if (event.key.code == Keyboard::Escape) {
                        leaderboard.addScore(playerName, scoreManager.getScore());
                        scoreManager.reset();
                        state = STATE_MENU;
                    }
                }
                else if (state == STATE_VICTORY) {
                    if (event.key.code == Keyboard::Enter || event.key.code == Keyboard::Escape) {
                        leaderboard.addScore(playerName, scoreManager.getScore());
                        scoreManager.reset();
                        state = STATE_MENU;
                    }
                }

                if (event.key.code == Keyboard::Escape) {
                    if (state == STATE_GAMEPLAY) {
                        state = STATE_PAUSED;
                        pauseSelectedOption = 0;
                        updatePauseMenuSelection();
                    }
                    else if (state == STATE_PAUSED || state == STATE_INSTRUCTIONS || state == STATE_LEADERBOARD) {
                        leaderboard.addScore(playerName, scoreManager.getScore());
                        scoreManager.reset();
                        state = STATE_MENU;
                    }
                    else if (state == STATE_NAME_INPUT) {
                        state = STATE_MENU;
                    }
                    else if (state == STATE_MENU) {
                        window.close();
                    }
                }
            }
        }
    }

    void updateMenuSelection() {
        for (int i = 0; i < NUM_OPTIONS; ++i) {
            menuOptions[i].setFillColor(i == selectedOption ? Color::Yellow : Color::White);
        }
    }

    void updatePauseMenuSelection() {
        for (int i = 0; i < PAUSE_NUM_OPTIONS; ++i) {
            pauseOptions[i].setFillColor(i == pauseSelectedOption ? Color::Yellow : Color::White);
        }
    }

    void updateGameOverMenuSelection() {
        for (int i = 0; i < GAMEOVER_NUM_OPTIONS; ++i) {
            gameOverOptions[i].setFillColor(i == gameOverSelectedOption ? Color::Yellow : Color::White);
        }
    }

    void reset() {
        for (int i = 0; i < enemyCount; ++i) {
            delete enemies[i];
            enemies[i] = nullptr;
        }
        enemyCount = 0;

        for (int i = 0; i < ringCount; ++i) {
            delete rings[i];
            rings[i] = nullptr;
        }
        ringCount = 0;
        totalRings = 0;

        for (int i = 0; i < spikeCount; ++i) {
            delete spikes[i];
            spikes[i] = nullptr;
        }
        spikeCount = 0;

        for (int i = 0; i < wallCount; ++i) {
            delete walls[i];
            walls[i] = nullptr;
        }
        wallCount = 0;

        for (int i = 0; i < extraLifeCount; ++i) {
            delete extraLives[i];
            extraLives[i] = nullptr;
        }
        extraLifeCount = 0;

        delete sonic;
        delete tails;
        delete knuckles;
        delete boss;
        sonic = nullptr;
        tails = nullptr;
        knuckles = nullptr;
        boss = nullptr;

        initCharacters();

        camera.setCenter(screen_x / 2.0f, screen_y / 2.0f);
        window.setView(camera);

        scoreManager.reset();
    }

    void update(float deltaTime) {
        if (state == STATE_GAMEPLAY) {
            Character* focus = characterManager.getCurrentCharacter();
            if (focus) {
                Tails* tails = dynamic_cast<Tails*>(focus);
                //flight logic in tails class

                Knuckles* knuckles = dynamic_cast<Knuckles*>(focus);
                if (knuckles) {
                    for (int i = 0; i < wallCount; ++i) {
                        if (walls[i] && !walls[i]->isBroken()) {
                            if (walls[i]->isNear(focus->x, focus->Pwidth, 1.5f * cell_size) && knuckles->isPunching) {
                                walls[i]->destroy();
                            }
                        }
                    }
                }

                characterManager.updateAll(deltaTime, level, window);

                camera.setCenter(focus->x + focus->Pwidth / 2, focus->y + focus->Pheight / 2);
                window.setView(camera);

                for (int i = 0; i < spikeCount; ++i) {
                    if (spikes[i]) spikes[i]->checkCollision(*focus);
                }

                for (int i = 0; i < wallCount; ++i) {
                    if (walls[i] && !walls[i]->isBroken()) {
                        walls[i]->checkCollision(*focus);
                    }
                }

                for (int i = 0; i < extraLifeCount; ++i) {
                    if (extraLives[i]) {
                        extraLives[i]->update(deltaTime);
                        extraLives[i]->checkCollision(*focus);
                    }
                }

                if (level.checkTriggerBlockCollision(focus->x, focus->y, focus->Pwidth, focus->Pheight)) {
                    level.clearTempBlocks();
                }

                

                if (level.checkEndBlockCollision(focus->x, focus->y, focus->Pwidth, focus->Pheight)) {
                    scoreManager.addScore(5000);
                    if (currentLevel < 3) {
                        loadNextLevel();
                    }else{
                        scoreManager.addScore(10000); //for defeating boss
                        leaderboard.addScore(playerName, scoreManager.getScore());
                        state = STATE_VICTORY; //victory screen
                    }
                }

                if (focus->y > (level.getGroundLevel() + 4) * cell_size) {
                    leaderboard.addScore(playerName, scoreManager.getScore());
                    state = STATE_GAMEOVER;
                    gameOverSelectedOption = 0;
                    updateGameOverMenuSelection();
                }
                else if (focus->hp <= 0) {
                    leaderboard.addScore(playerName, scoreManager.getScore());
                    state = STATE_GAMEOVER;
                    gameOverSelectedOption = 0;
                    updateGameOverMenuSelection();
                }

                for (int i = 0; i < enemyCount; ++i) {
                    if (enemies[i]) {
                        enemies[i]->update(deltaTime);
                        enemies[i]->checkCollision(*focus);
                        if (!enemies[i]->isAlive()) {
                            scoreManager.addScore(1000);
                            if (enemies[i] == boss) {
                                boss = nullptr; //boss pointer cleared
                            }
                            delete enemies[i];
                            enemies[i] = nullptr;
                        }
                    }
                }

                for (int i = 0; i < wallCount; ++i) {
                    if (walls[i] && walls[i]->isBroken()) {
                        delete walls[i];
                        walls[i] = nullptr;
                    }
                }

                for (int i = 0; i < ringCount; ++i) {
                    if (rings[i]) {
                        rings[i]->update(deltaTime);
                        if (rings[i]->checkCollision(*focus)) {
                            totalRings++;
                            scoreManager.addScore(100);
                        }
                    }
                }

                cout << "\r" << (focus->x)/cell_size << ", " << (focus->y)/cell_size;
            }

            hud.update(deltaTime, focus ? focus->hp : 0, totalRings, scoreManager.getScore());
        }
    }

    void renderMenu() {
        window.setView(window.getDefaultView());
        window.draw(menuBackground);
        for (int i = 0; i < NUM_OPTIONS; ++i) {
            window.draw(menuOptions[i]);
        }
    }

    void renderNameInput() {
        window.setView(window.getDefaultView());
        window.draw(pauseBackground);
        string display = "Enter Name (Enter to confirm):\n" + string(nameInput);
        nameInputText.setString(display);
        nameInputText.setPosition(screen_x / 2 - nameInputText.getGlobalBounds().width / 2, screen_y / 2 - 50);
        window.draw(nameInputText);
    }

    void renderLeaderboard() {
        window.setView(window.getDefaultView());
        window.draw(pauseBackground);
        window.draw(leaderboardText);
        leaderboard.draw(window, font, screen_x / 2 - 100, 300);
    }

    void renderInstructions() {
        window.setView(window.getDefaultView());
        window.draw(instructionsText);
    }

    void renderPauseMenu() {
        window.setView(window.getDefaultView());
        window.draw(pauseBackground);
        window.draw(pauseTitleText);
        for (int i = 0; i < PAUSE_NUM_OPTIONS; ++i) {
            window.draw(pauseOptions[i]);
        }
    }

    void renderGameOverMenu() {
        window.setView(window.getDefaultView());
        window.draw(gameOverBackground);
        window.draw(gameOverTitleText);
        for (int i = 0; i < GAMEOVER_NUM_OPTIONS; ++i) {
            window.draw(gameOverOptions[i]);
        }
    }

    void renderVictoryMenu() {
        window.setView(window.getDefaultView());
        window.draw(victoryBackground);
        window.draw(victoryTitleText);
        victoryScoreText.setString("Score: " + to_string(scoreManager.getScore()));
        victoryScoreText.setPosition(screen_x / 2 - victoryScoreText.getGlobalBounds().width / 2, 200);
        window.draw(victoryScoreText);
        window.draw(victoryOptionText);
    }

    void render() {
        window.clear(Color::Black);

        if (state == STATE_MENU) {
            renderMenu();
        }
        else if (state == STATE_NAME_INPUT) {
            renderNameInput();
        }
        else if (state == STATE_LEADERBOARD) {
            renderLeaderboard();
        }
        else if (state == STATE_INSTRUCTIONS) {
            renderInstructions();
        }
        else if (state == STATE_VICTORY) {
            renderVictoryMenu();
        }
        else if (state == STATE_GAMEPLAY || state == STATE_PAUSED || state == STATE_GAMEOVER) {
            window.setView(camera);

            backgroundSprites[currentLevel - 1].setPosition(camera.getCenter().x - screen_x / 2, camera.getCenter().y - screen_y / 2);
            window.draw(backgroundSprites[currentLevel - 1]);

            level.draw(window, wallSprites[currentLevel - 1][0], wallSprites[currentLevel - 1][1], wallSprites[currentLevel - 1][2], tempBlockSprite);

            for (int i = 0; i < spikeCount; i++) {
                if (spikes[i]) spikes[i]->draw(window);
            }

            for (int i = 0; i < wallCount; i++) {
                if (walls[i]) walls[i]->draw(window);
            }

            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i]) enemies[i]->draw(window);
            }

            for (int i = 0; i < ringCount; i++) {
                if (rings[i]) rings[i]->draw(window);
            }

            for (int i = 0; i < extraLifeCount; i++) {
                if (extraLives[i]) extraLives[i]->draw(window);
            }

            characterManager.drawAll(window);

            window.setView(window.getDefaultView());
            hud.draw(window);

            if (state == STATE_PAUSED) {
                renderPauseMenu();
            }
            else if (state == STATE_GAMEOVER) {
                renderGameOverMenu();
            }
        }

        window.display();
    }
};