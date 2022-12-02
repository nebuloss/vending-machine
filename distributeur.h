#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //fonction usleep
#include <stdbool.h> //type booléen

#define XO_SCREEN 54 //constante
#define YO_SCREEN 2
#define SCREEN_DIMX 35
#define SCREEN_DIMY 15
#define WINDOW_DIMX 90
#define WINDOW_DIMY 38
#define KEYS_HIGHLIGHT_TIME 150

#define DARK_GREY 235 //couleurs
#define GREY 0
#define LIGHT_GREY 8
#define RED 1
#define GREEN 155
#define YELLOW 11
#define BLUE 105
#define WHITE 254
#define BROWN 130

#define RIGHT_SIDE_COLOR LIGHT_GREY
#define SCREEN_BACKGROUND BLUE
#define KEYS_BACKGROUND DARK_GREY
#define KEYS_FOREGROUND LIGHT_GREY
#define KEYS_HIGHLIGHT_BACKGROUND RED
#define KEYS_HIGHLIGHT_FOREGROUND LIGHT_GREY
#define LEFT_SIDE_COLOR GREY
#define BASKET_BORDER WHITE
#define ARTICLE_BACKGROUND WHITE
#define ARTICLE_FOREGROUND BROWN
#define PRODUCT_CLASS_BACKGROUND BROWN
#define PRODUCT_CLASS_FOREGROUND WHITE
#define SCREEN_TITLE_COLOR GREEN
#define SCREEN_MENU_COLOR WHITE
#define SCREEN_BASKET_COLOR WHITE
#define SCREEN_INFORMATION_COLOR WHITE
#define SCREEN_INPUT_TITLE_COLOR GREEN
#define SCREEN_INPUT_TEXT_COLOR WHITE
#define SCREEN_ERROR_COLOR GREEN
#define SCREEN_HIGHLIGHT_COLOR YELLOW
#define SCREEN_WAIT_COLOR WHITE

#define SET_WINDOW_DIMENSION(x,y) printf("\e[8;%u;%ut",y,x) //macro
#define FILL_SCREEN(color) fill_surface(XO_SCREEN,YO_SCREEN,XO_SCREEN+SCREEN_DIMX,YO_SCREEN+SCREEN_DIMY,color)
#define HIDE_CURSOR() puts("\e[?25l")
#define SHOW_CURSOR() puts("\e[?25h")
#define CLEAR_SCREEN() printf("\e[2J")
#define PRINTF(x,y,bg,fg,__restrict__format,...) printf("\e[1;38;5;%d;48;5;%dm\e[%d;%dH"__restrict__format"\e[0m",fg,bg,y,x,##__VA_ARGS__)
#define PRINTF_SCREEN(x,y,bg,fg,__restrict__format,...) PRINTF(x+XO_SCREEN,y+YO_SCREEN,bg,fg,__restrict__format,##__VA_ARGS__)

typedef struct linked_list{
    void* element;
    struct linked_list* next;
}linked_list;

typedef struct article{
    float price;
    char picture[4],name[15],code[3];
}article;

typedef struct product_class{
    linked_list* article_list;
    char name[15];
}product_class;

typedef struct key{
    char code; //code ascii de la touche
    int x,y; //pour afficher la touche
}key;

typedef struct vending_machine_function vending_machine_function;

struct vending_machine_function{
    vending_machine_function (*func)(); //pointer vers une fonctions retournant le type vending_machine_function
};

product_class* create_new_product_class(char* file_name);
void fill_surface(int x1 ,int y1,int x2,int y2,int color);
void display_article(article* a,unsigned int x,unsigned int y);
void display_product_class(product_class* p,unsigned int x,unsigned int y,int dimx,unsigned int dimy);
int get_vending_machine_key(key* vending_machine_keys,int key_number); //return the position of the key in the list (-1 if it fails)
int upper(int c);
void display_key(key k,unsigned int bg,unsigned int fg);
char* screen_gets(char* str,int max_character,key* vending_machine_keys,int key_number,int x,int y);
void screen_wait_message(int time,int y);
void* linked_list_get_element(linked_list* l, unsigned int range);
int linked_list_get_lenght(linked_list* l);
int linked_list_get_range(linked_list* l, void* element);
linked_list* linked_list_remove_range(linked_list* l,int range);
linked_list* linked_list_add_element(linked_list* ll,void* element);
void free_linked_list(linked_list* l,bool free_elements);

vending_machine_function main_menu(linked_list* basket,key* vending_machine_keys,int key_number);
vending_machine_function add_element(linked_list* basket,key* vending_machine_keys,int key_number);
vending_machine_function pay(linked_list* basket,key* vending_machine_keys,int key_number);
vending_machine_function end_part(linked_list* basket,key* vending_machine_keys,int key_number);
vending_machine_function remove_article(linked_list* basket,key* vending_machine_keys,int key_number);
