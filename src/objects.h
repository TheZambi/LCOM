#pragma once
#include <lcom/lcf.h>

/** @defgroup enemy enemy
 * @{
 *
 * Functions and structs used to control varius objects(like platforms, the cursor and score).
 */


/**
 * @brief Represents a platform
 * 
 */
struct Platform {
  int x; /**< @brief platform's x */
  int y; /**< @brief platform's y */
  int height; /**< @brief platform's xpm height */
  int width; /**< @brief platform's xpm width */
  uint8_t *platform; /**< @brief platform's xpm */ 
};

/**
 * @brief Represents the cursor on the screen
 * 
 */
struct Cursor {
  int x, y; /**< @brief cursor x and y */ 
  uint8_t height, width; /**< @brief cursor's xpm height and width*/ 
  uint8_t *sprite; /**< @brief cursor's xpm */ 
};

/**
 * @brief Representes the score on the screen. Also saves the xpm of all the numbers
 * 
 */
struct Score {
  uint8_t *scoreText; /**< @brief score word xpm */ 
  int playerScore; /**< @brief int that represents player score */ 
  uint8_t *numbers[10]; /**< @brief all the number's xpm */ 
};
 /**
  * @brief Frees allocated memory for platforms
  * 
  */

void freePlatforms();
/**
 * @brief Create a Cursor object
 * 
 */
void createCursor();
/**
 * @brief Create a the background
 * 
 */
void createBackground();
/**
 * @brief Create the floor
 * 
 */
void createFloor();
/**
 * @brief Create a Random Platform
 * 
 * @param x x the platform is created
 * @param y y the platform is created
 */
void createRandomPlat(int x, int y);

/**
 * @brief Draws all the platforms
 * 
 */
void drawPlatforms();

/**
 * @brief Draws the cursor on the screen
 * 
 */
void drawCursor();
/**
 * @brief Creates the arrow to be used in single player mode
 * 
 */
void createArrow();
/**
 * @brief Draws the arrow
 * 
 * @param toMult A constant to animate the arrow. 
 */
void drawArrow(int toMult, int stage);
/**
 * @brief Clears the arrow
 * 
 * @param toMult A constant to animate the arrow
 */
void clearArrow(int toMult, int stage);
/**
 * @brief Clears the score on the screen so that it leaves no trace of old numbers behind
 * 
 */
void clearScore();
/**
 * @brief loads all the xpm needed to display the score 
 * 
 */
void initScore();
/**
 * @brief Frees allocated memory by the score
 * 
 */
void freeScore();
/**
 * @brief Draws the score on the screen
 * 
 */
void drawScore();
/**
 * @brief Draws the time spent on the level
 * 
 * @param pause if true, pauses the time
 */
void drawTime(bool pause);
/**
 * @brief Clears the time on the screen so that it leaves no trace of old numbers behind
 * 
 */
void clearTime();
/**
 * @brief Draws the default single player and multi player background
 * 
 */
void drawBackground();
/**
 * @brief Draws the RTC time on main menu
 * 
 * @param seconds current seconds 
 * @param minutes current minutes
 * @param hours current hours
 */
void drawRTCTime(uint8_t seconds, uint8_t minutes, uint8_t hours);
/**
 * @brief Draws the death menu
 * 
 * @param menu Death menu xpm
 * @param stage final stage
 */
void drawDeathMenu(uint8_t *menu, int stage);
/**
 * @brief Draws the pause menu
 * 
 * @param menu Pause menu xpm
 */
void drawPauseMenu(uint8_t *menu);
/**
 * @brief clears the pause menu
 *
 */
void clearPauseMenu();

/**@}*/
