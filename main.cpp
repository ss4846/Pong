#include "raylib.h"
#include <algorithm>

const int W = 800, H = 600;
const float PADDLE_W = 12, PADDLE_H = 90, PADDLE_SPEED = 400;
const float BALL_R = 8, BALL_START_SPEED = 300;
const float AI_SPEED = 400;

enum class State { Menu, Playing, GameOver};
const int WIN_SCORE = 5;

struct Ball{
    Vector2 pos, vel;
};

void ResetBall(Ball& b, int dir){
    b.pos = {W / 2.0f, H / 2.0F};
    b.vel = {BALL_START_SPEED * dir, (float)GetRandomValue(-150, 150)};
}


int main(){
    InitWindow(W, H, "Pong");
    SetTargetFPS(60);
    State state = State::Menu;

    Rectangle left = {30, H / 2.0f - PADDLE_H / 2, PADDLE_W, PADDLE_H};
    Rectangle right = {W - 30 - PADDLE_W, H / 2.0f - PADDLE_H / 2, PADDLE_W, PADDLE_H};
    Ball ball;
    ResetBall(ball, 1);
    int scoreL = 0, scoreR = 0;

    while(!WindowShouldClose()){
        float dt = GetFrameTime();

        switch(state){
            case State::Menu:
                if (IsKeyPressed(KEY_SPACE)) state = State::Playing;
                break;
            
            case State::Playing:
                // Input: W/S for left paddle
                if (IsKeyDown(KEY_W)) left.y -= PADDLE_SPEED * dt;
                if (IsKeyDown(KEY_S)) left.y += PADDLE_SPEED * dt;
                        
                //Computer as right opponent
                if (ball.vel.x > 0) {
                    float target = ball.pos.y - PADDLE_H / 2;
                    float diff = target - right.y;
                    float step = AI_SPEED * dt;
                    if (diff > step) right.y += step;
                    if (diff < -step) right.y -=step;
                    else right.y = target;
                    }
                        
                left.y = std::clamp(left.y, 0.0f, H - PADDLE_H);
                right.y = std::clamp(right.y, 0.0f, H - PADDLE_H);
                        
                // Ball Movement
                ball.pos.x += ball.vel.x * dt;
                ball.pos.y += ball.vel.y * dt;
                        
                // Bounce off top or bottom walls
                if (ball.pos.y < BALL_R)    {ball.pos.y = BALL_R;      ball.vel.y *= -1;} //bot wall
                if (ball.pos.y > H - BALL_R){ball.pos.y = H - BALL_R;  ball.vel.y *= -1;} //top wall
                        
                //Paddle Collision
                for (Rectangle* p: {&left, &right}) {
                    if (CheckCollisionCircleRec(ball.pos, BALL_R, *p)) {
                        //Push the ball out so it can't get stuck inside the paddle
                        ball.pos.x = (p == &left)? p->x + p->width + BALL_R : p->x - BALL_R;
                        ball.vel.x *= -1.05f;
                        // Where it hit the paddle (-1 top .. +1 bottom) controls the angle
                        float hit = (ball.pos.y - (p->y + p->height / 2)) / (p->height / 2);
                        ball.vel.y = hit * 300;
                    }
                }
                        
                // Scoring
                if (ball.pos.x < 0) { scoreR++; ResetBall(ball, 1); }
                if (ball.pos.x > W) { scoreL++; ResetBall(ball, -1); }

                if (scoreL >= WIN_SCORE || scoreR >= WIN_SCORE) state = State::GameOver;
                break;
            
            case State::GameOver:
                if (IsKeyPressed(KEY_SPACE)) {
                    scoreL = scoreR = 0;
                    ResetBall(ball, 1);
                    left.y = right.y = H / 2.0f - PADDLE_H / 2;
                    state = State::Playing;
                }
                break;
        }


        //Draw
        BeginDrawing();
        ClearBackground(BLACK);
        for (int y = 0; y < H; y+= 30) DrawRectangle(W / 2 - 2, y, 4, 15, DARKGRAY);
        DrawRectangleRec(left, WHITE);
        DrawRectangleRec(right, WHITE);
        DrawCircleV(ball.pos, BALL_R, WHITE);
        DrawText(TextFormat("%d", scoreL), W / 2 - 60, 20, 40, WHITE);
        DrawText(TextFormat("%d", scoreR), W / 2 + 40, 20, 40, WHITE);
        if (state == State::Menu) {
            DrawText("Pong", W / 2 - 60, 220, 60, WHITE);
            DrawText("Press SPACE to start", W / 2 - 110, 300, 20, GRAY);
        }
        if (state == State::GameOver) {
            const char* msg = (scoreL >= WIN_SCORE)? "You win!": "AI wins!";
            DrawText(msg, W / 2 - MeasureText(msg, 50) / 2, 240, 50, WHITE);
            DrawText("Press SPACE to play again", W / 2 - 130, 310, 20, GRAY);
        }
        EndDrawing();
    }
    
    CloseWindow();
}