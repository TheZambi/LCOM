#include "fireball.h"
#include "../res/fireball0.xpm"
#include "enemy.h"
#include "math.h"
#include "player.h"
#include "vbe.h"

extern vbe_mode_info_t vbe;
extern uint8_t *background;
extern void *double_buffer;
extern int totalMovX;
extern int totalMovY;
extern struct Player player2;
extern struct Fireball fireball;
extern struct Platform *(platforms[10]);
extern bool fireballThrown;
extern struct Enemy *(enemies[10]);
extern size_t nEnemies;
extern size_t nPlatforms;
extern struct Score score;
struct Fireball fireball, fireball2;
extern bool fireball2Thrown;
extern struct Player player1;

void createFireball2() {
  xpm_image_t img;
  fireball2.sprite = xpm_load(fireball0, XPM_8_8_8, &img);
  fireball2.x = player2.x;
  fireball2.y = player2.y;
  fireball2.height = img.height;
  fireball2.width = img.width;
  fireball2.baseSpeed = 8;
}

void createFireball() {
  xpm_image_t img;
  fireball.sprite = xpm_load(fireball0, XPM_8_8_8, &img);
  fireball.x = 0;
  fireball.y = 0;
  fireball.height = img.height;
  fireball.width = img.width;
  fireball.baseSpeed = 8;
}

void checkHit() {
  for (size_t i = 0; i < nEnemies; i++) {
    if (enemies[i] != NULL && enemies[i]->hp != 0) {
      if ((fireball.x < enemies[i]->x + enemies[i]->width && fireball.x > enemies[i]->x) ||
          (fireball.x + fireball.width < enemies[i]->x + enemies[i]->width && fireball.x + fireball.width > enemies[i]->x)) {
        if (fireball.speedY < 0) // GOING DOWN
        {
          if ((fireball.y + fireball.height > enemies[i]->y &&
               fireball.y + fireball.height < enemies[i]->y + enemies[i]->height) ||
              (fireball.y > enemies[i]->y && fireball.y < enemies[i]->y + enemies[i]->height)) {
            fireballThrown = false;
            enemies[i]->hp--;
            score.playerScore += 100;
            break;
          }
        }
        else {
          if ((fireball.y + fireball.height > enemies[i]->y &&
               fireball.y + fireball.height < enemies[i]->y + enemies[i]->height) ||
              (fireball.y > enemies[i]->y && fireball.y < enemies[i]->y + enemies[i]->height)) {
            fireballThrown = false;
            enemies[i]->hp--;
            score.playerScore += 100;
            break;
          }
        }
      }
    }
  }
}

void fireballAngle() {
  if (totalMovX == 0) {
    fireball.speedY = fireball.baseSpeed;
    fireball.speedX = 0;
    return;
  }
  double declive = (double) (totalMovY) / totalMovX;
  double angle = atan(declive);
  fireball.angle = angle;
  fireball.speedX = round((cos(fireball.angle) * fireball.baseSpeed));
  fireball.speedY = round((sin(fireball.angle) * fireball.baseSpeed));
  if (totalMovX > 0)
    fireball.speedX = abs(fireball.speedX);
  else
    fireball.speedX = -abs(fireball.speedX);
  if (totalMovY < 0)
    fireball.speedY = -abs(fireball.speedY);
  else
    fireball.speedY = abs(fireball.speedY);
}

void clearFireball(uint8_t *buff) {
  for (int row = 0; row < fireball.height; row++) ///// BACKGROUND
  {
    for (int col = 0; col < fireball.width; col++) {

      drawPixel(&(background[((fireball.x + col) + (fireball.y + row) * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), buff, fireball.y + row, fireball.x + col);
    }
  }
}

void clearFireball2(uint8_t *buff) {
  for (int row = 0; row < fireball2.height; row++) ///// BACKGROUND
  {
    for (int col = 0; col < fireball2.width; col++) {

      drawPixel(&(background[((fireball2.x + col) + (fireball2.y + row) * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), buff, fireball2.y + row, fireball2.x + col);
    }
  }
}

void fireballCollideWithPlat() {
  if (fireball.x > vbe.XResolution) {
    fireballThrown = false;
    fireball.x = vbe.XResolution - fireball.width;
    fireball.speedX = 0;
    return;
  }
  if ((fireball.x + fireball.width) < 0) {
    fireball.x = fireball.speedX;
    fireball.speedX = 0;
    fireballThrown = false;
    return;
  }
  if (fireball.y > vbe.YResolution) {
    fireball.y = vbe.YResolution - fireball.height;
    fireball.speedY = 0;
    fireballThrown = false;
    fireball.speedY = 0;
    fireball.speedX = 0;
    return;
  }
  if ((fireball.y + fireball.height) < 0) {
    fireball.y = 1;
    fireball.speedY = 0;
    fireballThrown = false;
    fireball.speedY = 0;
    fireball.speedX = 0;
    return;
  }
  for (size_t i = 0; i < nPlatforms; i++) {
    if ((fireball.x < platforms[i]->x + platforms[i]->width && fireball.x > platforms[i]->x) ||
        (fireball.x + fireball.width < platforms[i]->x + platforms[i]->width && fireball.x + fireball.width > platforms[i]->x)) {
      if (fireball.speedY < 0) // GOING DOWN
      {
        if ((fireball.y + fireball.speedY + fireball.height > platforms[i]->y &&
             fireball.y + fireball.speedY + fireball.height < platforms[i]->y + platforms[i]->height) ||
            (fireball.y + fireball.speedY > platforms[i]->y && fireball.y + fireball.speedY < platforms[i]->y + platforms[i]->height)) {
          fireballThrown = false;
        }
      }
      else {

        if ((fireball.y + fireball.speedY + fireball.height > platforms[i]->y &&
             fireball.y + fireball.speedY + fireball.height < platforms[i]->y + platforms[i]->height) ||
            (fireball.y + fireball.speedY > platforms[i]->y && fireball.y + fireball.speedY < platforms[i]->y + platforms[i]->height)) {
          fireballThrown = false;
        }
      }
    }
  }
  if (fireballThrown) {
    fireball.y += fireball.speedY;
    fireball.x += fireball.speedX;
  }
}

void fireball2CollideWithPlat() {
  if (fireball2.x > vbe.XResolution) {
    fireball2Thrown = false;
    fireball2.x = vbe.XResolution - fireball2.width;
    fireball2.speedX = 0;
    return;
  }
  if ((fireball2.x + fireball2.width) < 0) {
    fireball2.x = fireball2.speedX;
    fireball2.speedX = 0;
    fireball2Thrown = false;
    return;
  }
  if (fireball2.y > vbe.YResolution) {
    fireball2.y = vbe.YResolution - fireball2.height;
    fireball2.speedY = 0;
    fireball2Thrown = false;
    fireball2.speedY = 0;
    fireball2.speedX = 0;
    return;
  }
  if ((fireball2.y + fireball2.height) < 0) {
    fireball2.y = 1;
    fireball2.speedY = 0;
    fireball2Thrown = false;
    fireball2.speedY = 0;
    fireball2.speedX = 0;
    return;
  }
  for (size_t i = 0; i < nPlatforms; i++) {
    if ((fireball2.x < platforms[i]->x + platforms[i]->width && fireball2.x > platforms[i]->x) ||
        (fireball2.x + fireball2.width < platforms[i]->x + platforms[i]->width && fireball2.x + fireball2.width > platforms[i]->x)) {
      if (fireball2.speedY < 0) // GOING DOWN
      {
        if ((fireball2.y + fireball2.speedY + fireball2.height > platforms[i]->y &&
             fireball2.y + fireball2.speedY + fireball2.height < platforms[i]->y + platforms[i]->height) ||
            (fireball2.y + fireball2.speedY > platforms[i]->y && fireball2.y + fireball2.speedY < platforms[i]->y + platforms[i]->height)) {
          fireball2Thrown = false;
        }
      }
      else {

        if ((fireball2.y + fireball2.speedY + fireball2.height > platforms[i]->y &&
             fireball2.y + fireball2.speedY + fireball2.height < platforms[i]->y + platforms[i]->height) ||
            (fireball2.y + fireball2.speedY > platforms[i]->y && fireball2.y + fireball2.speedY < platforms[i]->y + platforms[i]->height)) {
          fireball2Thrown = false;
        }
      }
    }
  }
  if (fireball2Thrown) {
    fireball2.y += fireball2.speedY;
    fireball2.x += fireball2.speedX;
  }
}

void drawFireball() {
  for (int row = 0; row < fireball.height; row++) {
    for (int col = 0; col < fireball.width; col++) {
      drawPixel(&(fireball.sprite[(col + row * fireball.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + fireball.y, col + fireball.x);
    }
  }
}

void drawFireball2() {
  for (int row = 0; row < fireball2.height; row++) {
    for (int col = 0; col < fireball2.width; col++) {
      drawPixel(&(fireball2.sprite[(col + row * fireball2.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + fireball2.y, col + fireball2.x);
    }
  }
}

void fireball1Player2() {
  if ((fireball.x < player2.x + player2.width && fireball.x > player2.x) ||
      (fireball.x + fireball.width < player2.x + player2.width && fireball.x + fireball.width > player2.x)) {
    if (fireball.speedY < 0) // GOING DOWN
    {
      if ((fireball.y + fireball.height > player2.y &&
           fireball.y + fireball.height < player2.y + player2.height) ||
          (fireball.y > player2.y && fireball.y < player2.y + player2.height)) {
        fireballThrown = false;
        player2.hp--;
      }
    }
    else {
      if ((fireball.y + fireball.height > player2.y &&
           fireball.y + fireball.height < player2.y + player2.height) ||
          (fireball.y > player2.y && fireball.y < player2.y + player2.height)) {
        fireballThrown = false;
        player2.hp--;
      }
    }
  }
}

void fireball2Player1() {
      if ((fireball2.x < player1.x + player1.width && fireball2.x > player1.x) ||
          (fireball2.x + fireball2.width < player1.x + player1.width && fireball2.x + fireball2.width > player1.x)) {
        if (fireball2.speedY < 0) // GOING DOWN
        {
          if ((fireball2.y + fireball2.height > player1.y &&
               fireball2.y + fireball2.height < player1.y + player1.height) ||
              (fireball2.y > player1.y && fireball2.y < player1.y + player1.height)) {
            fireball2Thrown = false;
            player1.hp--;
          }
        }
        else {
          if ((fireball2.y + fireball2.height > player1.y &&
               fireball2.y + fireball2.height < player1.y + player1.height) ||
              (fireball2.y > player1.y && fireball2.y < player1.y + player1.height)) {
            fireball2Thrown = false;
            player1.hp--;
          }
        }
      }
}
