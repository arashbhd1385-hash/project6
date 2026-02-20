
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
int getBlockCategory(int type) {
    switch (type) {
        case BLOCK_MOVE_STEPS:
        case BLOCK_TURN_RIGHT:
        case BLOCK_TURN_LEFT:
        case BLOCK_GOTO_MOUSE:
        case BLOCK_GOTO_X_Y:
        case BLOCK_CHANGE_X:
        case BLOCK_CHANGE_Y:
        case BLOCK_SET_X:
        case BLOCK_SET_Y:
        case BLOCK_SET_ANGLE:
        case BLOCK_IF_ON_EDGE_BOUNCE:
            return BLOCK_CATEGORY_MOTION;
        case BLOCK_SAY:
        case BLOCK_SAY_FOR_SECONDS:
        case BLOCK_THINK:
        case BLOCK_THINK_FOR_SECONDS:
        case BLOCK_SHOW:
        case BLOCK_HIDE:
        case BLOCK_SET_SIZE:
        case BLOCK_CHANGE_SIZE:
        case BLOCK_GO_TO_FRONT:
        case BLOCK_GO_BACK_LAYERS:
        case BLOCK_SET_COSTUME:
        case BLOCK_NEXT_COSTUME:
        case BLOCK_SET_BACKDROP:
        case BLOCK_NEXT_BACKDROP:
        case BLOCK_FLIP_HORIZONTAL:
        case BLOCK_FLIP_VERTICAL:
            return BLOCK_CATEGORY_LOOKS;
        case BLOCK_PLAY_SOUND:
        case BLOCK_PLAY_SOUND_MEOW:
        case BLOCK_PLAY_SOUND_UNTIL_DONE:
        case BLOCK_STOP_ALL_SOUNDS:
        case BLOCK_CHANGE_VOLUME:
        case BLOCK_SET_VOLUME:
            return BLOCK_CATEGORY_SOUND;
        case BLOCK_WHEN_GREEN_FLAG:
        case BLOCK_WHEN_KEY_PRESSED:
        case BLOCK_WHEN_SPRITE_CLICKED:
        case BLOCK_BROADCAST:
        case BLOCK_BROADCAST_AND_WAIT:
            return BLOCK_CATEGORY_EVENTS;
        case BLOCK_REPEAT:
        case BLOCK_FOREVER:
        case BLOCK_WAIT:
        case BLOCK_WAIT_UNTIL:
        case BLOCK_IF_THEN:
        case BLOCK_IF_THEN_ELSE:
        case BLOCK_STOP_ALL:
        case BLOCK_STOP_THIS_SCRIPT:
        case BLOCK_STOP_OTHER_SCRIPTS:
            return BLOCK_CATEGORY_CONTROL;
        case BLOCK_TOUCHING_MOUSE:
        case BLOCK_TOUCHING_EDGE:
        case BLOCK_TOUCHING_COLOR:
        case BLOCK_KEY_PRESSED:
        case BLOCK_MOUSE_X:
        case BLOCK_MOUSE_Y:
        case BLOCK_MOUSE_DOWN:
        case BLOCK_TIMER:
        case BLOCK_RESET_TIMER:
        case BLOCK_DISTANCE_TO:
        case BLOCK_ASK_AND_WAIT:
        case BLOCK_ANSWER:
        case BLOCK_SET_DRAG_MODE:
        case BLOCK_DRAG_MODE:
            return BLOCK_CATEGORY_SENSING;
        case BLOCK_ADD:
        case BLOCK_SUBTRACT:
        case BLOCK_MULTIPLY:
        case BLOCK_DIVIDE:
        case BLOCK_RANDOM:
        case BLOCK_LESS_THAN:
        case BLOCK_EQUAL:
        case BLOCK_GREATER_THAN:
        case BLOCK_AND:
        case BLOCK_OR:
        case BLOCK_NOT:
        case BLOCK_JOIN_STRINGS:
        case BLOCK_LENGTH_OF:
        case BLOCK_MOD:
        case BLOCK_ROUND:
        case BLOCK_ABS:
        case BLOCK_SQRT:
        case BLOCK_SIN:
        case BLOCK_COS:
            return BLOCK_CATEGORY_OPERATORS;
        case BLOCK_SET_VARIABLE:
        case BLOCK_CHANGE_VARIABLE:
        case BLOCK_SHOW_VARIABLE:
        case BLOCK_HIDE_VARIABLE:
            return BLOCK_CATEGORY_VARIABLES;
        case BLOCK_PEN_DOWN:
        case BLOCK_PEN_UP:
        case BLOCK_ERASE_ALL:
        case BLOCK_SET_PEN_COLOR:
        case BLOCK_SET_PEN_COLOR_TO:
        case BLOCK_CHANGE_PEN_COLOR:
        case BLOCK_SET_PEN_SIZE:
        case BLOCK_CHANGE_PEN_SIZE:
        case BLOCK_STAMP:
        case BLOCK_CHANGE_BRIGHTNESS:
        case BLOCK_SET_BRIGHTNESS:
        case BLOCK_CHANGE_SATURATION:
        case BLOCK_SET_SATURATION:
            return BLOCK_CATEGORY_PEN;
        default:
            return BLOCK_CATEGORY_MOTION;
    }
}

SDL_Color getBlockColor(int type) {
    int cat = getBlockCategory(type);
    for (const auto &c: categories) {
        if (c.category == cat) { return c.color; }
    }
    return {150, 150, 150, 255};
}

struct Sprite {
    string name;
    float x, y;
    float angle;
    float size;
    bool isPenDown;
    bool isVisible;
    bool draggable;
    SDL_Color penColor;
    int penSize;
    float volume;
    float colorEffect;
    float brightness;
    float saturation;
    SDL_Color bodyColor;
    SDL_Texture *costume;
    string currentCostume;
    string sayText;
    Uint32 sayEndTime;
    int layerOrder;
    string thinkText;
    Uint32 thinkEndTime;
    SDL_RendererFlip flip;
    map<string, float> variables;
    map<string, bool> variableVisible;
    bool isDragging;
    int dragOffsetX, dragOffsetY;
    vector<string> costumes;
    int currentCostumeIndex;
    bool wasDoubleClicked;
    Uint32 doubleClickTime;
};

void Sprite_init(struct Sprite *s, string n, float startX, float startY) {
    s->name = n;
    s->x = startX;
    s->y = startY;
    s->angle = 0;
    s->size = 40;
    s->isPenDown = false;
    s->isVisible = true;
    s->draggable = true;
    s->penSize = 2;
    s->volume = 100;
    s->colorEffect = 0;
    s->brightness = 100;
    s->saturation = 100;
    s->costume = nullptr;
    s->currentCostume = "";
    s->sayText = "";
    s->sayEndTime = 0;
    s->thinkText = "";
    s->thinkEndTime = 0;
    s->layerOrder = 0;
    s->flip = SDL_FLIP_NONE;
    s->isDragging = false;
    s->dragOffsetX = 0;
    s->dragOffsetY = 0;
    s->currentCostumeIndex = 0;
    s->wasDoubleClicked = false;
    s->doubleClickTime = 0;
    s->penColor = {0, 0, 0, 255};
    s->bodyColor = {0, 0, 255, 255};
    s->variables["my variable"] = 0;
    s->variableVisible["my variable"] = true;
}
void Sprite_destroy(struct Sprite *s) {
    if (s->costume) SDL_DestroyTexture(s->costume);
}

void Sprite_setSay(struct Sprite *s, string text, float seconds) {
    s->sayText = text;
    s->sayEndTime = SDL_GetTicks() + (Uint32) (seconds * 1000);
    s->thinkText = "";
    s->thinkEndTime = 0;
}

void Sprite_setThink(struct Sprite *s, string text, float seconds) {
    s->thinkText = text;
    s->thinkEndTime = SDL_GetTicks() + (Uint32) (seconds * 1000);
    s->sayText = "";
    s->sayEndTime = 0;
}

void Sprite_clearSay(struct Sprite *s) {
    s->sayText = "";
    s->sayEndTime = 0;
    s->thinkText = "";
    s->thinkEndTime = 0;
}

bool Sprite_isSaying(struct Sprite *s) {
    if (s->sayText.empty()) return false;
    if (s->sayEndTime == 0) return false;
    if (s->sayEndTime >= 99999999u) return true;
    return SDL_GetTicks() < s->sayEndTime;
}

bool Sprite_isThinking(struct Sprite *s) {
    if (s->thinkText.empty()) return false;
    if (s->thinkEndTime == 0) return false;
    if (s->thinkEndTime >= 99999999u) return true;
    return SDL_GetTicks() < s->thinkEndTime;
}

void Sprite_startDrag(struct Sprite *s, int mouseX, int mouseY) {
    if (!s->draggable) return;
    s->isDragging = true;
    s->dragOffsetX = mouseX - (int) s->x;
    s->dragOffsetY = mouseY - (int) s->y;
}

void Sprite_drag(struct Sprite *s, int mouseX, int mouseY) {
    if (s->isDragging) {
        s->x = mouseX - s->dragOffsetX;
        s->y = mouseY - s->dragOffsetY;
    }
}

void Sprite_stopDrag(struct Sprite *s) {
    s->isDragging = false;
}

void Sprite_checkDoubleClick(struct Sprite *s, Uint32 currentTime) {
    if (s->wasDoubleClicked && currentTime - s->doubleClickTime < 300) {
        s->wasDoubleClicked = false;
    } else {
        s->wasDoubleClicked = true;
        s->doubleClickTime = currentTime;
    }
}

void Sprite_nextCostume(struct Sprite *s) {
    if (s->costumes.empty()) return;
    s->currentCostumeIndex = (s->currentCostumeIndex + 1) % s->costumes.size();
}

void Sprite_flipHorizontal(struct Sprite *s) {
    if (s->flip == SDL_FLIP_HORIZONTAL) {
        s->flip = SDL_FLIP_NONE;
    } else {
        s->flip = SDL_FLIP_HORIZONTAL;
    }
}

void Sprite_flipVertical(struct Sprite *s) {
    if (s->flip == SDL_FLIP_VERTICAL) {
        s->flip = SDL_FLIP_NONE;
    } else {
        s->flip = SDL_FLIP_VERTICAL;
    }
}

struct SensingData {
    int mouseX, mouseY;
    bool isMouseDown;
    const Uint8 *keys;
    Uint32 startTime;
    string answer;
    bool waitingForAnswer;
    string currentQuestion;
    bool stopRequested;
    bool isPaused;
    string lastKey;
};

void SensingData_init(struct SensingData *s) {
    s->isMouseDown = false;
    s->keys = nullptr;
    s->startTime = 0;
    s->waitingForAnswer = false;
    s->stopRequested = false;
    s->isPaused = false;
    s->lastKey = "";
}

void SensingData_update(struct SensingData *s) {
    s->isMouseDown = SDL_GetMouseState(&s->mouseX, &s->mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT);
    s->keys = SDL_GetKeyboardState(NULL);
}

float SensingData_getTimer(struct SensingData *s) {
    return (SDL_GetTicks() - s->startTime) / 1000.0f;
}

void SensingData_resetTimer(struct SensingData *s) {
    s->startTime = SDL_GetTicks();
}

bool SensingData_isKeyPressed(struct SensingData *s, const string &key) {
    if (!s->keys) return false;
    if (key == "space") return s->keys[SDL_SCANCODE_SPACE];
    if (key == "up") return s->keys[SDL_SCANCODE_UP];
    if (key == "down") return s->keys[SDL_SCANCODE_DOWN];
    if (key == "left") return s->keys[SDL_SCANCODE_LEFT];
    if (key == "right") return s->keys[SDL_SCANCODE_RIGHT];
    if (key.length() == 1 && key[0] >= 'a' && key[0] <= 'z') { return s->keys[SDL_SCANCODE_A + (key[0] - 'a')]; }
    return false;
}

struct Block {
    int type;
    float value;
    string textData;
    vector<Block *> children;
    Block *next;
    Block *prev;
    Block *parent;
    SDL_Rect rect;
    bool isDragging;
    int dragOffsetX, dragOffsetY;
    bool isSelected;
    bool isActive;
    bool hasInput;
    char inputText[32];
    bool isEditingInput;
    SDL_Rect inputRect;
    bool hasSecondInput;
    char inputText2[32];
    bool isEditingInput2;
    SDL_Rect inputRect2;
    bool hasPenColorPicker;
    SDL_Rect colorPickerRect;
    bool showColorPicker;
    bool isOperator;
    bool isBinaryOperator;
    vector<Block *> operatorInputs;
    SDL_Rect operatorInputRects[2];
    string lastResult;
    Uint32 resultEndTime;
    SDL_Rect resultRect;
    bool isLoop;
    SDL_Rect loopBodyRect;
};

Block *Block_create(int type, float v, string text) {
    Block *block = new Block();
    block->type = type;
    block->value = v;
    block->textData = text;
    block->next = nullptr;
    block->prev = nullptr;
    block->parent = nullptr;
    block->isDragging = false;
    block->dragOffsetX = 0;
    block->dragOffsetY = 0;
    block->isSelected = false;
    block->isActive = false;
    block->hasInput = false;
    block->isEditingInput = false;
    block->hasSecondInput = false;
    block->isEditingInput2 = false;
    memset(block->inputText2, 0, sizeof(block->inputText2));
    strcpy(block->inputText2, "0");
    block->hasPenColorPicker = false;
    block->showColorPicker = false;
    block->isOperator = false;
    block->isBinaryOperator = false;
    block->isLoop = false;
    block->lastResult = "";
    block->resultEndTime = 0;
    block->rect = {0, 0, 180, 40};
    memset(block->inputText, 0, sizeof(block->inputText));
    string defaultVal = to_string((int) v);
    strcpy(block->inputText, defaultVal.c_str());
    block->inputRect = {block->rect.x + block->rect.w - 55, block->rect.y + 5, 45, 30};
    block->inputRect2 = {block->rect.x + block->rect.w - 55, block->rect.y + 5, 45, 30};
    block->colorPickerRect = {block->rect.x + block->rect.w - 30, block->rect.y + 5, 20, 20};
    switch (block->type) {
        case BLOCK_MOVE_STEPS:
        case BLOCK_TURN_RIGHT:
        case BLOCK_TURN_LEFT:
        case BLOCK_WAIT:
        case BLOCK_CHANGE_X:
        case BLOCK_CHANGE_Y:
        case BLOCK_SET_X:
        case BLOCK_SET_Y:
        case BLOCK_SET_SIZE:
        case BLOCK_CHANGE_SIZE:
        case BLOCK_CHANGE_VOLUME:
        case BLOCK_SET_VOLUME:
        case BLOCK_CHANGE_PEN_COLOR:
        case BLOCK_SET_PEN_SIZE:
        case BLOCK_CHANGE_PEN_SIZE:
        case BLOCK_CHANGE_BRIGHTNESS:
        case BLOCK_SET_BRIGHTNESS:
        case BLOCK_CHANGE_SATURATION:
        case BLOCK_SET_SATURATION:
        case BLOCK_SET_VARIABLE:
        case BLOCK_CHANGE_VARIABLE:
        case BLOCK_SET_ANGLE:
        case BLOCK_GO_BACK_LAYERS:
        case BLOCK_REPEAT:
        case BLOCK_WHEN_KEY_PRESSED:
            block->hasInput = true;
            if (block->type == BLOCK_WHEN_KEY_PRESSED) {
                memset(block->inputText, 0, sizeof(block->inputText));
                strcpy(block->inputText, "space");
            }
            break;
        case BLOCK_GOTO_X_Y:
            block->hasInput = true;
            block->hasSecondInput = true;
            block->rect.w = 220;
            memset(block->inputText, 0, sizeof(block->inputText));
            strcpy(block->inputText, "0");
            memset(block->inputText2, 0, sizeof(block->inputText2));
            strcpy(block->inputText2, "0");
            break;
        case BLOCK_SET_PEN_COLOR:
            block->hasPenColorPicker = true;
            block->rect.w = 200;
            break;
    }
    switch (block->type) {
        case BLOCK_ADD:
        case BLOCK_SUBTRACT:
        case BLOCK_MULTIPLY:
        case BLOCK_DIVIDE:
        case BLOCK_LESS_THAN:
        case BLOCK_EQUAL:
        case BLOCK_GREATER_THAN:
            block->isOperator = true;
            block->isBinaryOperator = true;
            block->rect.w = 250;
            for (int i = 0; i < 2; i++) {
                Block *input = Block_create(BLOCK_MOVE_STEPS, 0, "");
                input->hasInput = true;
                input->rect.w = 60;
                block->operatorInputs.push_back(input);
            }
            break;
        case BLOCK_RANDOM:
        case BLOCK_AND:
        case BLOCK_OR:
        case BLOCK_MOD:
            block->isOperator = true;
            block->isBinaryOperator = true;
            block->rect.w = 200;
            for (int i = 0; i < 2; i++) {
                Block *input = Block_create(BLOCK_MOVE_STEPS, 0, "");
                input->hasInput = true;
                input->rect.w = 60;
                block->operatorInputs.push_back(input);
            }
            break;
        case BLOCK_NOT:
        case BLOCK_ROUND:
        case BLOCK_ABS:
        case BLOCK_SQRT:
        case BLOCK_SIN:
        case BLOCK_COS:
        case BLOCK_LENGTH_OF:
            block->isOperator = true;
            block->isBinaryOperator = false;
            block->rect.w = 180;
            {
                Block *input = Block_create(BLOCK_MOVE_STEPS, 0, "");
                input->hasInput = true;
                input->rect.w = 60;
                block->operatorInputs.push_back(input);
            }
            break;
        default:
            break;
    }
    switch (block->type) {
        case BLOCK_REPEAT:
        case BLOCK_FOREVER:
        case BLOCK_IF_THEN:
        case BLOCK_IF_THEN_ELSE:
            block->isLoop = true;
            block->rect.h = 40;
            block->rect.w = 200;
            block->loopBodyRect = {block->rect.x + 20, block->rect.y + 40, block->rect.w - 20, 50};
            break;
        default:
            break;
    }
    return block;
}

#include <iostream>
using namespace std;

int main() {
    cout<<"Hello, World!"<<endl;
    return 0;
}

