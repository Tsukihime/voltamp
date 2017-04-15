#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define DSPLAY_A_CHAR 10
#define DSPLAY_I_CHAR 11
#define DSPLAY_11_CHAR 12
#define DSPLAY_U_CHAR 13
#define DSPLAY_DASH_CHAR 14
#define DSPLAY_BLANK_CHAR 15

#define DISPLAY_FIRST_DOT 8
#define DISPLAY_SECOND_DOT 4
#define DISPLAY_THIRD_DOT 2
#define DISPLAY_FOURTH_DOT 1

typedef struct {
    uint8_t digits[4];
    uint8_t points;
} TDisplay __attribute__ ((aligned (1)));

class Display {
    public:
        void sendToDisplay(TDisplay* firstDisplay, TDisplay* secondDisplay);
        void initialize();
        
    private:
        TDisplay oldFirstDisplay;
        TDisplay oldSecondDisplay;
    
        static void sendUpTo16Bit(uint16_t data, int bitCount);
        void sendDisplayBlock(TDisplay* display);
        void resetDisplay();
        void switchToNextDispay();
        int isSameDisplays(TDisplay* first, TDisplay* second);
        int copyDisplays(TDisplay* dest, TDisplay* src);
};

extern Display display;

#endif /* _DISPLAY_H_ */
