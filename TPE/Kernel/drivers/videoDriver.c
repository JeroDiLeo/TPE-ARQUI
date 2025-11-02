#include "videoDriver.h"
#include "font.h"

#define BLACK 0x00000000
#define MIN_SCALE 1
#define MAX_SCALE 3
#define DEFAULT_SCALE 1
#define BGR_ORDER 1

 struct vbe_mode_info_structure {
    uint16_t attributes;
	uint8_t window_a;
	uint8_t window_b;
	uint16_t granularity;
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;
	uint16_t pitch;
	uint16_t width;
	uint16_t height;
	uint8_t w_char;
	uint8_t y_char;
	uint8_t planes;
	uint8_t bpp;
	uint8_t banks;
	uint8_t memory_model;
	uint8_t bank_size;
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer;
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;
	uint8_t reserved1[206];
}__attribute__ ((packed)); //eliminar paddling del compilador para ahorrar espacio

//fijarse dsp si sirve ponerlo como const para no modificar los campos
typedef struct vbe_mode_info_structure * vbe_mode_info_ptr;

static const volatile vbe_mode_info_ptr vbe_mode_info = (vbe_mode_info_ptr) 0x0000000000005C00;
static volatile int scale = DEFAULT_SCALE;

static void drawScaledPixel(uint32_t hexColor, uint64_t x, uint64_t y);

int setScale(uint8_t newScale){
    if(newScale >= MIN_SCALE && newScale <= MAX_SCALE){
        scale = newScale;
        return 1;
    }
    return 0;
}

uint8_t getScale(){
    return scale;
}

uint16_t getScreenWidth(){
    return vbe_mode_info->width;
}

uint16_t getScreenHeight(){
    return vbe_mode_info->height;
}

void drawPixel(uint32_t hexColor, uint64_t x, uint64_t y){

     if (x >= vbe_mode_info->width || y >= vbe_mode_info->height) {
        return;  //fuera de límites, no dibujar
    }

    uint8_t * framebuffer = (uint8_t *)(uintptr_t)vbe_mode_info->framebuffer;
    uint64_t offset = (x * ((vbe_mode_info->bpp) / 8)) + (y * vbe_mode_info->pitch);
    uint8_t bytes_per_pixel = vbe_mode_info->bpp / 8;

    uint8_t r = (hexColor >> 16) & 0xFF;
    uint8_t g = (hexColor >> 8) & 0xFF;
    uint8_t b = hexColor & 0xFF;

    if(BGR_ORDER){
        framebuffer[offset] = b;
        framebuffer[offset + 1] = g;
        framebuffer[offset + 2] = r;
    } else{
        framebuffer[offset] = r;
        framebuffer[offset + 1] = g;
        framebuffer[offset + 2] = b;
    }

    if(bytes_per_pixel == 4){
        framebuffer[offset + 3] = 0xFF; //si es 32 bpp, hacemos paddling
    }
    
}

static void drawScaledPixel(uint32_t hexColor, uint64_t x, uint64_t y){
    for(uint64_t i = 0; i < (uint64_t)scale; i++){
        for(uint64_t j = 0; j < (uint64_t)scale; j++){
            drawPixel(hexColor, x + i, y + j);
        }
    }
}

void drawChar(char c, uint32_t charColor, uint32_t bgColor, uint64_t x, uint64_t y){
    uint8_t * charBitmap = getFontChar(c);

    if(charBitmap == 0){
        return;
    }

        // Validar que el char completo entra en pantalla
    uint64_t char_width = getFontWidth() * scale;
    uint64_t char_height = getFontHeight() * scale;
    
    if (x + char_width > getScreenWidth() || y + char_height > getScreenHeight()) {
        return;  //no dibujar si no entra 
    }

    for(uint64_t i = 0; i < getFontHeight(); i++){
        for(uint64_t j = 0; j < getFontWidth(); j++){
            // Changed bit extraction to read from left to right
            if((charBitmap[i] >> j) & 1){
                drawScaledPixel(charColor, x, y);
            } else{
                drawScaledPixel(bgColor, x, y);
            }
            x += scale;
        }
        x -= getFontWidth() * scale;
        y += scale;
    }
}

int drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height){
    if((x + width) > getScreenWidth() || (y + height) > getScreenHeight()){
        return 0;
    }

    for(uint64_t i = 0; i < height; i++){
        for(uint64_t j = 0; j < width; j++){
            drawPixel(hexColor, x + j, y + i);
        }
    }
    return 1;
}

void clearScreen(){
    drawRectangle(BLACK, 0, 0, getScreenWidth(), getScreenHeight());
}
