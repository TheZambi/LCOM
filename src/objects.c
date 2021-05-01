#include "objects.h"
#include "../res/arrow.xpm"
#include "../res/background0.xpm"
#include "../res/cursor.xpm"
#include "../res/doisPontos.xpm"

#include "../res/floatingPlatform.xpm"
#include "../res/number0.xpm"
#include "../res/number1.xpm"
#include "../res/number2.xpm"
#include "../res/number3.xpm"
#include "../res/number4.xpm"
#include "../res/number5.xpm"
#include "../res/number6.xpm"
#include "../res/number7.xpm"
#include "../res/number8.xpm"
#include "../res/number9.xpm"
#include "../res/platform.xpm"
#include "../res/scoreXPM.xpm"
#include "../res/plusHP.xpm"
#include "../res/plusSpeed.xpm"
#include "enemy.h"
#include "fireball.h"
#include "math.h"
#include "player.h"
#include "vbe.h"

struct Cursor cursor;
extern void *video_mem, *double_buffer;
extern vbe_mode_info_t vbe;

uint8_t *arrow, *HP, *speed;
extern struct Fireball fireball;
uint8_t *background;
uint8_t *pontos;
size_t nPlatforms = 0;
size_t nEnemies = 0;
struct Platform *(platforms[10]);
extern struct Enemy *(enemies[10]);
struct Score score;
extern int nInterrupts;
int gameTime = 0;
extern struct Player player1;

void freePlatforms() {
  for (int i = 0; i < 10; i++) {
    if (platforms[i] != NULL) {
      struct Platform *tmp = platforms[i];
      free(tmp->platform);
      free(tmp);
    }
  }
}

void createArrow() {
  xpm_image_t img;
  arrow = xpm_load(arrow0, XPM_8_8_8, &img);
  HP = xpm_load(plusHP,XPM_8_8_8, &img);
  speed = xpm_load(plusSpeed,XPM_8_8_8, &img);
}

void drawArrow(int toMult, int stage) {
  if (stage % 5 == 0) {
    for (int row = 0; row < 60; row++) {
      for (int col = 0; col < 100; col++) {
        drawPixel(&(speed[(col + row * 100) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, 350 + row, 600 + col);
      }
    }
  }
  if (stage % 10 == 0) {
    for (int row = 0; row < 30; row++) {
      for (int col = 0; col < 60; col++) {

        drawPixel(&(HP[(col + row * 60) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, 410 + row, 600 + col);
      }
    }
  }
  for (int row = 0; row < 60; row++) {
    for (int col = 0; col < 100; col++) {

      drawPixel(&(arrow[(col + row * 100) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, 450 + row, 600 + col + 10 * toMult);
    }
  }
}

void clearArrow(int toMult, int stage) {
  uint8_t *colorToBuild32Bits;
  uint32_t colorAux=0;
  if (stage % 5 == 0) {
    for (int row = 0; row < 60; row++) {
      for (int col = 0; col < 100; col++) {
        colorAux=0;
        colorToBuild32Bits = &speed[(col + row * 100) * ((vbe.BitsPerPixel + 7) / 8)];
        for (size_t i = 0; i < 3; i++) {
          colorAux |= ((*colorToBuild32Bits) << (8 * i));
          colorToBuild32Bits++;
        }
        if (colorAux != (xpm_transparency_color(XPM_8_8_8))) {
          drawPixel(&(background[(col + 600 + (row + 350) * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, 350 + row, 600 + col);
        }
      }
    }
  }
  if (stage % 10 == 0) {
    for (int row = 0; row < 30; row++) {
      for (int col = 0; col < 60; col++) {
        colorAux=0;
        colorToBuild32Bits = &HP[(col + row * 60) * ((vbe.BitsPerPixel + 7) / 8)];
        for (size_t i = 0; i < 3; i++) {
          colorAux |= ((*colorToBuild32Bits) << (8 * i));
          colorToBuild32Bits++;
        }
        drawPixel(&(background[(col + 600 + (row+410) * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, 410 + row, 600 + col);
      }
    }
  }
  for (int row = 0; row < 60; row++) ///// BACKGROUND
  {
    for (int col = 0; col < 100; col++) {
      drawPixel(&(background[((600 + 10 * toMult + col) + (450 + row) * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, 450 + row, 600 + 10 * toMult + col);
    }
  }
}

void clearScore() {
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 80; col++) {

      drawPixel(&background[(col + row * 80) * ((vbe.BitsPerPixel + 7) / 8)], double_buffer, row, col);
    }
  }
  unsigned int digit = 0;
  unsigned int counter = 0;
  if (score.playerScore == 0) {
    for (int row = 0; row < 40; row++) {
      for (int col = 0; col < 40; col++) {

        drawPixel(&(background[(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col + 80 + (40 * counter));
      }
    }
  }
  else {
    digit = pow(10, 1.0 + floor(log10(score.playerScore)));
    while (digit /= 10) {
      for (int row = 0; row < 40; row++) {
        for (int col = 0; col < 40; col++) {

          drawPixel(&(background[(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col + 80 + (40 * counter));
        }
      }
      counter++;
    }
  }
}

void drawScore() {
  unsigned int digit = 0;
  unsigned int counter = 0;
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 80; col++) {

      drawPixel(&(score.scoreText[(col + row * 80) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col);
    }
  }
  if (score.playerScore == 0) {
    for (int row = 0; row < 40; row++) {
      for (int col = 0; col < 40; col++) {

        drawPixel(&(score.numbers[0][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col + 80 + (40 * counter));
      }
    }
  }
  else {
    digit = pow(10, 1.0 + floor(log10(score.playerScore)));
    while (digit /= 10) {
      for (int row = 0; row < 40; row++) {
        for (int col = 0; col < 40; col++) {

          drawPixel(&(score.numbers[(score.playerScore / digit) % 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col + 80 + (40 * counter));
        }
      }
      counter++;
    }
  }
}

void drawTime(bool pause) {
  int digit;
  int counter = 0;
  if (nInterrupts % 60 == 0 && !enemiesAllDead() && player1.hp > 0 && !pause)
    gameTime++;
  if (gameTime == 0) {
    for (int row = 0; row < 40; row++) {
      for (int col = 0; col < 40; col++) {

        drawPixel(&(score.numbers[0][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col + 760 - (40 * counter));
      }
    }
  }
  else {
    digit = gameTime * 10;
    while (digit /= 10) {
      for (int row = 0; row < 40; row++) {
        for (int col = 0; col < 40; col++) {

          drawPixel(&(score.numbers[digit % 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col + 760 - (40 * counter));
        }
      }
      counter++;
    }
  }
}

void clearTime() {
  int digit;
  int counter = 0;
  if (gameTime == 0) {
    for (int row = 0; row < 40; row++) {
      for (int col = 0; col < 40; col++) {

        drawPixel(&(background[(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col + 760 - (40 * counter));
      }
    }
  }
  else {
    digit = pow(10, 1.0 + floor(log10(gameTime)));
    while (digit /= 10) {
      for (int row = 0; row < 40; row++) {
        for (int col = 0; col < 40; col++) {

          drawPixel(&(background[(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col + 760 - (40 * counter));
        }
      }
      counter++;
    }
  }
}

void initScore() {
  xpm_image_t img;
  score.scoreText = malloc(sizeof(uint8_t));
  score.scoreText = xpm_load(scoreXPM, XPM_8_8_8, &img);
  for (size_t i = 0; i < 10; i++)
    score.numbers[i] = malloc(sizeof(uint8_t));
  score.numbers[0] = xpm_load(number0, XPM_8_8_8, &img);
  score.numbers[1] = xpm_load(number1, XPM_8_8_8, &img);
  score.numbers[2] = xpm_load(number2, XPM_8_8_8, &img);
  score.numbers[3] = xpm_load(number3, XPM_8_8_8, &img);
  score.numbers[4] = xpm_load(number4, XPM_8_8_8, &img);
  score.numbers[5] = xpm_load(number5, XPM_8_8_8, &img);
  score.numbers[6] = xpm_load(number6, XPM_8_8_8, &img);
  score.numbers[7] = xpm_load(number7, XPM_8_8_8, &img);
  score.numbers[8] = xpm_load(number8, XPM_8_8_8, &img);
  score.numbers[9] = xpm_load(number9, XPM_8_8_8, &img);
  score.playerScore = 0;
  pontos = xpm_load(doisPontos, XPM_8_8_8, &img);
}

void freeScore() {
  for (size_t i = 0; i < 10; i++)
    free(score.numbers[i]);
  free(score.scoreText);
}

void createCursor() {
  xpm_image_t img;
  cursor.sprite = xpm_load(cursor0, XPM_8_8_8, &img);
  cursor.x = 100;
  cursor.y = 100;
  cursor.height = img.height;
  cursor.width = img.width;
  for (int row = 0; row < cursor.height; row++) {
    for (int col = 0; col < cursor.width; col++) {

      drawPixel(&(cursor.sprite[(col + row * cursor.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, cursor.y + row, cursor.x + col);
    }
  }
}

void drawBackground() {
  for (int row = 0; row < vbe.YResolution; row++) ///// BACKGROUND
  {
    for (int col = 0; col < vbe.XResolution; col++) {

      drawPixel(&(background[(col + row * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row, col);
    }
  }
}

void createBackground() {
  xpm_image_t img;
  background = malloc(sizeof(uint8_t));
  background = xpm_load(background0, XPM_8_8_8, &img);
}

void createFloor() {
  for (int i = 0; i < 10; i++)
    platforms[i] = NULL;
  xpm_image_t img;
  struct Platform *platform1 = malloc(sizeof(struct Platform));
  platform1->platform = malloc(sizeof(uint8_t));                   //FLOOR
  platform1->platform = xpm_load(bottomPlatform, XPM_8_8_8, &img); //FLOOR
  platform1->x = 0;
  platform1->y = vbe.YResolution - img.height;
  platform1->height = img.height;
  platform1->width = img.width;
  if (platforms[nPlatforms] == NULL) {
    platforms[nPlatforms] = platform1;
  }
  nPlatforms++;

  for (int row = 0; row < platform1->height; row++) {
    for (int col = 0; col < platform1->width; col++) {

      drawPixel(&(platform1->platform[(col + row * platform1->width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + platform1->y, col + platform1->x);
    }
  }
}

void createRandomPlat(int x, int y) {
  xpm_image_t img;
  struct Platform *platform2 = malloc(sizeof(struct Platform));
  platform2->platform = malloc(sizeof(uint8_t));
  platform2->platform = xpm_load(floatingPlatform, XPM_8_8_8, &img);
  platform2->x = x;
  platform2->y = y;
  platform2->height = img.height;
  platform2->width = img.width;
  if (platforms[nPlatforms] == NULL) {
    platforms[nPlatforms] = platform2;
  }
  nPlatforms++;

  for (int row = 0; row < platform2->height; row++) {
    for (int col = 0; col < platform2->width; col++) {

      drawPixel(&(platform2->platform[(col + row * platform2->width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + platform2->y, col + platform2->x);
    }
  }
}

void drawPlatforms() {
  for (size_t i = 0; i < nPlatforms; i++) {
    if (platforms[i] != NULL) {
      for (int row = 0; row < platforms[i]->height; row++) {
        for (int col = 0; col < platforms[i]->width; col++) {
          drawPixel(&(platforms[i]->platform[(col + row * platforms[i]->width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + platforms[i]->y, col + platforms[i]->x);
        }
      }
    }
  }
}

void drawCursor() {
  for (int row = 0; row < cursor.height; row++) {
    for (int col = 0; col < cursor.width; col++) {

      drawPixel(&(cursor.sprite[(col + row * cursor.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, cursor.y + row, cursor.x + col);
    }
  }
}

void drawRTCTime(uint8_t seconds, uint8_t minutes, uint8_t hours) {
  int second = (int) seconds;
  int minute = (int) minutes;
  int hour = (int) hours;
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 40; col++) {

      drawPixel(&(score.numbers[hour / 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 500, col + 460);
    }
  }
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 40; col++) {

      drawPixel(&(score.numbers[hour % 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 500, col + 500);
    }
  }
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 10; col++) {

      drawPixel(&(pontos[(col + row * 10) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 500, col + 540);
    }
  }
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 40; col++) {

      drawPixel(&(score.numbers[minute / 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 500, col + 550);
    }
  }
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 40; col++) {

      drawPixel(&(score.numbers[minute % 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 500, col + 590);
    }
  }
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 10; col++) {

      drawPixel(&(pontos[(col + row * 10) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 500, col + 630);
    }
  }
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 40; col++) {

      drawPixel(&(score.numbers[second / 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 500, col + 640);
    }
  }
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 40; col++) {

      drawPixel(&(score.numbers[second % 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 500, col + 680);
    }
  }
}

void drawPauseMenu(uint8_t *menu) {
  for (int row = 0; row < 267; row++) {
    for (int col = 0; col < 290; col++) {

      drawPixel(&(menu[(col + row * 290) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 99, col + 248);
    }
  }
}

void clearPauseMenu() {
  for (int row = 0; row < 267; row++) {
    for (int col = 0; col < 290; col++) {

      drawPixel(&(background[((248 + col) + (row + 99) * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 99, col + 248);
    }
  }
}

void drawDeathMenu(uint8_t *menu, int stage) {
  int counter = 0, digit = 0;
  for (int row = 0; row < 470; row++) {
    for (int col = 0; col < 510; col++) {

      drawPixel(&(menu[(col + row * 510) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 34, col + 135);
    }
  }
  if (score.playerScore == 0) {
    for (int row = 0; row < 40; row++) {
      for (int col = 0; col < 40; col++) {

        drawPixel(&(score.numbers[0][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 250, col + 296 + (40 * counter));
      }
    }
  }
  else {
    digit = pow(10, 1.0 + floor(log10(score.playerScore)));
    while (digit /= 10) {
      for (int row = 0; row < 40; row++) {
        for (int col = 0; col < 40; col++) {

          drawPixel(&(score.numbers[(score.playerScore / digit) % 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 250, col + 296 + (40 * counter));
        }
      }
      counter++;
    }
  }
  counter = 0;
  digit = pow(10, 1.0 + floor(log10(stage)));
  while (digit /= 10) {
    for (int row = 0; row < 40; row++) {
      for (int col = 0; col < 40; col++) {

        drawPixel(&(score.numbers[(stage / digit) % 10][(col + row * 40) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 304, col + 390 + (40 * counter));
      }
    }
    counter++;
  }
}
