 #include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// ADD INCLUDES HERE

// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

#define PADDLE_WIDTH (1)
#define BALL_WIDTH (1)
#define BALL_HEIGHT (1)

// Game state.
bool game_over = false; /* Set this to true wpen game is over */
bool update_screen = true; /* Set to false to prevent screen update. */
int lives;
int score;
int level;
timer_id timer;
int sec;
int min;
int time2;
int sprite_number;
double x_star;
double y_star;
double a;
double dist;
double dist_squared;
double dx;
double dy;
sprite_id star;
char * star_image =
" \\  |  / "
"  \\ | /  "
" --   -- "
"  / | \\  "
" /  |  \\ ";

char * paddle_image =
/**/	"|"
        "|"
        "|"
        "|"
        "|"
        "|"
        "|";


char * ball_image =
/**/	"O";


char * msg_image =
/**/	"Goodbye and thank-you for playing Pong      "
/**/	"        Press any key to exit...            ";

sprite_id paddle;
sprite_id ball;
sprite_id paddle_computer;
sprite_id rail;
char * collision_msg =
/**/	"  You're ball food!     "
/**/    "Press any key to exit...";


bool hud_on;

bool help_screen(){
    
    char * help_text = "CAB202 Assignment 1 - Pong"
                       "      Morgan Frearson     "
                       "          N9510761        "
                       "Controls:                 "
                       "  Arrow Keys: Move Up/Down"
                       "  h: Show This Help Screen"
                       "  q: Quit Game            "
                       "  l: Cycle Levels         "
                       "                          "
                       "  Press ANY KEY to Play...";
    
    sprite_id help;
    
    int w = screen_width();
    int h = screen_height();
    
    help = sprite_create(w/2-13, h/2-5, 26,10, help_text);
    sprite_draw(help);
    show_screen();

    getchar();
    clear_screen();
    return true;
    
}

bool game_over_screen(){
    
    char * over_game = "+-------------------------+"
                       "|                         |"
                       "|        Game Over        |"
                       "|    Play again (y/n)?    |"
                       "|                         |"
                       "+-------------------------+";
    
    sprite_id finished;
    
    int w = screen_width();
    int h = screen_height();
    
    finished = sprite_create(w/2-14, h/2-3, 27, 6, over_game);
    sprite_draw(finished);
    show_screen();
    
    int key = getchar();
    if (key == 'Y' || key == 'y'){
        clear_screen();
        return true;
    }
    else {
        return false;
    }
    
}





void wait_screen(){
    int w = screen_width();
    int h = screen_height();
    sprite_id time;
    
    char * one = "+---------+"
                 "|         |"
                 "|    1    |"
                 "|         |"
                 "+---------+";
    char * two = "+---------+"
                 "|         |"
                 "|    2    |"
                 "|         |"
                 "+---------+";
    char * three = "+---------+"
                   "|         |"
                   "|    3    |"
                   "|         |"
                   "+---------+";
    timer_pause(100);
    time = sprite_create(w/2-6, h/2-3, 11,5, three);
    sprite_draw(time);
    show_screen();
    timer_pause(500);
    time = sprite_create(w/2-6, h/2-3, 11,5, two);
    sprite_draw(time);
    show_screen();
    timer_pause(500);
    time = sprite_create(w/2-6, h/2-3, 11,5, one);
    sprite_draw(time);
    show_screen();
    timer_pause(500);
    time2 = get_current_time();
    
}

void show_hud(void){
    char * LivesMessage = "* Lives = ";
    char * LevelMessage = "* Level = ";
    char * ScoreMessage = "* Score = ";
    int w = screen_width();
    
    draw_formatted(w * 0.5/8 , 1, "%s %d", LivesMessage, lives);
    draw_formatted(w * 2.5/8, 1, "%s %d", ScoreMessage, score);
    draw_formatted(w * 4.5/8, 1, "%s %d", LevelMessage, level);
    draw_formatted(w * 6.5/8, 1, "* Time: %d:%d", min, sec);
}

void update_timer(){
    if (timer_expired(timer)){
        sec++;
        if (sec == 60){
            min++;
            sec = 0;
        }
    }
}


void respawn_ball(){
    int w = screen_width(), wb = BALL_WIDTH;
    int h = screen_height(), hb = BALL_HEIGHT;
    
    int now = get_current_time();
    srand(now);
    
    ball = sprite_create(w/2, h/2, wb, hb, ball_image);
    // Draw the ball.
    sprite_draw(ball);
    
    // Set the ball in motion.
    sprite_turn_to(ball, 0.2, 0.0); //0.22
    
    int angle = rand() % (45 + 1 - -45) + -45; //45
    sprite_turn(ball, angle);
    
}


// Setup game.
void setup(int pad_h) {
    // Useful variables.
    int w = screen_width(), wp = PADDLE_WIDTH;
    int h = screen_height(), ph = pad_h;
    
    // Draw the border (setup).
    int left = 0, right = w - 1, top = 0, bottom = h - 1;
    draw_line(left, top, right, top, '*');
    draw_line(left, bottom, right, bottom, '*');
    draw_line(left, top, left, bottom, '*');
    draw_line(right, top, right, bottom, '*');
    draw_line(left, top+2, right, top+2, '*');

    lives = 3;
    score = 0;
    level = 1;
    timer = create_timer(1000);
    sec = 0;
    min = 0;
    // Keep the next line intact.
    show_screen();

    // Set up the paddle at the centre of the screen.
    int paddle_x = (w - wp) - 3;
    int paddle_y = (h - ph) / 2;
    
    paddle = sprite_create(paddle_x, paddle_y, PADDLE_WIDTH, ph, paddle_image);
    
    // Draw the paddle.
    sprite_draw(paddle);
    
    // Keep the next line intact.
    show_hud();
    show_screen();
    wait_screen();
    respawn_ball();
    
    // Keep the next line intact.
    show_screen();
}

void setup_comuterised(int pad_h){ //Gotta pass the int from the function
    
    int h = screen_height(), ph = pad_h;
    
    // Set up the paddle at the centre of the screen.
    int paddle_x = 3;
    int paddle_y = (h - ph) / 2;
    
    paddle_computer = sprite_create(paddle_x, paddle_y, PADDLE_WIDTH, ph, paddle_image);
    
    // Draw the paddle.
    sprite_draw(paddle_computer);

    // Keep the next line intact.
    show_screen();

    
}

int paddle_height() {
    int height;
    int h = screen_height();
    
    if (h >= 21){
        height = 7;
    }
    else {
        height = (h - 2 - 1)/2;
    }
    
    
    return height;
}

void setting_up_3(int ball_x, int ball_y){
    x_star = screen_width()/2 - 5;
    y_star = screen_height()/2 - 3;
    
    dx = x_star - ball_x;
    dy = y_star - ball_y;
    dist = sqrt(( (dx * dx) + (dy * dy) ));
    dx = dx / dist;
    dy = dy / dist;
    dx = dx * 0.00001;
    dy = dy * 0.00001;
    if (x_star-ball_x < 10 && y_star < 10){
       sprite_turn_to ( ball, dx, dy);
    }
    
    star = sprite_create(x_star, y_star, 9, 5, star_image);
    sprite_draw(star);
    
}

void accelerate_ball_towards_star(int ball_x, int ball_y) {
    double x_diff = x_star - ball_x;
    double y_diff = y_star - ball_y;
    
    double dist_squared = (x_diff * x_diff) + (y_diff * y_diff);
    
    if (dist_squared < 1e-10 ) {
        dist_squared = 1e-10;
    }
    
    double dist = sqrt(dist_squared);
    
    dx = sprite_dx ( ball );
    dy = sprite_dy ( ball );
    
    a = 0.1 / dist_squared;  //MIGHT HAVE TO CHANGE
    
    dx = (dx + (a * x_diff / dist ) );
    dy = (dy + (a * y_diff / dist ) );
    
    double v = sqrt( (dx * dx) + (dy * dy) );
    
    if ( v > 0.7 ) {  //Reduced it cause 1 was too large
        dx = dx * v;
        dy = dy * v;
    }
    sprite_turn_to( ball, dx, dy);
}





// Play one turn of game.
void process(int pad_h) {
    
    int w = screen_width(), wb = BALL_WIDTH;
    int h = screen_height(), ph = pad_h, hb = BALL_HEIGHT;
    
    // Get a character code from standard input without waiting.
    int key = get_char();
    
    // Test for end of game.
    if ( key == 'q' ) {
        game_over = true;
        clear_screen();
        int msg_height = 2;
        int msg_width = strlen(msg_image) / msg_height;
        sprite_id msg_box = sprite_create((w - msg_width) / 2, (h - msg_height) / 2, msg_width, msg_height, msg_image);
        sprite_draw(msg_box);
        show_screen();
        wait_char();
        return;
    }
    
    if ( key == 'h' || key == 'H' ) {
        clear_screen();
        help_screen();
        return;
    }
    
    if ( key == 'l' || key == 'L' ) {
        if (level < 4) {
            wait_screen();
            level++;
            respawn_ball();
        }
        else {
            wait_screen();
            level = 1;
            respawn_ball();
        }
        return;
    }
    
    
   
    
    // Get the current screen coordinates of the paddle in integer variables
    // by rounding the actual coordinates.
    int yh = round(sprite_y(paddle));
    

    // Move paddle up according to specification.
    if ( KEY_UP == key && yh > 3 ) {
        sprite_move(paddle, 0, -1);
    }
    
    // Move paddle down according to specification.
    else if ( KEY_DOWN == key && yh < h - ph - 1 ) {
        sprite_move(paddle, 0, +1);
    }
    
    // Test to move the ball if key is 'z' or ERROR.
    else if ( 'z' == key || key < 0 ) {
        // ball takes one step.
        sprite_step(ball);
        
        // Get screen location of ball.
        int x = round(sprite_x(ball));
        int y = round(sprite_y(ball));
        
        // Get the displacement vector of the ball.
        double dx = sprite_dx(ball);
        double dy = sprite_dy(ball);
        
        // Test to see if the ball hit the left or right border.
        if ( x == 0 ) dx = -dx;
        
        if(x == w - wb + 2) {
            lives = lives -1;
            if (lives >= 0){
                wait_screen();
                respawn_ball();
            }
            else {
                respawn_ball();

            }
            
        }
    
        
        // Test to see if the ball hit the top or bottom border.
        if ( (y == 2) || (y == h - hb) ) dy = -dy;
        
    
        
        // Test to see if the ball needs to step back and change direction.
        if ( dx != sprite_dx(ball) || dy != sprite_dy(ball) ) {
            sprite_back(ball);
            sprite_turn_to(ball, dx, dy);
        }
    }
    
    
    if (level > 1){
        
        
        
        int y_top_computer = round(sprite_y(paddle_computer));
        int y_bottom_computer = y_top_computer + ph - 1;
        int y_centre_computer = y_top_computer + (ph - 1)/2;
        int xc = round(sprite_x(paddle_computer));
        int ball_y = round(sprite_y(ball));
        int ball_x = round(sprite_x(ball));
     
        
        if ( ball_y < y_centre_computer && y_top_computer > 3 ) {
            
            sprite_move(paddle_computer, 0, -1);
        }
        
        // Move paddle down according to specification.
        else if (ball_y > y_centre_computer && y_bottom_computer < h - 2) {
            sprite_move(paddle_computer, 0, +1);
        }
      
        
        
        //rbound
        if ((ball_x == xc) && ((y_top_computer-1 <= ball_y) && (y_bottom_computer-1 >= ball_y)) ) {
            
            double dx = sprite_dx(ball);
            double dy = sprite_dy(ball);
            
            dx = -dx;
            
            if ( dx != sprite_dx(ball) ) {
                sprite_back(ball);
                sprite_turn_to(ball, dx, dy);
            }
        }
        
        
        //Paddle Top Dynamics
        if ((ball_x == xc) && (y_top_computer == ball_y)) {
            
            double dx = sprite_dx(ball);
            double dy = sprite_dy(ball);
            
            if ((dy < 0) && (y_top_computer-2 > 1)){ ///////////
                dy = -dy;
            }
            
            else {
                
                dx = -dx;
            }
            
            if ( dx != sprite_dx(ball) || dy != sprite_dy(ball) ) {
                sprite_back(ball);
                sprite_turn_to(ball, dx, dy);
            }
        }
        
        
        
        //Paddle Bottom Dynamics
        if ((ball_x == xc) && (y_bottom_computer == ball_y)) {
            
            double dx = sprite_dx(ball);
            double dy = sprite_dy(ball);
            
            if ((dy > 0) && (h - y_bottom_computer-1 > 1)){ ///////////
                dy = -dy;
            }
            
            else {
                
                dx = -dx;
            }
            
            if ( dx != sprite_dx(ball) || dy != sprite_dy(ball) ) {
                sprite_back(ball);
                sprite_turn_to(ball, dx, dy);
            }
        }
        
        
    }
    
    // Detect collision between ball and paddle using bounding box.
    
    //		Declare a boolean variable called collided, and set the
    //		initial value to true: assume they have collided, then try
    //		to prove otherwise.
    bool collied = false;
    
    //		Use integer variables paddle_top, paddle_bottom, paddle_left,
    //		paddle_right, ball_top, ball_bottom, ball_left, and
    //		ball_right to store the current values of the respective
    //		boundaries of the paddle and the ball.
    int paddle_top = round(sprite_y(paddle));
    int paddle_bottom = paddle_top + ph - 1;
    int paddle_x = round(sprite_x(paddle));
    int ball_y = round(sprite_y(ball));
    int ball_x = round(sprite_x(ball));
    
    
    
    //		Test to see if the bottom edge of the paddle is above the top
    //		edge of the ball. If it is, then they do not overlap, set
    //		collided to false.
    if ((ball_x == paddle_x) && ((paddle_top <= ball_y) && (paddle_bottom-1 >= ball_y)) ) {
        
        double dx = sprite_dx(ball);
        double dy = sprite_dy(ball);
        
        dx = -dx;
        
        if ( dx != sprite_dx(ball) ) {
            sprite_back(ball);
            sprite_turn_to(ball, dx, dy);
        }
        collied = true;
    }
    
    
    //Paddle Top Dynamics
    if ((ball_x == paddle_x) && (paddle_top == ball_y)) {
        
        double dx = sprite_dx(ball);
        double dy = sprite_dy(ball);
        
        if ((dy < 0) && (paddle_top-2 > 1)){ ///////////
            dy = -dy;
        }
        
        else {
            
            dx = -dx;
        }
        
        if ( dx != sprite_dx(ball) || dy != sprite_dy(ball) ) {
            sprite_back(ball);
            sprite_turn_to(ball, dx, dy);
        }
        collied = true;
    }
    
    
    
    //Paddle Bottom Dynamics
    if ((ball_x == paddle_x) && (paddle_bottom == ball_y)) {
        
        double dx = sprite_dx(ball);
        double dy = sprite_dy(ball);
        
        if ((dy > 0) && (h - paddle_bottom-1 > 1)){ ///////////
            dy = -dy;
        }
        
        else {
            
            dx = -dx;
        }
        
        if ( dx != sprite_dx(ball) || dy != sprite_dy(ball) ) {
            sprite_back(ball);
            sprite_turn_to(ball, dx, dy);
        }
        collied = true;
    }
    
    
    if (collied == true) {
        score++;
    }

    
    if (lives < 0){
        clear_screen();
        if(game_over_screen() == true){
            clear_screen();
            if (help_screen() == true){
                int Pad_height = paddle_height();
                setup(Pad_height);
                
                show_screen();
                
            }
            game_over = false;

        }
        else {
            game_over = true;
        }
        
    }
    

    // Leave next line intact
    clear_screen();
    
    // Draw the border.
    int left = 0, right = w - 1, top = 0, bottom = h - 1;
    draw_line(left, top, right, top, '*');
    draw_line(left, bottom, right, bottom, '*');
    draw_line(left, top, left, bottom, '*');
    draw_line(right, top, right, bottom, '*');
    draw_line(left, top+2, right, top+2, '*');

    
    // Draw the paddle and ball.
    //Create an if loop to draw the different sprites needed
    if (level > 1){
        sprite_draw(paddle_computer);
    }
    sprite_draw(paddle);
    sprite_draw(ball);
    
    //	Create a heads-up display for debugging.
    
    //		If key is 'D', toggle the HUD: if it is on, turn it off;
    //		if it is off, turn it on.
    if ( key == 'D' ) {
        hud_on = !hud_on;
    }
    //		If the HUD is on, display state of the game.
    if ( hud_on ) {
        draw_formatted(0, 0, "   paddle top-left      : (%3d, %3d)   ", paddle_x, paddle_top);
        draw_formatted(0, 1, "   paddle bottom-right  : (%3d, %3d)   ", paddle_x, paddle_bottom);
        draw_formatted(0, 2, "   ball top-left    : (%3d, %3d)   ", ball_x, ball_y);
        draw_formatted(0, 3, "   Hit           : %d               ", sprite_number);
    }
    
    show_hud();
    
    
    
    if(level == 3){
        
        
        if (get_current_time() >= time2 + 5){
            setting_up_3(ball_x, ball_y);
            accelerate_ball_towards_star(ball_x, ball_y);
            sprite_step( ball );
            sprite_draw( ball );
            sprite_draw( star );
        }
        
        
    }
    
   
    
    
    #define Rails_Size (w*3/4 - w*1/4)
    sprite_id rails_top[Rails_Size];
    sprite_id rails_bottom[Rails_Size];
    bool delete_these[Rails_Size];
    bool delete_these_bottom[Rails_Size];
    //for (int i = 0; i < Rails_Size; i++){
      //  delete_these[i] = false;
    //}

    
    
    if(level == 4){
    
        
        for (int i = 0; i < Rails_Size; i++){
            
            rails_top[i] = sprite_create((w*1/4 + i), h*1/3, 1, 1, "=" );
            rails_bottom[i] = sprite_create((w*1/4 + i), h*2/3, 1, 1, "=" );
            
        }
        
        
        
        
        for (int i = 0; i < Rails_Size; i++){

            int rail_top_y = sprite_y(rails_top[i]);
            int rail_top_x = sprite_x(rails_top[i]);
            
            if ((ball_x == rail_top_x) && (ball_y == rail_top_y)){
                sprite_number = i;
                delete_these[i] = true;

                double dx = sprite_dx(ball);
                double dy = sprite_dy(ball);
                dy = -dy;
                
                
                if ( dy != sprite_dy(ball) ) {
                    sprite_back(ball);
                    sprite_turn_to(ball, dx, dy);
                }
            }
            
            
            
            int rail_bottom_y = sprite_y(rails_bottom[i]);
            int rail_bottom_x = sprite_x(rails_bottom[i]);
            
            if ((ball_x == rail_bottom_x) && (ball_y == rail_bottom_y)){
                double dx = sprite_dx(ball);
                double dy = sprite_dy(ball);
                delete_these_bottom[i] = true;
                
                dy = -dy;
                
                if ( dy != sprite_dy(ball) ) {
                    sprite_back(ball);
                    sprite_turn_to(ball, dx, dy);
                }
            }
        }
        
    } //End of Level 4
    
    
    
    
    if(level == 4){

        
        
            
        for (int i = 0; i < Rails_Size; i++){
            
            
                if(delete_these[i] == true){
                    sprite_hide(rails_top[i]);
                }
                else {
                    delete_these[i] = false;
                }
            
            
                if(delete_these_bottom[i] == true){
                    sprite_hide(rails_bottom[i]);
                }
                else {
                    delete_these_bottom[i] = false;
                }
            
        }
        
        for (int i = 0; i < Rails_Size; i++){
            
            sprite_draw(rails_top[i]);
            sprite_draw(rails_bottom[i]);
        }
        
    }
    
    show_screen();
    
    
}



int main(void) {
    
    setup_screen();
    
    int Pad_height = paddle_height();
    
    help_screen();

    setup(Pad_height);
    
    setup_comuterised(Pad_height);
    
    show_screen();
    
    
    while ( !game_over ) {
        
        update_timer();
        process(Pad_height);
        
        if ( update_screen ) {
            show_screen();
        }
        
        timer_pause(DELAY);
    }
    
    cleanup_screen();
    
    return 0;
}
