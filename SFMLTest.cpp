#include <iostream>
#include <math.h>
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#define PI 3.1415926

using namespace sf;
using namespace std;

void centerText(Text& text, RenderWindow& window) {
    auto center = text.getGlobalBounds().getSize() / 2.f;
    auto localBounds = center + text.getLocalBounds().getPosition();
    text.setOrigin(localBounds);
    text.setPosition(Vector2f(window.getSize().x / 2u, 30.f));
    text.setFillColor(Color{ 0x655A7CFF });
}

void init(RenderWindow& window, Vector2f& ballVelocity, float& ballSpeed, int& gameState, int& numberOfPlayers, CircleShape& ball, RectangleShape& paleta1, RectangleShape& paleta2, Text& text, int& score1, int& score2) {
    ball.setPosition(window.getSize().x / 2 - ball.getRadius(), window.getSize().y / 2 - ball.getRadius());

    paleta1.setPosition(25, window.getSize().y / 2);

    paleta2.setPosition(window.getSize().x - 25 - paleta2.getSize().x, window.getSize().y / 2);

    ballVelocity = Vector2f(ballSpeed, 0.1f);

    score1 = 0;
    score2 = 0;

    gameState = -1;//-1 = Menu, 0 = Playing, 1 = Player1 won, 2 = Player2 won
    numberOfPlayers = 1;
    text.setString("Press [1] to select 1 player, [2] to select 2 players and [ENTER] to play.");
    centerText(text, window);
}

int main()
{
    RenderWindow window(VideoMode(1280, 720), "Pong");
    window.setFramerateLimit(60);

    CircleShape ball(20);
    RectangleShape paleta(Vector2f(25, 150));
    RectangleShape paleta2(Vector2f(25, 150));

    float ballSpeed = 25.f;
    Vector2f ballVelocity;
    float playerSpeed = 8.f;

    int score1 = 0, score2 = 0;

    Font font;
    font.loadFromFile("Roboto-Black.ttf");

    int gameState;//-1 = Menu, 0 = Playing, 1 = Player1 won, 2 = Player2 won
    int numberOfPlayers;
    auto text = sf::Text{ "Press [1] to select 1 player, [2] to select 2 players and [ENTER] to play.", font };
    init(window, ballVelocity, ballSpeed, gameState, numberOfPlayers, ball, paleta, paleta2, text, score1, score2);



    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        //Update
        if (gameState == -1) {
            if (Keyboard::isKeyPressed(Keyboard::Num1))
                numberOfPlayers = 1;
            if (Keyboard::isKeyPressed(Keyboard::Num2))
                numberOfPlayers = 2;
            if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                gameState = 0;
                text.setString(to_string(score1) + " - " + to_string(score2));
                centerText(text, window);
            }
        }
        if (gameState == 0) {
            //Ball up and down
            if (ball.getPosition().y <= 0)
                ballVelocity.y = abs(ballVelocity.y);
            if (ball.getPosition().y + ball.getRadius() * 2 >= window.getSize().y)
                ballVelocity.y = -abs(ballVelocity.y);

            //Left pallete
            if (Keyboard::isKeyPressed(Keyboard::W))
                paleta.move(0, -playerSpeed);
            if (Keyboard::isKeyPressed(Keyboard::S))
                paleta.move(0, playerSpeed);

            if (paleta.getPosition().y < 0)
                paleta.setPosition(paleta.getPosition().x, 0);
            if (paleta.getPosition().y > window.getSize().y - paleta.getSize().y)
                paleta.setPosition(paleta.getPosition().x, window.getSize().y - paleta.getSize().y);

            //Right pallete
            if (numberOfPlayers == 2) {
                if (Keyboard::isKeyPressed(Keyboard::Up))
                    paleta2.move(0, -playerSpeed);
                if (Keyboard::isKeyPressed(Keyboard::Down))
                    paleta2.move(0, playerSpeed);
            }
            else {
                if (ball.getPosition().y - ball.getRadius() < paleta2.getPosition().y)
                    paleta2.move(0, -playerSpeed);
                else if (ball.getPosition().y - ball.getRadius() > paleta2.getPosition().y + paleta2.getSize().y)
                    paleta2.move(0, playerSpeed);
                else
                    paleta2.move(0, playerSpeed * ballVelocity.y / abs(ballVelocity.y));
            }

            if (paleta2.getPosition().y < 0)
                paleta2.setPosition(paleta2.getPosition().x, 0);
            if (paleta2.getPosition().y > window.getSize().y - paleta2.getSize().y)
                paleta2.setPosition(paleta2.getPosition().x, window.getSize().y - paleta2.getSize().y);

            //Ball collision with players
            if (paleta.getGlobalBounds().intersects(ball.getGlobalBounds())) {
                float relativeIntersectionY = paleta.getPosition().y + paleta.getSize().y / 2 - ball.getPosition().y - ball.getRadius();
                float normalizedRelativeIntersectionY = relativeIntersectionY / paleta.getSize().y;
                //cout << normalizedRelativeIntersectionY << endl;
                float bounceAngle = normalizedRelativeIntersectionY * 5.f / 12.f * float(PI);
                ballVelocity.x = ballSpeed * cos(bounceAngle);
                ballVelocity.y = ballSpeed * -sin(bounceAngle);
            }
            if (paleta2.getGlobalBounds().intersects(ball.getGlobalBounds())) {
                float relativeIntersectionY = paleta2.getPosition().y + paleta2.getSize().y / 2 - ball.getPosition().y - ball.getRadius();
                float normalizedRelativeIntersectionY = relativeIntersectionY / paleta2.getSize().y;
                //cout << normalizedRelativeIntersectionY << endl;
                float bounceAngle = normalizedRelativeIntersectionY * 5.f / 12.f * float(PI);
                ballVelocity.x = -ballSpeed * cos(bounceAngle);
                ballVelocity.y = ballSpeed * -sin(bounceAngle);
            }

            ball.move(ballVelocity);

            //Check for ending the game
            if (ball.getPosition().x < 0) {
                score2++;
                text.setString(to_string(score1) + " - " + to_string(score2));
                centerText(text, window);

                ball.setPosition(window.getSize().x / 2 - ball.getRadius(), window.getSize().y / 2 - ball.getRadius());

                paleta.setPosition(25, window.getSize().y / 2);

                paleta2.setPosition(window.getSize().x - 25 - paleta2.getSize().x, window.getSize().y / 2);

                ballVelocity = Vector2f(ballSpeed, 0.1f);

                if (score2 >= 10) {
                    text.setString("Player 2 won! Press [R] to restart.");
                    centerText(text, window);
                    gameState = 2;
                }
            }
            if (ball.getPosition().x > window.getSize().x - ball.getRadius() * 2) {
                score1++;
                text.setString(to_string(score1) + " - " + to_string(score2));
                centerText(text, window);

                ball.setPosition(window.getSize().x / 2 - ball.getRadius(), window.getSize().y / 2 - ball.getRadius());

                paleta.setPosition(25, window.getSize().y / 2);

                paleta2.setPosition(window.getSize().x - 25 - paleta2.getSize().x, window.getSize().y / 2);

                ballVelocity = Vector2f(ballSpeed, 0.1f);

                if (score1 >= 10) {
                    text.setString("Player 1 won! Press [R] to restart.");
                    centerText(text, window);
                    gameState = 1;
                }
            }
        }
        if (gameState == 1 || gameState == 2) {
            if (Keyboard::isKeyPressed(Keyboard::R))
                init(window, ballVelocity, ballSpeed, gameState, numberOfPlayers, ball, paleta, paleta2, text, score1, score2);
        }

        //Draw
        window.clear();

        if (gameState != 2)
            window.draw(paleta);
        if (gameState != 1 && (gameState != -1 || numberOfPlayers == 2))
            window.draw(paleta2);
        if (gameState == 0)
            window.draw(ball);

        window.draw(text);

        window.display();
    }

    return 0;
}