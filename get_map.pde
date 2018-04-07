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

import processing.serial.*;
import java.util.Scanner;

Serial myPort;  // The serial port

PImage img0, img1, img2, img3;
static final int IMG_HEIGHT = 256;
static final int IMG_WIDTH = 256;
static final int X_START = 58196;
static final int Y_START = 25823;
static final String URL = "https://cyberjapandata.gsi.go.jp/xyz/";
static final String MAP_TYPE = "std";
static final int SCALE = 16;
static final String EXTENTION = ".png";
String urlPrefix = URL + MAP_TYPE + "/" + SCALE + "/";

int x, y;

String getUrl(int x, int y) {
    return urlPrefix + x + "/" + y + EXTENTION;
}

void setup() {
    size(512, 512);
    String portName = "/dev/cu.usbmodem1412";
    myPort = new Serial(this, portName, 9600);
    x = X_START;
    y = Y_START;
    img0 = loadImage(getUrl(X_START  , Y_START  ));
    img1 = loadImage(getUrl(X_START+1, Y_START  ));
    img2 = loadImage(getUrl(X_START  , Y_START+1));
    img3 = loadImage(getUrl(X_START+1, Y_START+1));
}

void draw() {
    if (myPort.available() > 0) {
        char c = myPort.readChar();
        switch (c) {
            case 'j':
                y++;
                img0 = img2.get();
                img1 = img3.get();
                img2 = loadImage(getUrl(x  , y+1));
                img3 = loadImage(getUrl(x+1, y+1));
                myPort.clear();
                //delay(100);
                myPort.write(0x61);
                break;
            case 'l':
                x--;
                img1 = img0.get();
                img3 = img2.get();
                img0 = loadImage(getUrl(x, y  ));
                img2 = loadImage(getUrl(x, y+1));
                myPort.clear();
                //delay(100);
                myPort.write(0x61);
                break;
            case 'p':
                String msg;
                //myPort.write(0x62);
                delay(100);
                while ((msg = myPort.readString()) == null) {}
                //while ((tmp = myPort.readString()) != null) {
                //  msg += tmp;
                //}
                Scanner scan = new Scanner(msg);
                if (scan.hasNextInt()) {
                    x = scan.nextInt();
                    if (scan.hasNextInt()) {
                        y = scan.nextInt();
                        img0 = loadImage(getUrl(x  , y  ));
                        img1 = loadImage(getUrl(x+1, y  ));
                        img2 = loadImage(getUrl(x  , y+1));
                        img3 = loadImage(getUrl(x+1, y+1));
                    }
                }
                myPort.clear();
                //delay(100);
                myPort.write(0x61);
                break;
            case 'u':
                y--;
                img2 = img0.get();
                img3 = img1.get();
                img0 = loadImage(getUrl(x  , y));
                img1 = loadImage(getUrl(x+1, y));
                myPort.clear();
                //delay(100);
                myPort.write(0x61);
                break;
            case 'r':
                x++;
                img0 = img1.get();
                img2 = img3.get();
                img1 = loadImage(getUrl(x+1, y  ));
                img3 = loadImage(getUrl(x+1, y+1));
                myPort.clear();
                //delay(100);
                myPort.write(0x61);
                break;
            default:
                break;
        }
    }

    image(img0, 0, 0);
    image(img1, IMG_WIDTH, 0);
    image(img2, 0, IMG_HEIGHT);
    image(img3, IMG_WIDTH, IMG_HEIGHT);
    delay(100);
}
