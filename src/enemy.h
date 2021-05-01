#pragma once

#include "objects.h"
#include "vbe.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

/** @defgroup enemy enemy
 * @{
 *
 * Functions and structs used to control enemies.
 */

/**
 * @brief Enemy struct
 * 
 */
struct Enemy {
  uint8_t *animationRight[3]; /**< @brief array of animations */
  int x, y; /**< @brief enemies x and y coordinates */
  size_t platform; /**< @brief platform the enemy is in */
  int speedX, speedY; /**< @brief enemy's speed in y and x axis  */
  bool right; /**< @brief 1 if facing right, 0 if left */
  int height, width; /**< @brief xpm height and width */
  int hp; /**< @brief enemy's hp */
  int animeState; /**< @brief enemy's animation state */
};

/**
 * @brief Clears the area behind the enemy after it moves so that it leaves no trace behind
 * 
 * @param video_mem adress of the video memory
 */
void clearEnemy(uint8_t *video_mem);

/**
 * @brief Checks collisions for every enemy
 * 
 * @param speed speed of the enemies
 */
void enemyCollide(int speed);

/**
 * @brief Draws all the enemies
 * 
 */
void drawEnemies();

/**
 * @brief checks if all the enemies are dead
 * 
 * @return true if all dead
 * @return false if at least 1 is alive
 */

bool enemiesAllDead();

/**
 * @brief frees allocated memory for enemies
 * 
 */
void freeEnemies();

/**
 * @brief initializes enemey array
 * 
 */
void initEnemyArray();

/**
 * @brief Create an Enemy object on param x with hp
 * 
 * @param x x the enemy will be created at
 * @param hp the enemy's hp
 */
void createEnemy(int x, int y, int hp);

/**@}*/
