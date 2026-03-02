#include <iostream>
#include <easyx.h>
#include <vector>
#include <ctime>
#include <windows.h>

// 精灵类
class Sprite {
public:
    Sprite(): Sprite (0, 0) {};
    Sprite (int x, int y): m_x(x), m_y(y), m_color(RED) {};
    // 绘制精灵
    virtual void draw ( ) {
        // 设置填充颜色
        setfillcolor (m_color);
        // 绘制矩形
        fillrectangle (m_x, m_y, m_x + 10, m_y + 10);
    }
    // 移动
    void moveBy (int dx, int dy) {
        m_x += dx;
        m_y += dy;
    }
    // 碰撞检测
    bool collision (const Sprite& other) {
        return m_x == other.m_x && m_y == other.m_y;
    }
    // 获取坐标接口，供子类访问私有/保护成员
    int getX ( ) const {
        return m_x;
    }
    int getY ( ) const {
        return m_y;
    }
protected:
    int m_x;
    int m_y;
    COLORREF m_color;
};

// 蛇类
class Snake: public Sprite {
public:
    Snake ( ): Snake (0, 0) {}
    Snake (int x, int y): Sprite (x, y), dir (VK_RIGHT) {
        // 初始化三节蛇
        nodes.push_back(Sprite (20, 0));
        nodes.push_back(Sprite (10, 0));
        nodes.push_back(Sprite (0, 0));
    }
    void draw ( ) override {
        for ( int i = 0; i < nodes.size(); ++ i ) {
            nodes[i].draw();
        }
    }
    // 蛇的身体移动
    void bodyMove ( ) {
        if (nodes.empty()) return;
        // 身体跟着蛇头移动
        for (size_t i = nodes.size() - 1; i > 0;  -- i) {
            nodes[i] = nodes[i - 1];
        }
        // 移动蛇头
        switch (dir) {
            case VK_UP:
                nodes[0].moveBy(0, -10);
                break;
            case VK_DOWN:
                nodes[0].moveBy(0, 10);
                break;
            case VK_LEFT:
                nodes[0].moveBy(-10, 0);
                break;
            case VK_RIGHT:
                nodes[0].moveBy(10, 0);
                break;
        }
    }
    bool collision (const Sprite& other) {
        return nodes[0].collision(other);
    }
    // 蛇增加一节
    void incrment ( ) {
        nodes.push_back(Sprite());
    }
    bool checkCollision (int width, int height) {
        int nextX = nodes[0].getX();
        int nextY = nodes[0].getY();

        switch (dir) {
            case VK_UP: nextY -= 10; break;
            case VK_DOWN: nextY += 10; break;
            case VK_LEFT: nextX -= 10; break;
            case VK_RIGHT: nextX += 10; break;
        }
        // 检查是否撞墙
        if (nextX < 0 || nextX >= width || nextY < 0 || nextY >= height) {
            return true;
        }
        // 检查是否撞到身体
        for (size_t i = 1; i < nodes.size(); ++ i) {
            if ( nextX == nodes[i].getX() && nextY == nodes[i].getY()) {
                return true;
            }
        }
        return false;
    }
private:
    std:: vector <Sprite> nodes; // 蛇的所有节点
// 蛇的方向
public:
    int dir;
};

// 食物
class Food: public Sprite {
public:
    Food ( ): Sprite (0, 0) {
        changePos();
    }
    void draw ( ) override {
        setfillcolor(m_color);
        solidellipse (m_x, m_y, m_x + 10, m_y + 10);
    }
    // 改变食物坐标
    void changePos ( ) {
        // 随机生成坐标 （保证食物的坐标一定是10的整数倍）
        m_x = rand() % 64 * 10;
        m_y = rand() % 48 * 10;
    }
};

// 场景
class GameScene {
public:
    GameScene ( ): m_isOver(false) {}; // 初始化游戏结束标志位false
    void run ( ) {
        // 如果游戏已经结束，只绘制画面，不再更新逻辑
        if (m_isOver) {
            drawGameOver();
            return;
        }
        // 获取消息
        ExMessage msg = {0};
        while (peekmessage (&msg, EX_KEY)) {
            onMsg(msg);
        }
        
        // 检测死亡
        if (snake.checkCollision (640, 480)) {
            m_isOver = true;
            drawGameOver();
            return; // 结束当前帧逻辑
        }

        // 移动蛇，改变蛇的坐标
        snake.bodyMove();
        snakeEatFood();
        // 双缓冲绘图
        BeginBatchDraw();
        // 清屏
        cleardevice();
        snake.draw();
        food.draw();
        EndBatchDraw();
    }
    // 响应消息（鼠标、键盘）
    void onMsg (const ExMessage& msg) {
        // 如果有键盘消息（有没有按键按下）
        if (msg.message == WM_KEYDOWN) {
            // 改变蛇的移动方向 获取键盘按键
            // 判断具体是哪个按键按下
            // virtual key code 虚拟键码
            switch  (msg.vkcode) {
                case VK_UP:
                    if (snake.dir != VK_DOWN) 
                        snake.dir = msg.vkcode;
                    break;
                case VK_DOWN:
                    if (snake.dir != VK_UP) 
                        snake.dir = msg.vkcode;
                    break;
                case VK_LEFT:
                    if (snake.dir != VK_RIGHT) 
                        snake.dir = msg.vkcode;
                    break;
                case VK_RIGHT:
                    if (snake.dir != VK_LEFT) 
                        snake.dir = msg.vkcode;
                    break;
            }
        }
    }
    // 判断蛇能否吃到食物
    void snakeEatFood ( ) {
        // 如果蛇和食物产生了碰撞
        if (snake.collision(food)) {
            // 蛇的节数增加
            snake.incrment();
            // 食物重新产生在别的位置
            food.changePos();
        }
    }
    bool isGameOver ( ) const {
        return m_isOver;
    }
private:
    Snake snake;
    Food food;
    bool m_isOver;
    // 绘制游戏结束画面
    void drawGameOver ( ) {
        BeginBatchDraw();
        cleardevice();
        snake.draw();
        food.draw();
        // 显示Game Over文字
        settextcolor(RED);
        settextstyle (50, 0, _T("宋体")); // 设置字体大小和样式
        setbkmode(TRANSPARENT); // 设置文字背景透明避免遮盖背景
        outtextxy (220, 200, _T("Game Over"));
        EndBatchDraw();
    }
};

int main ( ) {
    initgraph (640, 480);

    // 设置随机数种子
    srand (time(nullptr));

    GameScene scene;
    while (true) {
        scene.run();
        if (scene.isGameOver()) {
            break;
        }
        Sleep(100);
    }

    getchar();
    closegraph();
    return 0;
}