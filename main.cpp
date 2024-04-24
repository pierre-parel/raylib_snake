#include <deque>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>

const int TILE_SIZE = 30;
const int CELLS_WIDTH = 20;
const int CELLS_HEIGHT = 20;

double lastUpdateTime = 0;

bool toUpdateSnake(double interval) 
{
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

void reset()
{
}

class Fruit
{
public:
	Vector2 pos = {0, 0};

	Fruit()
	{
		pos = generateRandomPosition();
	}

	Vector2 generateRandomPosition() 
	{
		float x = GetRandomValue(0, CELLS_WIDTH-1);
		float y = GetRandomValue(2, CELLS_HEIGHT-1);
		return Vector2{x,y};
	}

	void draw() 
	{
		// DrawRectangle(pos.x*TILE_SIZE, pos.y*TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
		DrawCircle(pos.x*TILE_SIZE + 15, pos.y*TILE_SIZE + 15, 15, RED);
	}
	
};

class Snake
{
public:
	std::deque<Vector2> snake_body = {Vector2{10,10},Vector2{9,10},Vector2{8,10}};
	Vector2 direction = {1, 0};

	void draw() 
	{
		for (auto &pos : snake_body) {
			// DrawRectangle(pos.x * TILE_SIZE, pos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GREEN);
			Rectangle rect;
			rect.x = pos.x * TILE_SIZE;
			rect.y = pos.y * TILE_SIZE;
			rect.width = TILE_SIZE;
			rect.height = TILE_SIZE;
			DrawRectangleRounded(rect, 0.4f, 2, GREEN);
		}
	}

	void update(Vector2& direction_in)
	{
		direction = direction_in;
		snake_body.pop_back();
		snake_body.push_front(Vector2Add(snake_body[0], direction));
	}

	void grow()
	{
		Vector2 new_body = Vector2Subtract(snake_body[snake_body.size()-1], snake_body[snake_body.size()-2]);
		snake_body.push_back(Vector2Add(snake_body[snake_body.size()-1], new_body));
		// snake_body.push_front(Vector2Add(snake_body[0], direction));
	}

	void collidedWithWall(int& score, Fruit& fruit)
	{
		for (auto &pos : snake_body) 
	        {
			if (pos.x >= CELLS_WIDTH || pos.y >= CELLS_HEIGHT || pos.x < 0 || pos.y < 0) 
			{
				score = 0;
				direction = {1, 0};
				*this = Snake();
			}
		}
	}

	void collidedWithSelf(int& score, Snake& snake)
	{
		std::deque<Vector2> headlessBody = snake.snake_body;
		headlessBody.pop_front();
		
		bool collided = false;

		for (Vector2 &body : headlessBody) 
		{
			if (Vector2Equals(body, snake.snake_body[0])) 
			{
				score = 0;
				direction = {1, 0};
				*this = Snake();
			}
		}
	}
};

bool fruitInBody(const Snake& snake, const Fruit& fruit)
{
	for (auto &body : snake.snake_body) 
	{
		if (Vector2Equals(fruit.pos, body))
		{
			return true;	
		}
	}
	return false;
}

int main() 
{
	InitWindow(CELLS_WIDTH * TILE_SIZE, CELLS_HEIGHT * TILE_SIZE, "Snake");
	SetTargetFPS(60);

	Fruit fruit = Fruit();
	Snake snake;
	int score = 0;
	int highscore = 0;

	Vector2 direction = {1, 0};
	
	while (!WindowShouldClose()) 
	{
		// Update
		if (IsKeyPressed(KEY_W) && snake.direction.y != 1) 
		{
			direction = {0, -1};
		}

		if (IsKeyPressed(KEY_S) && snake.direction.y != -1) 
		{
			direction = {0, 1};
		}

		if (IsKeyPressed(KEY_A) && snake.direction.x != 1) 
		{
			direction = {-1, 0};
		}

		if (IsKeyPressed(KEY_D) && snake.direction.x != -1) 
		{
			direction = {1, 0};
		}

		if (fruitInBody(snake, fruit))
		{
			fruit = Fruit();
			score++;
			snake.grow();

			if (highscore < score) 
			{
				highscore = score;
			}
		}
		
		snake.collidedWithWall(score, fruit);
		snake.collidedWithSelf(score, snake);

		if (toUpdateSnake(0.2)) {
			snake.update(direction);
		}



		// Render

		BeginDrawing();
		ClearBackground(BLACK);
		DrawText(TextFormat("Score: %d", score), 0, 0, 30, LIGHTGRAY);
		DrawText(TextFormat("High Score: %d", highscore), 0, 30, 30, LIGHTGRAY);
		snake.draw();
		fruit.draw();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
