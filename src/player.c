#include "player.h"
#include "vbe.h"
#include "../res/wizardIdle.xpm"
#include "../res/wizardWalkingAnim.xpm"
#include "../res/wizardWalkingAnim2.xpm"
#include "../res/wizardWalkingAnimLeft.xpm"
#include "../res/wizardWalkingAnimLeft2.xpm"
#include "../res/wizardWalkingAnimLeft3.xpm"
#include "../res/p2Idle.xpm"
#include "../res/p2WalkingAnim.xpm"
#include "../res/p2WalkingAnim2.xpm"
#include "../res/p2WalkingAnimLeft.xpm"
#include "../res/p2WalkingAnimLeft2.xpm"
#include "../res/p2WalkingAnimLeft3.xpm"

extern size_t nEnemies;
extern struct Enemy *enemies[10];
extern uint8_t *background;
extern void *double_buffer;
extern bool movingRight;
extern bool movingLeft;
extern int nInterrupts;
extern vbe_mode_info_t vbe;
extern struct Platform *(platforms[10]);
extern bool lastMoveWasRight;
extern size_t nPlatforms;
extern bool shotLeft, shotRight;
struct Player player1;
struct Player player2;

void createPlayer2() {
  xpm_image_t img;
  for (int i = 0; i < 3; i++) {
    player2.animationRight[i] = NULL;
    player2.animationRight[i] = malloc(sizeof(uint8_t));
    player2.animationLeft[i] = NULL;
    player2.animationLeft[i] = malloc(sizeof(uint8_t));
  }
  player2.animationRight[0] = xpm_load(p2Idle, XPM_8_8_8, &img);
  player2.animeState = 0;
  player2.hp = 1;
  player2.x = 799-img.width;
  player2.y = 460;
  player2.height = img.height;
  player2.width = img.width;
  player2.jump = false;
  player2.right = false;
  player2.hp = 1;
  player2.animationRight[1] = xpm_load(p2WalkingAnim, XPM_8_8_8, &img);
  player2.animationRight[2] = xpm_load(p2WalkingAnim2, XPM_8_8_8, &img);
  player2.animationLeft[0] = xpm_load(p2WalkingAnimLeft, XPM_8_8_8, &img);
  player2.animationLeft[1] = xpm_load(p2WalkingAnimLeft2, XPM_8_8_8, &img);
  player2.animationLeft[2] = xpm_load(p2WalkingAnimLeft3, XPM_8_8_8, &img);
  player2.speedY = 12;
}

void createPlayer() {
  xpm_image_t img;
  for (int i = 0; i < 3; i++) {
    player1.animationRight[i] = NULL;
    player1.animationRight[i] = malloc(sizeof(uint8_t));
    player1.animationLeft[i] = NULL;
    player1.animationLeft[i] = malloc(sizeof(uint8_t));
  }
  player1.animationRight[0] = xpm_load(wizardIdle, XPM_8_8_8, &img);
  player1.animeState = 0;
  player1.hp = 1;
  player1.x = 0;
  player1.y = 460;
  player1.height = img.height;
  player1.width = img.width;
  player1.jump = false;
  player1.right = true;
  player1.hp = 1;
  player1.animationRight[1] = xpm_load(wizardWalkingAnim, XPM_8_8_8, &img);
  player1.animationRight[2] = xpm_load(wizardWalkingAnim2, XPM_8_8_8, &img);
  player1.animationLeft[0] = xpm_load(wizardWalkingAnimLeft, XPM_8_8_8, &img);
  player1.animationLeft[1] = xpm_load(wizardWalkingAnimLeft2, XPM_8_8_8, &img);
  player1.animationLeft[2] = xpm_load(wizardWalkingAnimLeft3, XPM_8_8_8, &img);
  player1.speedY = 12;
}

void moveRight(bool makeCode) {
  if (makeCode && player1.hp > 0) {
    player1.speedX = 3;
    movingRight = true;
    movingLeft = false;
    lastMoveWasRight = true;
  }
  else {
    if (movingRight && player1.hp > 0) {
      player1.speedX = 0;
      movingRight = false;
    }
  }
}

void moveLeft(bool makeCode) {
  if (makeCode && player1.hp > 0) {
    player1.speedX = (-3);
    movingLeft = true;
    movingRight = false;
    lastMoveWasRight = false;
  }
  else if (movingLeft && player1.hp > 0) {
    player1.speedX = 0;
    movingLeft = false;
  }
  
}


bool movingRight2 = false;
bool movingLeft2 = false;
bool lastMoveWasRight2 = false;
bool spaceIsStillPressed2 = false;


void moveLeft2(bool makeCode){
  if (!makeCode && player2.hp > 0) {
    player2.speedX = 3;
    movingLeft2 = false;
    movingRight2 = true;
    lastMoveWasRight2 = true;
  }
  else if (movingRight2 && player2.hp > 0) {
    player2.speedX = 0;
    movingRight2 = false;
  }
}

void moveRight2(bool makeCode){
   if (!makeCode && player2.hp > 0) {
    player2.speedX = -3;
    movingRight2 = false;
    movingLeft2 = true;
    lastMoveWasRight2 = false;
  }
  else if (movingLeft2 && player2.hp > 0) {
      player2.speedX = 0;
      movingLeft2 = false;
    }
}

void hitEnemy() {
  for (size_t i = 0; i < nEnemies; i++) {
    if (enemies[i]->hp != 0) {
      if ((player1.x + 20 >= enemies[i]->x && player1.x + 20 <= enemies[i]->x + enemies[i]->width) || (player1.x - 20 + player1.width >= enemies[i]->x && player1.x - 20 + player1.width <= enemies[i]->x + enemies[i]->width)) {
        if ((player1.y + 10 >= enemies[i]->y && player1.y + 10 <= enemies[i]->y + enemies[i]->height) || (player1.y + player1.height - 10 >= enemies[i]->y && player1.y - 10 + player1.height <= enemies[i]->y + enemies[i]->height))
          player1.hp--;
      }
    }
  }
}

void cleanPlayer(uint8_t *video_mem) {

  for (int row = 0; row < player1.height; row++) ///// BACKGROUND
  {
    for (int col = 0; col < player1.width; col++) {
      drawPixel(&(background[((player1.x + col) + (player1.y + row) * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), video_mem, player1.y + row, player1.x + col);
    }
  }
}

void cleanPlayer2(uint8_t *video_mem) {

  for (int row = 0; row < player2.height; row++) ///// BACKGROUND
  {
    for (int col = 0; col < player2.width; col++) {
      drawPixel(&(background[((player2.x + col) + (player2.y + row) * vbe.XResolution) * ((vbe.BitsPerPixel + 7) / 8)]), video_mem, player2.y + row, player2.x + col);
    }
  }
}

void collideOnY() {
  bool collideWithFeet = false;
  bool collideWithHead = false;
  static int currentPlat = 0;
  int nextY;
  if (player1.y + player1.speedY < 0) {
    player1.speedY = 8;
    player1.y = 0;
    return;
  }
  if(player1.y+player1.speedY > 780)
  {
    player1.hp=0;
    return;
  }
  for (size_t i = 0; i < nPlatforms; i++) {
    if (platforms[i] != NULL) {
      if ((platforms[i]->x < player1.x && platforms[i]->x + platforms[i]->width > player1.x) || (platforms[i]->x < player1.x + player1.width && platforms[i]->x + platforms[i]->width > player1.x + player1.width) || (platforms[i]->x + platforms[i]->width > player1.x && platforms[i]->x + platforms[i]->width < player1.x + player1.width) || (platforms[i]->x > player1.x + player1.width && platforms[i]->x + platforms[i]->width < player1.x + player1.width)) {
        if (player1.speedY > 0) { //FALLING
          if (player1.y + player1.height + player1.speedY > platforms[i]->y && player1.x + player1.width > platforms[i]->x && player1.x < platforms[i]->x + platforms[i]->width && player1.y < platforms[i]->y) {
            collideWithFeet = true;
            nextY = platforms[i]->y - player1.height;
            currentPlat = i;
          }
        }
        else if (player1.speedY < 0) { //GOING UP
          if (player1.y + player1.speedY < platforms[i]->y + platforms[i]->height && player1.x + player1.width > platforms[i]->x && player1.x < platforms[i]->x + platforms[i]->width && player1.y > platforms[i]->y) {
            collideWithHead = true;
            nextY = platforms[i]->y + platforms[i]->height;
          }
        }
      }
    }
  }
  if (collideWithFeet) {
    player1.y = nextY;
    player1.speedY = 0;
    player1.jump = false;
  }
  else if (collideWithHead) {
    player1.y = nextY;
    player1.speedY = 12;
  }
  else {
    if (player1.speedY < 12 && player1.speedY != 0 && nInterrupts % 2 == 0) {
      player1.jump = true;
      player1.speedY += 2;
    }
    player1.y += player1.speedY;
  }
  if (player1.speedY == 0) {
    if (player1.x > platforms[currentPlat]->x + platforms[currentPlat]->width || player1.x + player1.width < platforms[currentPlat]->x) {
      player1.speedY += 2;
      player1.y += player1.speedY;
      player1.jump = true;
    }
  }
}

void collideOnY2() {
  bool collideWithFeet = false;
  bool collideWithHead = false;
  static int currentPlat = 0;
  int nextY;
  if (player2.y + player2.speedY < 0) {
    player2.speedY = 8;
    player2.y = 0;
    return;
  }
  if(player2.y+player2.speedY > 780)
  {
    player2.hp=0;
    return;
  }
  for (size_t i = 0; i < nPlatforms; i++) {
    if (platforms[i] != NULL) {
      if ((platforms[i]->x < player2.x && platforms[i]->x + platforms[i]->width > player2.x) || (platforms[i]->x < player2.x + player2.width && platforms[i]->x + platforms[i]->width > player2.x + player2.width) || (platforms[i]->x + platforms[i]->width > player2.x && platforms[i]->x + platforms[i]->width < player2.x + player2.width) || (platforms[i]->x > player2.x + player2.width && platforms[i]->x + platforms[i]->width < player2.x + player2.width)) {
        if (player2.speedY > 0) { //FALLING
          if (player2.y + player2.height + player2.speedY > platforms[i]->y && player2.x + player2.width > platforms[i]->x && player2.x < platforms[i]->x + platforms[i]->width && player2.y < platforms[i]->y) {
            collideWithFeet = true;
            nextY = platforms[i]->y - player2.height;
            currentPlat = i;
          }
        }
        else if (player2.speedY < 0) { //GOING UP
          if (player2.y + player2.speedY < platforms[i]->y + platforms[i]->height && player2.x + player2.width > platforms[i]->x && player2.x < platforms[i]->x + platforms[i]->width && player2.y > platforms[i]->y) {
            collideWithHead = true;
            nextY = platforms[i]->y + platforms[i]->height;
          }
        }
      }
    }
  }
  if (collideWithFeet) {
    player2.y = nextY;
    player2.speedY = 0;
    player2.jump = false;
  }
  else if (collideWithHead) {
    player2.y = nextY;
    player2.speedY = 12;
  }
  else {
    if (player2.speedY < 12 && player2.speedY != 0 && nInterrupts % 2 == 0) {
      player2.jump = true;
      player2.speedY += 2;
    }
    player2.y += player2.speedY;
  }
  if (player2.speedY == 0) {
    if (player2.x > platforms[currentPlat]->x + platforms[currentPlat]->width || player2.x + player2.width < platforms[currentPlat]->x) {
      player2.speedY += 2;
      player2.y += player2.speedY;
      player2.jump = true;
    }
  }
}

void collideOnX() {
  if (player1.x + player1.speedX + player1.width > vbe.XResolution - 5) {
    player1.speedX = 0;
    player1.x = 794 - player1.width;
    return;
  }
  if ((player1.x + player1.speedX) < 5) {
    player1.speedX = 0;
    player1.x = 5;
    return;
  }
  bool collideLeft = false;
  bool collideRight = false;
  int nextX;
  for (size_t i = 0; i < nPlatforms; i++) {
    if ((platforms[i]->y > player1.y && platforms[i]->y < player1.y + player1.height) || (platforms[i]->y + platforms[i]->height > player1.y && platforms[i]->y + platforms[i]->height < player1.y + player1.height) || (platforms[i]->y > player1.y && platforms[i]->y < player1.y + player1.height) || (platforms[i]->y + platforms[i]->height > player1.y && platforms[i]->y + platforms[i]->height < player1.y + player1.height)) {
      if (player1.speedX > 0) { //WALKING RIGHT
        if (player1.x + player1.width + player1.speedX > platforms[i]->x && player1.x < platforms[i]->x + platforms[i]->width) {
          collideRight = true;
          nextX = platforms[i]->x - player1.width - 2;
        }
      }
      else if (player1.speedX < 0) { //WALKING LEFT
        if (player1.x + player1.speedX < platforms[i]->x + platforms[i]->width && player1.x > platforms[i]->x) {
          collideLeft = true;
          nextX = platforms[i]->x + platforms[i]->width + 1;
        }
      }
    }
  }
  if (collideRight || collideLeft) {
    player1.x = nextX;
  }
  else {
    player1.x += player1.speedX;
  }
}

void collideOnX2() {
  if (player2.x + player2.speedX + player2.width > vbe.XResolution - 5) {
    player2.speedX = 0;
    player2.x = 794 - player2.width;
    return;
  }
  if ((player2.x + player2.speedX) < 5) {
    player2.speedX = 0;
    player2.x = 5;
    return;
  }
  bool collideLeft = false;
  bool collideRight = false;
  int nextX;
  for (size_t i = 0; i < nPlatforms; i++) {
    if ((platforms[i]->y > player2.y && platforms[i]->y < player2.y + player2.height) || (platforms[i]->y + platforms[i]->height > player2.y && platforms[i]->y + platforms[i]->height < player2.y + player2.height) || (platforms[i]->y > player2.y && platforms[i]->y < player2.y + player2.height) || (platforms[i]->y + platforms[i]->height > player2.y && platforms[i]->y + platforms[i]->height < player2.y + player2.height)) {
      if (player2.speedX > 0) { //WALKING RIGHT
        if (player2.x + player2.width + player2.speedX > platforms[i]->x && player2.x < platforms[i]->x + platforms[i]->width) {
          collideRight = true;
          nextX = platforms[i]->x - player2.width - 2;
        }
      }
      else if (player2.speedX < 0) { //WALKING LEFT
        if (player2.x + player2.speedX < platforms[i]->x + platforms[i]->width && player2.x > platforms[i]->x) {
          collideLeft = true;
          nextX = platforms[i]->x + platforms[i]->width + 1;
        }
      }
    }
  }
  if (collideRight || collideLeft) {
    player2.x = nextX;
  }
  else {
    player2.x += player2.speedX;
  }
}

void drawPlayer() {
  static int counter = 0;
  if (shotRight) {
    for (int row = 0; row < player1.height; row++) ///// PLAYER1
    {
      for (int col = 0; col < player1.width; col++) {

        drawPixel(&(player1.animationRight[0][(col + row * player1.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player1.y + row, player1.x + col);
      }
    }
    counter++;
    if (counter == 20) {
      shotRight = false;
      counter = 0;
    }
  }
  else if (shotLeft) {
    for (int row = 0; row < player1.height; row++) ///// PLAYER1
    {
      for (int col = 0; col < player1.width; col++) {

        drawPixel(&(player1.animationLeft[0][(col + row * player1.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player1.y + row, player1.x + col);
      }
    }
    counter++;
    if (counter == 20) {
      shotLeft = false;
      counter = 0;
    }
  }
  else if (movingRight) {
    for (int row = 0; row < player1.height; row++) ///// PLAYER1
    {
      for (int col = 0; col < player1.width; col++) {

        drawPixel(&(player1.animationRight[player1.animeState][(col + row * player1.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player1.y + row, player1.x + col);
      }
    }
  }
  else if (movingLeft) {
    for (int row = 0; row < player1.height; row++) ///// PLAYER1
    {
      for (int col = 0; col < player1.width; col++) {

        drawPixel(&(player1.animationLeft[player1.animeState][(col + row * player1.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player1.y + row, player1.x + col);
      }
    }
  }
  else if (lastMoveWasRight) {
    for (int row = 0; row < player1.height; row++) ///// PLAYER1
    {
      for (int col = 0; col < player1.width; col++) {

        drawPixel(&(player1.animationRight[0][(col + row * player1.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player1.y + row, player1.x + col);
      }
    }
  }
  else {
    for (int row = 0; row < player1.height; row++) ///// PLAYER1
    {
      for (int col = 0; col < player1.width; col++) {

        drawPixel(&(player1.animationLeft[0][(col + row * player1.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player1.y + row, player1.x + col);
      }
    }
  }
}

extern bool shotRight2;
extern bool shotLeft2;

void drawPlayer2() {
  static int counter = 0;
  if (shotRight2) {
    for (int row = 0; row < player2.height; row++) ///// player2
    {
      for (int col = 0; col < player2.width; col++) {

        drawPixel(&(player2.animationRight[0][(col + row * player2.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player2.y + row, player2.x + col);
      }
    }
    counter++;
    if (counter == 20) {
      shotRight = false;
      counter = 0;
    }
  }
  else if (shotLeft2) {
    for (int row = 0; row < player2.height; row++) ///// player2
    {
      for (int col = 0; col < player2.width; col++) {

        drawPixel(&(player2.animationLeft[0][(col + row * player2.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player2.y + row, player2.x + col);
      }
    }
    counter++;
    if (counter == 20) {
      shotLeft2 = false;
      counter = 0;
    }
  }
  else if (movingRight2) {
    for (int row = 0; row < player2.height; row++) ///// player2
    {
      for (int col = 0; col < player2.width; col++) {

        drawPixel(&(player2.animationRight[player2.animeState][(col + row * player2.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player2.y + row, player2.x + col);
      }
    }
  }
  else if (movingLeft2) {
    for (int row = 0; row < player2.height; row++) ///// player2
    {
      for (int col = 0; col < player2.width; col++) {

        drawPixel(&(player2.animationLeft[player2.animeState][(col + row * player2.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player2.y + row, player2.x + col);
      }
    }
  }
  else if (lastMoveWasRight2) {
    for (int row = 0; row < player2.height; row++) ///// player2
    {
      for (int col = 0; col < player2.width; col++) {

        drawPixel(&(player2.animationRight[0][(col + row * player2.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player2.y + row, player2.x + col);
      }
    }
  }
  else {
    for (int row = 0; row < player2.height; row++) ///// player2
    {
      for (int col = 0; col < player2.width; col++) {

        drawPixel(&(player2.animationLeft[0][(col + row * player2.width) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, player2.y + row, player2.x + col);
      }
    }
  }
}

bool atCorner(){
  if(player1.x == vbe.XResolution - 6 - player1.width && player1.y == platforms[0]->y-player1.height)
    return true;
  return false;
}

void freePlayer() {
  free(player1.animationRight[0]);
  free(player1.animationRight[1]);
  free(player1.animationRight[2]);
  free(player1.animationLeft[0]);
  free(player1.animationLeft[1]);
  free(player1.animationLeft[2]);
}
