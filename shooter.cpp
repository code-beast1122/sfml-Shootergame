#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

using namespace sf;
using namespace std;

VideoMode desktop = VideoMode::getDesktopMode();
const int MAX_WIDTH = desktop.width;
const int MAX_HEIGHT = desktop.height;
const int MAX_enemies = 100;
const int MAX_BULLETS = 50;
const int MAX_LIVES = 3;

struct Bullet {
    Sprite sprite;
    float dx, dy;
    bool active = false;
};

struct GameState {
    Sprite enemies[MAX_enemies];
    Bullet bullets[MAX_BULLETS];
    int enemyCount = 0;
    int bulletCount = 0;
};

void LiveHearts(RenderWindow& window, Sprite hearts[], int lives) {
    for (int i = 0; i < lives; ++i) {
        window.draw(hearts[i]);
    }
}

void spawnenemy(GameState& gameState, Texture& enemyTexture, float dxx = 0 + rand() % MAX_WIDTH, float dyy = -10 + rand() % -1) {
    if (gameState.enemyCount < MAX_enemies) {
        Sprite& enemy = gameState.enemies[gameState.enemyCount];
        enemy = Sprite(enemyTexture);
        enemy.setPosition(dxx, dyy);
        enemy.scale(0.25f, 0.25f);
        gameState.enemyCount++;
    }
}

void updateenemy(GameState& gameState, float speed) {
    for (int i = 0; i < gameState.enemyCount; i++) {
        gameState.enemies[i].move(0, float(speed));
    }
}

void removeenemy(GameState& gameState) {
    for (int i = gameState.enemyCount - 1; i >= 0; i--) {
        if (gameState.enemies[i].getPosition().y > MAX_HEIGHT) {
            gameState.enemies[i] = gameState.enemies[gameState.enemyCount - 1];
            gameState.enemyCount--;
        }
    }
}

void updateBullets(GameState& gameState) {
    for (int i = gameState.bulletCount - 1; i >= 0; i--) {
        Bullet& bullet = gameState.bullets[i];
        bullet.sprite.move(bullet.dx, bullet.dy);

        auto pos = bullet.sprite.getPosition(); // auto automatically deduce the data type
        if (pos.x < 0 || pos.x > MAX_WIDTH || pos.y < 0 || pos.y > MAX_HEIGHT) {
            gameState.bullets[i] = gameState.bullets[gameState.bulletCount - 1];
            gameState.bulletCount--;
        }
    }
}

bool checkCollision(const Sprite& sp1, const Sprite& sp2) {
    FloatRect a = sp1.getGlobalBounds();
    FloatRect b = sp2.getGlobalBounds();

    float adjustment1 = 20.0f;
    a.left += adjustment1;
    a.top += adjustment1;
    a.width -= 1 * adjustment1;
    a.height -= 1 * adjustment1;
    float adjustment2 = 10.0f;
    b.left += adjustment2;
    b.top += adjustment2;
    b.width -= 1 * adjustment2;
    b.height -= 1 * adjustment2;

    return a.intersects(b);
}

int main() {
    int x = 0;
    srand(time(0));
    RenderWindow window(VideoMode(MAX_WIDTH, MAX_HEIGHT), "Space Shooter Game", Style::Fullscreen);
    window.setFramerateLimit(80);

    // Load textures
    Texture backgroundTexture, spaceshipTexture, enemyTexture, bulletTexture, HeartTexture;
    backgroundTexture.loadFromFile("imags.jpeg");
    spaceshipTexture.loadFromFile("spaceship1.png");
    enemyTexture.loadFromFile("enemy.png");
    bulletTexture.loadFromFile("bullet.png");
    HeartTexture.loadFromFile("heart.png");

    Sprite hearts[MAX_LIVES];

    // Setup background
    Sprite background(backgroundTexture);
    background.setScale(static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x, static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y);

    // Setup spaceship
    Sprite spaceship(spaceshipTexture);
    spaceship.setOrigin(spaceshipTexture.getSize().x / 2.0f, spaceshipTexture.getSize().y / 2.0f);
    spaceship.setPosition(MAX_WIDTH / 2, 900);
    spaceship.setRotation(270.f);
    spaceship.scale(0.25f, 0.25f);

    //lives
    for (int i = 0; i < MAX_LIVES; ++i) {
        hearts[i].setTexture(HeartTexture);
        hearts[i].setPosition(MAX_WIDTH - 190 + i * 30, 10);
        hearts[i].scale(0.1f, 0.1f);
    }

    // Game variables
    float speed = 300.0f;
    float rotationSpeed = 180.0f;
    float bulletSpeed = 50.0f;
    int score = 0;
    bool gameOver = false;
    Clock clock;
    Clock shootCooldown;
    GameState gameState;

    // Setup score text
    Font font;
    font.loadFromFile("arial.ttf");

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10);

    Text levelText;
    levelText.setFont(font);
    levelText.setCharacterSize(30);
    levelText.setFillColor(Color::White);
    levelText.setPosition(10, MAX_HEIGHT-30);

    Text wontext;
    wontext.setFont(font);
    wontext.setCharacterSize(100);
    wontext.setFillColor(Color::Red);
    wontext.setOutlineColor(Color::Yellow);
    wontext.setOutlineThickness(10);
    wontext.setPosition((MAX_WIDTH/2)-550, (MAX_HEIGHT/2)-100);

    SoundBuffer bulletsoundBuffer;
    if (!bulletsoundBuffer.loadFromFile("sound.wav")) {
        cerr << "Error loading sound file!" << endl;
        return -1;
    }

    Sound bulletsound;
    bulletsound.setBuffer(bulletsoundBuffer);

    SoundBuffer gameoversoundBuffer;
    if (!gameoversoundBuffer.loadFromFile("gameover.wav")) {
        cerr << "Error loading sound file!" << endl;
        return -1;
    }

    Sound gameoversound;
    gameoversound.setBuffer(gameoversoundBuffer);

    SoundBuffer explosionBuffer;
    if (!explosionBuffer.loadFromFile("explosion.wav")) {
        cerr << "Error loading sound file!" << endl;
        return -1;
    }

    Sound explosionsound;
    explosionsound.setBuffer(explosionBuffer);

    SoundBuffer loseBuffer;
    if (!loseBuffer.loadFromFile("lose.wav")) {
        cerr << "Error loading sound file!" << endl;
        return -1;
    }
    Sound losesound;
    losesound.setBuffer(loseBuffer);

    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(100);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setOutlineColor(Color::Black);
    gameOverText.setOutlineThickness(20);

    RectangleShape textbox;
    textbox.setSize(Vector2f(900, 500));
    textbox.setPosition((MAX_WIDTH / 2) - 450, (MAX_HEIGHT / 2) - 250);
    textbox.setFillColor(Color::Yellow);

    int lives = MAX_LIVES;
    int level = 0;
    bool won = 0;
    int enespeed = 3.0f;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                window.close();

            if (gameOver && event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                // Reset game
                gameOver = false;
                lives = MAX_LIVES;
                score = 0;
                level = 0;
                gameState.enemyCount = 0;
                gameState.bulletCount = 0;
                spaceship.setPosition(MAX_WIDTH / 2, 900);
                spaceship.setRotation(270.f);
            }
            if (won && event.type == Event::KeyPressed && event.key.code == Keyboard::Return) {
                // Reset game
                won = 0;
                lives = MAX_LIVES;
                score = 0;
                level = 0;
                gameState.enemyCount = 0;
                gameState.bulletCount = 0;
                spaceship.setPosition(MAX_WIDTH / 2, 900);
                spaceship.setRotation(270.f);
            }
        }
        if (won == 0) {
            if (!gameOver) {
                float deltaTime = clock.restart().asSeconds();
                if (level == 0) {
                    if (x % 50 == 0) {
                        spawnenemy(gameState, enemyTexture);
                    }
                }
                if (level == 1) {
                    if (x % 40 == 0) {
                        spawnenemy(gameState, enemyTexture);
                    }
                }
                if (level == 2) {
                    if (x % 20 == 0) {
                        spawnenemy(gameState, enemyTexture);
                    }
                }
                if (level == 3) {
                    if (x % 10 == 0) {
                        spawnenemy(gameState, enemyTexture);
                    }
                }
                
                x++;
                // Update enemies
                if (level == 0) {
                    enespeed = 3.0f;
                }
                if (level == 1) {
                    enespeed = 4.0f;
                }
                if (level == 2) {
                    enespeed = 5.0f;
                }
                if (level == 3) {
                    enespeed = 6.0f;
                    speed = 350.f;
                }
                updateenemy(gameState, enespeed);
                removeenemy(gameState);

                // Spaceship controls
                if (Keyboard::isKeyPressed(Keyboard::Left))
                    spaceship.rotate(-rotationSpeed * deltaTime);
                if (Keyboard::isKeyPressed(Keyboard::Right))
                    spaceship.rotate(rotationSpeed * deltaTime);

                // Movement and boundary
                if (Keyboard::isKeyPressed(Keyboard::Up)) {
                    spaceship.move(0.1f, 0.1f);
                    if (spaceship.getPosition().x > 0 && spaceship.getPosition().x < MAX_WIDTH && spaceship.getPosition().y < MAX_HEIGHT && spaceship.getPosition().y > 0) {
                        float angleRad = spaceship.getRotation() * (3.14159f / 180.0f);
                        spaceship.move(std::cos(angleRad) * speed * deltaTime, std::sin(angleRad) * speed * deltaTime);
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Down)) {
                    spaceship.move(-0.1f, -0.1f);
                    if (spaceship.getPosition().x > 0 && spaceship.getPosition().x < MAX_WIDTH && spaceship.getPosition().y < MAX_HEIGHT && spaceship.getPosition().y > 0) {
                        float angleRad = spaceship.getRotation() * (3.14159f / 180.0f);
                        spaceship.move(-std::cos(angleRad) * speed * deltaTime, -std::sin(angleRad) * speed * deltaTime);
                    }
                }

                // Shooting
                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                    if (level == 0 && shootCooldown.getElapsedTime().asSeconds() > 0.4f && gameState.bulletCount < MAX_BULLETS) {
                        Bullet& bullet = gameState.bullets[gameState.bulletCount];
                        bullet.sprite.setTexture(bulletTexture);
                        bullet.sprite.setScale(0.5f, 0.5f);
                        bullet.sprite.setRotation(360.f);
                        bullet.sprite.setOrigin(bulletTexture.getSize().x / 2.0f, bulletTexture.getSize().y / 2.0f);
                        bullet.sprite.setPosition(spaceship.getPosition());
                        bullet.sprite.setRotation(spaceship.getRotation());

                        float angleRad = spaceship.getRotation() * (3.14159f / 180.0f);
                        bullet.dx = cos(angleRad) * bulletSpeed;
                        bullet.dy = sin(angleRad) * bulletSpeed;

                        bulletsound.play();
                        gameState.bulletCount++;
                        shootCooldown.restart();
                    }
                    if (level == 1 && shootCooldown.getElapsedTime().asSeconds() > 0.3f && gameState.bulletCount < MAX_BULLETS) {
                        Bullet& bullet = gameState.bullets[gameState.bulletCount];
                        bullet.sprite.setTexture(bulletTexture);
                        bullet.sprite.setScale(0.5f, 0.5f);
                        bullet.sprite.setRotation(360.f);
                        bullet.sprite.setOrigin(bulletTexture.getSize().x / 2.0f, bulletTexture.getSize().y / 2.0f);
                        bullet.sprite.setPosition(spaceship.getPosition());
                        bullet.sprite.setRotation(spaceship.getRotation());

                        float angleRad = spaceship.getRotation() * (3.14159f / 180.0f);
                        bullet.dx = cos(angleRad) * bulletSpeed;
                        bullet.dy = sin(angleRad) * bulletSpeed;

                        bulletsound.play();
                        gameState.bulletCount++;
                        shootCooldown.restart();
                    }
                    if (level >= 2 && shootCooldown.getElapsedTime().asSeconds() > 0.2f && gameState.bulletCount < MAX_BULLETS) {
                        Bullet& bullet = gameState.bullets[gameState.bulletCount];
                        bullet.sprite.setTexture(bulletTexture);
                        bullet.sprite.setScale(0.5f, 0.5f);
                        bullet.sprite.setRotation(360.f);
                        bullet.sprite.setOrigin(bulletTexture.getSize().x / 2.0f, bulletTexture.getSize().y / 2.0f);
                        bullet.sprite.setPosition(spaceship.getPosition());
                        bullet.sprite.setRotation(spaceship.getRotation());

                        float angleRad = spaceship.getRotation() * (3.14159f / 180.0f);
                        bullet.dx = cos(angleRad) * bulletSpeed;
                        bullet.dy = sin(angleRad) * bulletSpeed;

                        bulletsound.play();
                        gameState.bulletCount++;
                        shootCooldown.restart();
                    }
                }

                updateBullets(gameState);


                for (int i = gameState.bulletCount - 1; i >= 0; i--) {
                    bool bulletHit = false;
                    for (int j = gameState.enemyCount - 1; j >= 0; j--) {
                        if (checkCollision(gameState.bullets[i].sprite, gameState.enemies[j])) {
                            gameState.enemies[j] = gameState.enemies[gameState.enemyCount - 1];
                            gameState.enemyCount--;
                            bulletHit = true;
                            explosionsound.play();
                            score += 10;
                            if (score == 200 || score == 500 || score == 800 || score == 1500) {
                                level += 1;
                            }
                            if (level == 4) {
                                won = 1;
                            }


                            break;
                        }
                    }

                    if (bulletHit) {
                        gameState.bullets[i] = gameState.bullets[gameState.bulletCount - 1];
                        gameState.bulletCount--;
                    }
                }

                for (int i = 0; i < gameState.enemyCount; i++) {
                    if (checkCollision(spaceship, gameState.enemies[i])) {
                        if (lives > 0) {
                            lives--;
                            gameState.enemies[i].setPosition(MAX_WIDTH / 2, MAX_HEIGHT + 500);
                            spaceship.setPosition(MAX_WIDTH / 2, 900);
                            spaceship.setRotation(270.f);
                            losesound.play();
                        }
                        if (lives == 0) {
                            gameOver = true;
                            gameoversound.play();
                            break;
                        }
                    }
                }
            }
        }
        // Render
        window.clear();
        window.draw(background);
        
        scoreText.setString("Score: " + to_string(score));
        levelText.setString("LEVEL: " + to_string(level));
        wontext.setString("CONGRATULATIONS!!!!\n          YOU WON");

        // Draw enemies
        for (int i = 0; i < gameState.enemyCount; i++)
            window.draw(gameState.enemies[i]);

        // Draw bullets
        for (int i = 0; i < gameState.bulletCount; i++)
            window.draw(gameState.bullets[i].sprite);

        window.draw(spaceship);
        LiveHearts(window, hearts, lives);
        window.draw(scoreText);
        window.draw(levelText);

        if (gameOver) {
            window.draw(textbox);
            gameOverText.setPosition((MAX_WIDTH / 2) - 425, (MAX_HEIGHT / 2) - 300);
            gameOverText.setString("\n    GAME OVER!\n Press R to restart\n       Score: " + std::to_string(score));
            window.draw(gameOverText);
        }
        if (won) {
            window.draw(wontext);
        }

        window.display();
    }

    return 0;
}