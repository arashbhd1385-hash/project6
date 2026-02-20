#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <functional>
#include <sstream>
using namespace std;

struct ScratchEngine;
void render(struct ScratchEngine *engine);
void drawText(struct ScratchEngine *engine, const string &text, int x, int y, SDL_Color color, bool center = false);
void showMakeBlockDialog(struct ScratchEngine *engine);
void showMakeVariableDialog(struct ScratchEngine *engine);
bool isClickInRect(int x, int y, SDL_Rect rect);
void saveProject(struct ScratchEngine *engine, const string &filepath = "");

static struct ScratchEngine *g_engine = nullptr;
struct PixelGlyph {
    bool rows[5][3];
};
static const PixelGlyph PIXEL_FONT[40] = {
        {{{1, 1, 1}, {1, 0, 1}, {1, 1, 1}, {1, 0, 1}, {1, 0, 1}}},
        {{{1, 1, 0}, {1, 0, 1}, {1, 1, 0}, {1, 0, 1}, {1, 1, 0}}},
        {{{0, 1, 1}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {0, 1, 1}}},
        {{{1, 1, 0}, {1, 0, 1}, {1, 0, 1}, {1, 0, 1}, {1, 1, 0}}},
        {{{1, 1, 1}, {1, 0, 0}, {1, 1, 0}, {1, 0, 0}, {1, 1, 1}}},
        {{{1, 1, 1}, {1, 0, 0}, {1, 1, 0}, {1, 0, 0}, {1, 0, 0}}},
        {{{0, 1, 1}, {1, 0, 0}, {1, 0, 1}, {1, 0, 1}, {0, 1, 1}}},
        {{{1, 0, 1}, {1, 0, 1}, {1, 1, 1}, {1, 0, 1}, {1, 0, 1}}},
        {{{1, 1, 1}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {1, 1, 1}}},
        {{{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {1, 0, 1}, {0, 1, 0}}},
        {{{1, 0, 1}, {1, 0, 1}, {1, 1, 0}, {1, 0, 1}, {1, 0, 1}}},
        {{{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 1, 1}}},
        {{{1, 0, 1}, {1, 1, 1}, {1, 1, 1}, {1, 0, 1}, {1, 0, 1}}},
        {{{1, 1, 1}, {1, 0, 1}, {1, 0, 1}, {1, 0, 1}, {1, 0, 1}}},
        {{{0, 1, 0}, {1, 0, 1}, {1, 0, 1}, {1, 0, 1}, {0, 1, 0}}},
        {{{1, 1, 0}, {1, 0, 1}, {1, 1, 0}, {1, 0, 0}, {1, 0, 0}}},
        {{{0, 1, 0}, {1, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}}},
        {{{1, 1, 0}, {1, 0, 1}, {1, 1, 0}, {1, 0, 1}, {1, 0, 1}}},
        {{{0, 1, 1}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}}},
        {{{1, 1, 1}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}}},
        {{{1, 0, 1}, {1, 0, 1}, {1, 0, 1}, {1, 0, 1}, {0, 1, 1}}},
        {{{1, 0, 1}, {1, 0, 1}, {1, 0, 1}, {0, 1, 0}, {0, 1, 0}}},
        {{{1, 0, 1}, {1, 0, 1}, {1, 0, 1}, {1, 1, 1}, {1, 0, 1}}},
        {{{1, 0, 1}, {1, 0, 1}, {0, 1, 0}, {1, 0, 1}, {1, 0, 1}}},
        {{{1, 0, 1}, {1, 0, 1}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}}},
        {{{1, 1, 1}, {0, 0, 1}, {0, 1, 0}, {1, 0, 0}, {1, 1, 1}}},
        {{{0, 1, 0}, {1, 0, 1}, {1, 0, 1}, {1, 0, 1}, {0, 1, 0}}},
        {{{0, 1, 0}, {1, 1, 0}, {0, 1, 0}, {0, 1, 0}, {1, 1, 1}}},
        {{{1, 1, 1}, {0, 0, 1}, {1, 1, 1}, {1, 0, 0}, {1, 1, 1}}},
        {{{1, 1, 1}, {0, 0, 1}, {0, 1, 1}, {0, 0, 1}, {1, 1, 1}}},
        {{{1, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 0, 1}, {0, 0, 1}}},
        {{{1, 1, 1}, {1, 0, 0}, {1, 1, 1}, {0, 0, 1}, {1, 1, 1}}},
        {{{1, 1, 1}, {1, 0, 0}, {1, 1, 1}, {1, 0, 1}, {1, 1, 1}}},
        {{{1, 1, 1}, {0, 0, 1}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}}},
        {{{1, 1, 1}, {1, 0, 1}, {1, 1, 1}, {1, 0, 1}, {1, 1, 1}}},
        {{{1, 1, 1}, {1, 0, 1}, {1, 1, 1}, {0, 0, 1}, {1, 1, 1}}},
        {{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 0, 0}}},
        {{{0, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 0}}},
        {{{0, 0, 0}, {0, 0, 0}, {1, 1, 1}, {0, 0, 0}, {0, 0, 0}}},
        {{{0, 0, 1}, {0, 1, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}}},
};
enum BlockCategory {
    BLOCK_CATEGORY_MOTION,
    BLOCK_CATEGORY_LOOKS,
    BLOCK_CATEGORY_SOUND,
    BLOCK_CATEGORY_EVENTS,
    BLOCK_CATEGORY_CONTROL,
    BLOCK_CATEGORY_SENSING,
    BLOCK_CATEGORY_OPERATORS,
    BLOCK_CATEGORY_VARIABLES,
    BLOCK_CATEGORY_PEN
};
enum BlockType {
    BLOCK_MOVE_STEPS,
    BLOCK_TURN_RIGHT,
    BLOCK_TURN_LEFT,
    BLOCK_GOTO_MOUSE,
    BLOCK_GOTO_X_Y,
    BLOCK_CHANGE_X,
    BLOCK_CHANGE_Y,
    BLOCK_SET_X,
    BLOCK_SET_Y,
    BLOCK_SET_ANGLE,
    BLOCK_IF_ON_EDGE_BOUNCE,
    BLOCK_SAY,
    BLOCK_SAY_FOR_SECONDS,
    BLOCK_THINK,
    BLOCK_THINK_FOR_SECONDS,
    BLOCK_SHOW,
    BLOCK_HIDE,
    BLOCK_SET_SIZE,
    BLOCK_CHANGE_SIZE,
    BLOCK_GO_TO_FRONT,
    BLOCK_GO_BACK_LAYERS,
    BLOCK_PLAY_SOUND,
    BLOCK_PLAY_SOUND_MEOW,
    BLOCK_PLAY_SOUND_UNTIL_DONE,
    BLOCK_STOP_ALL_SOUNDS,
    BLOCK_CHANGE_VOLUME,
    BLOCK_SET_VOLUME,
    BLOCK_WHEN_GREEN_FLAG,
    BLOCK_WHEN_KEY_PRESSED,
    BLOCK_WHEN_SPRITE_CLICKED,
    BLOCK_BROADCAST,
    BLOCK_BROADCAST_AND_WAIT,
    BLOCK_REPEAT,
    BLOCK_FOREVER,
    BLOCK_WAIT,
    BLOCK_WAIT_UNTIL,
    BLOCK_IF_THEN,
    BLOCK_IF_THEN_ELSE,
    BLOCK_STOP_ALL,
    BLOCK_STOP_THIS_SCRIPT,
    BLOCK_STOP_OTHER_SCRIPTS,
    BLOCK_TOUCHING_MOUSE,
    BLOCK_TOUCHING_EDGE,
    BLOCK_TOUCHING_COLOR,
    BLOCK_KEY_PRESSED,
    BLOCK_MOUSE_X,
    BLOCK_MOUSE_Y,
    BLOCK_MOUSE_DOWN,
    BLOCK_TIMER,
    BLOCK_RESET_TIMER,
    BLOCK_DISTANCE_TO,
    BLOCK_ASK_AND_WAIT,
    BLOCK_ANSWER,
    BLOCK_SET_DRAG_MODE,
    BLOCK_ADD,
    BLOCK_SUBTRACT,
    BLOCK_MULTIPLY,
    BLOCK_DIVIDE,
    BLOCK_RANDOM,
    BLOCK_LESS_THAN,
    BLOCK_EQUAL,
    BLOCK_GREATER_THAN,
    BLOCK_AND,
    BLOCK_OR,
    BLOCK_NOT,
    BLOCK_JOIN_STRINGS,
    BLOCK_LENGTH_OF,
    BLOCK_MOD,
    BLOCK_ROUND,
    BLOCK_ABS,
    BLOCK_SQRT,
    BLOCK_SIN,
    BLOCK_COS,
    BLOCK_SET_VARIABLE,
    BLOCK_CHANGE_VARIABLE,
    BLOCK_SHOW_VARIABLE,
    BLOCK_HIDE_VARIABLE,
    BLOCK_PEN_DOWN,
    BLOCK_PEN_UP,
    BLOCK_ERASE_ALL,
    BLOCK_SET_PEN_COLOR,
    BLOCK_SET_PEN_COLOR_TO,
    BLOCK_CHANGE_PEN_COLOR,
    BLOCK_SET_PEN_SIZE,
    BLOCK_CHANGE_PEN_SIZE,
    BLOCK_STAMP,
    BLOCK_DEFINE_FUNC,
    BLOCK_CALL_FUNC,
    BLOCK_CHANGE_BRIGHTNESS,
    BLOCK_SET_BRIGHTNESS,
    BLOCK_CHANGE_SATURATION,
    BLOCK_SET_SATURATION,
    BLOCK_FLIP_HORIZONTAL,
    BLOCK_FLIP_VERTICAL,
    BLOCK_SET_COSTUME,
    BLOCK_NEXT_COSTUME,
    BLOCK_SET_BACKDROP,
    BLOCK_NEXT_BACKDROP,
    BLOCK_DRAG_MODE
};
enum AttachType {
    ATTACH_NONE, ATTACH_ABOVE, ATTACH_BELOW, ATTACH_BETWEEN
};
struct CategoryInfo {
    string name;
    SDL_Color color;
    int category;
    SDL_Rect buttonRect;
};
vector<CategoryInfo> categories = {
        {"Motion",    {76,  151, 255, 255}, BLOCK_CATEGORY_MOTION},
        {"Looks",     {153, 102, 255, 255}, BLOCK_CATEGORY_LOOKS},
        {"Sound",     {207, 99,  207, 255}, BLOCK_CATEGORY_SOUND},
        {"Events",    {255, 215, 0,   255}, BLOCK_CATEGORY_EVENTS},
        {"Control",   {255, 171, 25,  255}, BLOCK_CATEGORY_CONTROL},
        {"Sensing",   {64,  224, 208, 255}, BLOCK_CATEGORY_SENSING},
        {"Operators", {64,  224, 128, 255}, BLOCK_CATEGORY_OPERATORS},
        {"Variables", {255, 140, 0,   255}, BLOCK_CATEGORY_VARIABLES},
        {"Pen",       {15,  189, 140, 255}, BLOCK_CATEGORY_PEN}
};
