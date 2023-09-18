#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL.h>

const int SCREEN_HEIGHT = 640;
const int SCREEN_WIDTH = 1200;

const double DEGREE_TO_RAD = 0.0174532925;

SDL_Rect player;
SDL_Renderer *screen_renderer = NULL;
SDL_Window* window =  NULL;

bool game_sate = true;
bool KEYS[322];


double player_angle = 0;
double player_x_movement;
double player_y_movement;
double player_x;
double player_y;

int player_speed = 5;
int map_length = 10;
int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 1, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 1, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 1, 1,
    1, 0, 0, 0, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 1, 1, 0, 0, 0, 0,
    1, 0, 1, 0, 0, 0, 0, 1, 1, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

void get_keyboard_input( void );
void handle_input( void );
void update_screen ( void );
void set_player ( void );
bool check_laterals( int side );
bool check ( int side );


int main( int argc, const char * argv[] ) {
    
    SDL_Init( SDL_INIT_VIDEO );
    
    window = SDL_CreateWindow("raycasting",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    screen_renderer =  SDL_CreateRenderer( window, -1,
                                          SDL_RENDERER_ACCELERATED);
    SDL_Surface *icon = SDL_LoadBMP("/Users/etrv/Desktop/img.bmp");
    SDL_SetWindowIcon(window, icon);
    
    set_player();
    while( game_sate ) {
        get_keyboard_input();
        handle_input();
        update_screen();
        SDL_RenderPresent(screen_renderer);
        SDL_Delay(30);
    }
    
    SDL_DestroyRenderer(screen_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void get_keyboard_input( void ) {
        
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            game_sate = 0;
            break;
        case SDL_KEYDOWN:
            if( event.key.keysym.sym <= 322)
                KEYS[event.key.keysym.sym] = true;
            break;
        case SDL_KEYUP:
            if( event.key.keysym.sym <= 322)
                KEYS[event.key.keysym.sym] = false;
            break;
        default:
            break;
        }
    }
}

void handle_input( void ) {
    if(KEYS[SDLK_d]) {
        player_angle += 5;
        if(player_angle > 360)
            player_angle = player_angle - 360;
        player_x_movement = sin(player_angle * DEGREE_TO_RAD) * 5;
        player_y_movement = cos(player_angle * DEGREE_TO_RAD) * 5;
    }
    if(KEYS[SDLK_a]) {
        player_angle -= 5;
        if(player_angle < 0)
            player_angle = 360 + player_angle;
        player_x_movement = sin(player_angle * DEGREE_TO_RAD) * 5;
        player_y_movement = cos(player_angle * DEGREE_TO_RAD) * 5;
    }
    if(KEYS[SDLK_w]) {
        if( check(0) ) {
            player_y += player_y_movement;
            player_x += player_x_movement;
            player.x = player_x;
            player.y = player_y;
        }
    }
    
    if(KEYS[SDLK_s]) {
        if( check(1) ) {
            player_y -= player_y_movement;
            player_x -= player_x_movement;
            player.x = player_x;
            player.y = player_y;
        }
    }
}

void generate_map( void ) {
    SDL_Rect r;
    int i;
    
    SDL_SetRenderDrawColor( screen_renderer, 255, 255, 255, 255 );
    int unit_len = SCREEN_HEIGHT / map_length;
    
    r.w = unit_len - 1;
    r.h = unit_len - 1;
    r.x = 0;
    r.y = 0;
    
    for(i = 0; i < map_length * map_length; i++){
        if (map[i] != 0) {
            r.x = i % 10 * unit_len;
            r.y = i / 10 * unit_len;
            SDL_RenderFillRect( screen_renderer, &r );
        }
    }
}



void draw_ray_other( void ) {
    double ray_angle = player_angle;
    double ray_x;
    double ray_y;
    
    int r, no_of_rays = 60;
    ray_angle -= no_of_rays / 2;
    
    SDL_Rect line;
    line.w = SCREEN_WIDTH / 2 / no_of_rays;
    
    for(r = 0; r < no_of_rays * 2; r++){
        ray_x = player.x + player.w / 2;
        ray_y = player.y + player.w / 2;
        ray_angle+=0.5;
        
        while(ray_x > 0 && ray_x < SCREEN_HEIGHT
              && map[(int)ray_y / 64 * map_length + (int)ray_x / 64] == 0){
            ray_x += sin(ray_angle * DEGREE_TO_RAD);
            ray_y += cos(ray_angle * DEGREE_TO_RAD);
        }
        
        line.h = SCREEN_HEIGHT*30 / (sqrt((ray_y - player.y)*(ray_y - player.y) + (ray_x - player.x)*(ray_x - player.x)) * cos((player_angle - ray_angle ) * DEGREE_TO_RAD ));
        
        if(line.h > SCREEN_HEIGHT)
            line.h = SCREEN_HEIGHT;
        
        line.y =(SCREEN_HEIGHT - line.h) / 2;
        line.x = line.w * r; //line.x = SCREEN_HEIGHT + line.w * r;
        
        if((int)ray_x % 64 <= 1 || (int)ray_x % 64 >= 63)
            SDL_SetRenderDrawColor( screen_renderer, 80, 133, 45, 255 );
        else if((int)ray_y % 64 <= 1 || (int)ray_y % 64 >= 63)
            SDL_SetRenderDrawColor( screen_renderer, 40, 100, 15, 255 );
        
        SDL_RenderFillRect(screen_renderer, &line);
        //SDL_SetRenderDrawColor( screen_renderer, 255, 255, 255, 255 );
        //SDL_RenderDrawLine(screen_renderer, player.x + player.w / 2, player.y+ player.w / 2, ray_x, ray_y);
    }
    
}
void update_screen ( void ) {
    SDL_SetRenderDrawColor( screen_renderer, 100, 100, 100, 255 );
    SDL_RenderClear( screen_renderer );
    //generate_map();
    SDL_SetRenderDrawColor( screen_renderer, 100, 100, 200, 255 );
    SDL_Rect sky = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/2};
    SDL_RenderFillRect( screen_renderer, &sky );
    draw_ray_other();
    //SDL_SetRenderDrawColor( screen_renderer, 0, 0, 0, 255 );
    //SDL_RenderFillRect( screen_renderer, &player );
    //SDL_RenderDrawLine( screen_renderer, player.x + 5, player.y + 5, player.x + 5 + 5 * player_x_movement, player.y + 5 + 5 * player_y_movement);
}

void set_player ( void ) {
    player.x = player_x = 100;
    player.y = player_y = 100;
    player.w = 10;
    player.h = 10;
    player_x_movement = sin(player_angle * DEGREE_TO_RAD) * 5;
    player_y_movement = cos(player_angle * DEGREE_TO_RAD) * 5;
    for(int i = 0; i < 322; i++) { // init them all to false
       KEYS[i] = false;
    }
}

bool check ( int side ) {
    int unit_len = SCREEN_HEIGHT / map_length;
    int px = player.x + 5 + 5 * player_x_movement;
    int py = player.y + 5 + 5 * player_y_movement;
    if(map[(px / unit_len ) + map_length * (py / unit_len )] == 0 && side == 0)
        return true;
    else if(map[(2 * player.x - px) / unit_len  + map_length * ((2 * player.y - py) / unit_len) ] == 0 && side == 1)
        return true;
    return false;
}
