#include "enemy.h"
#include "../res/enemyRight0.xpm"
#include "../res/enemyRight1.xpm"
#include "../res/enemyRight2.xpm"

extern size_t nEnemies;
extern struct Platform *(platforms[10]);
extern size_t nPlatforms;
extern struct Enemy *(enemies[10]);
extern uint8_t *background;
extern void *double_buffer;
struct Enemy *(enemies[10]);
extern vbe_mode_info_t vbe;

void clearEnemy(uint8_t *buff) {
  for (size_t i = 0; i < nEnemies; i++) {
    for (int row = 0; row < enemies[i]->height; row++) ///// BACKGROUND
    {
      for (int col = 0; col < enemies[i]->width + 1; col++) {
        drawPixel(&(background[((enemies[i]->x + col) + (enemies[i]->y + row) * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), buff, enemies[i]->y + row, enemies[i]->x + col);
      }
    }
  }
}

void enemyCollide(int speed) {
  for (size_t i = 0; i < nEnemies; i++) {
    if (enemies[i] != NULL && enemies[i]->hp != 0) {
      bool collision = false;
      if ((int) enemies[i]->platform == 20) {
        for (size_t j = 0; j < nPlatforms; j++) {
          if (enemies[i]->y + enemies[i]->height + enemies[i]->speedY > platforms[j]->y && enemies[i]->x < platforms[j]->x + platforms[j]->width && enemies[i]->x + enemies[i]->width > platforms[j]->x && enemies[i]->y<platforms[j]->y) {
            collision = true;
            enemies[i]->speedY = 0;
            enemies[i]->speedX = speed;
            if (j != 0)
              enemies[i]->speedX = speed-3;
            enemies[i]->y = platforms[j]->y - enemies[i]->height;

            enemies[i]->platform = j;
          }
        }
        if (!collision) {
          enemies[i]->y += enemies[i]->speedY;
        }
      }

      if (enemies[i]->speedY == 0) {
        if (enemies[i]->x + enemies[i]->width + enemies[i]->speedX > platforms[enemies[i]->platform]->x + platforms[enemies[i]->platform]->width && enemies[i]->right) {
          enemies[i]->speedX *= -1;
          enemies[i]->right = false;
        }
        else if (enemies[i]->x + enemies[i]->speedX < platforms[enemies[i]->platform]->x && !enemies[i]->right) {

          enemies[i]->speedX *= -1;
          enemies[i]->right = true;
        }
        enemies[i]->x += enemies[i]->speedX;
      }
    }
  }
}

void drawEnemies() {
  for (size_t i = 0; i < nEnemies; i++) {
    if (enemies[i]->right) {
      if (enemies[i]->hp != 0 && enemies[i] != NULL) {
        for (int row = 0; row < enemies[i]->height; row++) {
          for (int col = 0; col < enemies[i]->width; col++) {

            drawPixel(&(enemies[i]->animationRight[enemies[i]->animeState][(col + row * enemies[i]->width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + enemies[i]->y, col + enemies[i]->x);
          }
        }
      }
    }
    else {
      if (enemies[i]->hp != 0 && enemies[i] != NULL) {
        for (int row = 0; row < enemies[i]->height; row++) {
          for (int col = 0; col < enemies[i]->width; col++) {

            drawPixel(&(enemies[i]->animationRight[enemies[i]->animeState][(col + row * enemies[i]->width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + enemies[i]->y, enemies[i]->width + enemies[i]->x - col);
          }
        }
      }
    }
  }
}

bool enemiesAllDead() {
  for (size_t i = 0; i < nEnemies; i++) {
    if (enemies[i] != NULL && enemies[i]->hp != 0)
      return false;
  }
  return true;
}

void freeEnemies() {
  for (int i = 0; i < 10; i++) {
    if (enemies[i] != NULL) {
      struct Enemy *tmp = enemies[i];
      free(tmp->animationRight[0]);
      free(tmp->animationRight[1]);
      free(tmp->animationRight[2]);
      free(tmp);
    }
  }
}

void initEnemyArray() {
  for (int i = 0; i < 10; i++)
    enemies[i] = NULL;
}

void createEnemy(int x, int y, int hp) {
  xpm_image_t img;
  struct Enemy *enemy1 = malloc(sizeof(struct Enemy));
  enemy1->animeState = 0;
  enemy1->speedY = 4;
  enemy1->speedX = 0;
  enemy1->x = x;
  enemy1->y = y;
  for (int i = 0; i < 3; i++) {
    enemy1->animationRight[i] = NULL;
    enemy1->animationRight[i] = malloc(sizeof(uint8_t));
  }
  enemy1->animationRight[0] = xpm_load(enemyRight0, XPM_8_8_8, &img);
  enemy1->height = img.height;
  enemy1->width = img.width;
  enemy1->right = true;
  enemy1->animationRight[1] = xpm_load(enemyRight1, XPM_8_8_8, &img);
  enemy1->animationRight[2] = xpm_load(enemyRight2, XPM_8_8_8, &img);
  enemy1->platform = 20;
  enemy1->hp = hp;
  if (enemies[nEnemies] == NULL) {
    enemies[nEnemies] = enemy1;
  }
  nEnemies++;
  if (x == -1)
    enemy1->x = rand() % (vbe.XResolution - enemy1->width - 9) + 5; //(rand()%(upper–lower+1))+lower

  for (int row = 0; row < enemy1->height; row++) {
    for (int col = 0; col < enemy1->width; col++) {

      drawPixel(&(enemy1->animationRight[0][(col + row * enemy1->width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + enemy1->y, col + enemy1->x);
    }
  }
}

