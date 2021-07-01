#include "ht3216C.hpp"

grp::grp(hwlib::pin_in_out & data, hwlib::pin_in_out & write, hwlib::pin_in_out & read, hwlib::pin_in_out & cs):
        pinnen{ &data, &write, &read, & cs}{}

void grp::direction_set_output(){
    for(const auto &p : pinnen){
        p->direction_set_output();
    }
}
void grp::write(bool v){
    for(const auto &p : pinnen){
        p->write(v);
    }
}
void grp::flush(){
    for(const auto &p : pinnen){
        p->flush();
    }
}
void grp::direction_set_input(){
    for(const auto &p : pinnen){
        p->direction_set_input();
    }
}
bool grp::read(){
    bool read = false;
    for(const auto &p : pinnen){
        read |= p->read();
    }
    return read;
}
void grp::refresh(){
    for(const auto &p : pinnen){
        p->refresh();
    }
}
void grp::direction_flush(){
    for(const auto &p : pinnen){
        p->direction_flush();
    }
}
write_to::write_to(hwlib::pin_in_out &write, hwlib::pin_in_out &data, hwlib::pin_in_out & cs):
        write ( write),
        data ( data),
        cs ( cs )
{
    cs.write(0);
}

void write_to::writeData(uint8_t number, uint16_t d){
    for (uint16_t bit = 1<<(number-1); bit; bit >>= 1) {
        write.write(0);
        data.write((d & bit) ? 1 : 0);
        write.write(1);
    }
}

write_to::~write_to(){
    cs.write(1);
}

ht3216C::ht3216C(hwlib::pin_in_out &write, hwlib::pin_in_out &data, hwlib::pin_in_out & cs):
        write_to(write, data, cs),
        write ( write),
        data ( data),
        cs ( cs ){}
void ht3216C::cmd(uint8_t cmd){
    write_to command(write, data, cs);
    command.writeData(commands::total_command_length, (((uint16_t)commands::command_id << 8) | cmd) << 1 );
}
void ht3216C::initialize(){
    cmd(); //sys_en
    cmd(commands::LED_ON); //led_on
    cmd(commands::BLINK_OFF); // blink_off
    cmd(commands::MASTERMODE); //mastermode
    cmd(commands::COMOPTION); //com_option
    clear(); //clear matrix
}
void ht3216C::shutdown_leds(){
    cmd(commands::LED_OFF);
}
void ht3216C::shutdown(){
    cmd(commands::SYS_DIS);
}
void ht3216C::clear(){
    write_to command(write, data, cs);
    command.writeData(commands::id_length, commands::write_id);
    command.writeData(commands::addr_length, 0x00);
    for(int i = 0; i < commands::com; i++){
        command.writeData(commands::row, 0x0000);
        window[i] = 0x0000;
    }
}
void ht3216C::fill(){
    write_to command(write, data, cs);
    command.writeData(3, 0x05);
    command.writeData(7, 0x00);
    for(int i = 0; i < commands::com; i++){
        command.writeData(commands::row, 0xffff);
    }
}

void ht3216C::set_brightness(uint8_t brightness){
    if(brightness < 16){
        uint8_t value = commands::SET_BRIGHTNESS + brightness;
        cmd(value);
    }
}
void ht3216C::set_pixel(hwlib::xy xy){
    if(!((xy.x < 0) || (xy.x >= commands::row) || (xy.y < 0) || (xy.y >= commands::com)) ){
        window[xy.y] |= 0x0001 << xy.x;
    }
}
void ht3216C::clear_pixel(hwlib::xy xy){
    if(!((xy.x < 0) || (xy.x >= commands::row) || (xy.y < 0) || (xy.y >= commands::com)) ){
        window[xy.y] |= 0b0 << xy.x;
    }
}
void ht3216C::flush(){
    write_to command(write, data, cs);
    command.writeData(commands::id_length, commands::write_id);
    command.writeData(commands::addr_length, 0x00);
    for(int i = 0; i < commands::com; i++){
        command.writeData(commands::row, window[i]);
        window[i] = 0x0;
    }

}
void ht3216C::change_window(uint16_t w[24]) {
    for (int i = 0; i <= 24; ++i) {
        window[i] = w[i];
    }
}

window::window(hwlib::xy borders, ht3216C & matrix):
        hwlib::window(borders),
        matrix(matrix)
{
    matrix.initialize();
    matrix.set_brightness(0xff);
}
void window::write_implementation(hwlib::xy pixel, hwlib::color col){
    if(col != hwlib::black){
        matrix.set_pixel(pixel);
    }
}
void window::flush(){
    matrix.flush();
}
void window::test_function(){
    for(uint8_t rows = 0; rows <= commands::com; rows++){
        for(uint8_t coms = 0; coms <= commands::row; coms++){
            this->write(hwlib::xy(coms, rows));
            this->flush();
            hwlib::wait_ms(50);
        }
    }

    for(uint8_t rows = 0; rows <= commands::com; rows++){
        line lijn( *this, hwlib::xy(0, rows ), hwlib::xy(16, rows));
        lijn.draw();
        this->flush();
        hwlib::wait_ms(100);
    }
    for(uint8_t coms = 0; coms <= commands::row; coms++){
        line lijn(*this, hwlib::xy(coms, 0 ), hwlib::xy(coms, 24));
        lijn.draw();
        this->flush();
        hwlib::wait_ms(100);
    }
    for(uint8_t coms = 0; coms < commands::row; coms++){
        rectangle vierkant(*this, hwlib::xy(0,0), hwlib::xy(coms, coms * 1.5 +1));
        vierkant.draw();
        this->flush();
        hwlib::wait_ms(100);
    }
    for(uint8_t coms = 0; coms < commands::row; coms++){
        rectangle vierkant(*this, hwlib::xy(0,0), hwlib::xy(coms , coms * 1.5 +1), hwlib::xy(1,1),  true);
        vierkant.draw();
        this->flush();
        hwlib::wait_ms(100);
    }
    for(uint8_t i = 0; i <16;i++){
        circle test(*this, hwlib::xy(8, 12), i);
        test.draw();
        this->flush();
        hwlib::wait_ms(100);
    }
    for(uint8_t i = 0; i <20;i++){
        circle test(*this, hwlib::xy(8, 12+i), 7);
        test.draw();
        this->flush();
        hwlib::wait_ms(100);
    }
    for(uint8_t brightness = 0; brightness <= 15; brightness++){
        matrix.set_brightness(brightness);
        matrix.fill();
        hwlib::wait_ms(200);
    }
    for(uint8_t brightness = 0; brightness <= 15; brightness++){
        matrix.set_brightness(15 - brightness);
        matrix.fill();
        hwlib::wait_ms(200);
    }
    matrix.shutdown();
}