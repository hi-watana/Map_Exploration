/*
 * This file is part of Map_Exploration_Game.
 *
 * Map_Exploration_Game is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Map_Exploration_Game is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Map_Exploration_Game.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mbed.h"
#include "C12832.h"
#include <string>
#include <limits.h>
#include <cmath>

#define X_START 58196
#define Y_START 25823
#define X_MAX 58220
#define X_MIN 58120
#define Y_MAX 25855
#define Y_MIN 25795
#define X_LENGTH 100
#define Y_LENGTH 60

Serial pc(USBTX, USBRX);
InterruptIn jdown(p12), jleft(p13), button(p14), jup(p15), jright(p16);
C12832 lcd(p5, p7, p6, p8, p11);
DigitalOut led1(LED1), led2(LED2), led3(LED3), led4(LED4);

Mutex dst_mutex, here_mutex;
EventQueue queue;
Thread th1(osPriorityHigh), th2(osPriorityNormal);

class Tile {
private:
    std::string name;
    int x;
    int y;
public:
    Tile(std::string name, int x, int y);
    std::string getName();
    int getX();
    int getY();
    void setX(int x);
    void setY(int y);
    float distance(Tile tile);
    int roadLength(Tile tile);
    bool isReached(Tile tile);
};

Tile::Tile(std::string name, int x, int y) {
    this->name = name;
    this->x = x;
    this->y = y;
}

std::string Tile::getName() {
    return this->name;
}

int Tile::getX() {
    return this->x;
}

int Tile::getY() {
    return this->y;
}

void Tile::setX(int x) {
    this->x = x;
}

void Tile::setY(int y) {
    this->y = y;
}

int myabs(int x) {
    return (x > 0) ? x : -x;
}

float Tile::distance(Tile tile) {
    int dx = this->x - tile.getX();
    int dy = this->y - tile.getY();
    return std::sqrt(dx * dx + dy * dy);
}

int Tile::roadLength(Tile tile) {
    int dx = this->x - tile.getX();
    int dy = this->y - tile.getY();
    return myabs(dx) + myabs(dy);
}

bool Tile::isReached(Tile tile) {
    int dx = this->x - tile.getX();
    int dy = this->y - tile.getY();
    return (0 <= dx && dx <= 1 && 0 <= dy && dy <= 1) ? true : false;
    // return dx == 0 && dy == 0;
}

Tile here("present location", X_START, Y_START),
     dst("destination", X_START, Y_START);
Tile places[] = {Tile("Ookayama Station", 58196, 25823),
                 Tile("Yokohama Station", 58185, 25854),
                 Tile("Suzukakedai Station", 58160, 25843),
                 Tile("TOKYO SKYTREE", 58219, 25800),
                 Tile("Takaosanguchi Station", 58121, 25817),
                };
#define PLACE_LENGTH sizeof(places) / sizeof(*places)

int myrandom(int min, int max) {
    return min + rand() % (max - min);
}

void init() {
    here_mutex.lock();
    here.setX(myrandom(X_MIN, X_MAX + 1));
    here.setY(myrandom(Y_MIN, Y_MAX + 1));

    dst_mutex.lock();
    dst = places[myrandom(0, PLACE_LENGTH)];
    dst_mutex.unlock();

    pc.putc('p');
    pc.printf("%d %d\n", here.getX(), here.getY());
    here_mutex.unlock();
    while (pc.getc() != 'a') {
        Thread::wait(100);
    }
    lcd.cls();
    lcd.locate(0, 0);
    dst_mutex.lock();
    lcd.printf("Find %s!\n"
               "Restart: the joystick button\n",
               dst.getName().c_str());
    dst_mutex.unlock();
}

void turn_down() {
    here_mutex.lock();
    here.setY(here.getY() + 1);
    here_mutex.unlock();
    pc.putc('j');
    while (pc.getc() != 'a') {
        Thread::wait(100);
    }
}

void turn_left() {
    here_mutex.lock();
    here.setX(here.getX() - 1);
    here_mutex.unlock();
    pc.putc('l');
    while (pc.getc() != 'a') {
        Thread::wait(100);
    }
}

void press() {
    init();
}

void turn_up() {
    here_mutex.lock();
    here.setY(here.getY() - 1);
    here_mutex.unlock();
    pc.putc('u');
    while (pc.getc() != 'a') {
        Thread::wait(100);
    }
}

void turn_right() {
    here_mutex.lock();
    here.setX(here.getX() + 1);
    here_mutex.unlock();
    pc.putc('r');
    while (pc.getc() != 'a') {
        Thread::wait(100);
    }
}

void interrupt_handler() {
    jdown.rise(queue.event(turn_down));
    jleft.rise(queue.event(turn_left));
    button.rise(queue.event(press));
    jup.rise(queue.event(turn_up));
    jright.rise(queue.event(turn_right));
    queue.dispatch();
}

void check() {
    bool flag, flag_prev;
    while (true) {
        dst_mutex.lock();
        here_mutex.lock();
        if ((flag = dst.isReached(here)) && !flag_prev) {
            lcd.cls();
            lcd.locate(0, 0);
            lcd.printf("Congratulations!\n"
                       "press the joystick button\n");
        }
        here_mutex.unlock();
        dst_mutex.unlock();
        flag_prev = flag;
        Thread::wait(500);
    }
}

int main() {
    led1 = 1;
    led2 = 1;
    led3 = 1;
    led4 = 1;
    lcd.cls();
    lcd.locate(0, 0);
    lcd.printf("Map Exploration!\n"
               "Hold the joystick button\n"
               "for a short time.\n");
    DigitalIn *jbutton = new DigitalIn(p14);
    int i = 0;
    while (!*jbutton) {
        i++;
        Thread::wait(10);
    }
    delete jbutton;
    srand(i);
    init();
    th1.start(interrupt_handler);
    th2.start(check);

    led1 = 1;
    led2 = 0;
    led3 = 0;
    led4 = 0;
    int led1_prev = 1, led2_prev = 0, led3_prev = 0, led4_prev = 0;
    while (true) {
        dst_mutex.lock();
        here_mutex.lock();
        // int rl = dst.roadLength(here);
        float rl = dst.distance(here);
        bool flag = dst.isReached(here);
        here_mutex.unlock();
        dst_mutex.unlock();
        //if (rl > 0) {
        if (!flag) {
            led1 = led2_prev;
            led2 = led3_prev;
            led3 = led4_prev;
            led4 = led1_prev;
            led1_prev = led1;
            led2_prev = led2;
            led3_prev = led3;
            led4_prev = led4;
            Thread::wait(rl * 12);
        } else {
            led1 = 1;
            led2 = 1;
            led3 = 1;
            led4 = 1;
            Thread::wait(500);
        }
        Thread::wait(500);
    }
}
