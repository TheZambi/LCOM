  #include <lcom/lcf.h>

/** @defgroup vbe vbe
 * @{
 *
 * Made up of two functions, one to facilitate the display of objects in the screen and one to initialise the video_mem.
 */


/**
 * @brief Initializes the video card mode
 * 
 * @param mode Mode in wich the video card will be initialized
 * @return void* Pointer to the memory adress which will be displayed by the video card
 */
void *(vg_init)(uint16_t mode);

/**
 * @brief Fills a pixel with the corresponding color if it is within the vbe resolution and the color is not the mode's transparency color.
 * 
 * @param color Color with which the pixel will be changed
 * @param pixelToChange Pointer to the adress of the pixel which color will be changed
 * @param row Corresponds to the pixel y coordenate
 * @param col Corresponds to the pixel x coordenate
 */
void (drawPixel)(uint8_t *color,uint8_t *pixelToChange, int row, int col);

/**@}*/
