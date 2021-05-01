// IMPORTANT: you must include the following line in all your C files

#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include "../res/deathScreen.xpm"
#include "../res/mainMenu.xpm"
#include "../res/pauseScreen.xpm"
#include "../res/youLose.xpm"
#include "../res/youWin.xpm"
#include "enemy.h"
#include "fireball.h"
#include "kbd.h"
#include "mouse.h"
#include "objects.h"
#include "player.h"
#include "queue.h"
#include "rtc.h"
#include "timer.h"
#include "uart.h"
#include "vbe.h"
#include <stdbool.h>
#include <stdint.h>



#define CHOOSE_LVL rand() % 7

extern uint8_t mousePack;
extern fila *send_queue;
extern fila *receive_queue;
extern bool hadEvent;
extern struct Cursor cursor;
extern struct mouse_ev event;
extern uint8_t *background;
void *video_mem, *double_buffer;
extern int kbd_hook;
extern int nInterrupts;
extern vbe_mode_info_t vbe;
extern struct Player player1;
extern struct Player player2;
extern struct Fireball fireball2;
extern struct Platform *(platforms[10]);
extern struct Enemy *(enemies[10]);
extern int state;
extern int totalMovX;
extern int gameTime;
extern int totalMovY;
extern size_t nPlatforms;
extern size_t nEnemies;
extern struct Fireball fireball;
bool fireballThrown = false;
bool movingRight = false, movingLeft = false, lastMoveWasRight = true, spaceIsStillPressed = false;
bool newStage = true;
int completedStages = 0;
int stage = 0;
extern struct Score score;
bool singlePlayer = false, mainM = true;
bool wait = false, connect = false;
bool multiPlayer = false;
bool send=false;
bool fireball2Thrown = false;
bool sent = true;
extern bool movingLeft2;
extern bool movingRight2;
extern bool lastMoveWasRight2;

/**
 * @brief main
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

static int print_usage() {
  printf("Usage: <mode - hex>\n");

  return 1;
}

/**
 * @brief Game main loop
 * 
 */
int(proj_main_loop)(int argc, char *argv[]) {
  srand(time(NULL));

  xpm_image_t img;
  uint16_t mode;
  uint8_t mouse_bit_no = 12;
  uint32_t mouse_irq_set = BIT(mouse_bit_no);
  struct packet packet;
  uint32_t mouseByte = 0;
  uint8_t *mainMenu;
  bool exitButtonPress = false, gamePaused = false;
  uint8_t minutes = 0, seconds = 0, hours = 0;
  uint8_t *deathMenu, *pauseMenu, *youLoseMenu, *youWinMenu;

  if (argc != 1)
    return print_usage();

  // parse mode info (it has some limitations for the sake of simplicity)
  if (sscanf(argv[0], "%hx", &mode) != 1) {
    printf("%s: invalid mode (%s)\n", __func__, argv[0]);

    return print_usage();
  }

  vg_init(mode);
  unsigned char *send;
  unsigned char *speedX;
  unsigned char *speedY;
  unsigned char *scan;
  unsigned char *flag;
  bool singleDigit = true;
  int ipc_status, size;
  uint8_t bytes[2];
  bool make;
  int toMult = 1;
  message msg;
  uint8_t kbd_bit_no = 1, timer_bit_no = 0, rtc_bit_no = 8, serial_bit_no = 4;
  uint32_t kbd_irq_set = BIT(kbd_bit_no), timer_irq_set = BIT(timer_bit_no), rtc_irq_set = BIT(rtc_bit_no);
  int r;

  if (mouse_subscribe_int(&mouse_bit_no))
    return 1;
  if (timer_subscribe_int(&timer_bit_no))
    return 1;
  if (kbd_subscribe_int(&kbd_bit_no))
    return 1;
  if (rtc_subscribe_int(&rtc_bit_no))
    return 1;
  if (serial_subscribe_int(&serial_bit_no))
    return 1;

  readRegC();
  activate_rtc();

  mainMenu = xpm_load(mainMenu0, XPM_8_8_8, &img);

  deathMenu = xpm_load(deathScreen, XPM_8_8_8, &img);

  pauseMenu = xpm_load(pauseScreen, XPM_8_8_8, &img);

  youLoseMenu = xpm_load(youLose, XPM_8_8_8, &img);

  youWinMenu = xpm_load(youWin, XPM_8_8_8, &img);

  stage = CHOOSE_LVL;
  createCursor();
  createArrow();
  createPlayer();
  createBackground();
  initScore();
  createFireball();
  createFireball2();

  while (!exitButtonPress) {

    if (scanCode == ESC_SCANCODE && !multiPlayer && singlePlayer) //Pauses game
    {
      gamePaused = true;
    }

    if (scanCode == 0x20 || scanCode == 0xA0) {
      if (!gamePaused)
        moveRight(make);
    }

    if (scanCode == 0x1e || scanCode == 0x9e)
      if (!gamePaused)
        moveLeft(make);

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_irq_set) { /* subscribed interrupt */

            mouse_ih();

            if (mouseByte == 0 && !(mousePack & BIT(3))) {
              continue;
            }
            else if (mouseByte == 0) {
              packet.rb = mousePack & BIT(1);
              packet.lb = mousePack & BIT(0);
              packet.mb = mousePack & BIT(2);
              packet.x_ov = mousePack & BIT(6);
              packet.y_ov = mousePack & BIT(7);
              packet.bytes[0] = mousePack;
              mouseByte++;
            }
            else if (mouseByte == 1) {
              packet.bytes[1] = mousePack;
              mouseByte++;
            }
            else if (mouseByte == 2) {
              packet.delta_x = convert8to16(packet.bytes[1], packet.bytes[0] & BIT(4));
              packet.delta_y = convert8to16(mousePack, packet.bytes[0] & BIT(5));
              packet.bytes[2] = mousePack;
              mouseByte = 0;
              handleEvent(packet);
              if (player1.hp > 0 && !gamePaused)
                stateMachine();
            }
          }
          if (msg.m_notify.interrupts & timer_irq_set) { /* subscribed interrupt */
            timer_int_handler();                         //timer interrupt handler

            if (mainM) { //Deals with main menu buttons
              gameTime = 0;
              memcpy(double_buffer, mainMenu, vbe.XResolution * vbe.YResolution * (vbe.BitsPerPixel / 8));
              if (hadEvent && event.type == LB_PRESSED) {
                if (cursor.x >= 203 && cursor.x <= 603) {   //Every button has the same x
                  if (cursor.y >= 159 && cursor.y <= 256) { // single player
                    singlePlayer = true;
                    mainM = false;
                    multiPlayer = false;
                  }
                  if ((cursor.y >= 261 && cursor.y <= 358)) {
                    multiPlayer = true;
                    singlePlayer = false;
                  }
                  if (cursor.y >= 361 && cursor.y <= 459) { // exit
                    freePlayer();
                    freeScore();
                    free(double_buffer);
                    free(background);
                    exitButtonPress = true;
                  }
                }
              }
              drawRTCTime(seconds, minutes, hours);
            }

            if (multiPlayer && mainM) {
              receives_queue();
              if (!connect) {
                if (!wait) {
                  send = (unsigned char *)malloc(sizeof(unsigned char));
                  *send = (unsigned char)'W';
                  fila_push(send_queue, send);
                  transmit_queue();
                }
                wait = true;
              }
              if (connect && wait) {
                wait = false;
                send = (unsigned char *)malloc(sizeof(unsigned char));
                *send = (unsigned char)'C';
                fila_push(send_queue, send);
                transmit_queue();
              }
            }
            if (!gamePaused && multiPlayer && connect && !wait && (player2.hp <= 0 || player1.hp<=0) && !newStage) {
              if (player2.hp <= 0) {
                for (int row = 0; row < 270; row++) {
                  for (int col = 0; col < 510; col++) {

                    drawPixel(&(youWinMenu[(col + row * 510) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 165, col + 145);
                  }
                }
              }
              if (player1.hp <= 0) {
                for (int row = 0; row < 270; row++) {
                  for (int col = 0; col < 510; col++) {

                    drawPixel(&(youLoseMenu[(col + row * 510) * ((vbe.BitsPerPixel + 7) / 8)]), double_buffer, row + 165, col + 145);
                  }
                }
              }
              if (hadEvent && event.type == LB_PRESSED) {
                if (cursor.x >= 246 && cursor.x <= 552) {
                  if (cursor.y >= 349 && cursor.y <= 422) {
                    mainM = true;
                    multiPlayer = false;
                    wait = false;
                    connect = false;
                    fireballThrown = false;
                    fireball2Thrown = false;
                    player2.jump = false;
                    player2.right = true;
                    gamePaused = false;
                    lastMoveWasRight2 = false;
                    spaceIsStillPressed = false;
                    newStage = true;
                    player1.x = 0;
                    player1.y = 600 - 40 - player1.height;
                    player1.speedX = 0;
                    player1.speedY = 12;
                    player1.hp = 1;
                    player1.jump = false;
                    player1.right = true;
                    movingLeft = false;
                    movingRight = false;
                    movingLeft2 = false;
                    movingRight2 = false;
                    lastMoveWasRight = true;
                    stage = CHOOSE_LVL;
                    nEnemies = 0;
                    nPlatforms = 0;
                    mainM = true;
                    singlePlayer = false;
                    state = 0;
                    freePlatforms();
                  }
                }
              }
            }
            drawCursor();
            memcpy(video_mem, double_buffer, vbe.XResolution * vbe.YResolution * (vbe.BitsPerPixel / 8));

            if (multiPlayer && connect && !wait) {
              receives_queue();
              mainM = false;
              if (newStage) {
                createFloor();
                drawPlayer();
                createPlayer2();
                newStage = false;
                fireballThrown = false;
                fireball2Thrown = false;
                player1.jump = false;
                player1.right = true;
                gamePaused = false;
                movingLeft = false;
                movingRight = false;
                lastMoveWasRight = true;
                spaceIsStillPressed = false;
                
              }
              memcpy(double_buffer, background, vbe.XResolution * vbe.YResolution * (vbe.BitsPerPixel / 8));
              cleanPlayer(double_buffer);
              cleanPlayer2();
              clearMouse(double_buffer);
              clearFireball(double_buffer);
              clearFireball2(double_buffer);

              if (player1.hp > 0 && !gamePaused && player2.hp > 0) {
                collideOnX();
                collideOnY();
                collideOnX2();
                collideOnY2();

                if (movingRight && nInterrupts % 4 == 0) {
                  player1.x += player1.speedX;
                  if (nInterrupts % 2 == 0) {
                    if (player1.animeState != 2)
                      player1.animeState++;
                    else
                      player1.animeState = 0;
                  }
                }
                if (movingLeft && nInterrupts % 4 == 0) {
                  player1.x += player1.speedX;
                  if (nInterrupts % 2 == 0) {
                    if (player1.animeState != 2)
                      player1.animeState++;
                    else
                      player1.animeState = 0;
                  }
                }
                if (movingRight2 && nInterrupts % 4 == 0) {
                  player2.x += player2.speedX;
                  if (nInterrupts % 2 == 0) {
                    if (player2.animeState != 2)
                      player2.animeState++;
                    else
                      player2.animeState = 0;
                  }
                }
                if (movingLeft2 && nInterrupts % 4 == 0) {
                  player2.x += player2.speedX;
                  if (nInterrupts % 2 == 0) {
                    if (player2.animeState != 2)
                      player2.animeState++;
                    else
                      player2.animeState = 0;
                  }
                }

                if (scanCode == 0x39 && !player1.jump && !spaceIsStillPressed) {
                  player1.speedY = -23;
                  player1.jump = true;
                  spaceIsStillPressed = true;
                }
                if (scanCode == 0xB9 && spaceIsStillPressed)
                  spaceIsStillPressed = false;

                if (fireballThrown) {
                  if (!sent) {
                    send = (unsigned char *) malloc(sizeof(unsigned char));
                    *send = (unsigned char) 'f';
                    fila_push(send_queue, send);
                    speedX = (unsigned char *) malloc(sizeof(unsigned char));
                    *speedX = (unsigned char) fireball.speedX + 10;
                    fila_push(send_queue, speedX);
                    speedY = (unsigned char *) malloc(sizeof(unsigned char));
                    *speedY = (unsigned char) fireball.speedY + 10;
                    fila_push(send_queue, speedY);
                    transmit_queue();
                    sent = true;
                  }
                  fireballCollideWithPlat();
                  fireball1Player2();
                }
                if (fireballThrown) {
                  drawFireball();
                }
                if (fireball2Thrown) {
                  fireball2Player1();
                  fireball2CollideWithPlat();
                }
                if (fireball2Thrown) {
                  drawFireball2();
                }
                drawPlayer();
                drawPlayer2();
                drawPlatforms();
              }
              
                // MULTI PLAYER MAIN LOOP
            }

            if (singlePlayer) {
              if (newStage) {
                switch (stage) {
                  case 0:
                    drawBackground();
                    initEnemyArray();
                    createFloor();
                    drawScore();
                    drawTime(false);
                    drawPlayer();
                    createRandomPlat(60, 340);
                    createEnemy(50, 10, completedStages / 10 + 1);
                    createEnemy(-1, 10, completedStages / 10 + 1);
                    newStage = false;
                    fireballThrown = false;
                    player1.jump = false;
                    player1.right = true;
                    gamePaused = false;
                    movingLeft = false;
                    movingRight = false;
                    lastMoveWasRight = true;
                    spaceIsStillPressed = false;
                    break;
                  case 1:
                    drawBackground();
                    createFloor();
                    initEnemyArray();
                    drawScore();
                    drawTime(false);
                    drawPlayer();
                    createRandomPlat(550, 300);
                    createEnemy(700, 10, completedStages / 10 + 1);
                    createEnemy(-1, 10, completedStages / 10 + 1);
                    newStage = false;
                    fireballThrown = false;
                    player1.jump = false;
                    player1.right = true;
                    movingLeft = false;
                    gamePaused = false;
                    movingRight = false;
                    lastMoveWasRight = true;
                    spaceIsStillPressed = false;
                    break;
                  case 2:
                    drawBackground();
                    createFloor();
                    initEnemyArray();
                    drawScore();
                    drawTime(false);
                    player1.x = 350;
                    drawPlayer();
                    createRandomPlat(520, 350);
                    createRandomPlat(150, 350);
                    createEnemy(520, 10, completedStages / 10 + 1);
                    createEnemy(150, 10, completedStages / 10 + 1);
                    createEnemy(10, 10, completedStages / 10 + 1);
                    createEnemy(730, 10, completedStages / 10 + 1);
                    fireballThrown = false;
                    newStage = false;
                    player1.jump = false;
                    player1.right = true;
                    movingLeft = false;
                    gamePaused = false;
                    movingRight = false;
                    lastMoveWasRight = true;
                    spaceIsStillPressed = false;
                    break;
                  case 3:
                    drawBackground();
                    createFloor();
                    initEnemyArray();
                    drawScore();
                    drawTime(false);
                    drawPlayer();
                    createEnemy(325, 10, completedStages / 10 + 1);
                    createEnemy(100, 10, completedStages / 10 + 1);
                    createEnemy(570, 10, completedStages / 10 + 1);
                    createRandomPlat(325, 200);
                    createRandomPlat(100, 350);
                    createRandomPlat(570, 350);
                    fireballThrown = false;
                    newStage = false;
                    player1.jump = false;
                    player1.right = true;
                    movingLeft = false;
                    gamePaused = false;
                    movingRight = false;
                    lastMoveWasRight = true;
                    spaceIsStillPressed = false;
                    break;
                  case 4:
                    for (int i = 0; i < 10; i++)
                      platforms[i] = NULL;
                    drawBackground();
                    initEnemyArray();
                    drawScore();
                    drawTime(false);
                    drawPlayer();
                    createEnemy(100, 10, completedStages / 10 + 1);
                    createEnemy(180, 10, completedStages / 10 + 1);
                    createEnemy(410, 10, completedStages / 10 + 1);
                    createEnemy(720, 10, completedStages / 10 + 1);
                    createRandomPlat(0, 560);
                    createRandomPlat(195, 450);
                    createRandomPlat(400, 350);
                    createRandomPlat(710, 560);
                    fireballThrown = false;
                    newStage = false;
                    player1.jump = false;
                    player1.right = true;
                    movingLeft = false;
                    gamePaused = false;
                    movingRight = false;
                    lastMoveWasRight = true;
                    spaceIsStillPressed = false;
                    break;
                  case 5:
                    drawBackground();
                    createFloor();
                    initEnemyArray();
                    drawScore();
                    drawTime(false);
                    drawPlayer();
                    createEnemy(0, 10, completedStages / 10 + 1);
                    createEnemy(0, 300, completedStages / 10 + 1);
                    createEnemy(670, 10, completedStages / 10 + 1);
                    createEnemy(670, 300, completedStages / 10 + 1);
                    createRandomPlat(0, 150);
                    createRandomPlat(0, 400);
                    createRandomPlat(670, 150);
                    createRandomPlat(670, 400);
                    fireballThrown = false;
                    newStage = false;
                    player1.jump = false;
                    player1.right = true;
                    movingLeft = false;
                    gamePaused = false;
                    movingRight = false;
                    lastMoveWasRight = true;
                    spaceIsStillPressed = false;
                    break;
                  case 6:
                    drawBackground();
                    createFloor();
                    initEnemyArray();
                    drawScore();
                    drawTime(false);
                    drawPlayer();
                    createEnemy(730, 110, completedStages / 10 + 1);
                    createEnemy(730, 320, completedStages / 10 + 1);
                    createEnemy(730, 530, completedStages / 10 + 1);

                    fireballThrown = false;
                    newStage = false;
                    player1.jump = false;
                    player1.right = true;
                    movingLeft = false;
                    gamePaused = false;
                    movingRight = false;
                    lastMoveWasRight = true;
                    spaceIsStillPressed = false;
                    break;
                }
              }
              cleanPlayer(double_buffer);
              clearMouse(double_buffer);
              clearFireball(double_buffer);
              clearTime();
              clearEnemy(double_buffer);
              clearScore();

              if (nInterrupts % 60 == 0 && !enemiesAllDead() && player1.hp > 0 && !gamePaused) {
                if (score.playerScore != 0) {
                  score.playerScore -= 5;
                }
              }
              drawPlatforms();
              if (enemiesAllDead()) { // NEXT STAGE

                clearArrow(toMult, completedStages + 1);
                if (nInterrupts % 10 == 0)
                  toMult = toMult * -1;
                if (atCorner()) {
                  newStage = true;
                  player1.x = 0;
                  score.playerScore += (completedStages / 5 + 1) * 1000;
                  completedStages++;
                  stage = CHOOSE_LVL;
                  freePlatforms();
                  freeEnemies();
                  nEnemies = 0;
                  nPlatforms = 0;
                }
                else
                  drawArrow(toMult, completedStages + 1);
              }

              if (player1.hp > 0 && !gamePaused) {
                collideOnX();
                collideOnY();
                enemyCollide(5 + completedStages / 5);

                if (movingRight && nInterrupts % 4 == 0) {
                  player1.x += player1.speedX;
                  if (nInterrupts % 2 == 0) {
                    if (player1.animeState != 2)
                      player1.animeState++;
                    else
                      player1.animeState = 0;
                  }
                }
                if (movingLeft && nInterrupts % 4 == 0) {
                  player1.x += player1.speedX;
                  if (nInterrupts % 2 == 0) {
                    if (player1.animeState != 2)
                      player1.animeState++;
                    else
                      player1.animeState = 0;
                  }
                }

                for (size_t i = 0; i < nEnemies; i++) {
                  if (nInterrupts % 4 == 0 && enemies[i] != NULL && enemies[i]->platform != 20 && enemies[i]->hp != 0) {
                    if (enemies[i]->animeState != 2)
                      enemies[i]->animeState++;
                    else
                      enemies[i]->animeState = 0;
                  }
                }
                if (fireballThrown) {
                  fireballCollideWithPlat();
                }
                if (fireballThrown) {
                  checkHit();
                }
              }

              if (scanCode == 0x39 && !player1.jump && !spaceIsStillPressed) {
                player1.speedY = -23;
                player1.jump = true;
                spaceIsStillPressed = true;
              }
              if (scanCode == 0xB9 && spaceIsStillPressed)
                spaceIsStillPressed = false;

              drawPlayer();
              drawEnemies();

              if (fireballThrown) {
                drawFireball();
              }
              if (player1.hp > 0) {
                drawScore();
              }
              drawTime(gamePaused);

              hitEnemy();
              if (player1.hp <= 0) {
                drawDeathMenu(deathMenu, completedStages + 1);
                if (hadEvent && event.type == LB_PRESSED) {
                  if (cursor.x >= 243 && cursor.x <= 550) {
                    if (cursor.y >= 425 && cursor.y <= 501) {
                      newStage = true;
                      player1.x = 0;
                      player1.y = 600 - 40 - player1.height;
                      player1.speedX = 0;
                      player1.speedY = 12;
                      player1.hp = 1;
                      player1.jump = false;
                      player1.right = true;
                      movingLeft = false;
                      movingRight = false;
                      lastMoveWasRight = true;
                      spaceIsStillPressed = false;
                      fireballThrown = false;
                      score.playerScore = 0;
                      completedStages = 0;
                      stage = CHOOSE_LVL;
                      freePlatforms();
                      freeEnemies();
                      nEnemies = 0;
                      nPlatforms = 0;
                      mainM = true;
                      singlePlayer = false;
                      state = 0;
                    }
                  }
                }
              }
              if (gamePaused) {
                drawPauseMenu(pauseMenu);
                if (hadEvent && event.type == LB_PRESSED) {
                  if (cursor.x >= 285 && cursor.x <= 501) {
                    if (cursor.y >= 245 && cursor.y <= 298) {
                      scanCode = 0x24;
                      clearPauseMenu();
                      gamePaused = false;
                    }
                    if (cursor.y >= 299 && cursor.y <= 353) {
                      scanCode = 0x24;
                      gamePaused = false;
                      newStage = true;
                      player1.x = 0;
                      player1.y = 600 - 40 - player1.height;
                      player1.speedX = 0;
                      player1.speedY = 12;
                      player1.hp = 1;
                      player1.jump = false;
                      player1.right = true;
                      movingLeft = false;
                      movingRight = false;
                      lastMoveWasRight = true;
                      spaceIsStillPressed = false;
                      fireballThrown = false;
                      score.playerScore = 0;
                      completedStages = 0;
                      stage = CHOOSE_LVL;
                      freePlatforms();
                      freeEnemies();
                      nEnemies = 0;
                      nPlatforms = 0;
                      mainM = true;
                      singlePlayer = false;
                      state = 0;
                    }
                  }
                }
              }
            }

            if (hadEvent && mouseByte == 0) {
              if (cursor.x + cursor.width + packet.delta_x >= vbe.XResolution) {
                cursor.x = vbe.XResolution - cursor.width;
              }
              else if (cursor.x + packet.delta_x <= 0) {
                cursor.x = 0;
              }
              else {
                cursor.x += packet.delta_x;
              }

              if (cursor.y + cursor.height - packet.delta_y >= vbe.YResolution) {
                cursor.y = vbe.YResolution - cursor.height;
              }
              else if (cursor.y - packet.delta_y <= 0) {
                cursor.y = 0;
              }
              else {
                cursor.y -= packet.delta_y;
              }
              hadEvent = false;
            }
          }
          if (msg.m_notify.interrupts & kbd_irq_set) { /* subscribed interrupt */
            kbc_ih();                                  //Interrupt handler
            if (!singleDigit) {                        //double byte scancode
              bytes[1] = scanCode;
              singleDigit = true;
            }
            else {
              if (scanCode == 0xE0) { // checks if scan code is double byte
                size = 2;
                bytes[0] = scanCode;
                singleDigit = false;
              }
              else {
                size = 1;
                bytes[0] = scanCode;
                 if ((scanCode == 0x20 || scanCode == 0xA0 || scanCode == 0x1e || scanCode == 0x9e || scanCode == 0x39 || scanCode == 0xB9) && multiPlayer && connect && !wait)
                 {
                   flag =(unsigned char *)malloc(sizeof(unsigned char));
                   *flag = (unsigned char)'k';
                   fila_push(send_queue,flag);
                   scan = (unsigned char *)malloc(sizeof(unsigned char));
                   *scan = (unsigned char)scanCode;
                   fila_push(send_queue,scan);
                   transmit_queue();
                 }
              }
            }
            if (scanCode & BIT(7)) { //breakcode
              make = false;
            }
            else {
              make = true; //makecode
            }
          }
          if (msg.m_notify.interrupts & rtc_irq_set) {
            readRegC();
            readAll(&seconds, &minutes, &hours);
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
    
  }

  deactivate_rtc();
  if (mouse_unsubscribe_int())
    return 1;

  if (kbd_unsubscribe_int())
    return 1;

  if (timer_unsubscribe_int())
    return 1;
  if (rtc_unsubscribe_int())
    return 1;

  if (serial_unsubscribe_int())
    return 1;
  free(send);
  free(speedX);
  free(speedY);
  free(scan);
  free(flag);
  vg_exit();
  return 0;
}
