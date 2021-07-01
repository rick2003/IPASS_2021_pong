#ifndef V1OOPC_EXAMPLES_HT3216C_H
#define V1OOPC_EXAMPLES_HT3216C_H
#include "hwlib.hpp"
#include "drawables.hpp"

/// \brief
/// struct with commands
/// \details
/// this struct contains all the binairy information you need to enable the system or write something to the ht3216C
/// all the values are defined as static const variables.
struct commands{
    static const int id_length = 3;
    static const int addr_length = 7;
    static const int com = 24;
    static const int row = 16;
    static const int total_command_length = 12;

    static const uint8_t write_id = 0x05;
    static const uint8_t command_id = 0x04;

    static const uint8_t SYS_EN = 0x01;
    static const uint8_t SYS_DIS = 0x00;
    static const uint8_t SET_BRIGHTNESS = 0xa0;
    static const uint8_t LED_ON = 0x03;
    static const uint8_t LED_OFF = 0x02;
    static const uint8_t BLINK_ON = 0x09;
    static const uint8_t BLINK_OFF = 0x08;
    static const uint8_t MASTERMODE = 0x18;
    static const uint8_t MASTERMODE_EXT_CLOCK = 0X1c;
    static const uint8_t SLAVEMODE = 0x10;
    static const uint8_t COMOPTION = 0x24;
};

/// \brief
/// This class is a collection of all the used pins
/// \details
/// This class can be used to easely set all the pins to a specified mode.
/// With this class you can write the same value to all the pins or set them all to output.
class grp : public hwlib::pin_in_out{
protected:
    std::array< hwlib::pin_in_out *, 4> pinnen;
public:
    /// \brief
    /// this constructor adds the given pins to the array
    grp(hwlib::pin_in_out & data, hwlib::pin_in_out & write  = hwlib::pin_in_out_dummy, hwlib::pin_in_out & read  = hwlib::pin_in_out_dummy, hwlib::pin_in_out & cs  = hwlib::pin_in_out_dummy);
    /// \brief
    /// this function set all the pins to output.
    void direction_set_output() override;
    /// \brief
    /// this function writes the given value to all the pins.
    /// @warning before writing, the direction needs to be output, and flushed.
    /// @see direction_set_output() direction_flush()
    void write(bool v) override;
    /// \brief
    /// this function flushes the pins
    /// @warning the pins have to be written first
    /// @see write()
    void flush() override;
    /// \brief
    /// this function set all the pins to input.
    void direction_set_input() override;
    /// \brief
    /// this function reads all the pins
    /// @warning the pins have to be set to input and flushed. Also the pin needs to be refreshed
    /// @see direction_set_input() direction_flush() refresh()
    /// @returns if one pin is high.
    bool read() override;
    /// \brief
    /// this function refreshes all the pins.
    /// @warning the pins have to be set to input and flushed.
    /// @see direction_set_input(), direction_flush()
    void refresh() override;
    /// \brief
    /// this function flushes the given direction.
    ///@attention the directions have to be set first.
    ///@see direction_set_output() direction_set_input()
    void direction_flush() override;
};
/// \brief
/// This class is a temporary class only used to write a sequence of bits to the ht3216C
/// \details
/// When this class is called, it instantly set the Chip Select pin **low**.
/// after the class is called, the data can be written by using the writeData() function.
/// When this class is done with writing the destructor sets the Chip Select pin **high**.
/// @attention The destructor needs to be called or automatically activate by leaving a scope!
class write_to{
protected:
    hwlib::pin_in_out &write;
    hwlib::pin_in_out &data;
    hwlib::pin_in_out &cs;
public:
    /// \brief
    /// This is the constructor for this class
    /// \details
    /// This constructor is used to set up this class
    /// It also starts the writing sequence by lowering the Chip Select pin
    write_to(hwlib::pin_in_out &write, hwlib::pin_in_out &data, hwlib::pin_in_out & cs);

    /// \brief
    /// This function is used to write the actual data to the ht3216C
    /// \details
    /// This function runs bit by bit through the data
    /// The data pin is set high when the current bit is a 1 and a 0 if the current bit is low.
    /// For every bit the write pin is pulsed.
    /// @param number is the number of bits in data. (unsigned int, 8bits)
    /// @param d is the actual data that needs to be send. (unsigned int, 16bits)
    /// @note as long as the destructor is not activated this function can be called again.
    void writeData(uint8_t aantal, uint16_t d);
    /// \brief
    /// This is the destructor of this class
    /// \details
    /// This destructor sets the Chip Select high, therefore the data transaction has ended.
    ~write_to();
};
/// \brief
/// This is the class for the ht3216C
/// \details
/// this class gives full control over the ledmatrix when writing data.
/// with these functions the matrix can be enabled, disabled, written or cleared etc.
class ht3216C : public write_to{
protected:
    hwlib::pin_in_out &write;
    hwlib::pin_in_out &data;
    hwlib::pin_in_out &cs;
    uint16_t window[24] = {0};
public:
    /// \brief
    /// This is the constructor of this class
    /// \details
    /// This constructor sets up this class, and the write_to class.
    /// @note All the pin_in_out's need to be on output mode.
    ht3216C(hwlib::pin_in_out &write, hwlib::pin_in_out &data, hwlib::pin_in_out & cs);
    /// \brief
    /// This function sends a command to the ht3216C.
    /// @see commands writeData()
    /// @param cmd is an 8 bit command. The default value is the SYS_EN. Used to enable the system.
    void cmd(uint8_t cmd = commands::SYS_EN);
    /// \brief
    /// This function initializes the full ht3216C system
    /// \details
    /// In this function the SYS_EN command is executed
    /// after that the LED_ON, BLINK_OFF, MASTERMODE and COMOPTION.
    /// It also resets the RAM data on the ht3216C.
    /// @see cmd() commands clear()
    void initialize();
    /// \brief
    /// This function turns off the leds.
    /// \details
    /// This function sends the LED_OFF command to the ht3216C.
    /// @see cmd() commands
    void shutdown_leds();
    /// \brief
    /// This function turns off the system
    /// \details
    /// This function sends the SYS_DIS command to the ht3216C, This stops the clock.
    /// @see cmd() commands
    void shutdown();
    /// \brief
    /// This function fills the RAM data with zeros
    /// \details
    /// This function is writes all zeros to the ht3216C, starting from addres zero to the last register.
    /// @see write_to writeData() fill()
    void clear();
    /// \brief
    /// This function fills the RAM data with ones
    /// \details
    /// This function is writes all ones to the ht3216C, starting from addres zero to the last register.
    /// @see write_to writeData() clear()
    void fill();
    /// \brief
    /// This function sets the brightness
    /// \details
    /// This function adds the given brightness parameter to the brightness-command.
    /// This result is the command to write
    /// @param brightness is an int between 0 and 15, 0 is off and 15 is full on.
    /// @note the parameter brightness can't be bigger than 15!
    /// @see commands cmd()
    void set_brightness(uint8_t brightness);
    /// \brief
    /// This function writes a 1 to a coordinate in window
    /// \details
    /// This function changes the value of the uint16_t array window.
    /// when all the pixels are set the flush() function writes it to the ht3216C.
    /// @note Set up function, doesn't write anything.
    /// @param xy is an hwlib::xy with a x value and a y value. these can't be bigger than the led matrix
    /// @see flush() clear_pixel()
    void set_pixel(hwlib::xy xy);
    /// \brief
    /// This function writes a 0 to a coordinate in window
    /// \details
    /// This function changes the value of the uint16_t array window.
    /// when all the pixels are set/reset the flush() function writes it to the ht3216C.
    /// @note Set up function, doesn't write anything.
    /// @param xy is an hwlib::xy with a x value and a y value. these can't be bigger than the led matrix
    /// @see flush() set_pixel()
    void clear_pixel(hwlib::xy xy);
    /// \brief
    /// This function writes the window values to the ht3216C
    /// \details
    /// This function writes the values in window to the ht3216C. It also resets the window.
    /// @see set_pixel write_to
    void flush();
    /// \brief
    /// This function overrites the window
    /// \details
    /// This function changes the window to the parameter. therefore the window can be written by hand.
    /// @see flush()
    /// @param w new (hardcoded) window.
    void change_window(uint16_t w[24]);
};

/// \brief
/// This is a decorator on hwlib::window.
/// \details
/// With this class the ht3216C can be interpreted as the hwlib::window.
/// Therefore this class would let you print lines circles or words.
class window : public hwlib::window{
protected:
    ht3216C &matrix;
public:
    /// \brief
    /// This is the constructor for the ht3216C window
    /// \details
    /// this constructor sets up the matrix and initializes it from the start.
    /// It also sets the brightness to the fullest.
    window(hwlib::xy borders, ht3216C & matrix);
    /// \brief
    /// This function writes a pixel.
    /// \details
    /// this function writes a pixel if it is not black.
    /// @see ht3216C::set_pixel()
    void write_implementation(hwlib::xy pixel, hwlib::color col = {255,0,0}) override;
    /// \brief
    /// This function flushes the matrix.
    /// @see ht3216C::flush()
    void flush() override;
    /// \brief
    /// This function is a test for the ht3216C.
    /// \details
    /// first of all a pixel will go down from the first com to the second and so forth.
    /// after the pixel is at the end it will restart in the next row. This part test the write(), clear() and flush() function.
    /// when the all the pixels have been shown a line will start top to bottom. the line moves from the left row to the right row.
    /// when this sequence is down it will do the same but then with a line from left to right moving down. This shows that the lines are working.
    /// than a rectangle will form from the right top and gets bigger until it fills the whole matrix. this part shows that the line is also working correctly.
    /// afterwards a filled rectangle will do the same. this shows that both the rectangles work correctly.
    /// when the rectangle is filled the whole matrix a circle start to draw from the center. The circle get bigger and bigger until the circle is bigger than the matrix.
    /// thereafter a new circle is drawn and moves from the center to the right. The circle is also working correctly when shown.
    /// at last the matrix.fill() and the matrix.set_brightness() are tested. This is shown by setting all the leds on and increasing the brightness.
    /// when de brightness is at full capacity the brightness decreases untill the leds are off.
    /// the matrix shuts down afterwards.
    /// these are most of the functionalities tested in one function.
    /// @see write_implementation() ht3216C::write() ht3216C::clear() ht3216C::flush() line rectangle circle ht3216C::fill() ht3216C::set_brightness() ht3216C::shutdown()
    /// @note To call this function a ht3216C class needs to be made with the working pin_in_out(). also a window class needs to be called.
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
    /// ht3216C matrix(write, data, cs);
    /// window w(hwlib::xy(16, 24), matrix);
    /// w.test_function();
    ///~~~~~~~~~~~~~~~~~~~~~~~~~~
    void test_function();
};
#endif //V1OOPC_EXAMPLES_HT3216C_H
