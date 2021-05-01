#pragma once
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include "objects.h"
#include "enemy.h"

/** @defgroup player player
 * @{
 *
 * Functions and structs used to control player and his collisions.
 */



/**
 * @brief Represents the player
 * 
 */
struct Player {
  int x; /**< @brief player's x */ 
  int y; /**< @brief player's y */
  uint8_t *animationRight[3]; /**< @brief player's array of right animations */
  uint8_t *animationLeft[3]; /**< @brief player's array of left animations */
  uint8_t height, width; /**< @brief player's xpm height and width */
  int animeState; /**< @brief player's current animation state */
  int speedX; /**< @brief player's speed on x axis */
  int speedY; /**< @brief player's speed on y axis */
  bool jump; /**< @brief 1 if player is jumping */
  int hp; /**< @brief player's hp */ 
  bool right; /**< @brief 1 if player is facing right, else 0 */
};

/**
 * @brief Sets the bool movingRight to true and movingLeft to false, causing the player to move right
 * 
 * @param makeCode True if the scan code is a make code false if break code
 */
void moveRight(bool makeCode);
/**
 * @brief Sets the bool movingRight to false and movingLeft to true, causing the player to move left
 * 
 * @param makeCode True if the scan code is a make code false if break code
 */
void moveLeft(bool makeCode);

/**
 * @brief Sets the bool movingRight2 to false and movingLeft to true, causing the player to move left
 * 
 * @param makeCode True if the scan code is a make code false if break code
 */
void moveLeft2(bool makeCode);
/**
 * @brief Sets the bool movingRight2 to true and movingLeft2 to false, causing the player to move right
 * 
 * @param makeCode True if the scan code is a make code false if break code
 */
void moveRight2(bool makeCode);

/**
 * @brief Clears the player after it moves changing the place where it was to the background image
 * 
 */
void cleanPlayer();
/**
 * @brief Checks if the player collides on the Y coordenate. If it's falling, changes the Y speed to 0, if its jumping changes it to 8. If the player falls of the screen the player is killed
 * 
 */
void collideOnY();
/**
 * @brief Checks if the player collides in the X coordenate. If it collides with something the X will be the last X possible before collision. Also checks if the player would go off screen, not letting it go further if so were to happen
 * 
 */
void collideOnX();

/**
 * @brief Checks if the player2 collides on the Y coordenate. If it's falling, changes the Y speed to 0, if its jumping changes it to 8. If the player2 falls of the screen the player is killed
 * 
 */
void collideOnY2();
/**
 * @brief Checks if the playe2 collides in the X coordenate. If it collides with something the X will be the last X possible before collision. Also checks if the player2 would go off screen, not letting it go further if so were to happen
 * 
 */
void collideOnX2();
/**
 * @brief Checks if the player hits an enemy, reducing the player's HP
 * 
 */
void hitEnemy();
/**
 * @brief Draws the Player sprite in the correct coordenates
 * 
 */
void drawPlayer();
/**
 * @brief Checks , after the player defeats all the enemies, if it's coordenates correspond to the right corner of the screen letting him procead to the next stage
 * 
 * @return true 
 * @return false 
 */
bool atCorner();
/**
 * @brief Frees the player's XPMs before exiting the game
 * 
 */
void freePlayer();
/**
 * @brief Creates a Player object filling all the important struct variables before the game begins
 * 
 */
void createPlayer();
/**
 * @brief Draws the Player2 sprite in the correct coordenates
 * 
 */
void drawPlayer2();
/**
 * @brief Clears the player2 after it moves changing the place where it was to the background image
 * 
 */
void cleanPlayer2();
/**
 * @brief Creates a Player object filling all the important struct variables before the game begins
 * 
 */
void createPlayer2();

/**@}*/
