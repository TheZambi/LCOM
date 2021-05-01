#pragma once
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include "objects.h"

/** @defgroup enemy enemy
 * @{
 *
 * Functions and structs used to control fireball.
 */


/**
 * @brief Represents a fireball
 * 
 */
struct Fireball {
  int x; /**< @brief fireball's x */
  int y; /**< @brief fireball's y */
  uint8_t *sprite; /**< @brief fireball's xpm */
  int speedX; /**< @brief fireball's speed on x axis */
  int speedY; /**< @brief fireball's speed on y axis */
  double angle; /**< @brief fireball's angle */
  int baseSpeed; /**< @brief fireball's base speed */
  int height; /**< @brief fireball's xpm height */
  int width; /**< @brief fireball's xpm width */
};
/**
 * @brief Calculates the angle of the fireball dpeending on mouse movements
 * 
 */
void fireballAngle();
 /**
  * @brief  Clears the fireball after it moves so it doesnt leave trace
  * 
  * @param buff pointer to adress of video memory
  */
void clearFireball(uint8_t *buff);
/**
 * @brief checks for fireball collisions with platforms
 * 
 */
void fireballCollideWithPlat();
/**
 * @brief checks for fireball2 collisions with platforms
 * 
 */
void fireball2CollideWithPlat();
/**
 * @brief checks for fireball collisions with enemies, reduces their hp if it collides
 * 
 */
void checkHit();
/**
 * @brief draws the fireball 
 * 
 */
void drawFireball();
/**
 * @brief Create a Fireball object
 * 
 */
void createFireball();
/**
 * @brief Create a Fireball object
 * 
 */
void createFireball2();
/**
 * @brief draws the player2's fireball 
 * 
 */
void drawFireball2();
 /**
  * @brief  Clears the player2's fireball after it moves so it doesnt leave trace
  * 
  * @param buff pointer to adress of video memory
  */
void clearFireball2(uint8_t *buff);
/**
 * @brief collisions for fireball 2 with player 1 on multiplayer
 * 
 */
void fireball2Player1();
/**
 * @brief collision for fireball 1 with player 2
 * 
 */
void fireball1Player2(); 

/**@}*/
