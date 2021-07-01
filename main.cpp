
#include "hwlib.hpp"
#include "lib_ht3216C/ht3216C.hpp."
#include "lib_ht3216C/drawables.hpp"

/// \brief
/// this class is used to draw a player
/// \details
/// This class is a setup to draw a player. including the update function to move.
/// @see line
class player : public line {
    hwlib::pin_in & hoog;
    hwlib::pin_in & laag;
public:
    /// this construcor is used to set up a player.
    /// @warning hwlib::pin_in is used instead of pin_in_out.
    /// note location and end are only start coordinates.
    player(hwlib::pin_in & hoog, hwlib::pin_in & laag, hwlib::window & w, hwlib::xy location, hwlib::xy end, hwlib::xy bounce):
            line(w, location, end, bounce),
            hoog( hoog ),
            laag( laag ){}
    /// this function updates the position of the player.
    /// @note this changes the start and end location of the line.
    void update() override{
        laag.refresh();
        if(laag.read()){
            if(!(end.x < 4 && location.x <= 0)){
                location.x--;
                end.x--;
            }
        }
        hoog.refresh();
        if(hoog.read()){
            if(!(end.x > 15)){
                location.x++;
                end.x++;
            }
        }
    }
};

/// \brief
/// this class is used to start the game and draw a ball.
/// \details
/// This class is a setup to draw a ball. including the speed and interaction with other objects. also this class is used for the startscreen() and game reset.
/// @warning if a ball needs to be redrawn the last location need to be cleared.
/// @see ht3216C::clear() ht3216C::clear_pixel()
class game : public drawable{
protected:
    hwlib::xy speed;
    int p1 =0;
    int p2 = 0;
    bool point = false;
public:
    /// this constructor is used to the ball.
    /// @note location is only a start coordinate.
    game(hwlib::window & w, hwlib::xy location, hwlib::xy bounce,  hwlib::xy speed):
            drawable(w, location, hwlib::xy(1,1) ,  bounce), speed(speed){}
    /// this function is used to a ball.
    void draw(){
        w.write(location);
    }
    ///\brief
    /// identifier.
    ///\details
    /// this function is used to identify a object as ball.
    /// returns always true.
    /// @returns true;
    bool is_ball(){
        return true;
    }
    ///\brief
    /// this function updates the ball.
    ///\details
    /// this function updates the ball by adding speed to the location.
    /// It also check if the ball hits a border. the x borders (nonlethal borders) changes the speed.x.
    /// If a collision with the y borders. a player gets a point and the game is paused and ready to restart.
    void update(){
            if(location.x == 0 || location.x >= 15){
                speed.x *= -1;
            }
            else if(location.y < 0){ p2++; point = true;}
            else if(location.y >= 24){p1++; point = true;}
            location = location + speed;
    }
    ///\brief
    /// interact with objects.
    ///\details
    /// This function changes the speed if a collision happens with an object.
    /// @see overlaps()
    void interact(drawable & other) {
        if (this != &other) {
            if (overlaps(other)) {
                hwlib::xy bounce = other.get_bounce();
                speed.x *= bounce.x;
                speed.y *= bounce.y;
            }
        }
    }
    ///\brief
    /// returns a boolean.
    /// @returns true if a point has been scored or false if not.
    bool no_points(){
        return point;
    }
    ///\brief
    /// resets the game.
    /// \details
    /// location of the ball is set to the start location.
    /// point is set to false.
    /// ball speed is set to its default.
    /// a small wait is activated
    void reset_game(hwlib::xy loc){
        location = loc;
        point = false;
        speed = hwlib::xy(1,1);
        hwlib::wait_ms(500);
    }
    ///\brief
    /// returns the scores.
    /// @returns the scores in xy format.
    hwlib::xy get_scores(){
        return hwlib::xy(p1, p2);
    }
    ///\brief
    /// sets a hardcoded screen. In this game is says "pong ! \n start"
    ///@param chip is the ht3216C chip also used in window
    /// @see window chip
    void startscreen(ht3216C chip){
        uint16_t w[24] = {
                0x0, 0x0,
                0b0101111000111110,
                0b0101001000001010,
                0b0111011000001110,
                0x0,
                0b0000010000111110,
                0b0111110000100010,
                0b0000010000111110,
                0x0,
                0b0111110000111110,
                0b0001010000000010,
                0b0111110000111110,
                0x0,
                0b0111110000101110,
                0b0011010000101010,
                0b0101110000111110,
                0b0,
                0b0000010000000000,
                0b0111110001101110,
                0b0000010001101110, //uitroepteken en T
                0x0,
                0x0, 0x0
        };
        chip.change_window(w);
        chip.flush();
    }
};



int main(void){
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    namespace target = hwlib::target;
    //============================================================
    // pins initialize and set output for ledmatrix
    auto data = target::pin_in_out(target::pins::d8);
    auto write = target::pin_in_out(target::pins::d9);
    auto read = target::pin_in_out(target::pins::d10);
    auto cs = target::pin_in_out(target::pins::d11);
    auto group = grp(data, write, read, cs);
    group.direction_set_output();
    group.direction_flush();
    //============================================================
    // pins initialize for player buttons
    auto player1_hoog = target::pin_in(target::pins::d12);
    auto player1_laag = target::pin_in(target::pins::d13);
    auto player2_hoog = target::pin_in(target::pins::d7);
    auto player2_laag = target::pin_in(target::pins::d6);

    //============================================================
    // ht3216C and window initialization
    ht3216C chip(write, data, cs);
    window w(hwlib::xy(16, 24), chip);
    //============================================================
    // pong objects initialization like player and ball.
    player player1(player1_hoog, player1_laag, w,  hwlib::xy(6,0), hwlib::xy(10,0), hwlib::xy(1,-1));
    player player2(player2_hoog, player2_laag, w,  hwlib::xy(6,23), hwlib::xy(10,23), hwlib::xy(1,-1));
    hwlib::xy start_location = {12, 8};
    game bal(w, start_location, hwlib::xy(1,1), hwlib::xy(1,1));
    std::array<drawable *, 3>objects = {&bal, &player1, &player2};
    //============================================================
    // option testfunction();
    //w.test_function();
    //============================================================
    // set startscreen and wait for all players to be ready.
    // all buttons need to be pressed to start game.
    while((player1_laag.read() || player1_hoog.read() || player2_hoog.read() || player2_laag.read())){
        bal.startscreen(chip);
        hwlib::wait_ms(250);
    }
    //============================================================
    // game loop.
    for(;;) {
        //============================================================
        // start game
        for (;;) {
            //============================================================
            // clear window and redraw objects.
            w.clear();
            for (auto &p : objects) {
                p->draw();
            }
            w.flush();
            //============================================================
            // gameloop speed.
            hwlib::wait_ms(50);
            //============================================================
            // update objects.
            for (auto &p : objects) {
                p->update();
            }
            //============================================================
            // interact object with objects
            for (auto &p : objects) {
                for (auto &other : objects) {
                    p->interact(*other);
                }
            }
            //============================================================
            // check if someone made a point. if so reset game.
            if (bal.no_points()) {
                break;
            }
        }
        //============================================================
        // reset game, draw scores on terminal.
        hwlib::xy scores = bal.get_scores();
        hwlib::cout << "player1: " << scores.x << " player2: " << scores.y << "\n";
        bal.reset_game(start_location);
    }
}