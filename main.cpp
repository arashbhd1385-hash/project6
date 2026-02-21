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

void Block_destroy(Block *block) {
    for (auto child: block->children) { Block_destroy(child); }
    for (auto input: block->operatorInputs) { Block_destroy(input); }
    delete block;
}

void Block_startEditing(Block *block) {
    block->isEditingInput = true;
}

void Block_stopEditing(Block *block) {
    block->isEditingInput = false;
    block->value = atof(block->inputText);
    if (block->type != BLOCK_SET_VARIABLE && block->type != BLOCK_CHANGE_VARIABLE) {
        size_t pos = block->textData.find_first_of("0123456789");
        if (pos != string::npos) {
            string prefix = block->textData.substr(0, pos);
            string suffix = block->textData.substr(pos);
            size_t endPos = suffix.find_first_not_of("0123456789.-");
            if (endPos != string::npos) {
                suffix = suffix.substr(endPos);
            } else {
                suffix = "";
            }
            block->textData = prefix + string(block->inputText) + suffix;
        }
    }
}

void Block_setResult(Block *block, float res) {
    if (fabs(res - round(res)) < 0.0001) {
        block->lastResult = to_string((int) round(res));
    } else {
        char buffer[32];
        sprintf(buffer, "%.2f", res);
        block->lastResult = buffer;
    }
    block->resultEndTime = SDL_GetTicks() + 2000;
}

bool Block_hasResult(Block *block) {
    return !block->lastResult.empty() && SDL_GetTicks() < block->resultEndTime;
}

void Block_attachBelow(Block *block, Block *child) {
    if (!child) return;
    block->next = child;
    child->prev = block;
    child->rect.x = block->rect.x;
    child->rect.y = block->rect.y + block->rect.h;
}

void Block_attachAbove(Block *block, Block *parent) {
    if (!parent) return;
    Block *oldPrev = parent->prev;
    if (oldPrev) {
        oldPrev->next = block;
        block->prev = oldPrev;
    } else {
        block->prev = nullptr;
    }
    block->next = parent;
    parent->prev = block;
    block->rect.x = parent->rect.x;
    block->rect.y = parent->rect.y - block->rect.h;
    Block *cur = block;
    while (cur->next) {
        cur->next->rect.x = cur->rect.x;
        cur->next->rect.y = cur->rect.y + cur->rect.h;
        cur = cur->next;
    }
}

void Block_detachBelow(Block *block) {
    if (block->next) {
        block->next->prev = nullptr;
        block->next = nullptr;
    }
}

void Block_detachAbove(Block *block) {
    if (block->prev) {
        block->prev->next = nullptr;
        block->prev = nullptr;
    }
}

void Block_insertBetween(Block *block, Block *above, Block *below) {
    if (above) {
        above->next = block;
        block->prev = above;
    }
    if (below) {
        below->prev = block;
        block->next = below;
    }
    block->rect.x = (above ? above->rect.x : (below ? below->rect.x : block->rect.x));
    if (above) { block->rect.y = above->rect.y + above->rect.h; }
}

void Block_updateBelowPositions(Block *block) {
    if (block->next) {
        block->next->rect.x = block->rect.x;
        block->next->rect.y = block->rect.y + block->rect.h;
        Block_updateBelowPositions(block->next);
    }
}

Block *Block_getFirstInChain(Block *block) {
    Block *current = block;
    while (current->prev) { current = current->prev; }
    return current;
}

Block *Block_getLastInChain(Block *block) {
    Block *current = block;
    while (current->next) { current = current->next; }
    return current;
}

int Block_getChainLength(Block *block) {
    int count = 1;
    Block *current = block;
    while (current->next) {
        count++;
        current = current->next;
    }
    return count;
}

void Block_shiftChainDown(Block *block, int amount) {
    if (block->next) {
        block->next->rect.y += amount;
        Block_shiftChainDown(block->next, amount);
    }
}

float Block_evaluate(Block *block, struct Sprite *s, struct SensingData *sensing, SDL_Rect stageRect,
                     map<string, float> &globalVariables) {
    float val1 = 0, val2 = 0;
    if (block->operatorInputs.size() >= 1) val1 = block->operatorInputs[0]->value;
    if (block->operatorInputs.size() >= 2) val2 = block->operatorInputs[1]->value;
    switch (block->type) {
        case BLOCK_ADD:
            return val1 + val2;
        case BLOCK_SUBTRACT:
            return val1 - val2;
        case BLOCK_MULTIPLY:
            return val1 * val2;
        case BLOCK_DIVIDE:
            if (val2 != 0) return val1 / val2;
            printf("[ERROR] Math safeguard: division by zero!\n");
            return 0;
        case BLOCK_RANDOM:
            return val1 + (rand() / (float) RAND_MAX) * (val2 - val1);
        case BLOCK_LESS_THAN:
            return (val1 < val2) ? 1 : 0;
        case BLOCK_EQUAL:
            return (val1 == val2) ? 1 : 0;
        case BLOCK_GREATER_THAN:
            return (val1 > val2) ? 1 : 0;
        case BLOCK_AND:
            return (val1 != 0 && val2 != 0) ? 1 : 0;
        case BLOCK_OR:
            return (val1 != 0 || val2 != 0) ? 1 : 0;
        case BLOCK_NOT:
            return (val1 == 0) ? 1 : 0;
        case BLOCK_MOD:
            if (val2 != 0) return fmod(val1, val2);
            return 0;
        case BLOCK_ROUND:
            return round(val1);
        case BLOCK_ABS:
            return abs(val1);
        case BLOCK_SQRT:
            if (val1 >= 0) return sqrt(val1);
            printf("[ERROR] Math safeguard: sqrt of negative number!\n");
            return 0;
        case BLOCK_SIN:
            return sin(val1 * M_PI / 180.0);
        case BLOCK_COS:
            return cos(val1 * M_PI / 180.0);
        case BLOCK_LENGTH_OF:
            return to_string(val1).length();
        case BLOCK_MOUSE_X:
            return sensing->mouseX - stageRect.x;
        case BLOCK_MOUSE_Y:
            return sensing->mouseY - stageRect.y;
        case BLOCK_TIMER:
            return SensingData_getTimer(sensing);
        case BLOCK_MOUSE_DOWN:
            return sensing->isMouseDown ? 1 : 0;
        case BLOCK_ANSWER:
            if (!sensing->answer.empty()) {
                char *endPtr;
                float num = strtof(sensing->answer.c_str(), &endPtr);
                if (*endPtr == '\0') return num;
            }
            return 0;
        case BLOCK_SET_VARIABLE:
        case BLOCK_CHANGE_VARIABLE:
            return globalVariables["my variable"];
        default:
            return block->value;
    }
}

bool Block_evaluateCondition(Block *block, struct Sprite *s, struct SensingData *sensing, SDL_Rect stageRect,
                             map<string, float> &globalVariables) {
    switch (block->type) {
        case BLOCK_TOUCHING_MOUSE: {
            int mouseX = sensing->mouseX - stageRect.x;
            int mouseY = sensing->mouseY - stageRect.y;
            float dist = sqrt((s->x - mouseX) * (s->x - mouseX) + (s->y - mouseY) * (s->y - mouseY));
            return dist < 30;
        }
        case BLOCK_TOUCHING_EDGE:
            return s->x < 10 || s->x > stageRect.w - 10 || s->y < 10 || s->y > stageRect.h - 10;
        case BLOCK_KEY_PRESSED:
            return SensingData_isKeyPressed(sensing, block->textData);
        case BLOCK_LESS_THAN:
        case BLOCK_EQUAL:
        case BLOCK_GREATER_THAN:
        case BLOCK_AND:
        case BLOCK_OR:
        case BLOCK_NOT:
            return Block_evaluate(block, s, sensing, stageRect, globalVariables) != 0;
        default:
            return block->value > 0;
    }
}

bool Block_execute(Block *block, struct Sprite *s, SDL_Renderer *renderer, SDL_Texture *penLayer,
                   map<string, Block *> &customFunctions, struct SensingData *sensing,
                   SDL_Rect stageRect, Mix_Chunk *meowSound, Mix_Chunk *popSound,
                   map<string, float> *globalVariables) {
    if (!globalVariables) return true;
    float oldX = s->x;
    float oldY = s->y;
    if (block->isOperator) {
        float result = Block_evaluate(block, s, sensing, stageRect, *globalVariables);
        Block_setResult(block, result);
    }
    bool isSensingCondition = false;
    switch (block->type) {
        case BLOCK_TOUCHING_MOUSE:
        case BLOCK_TOUCHING_EDGE:
        case BLOCK_KEY_PRESSED:
        case BLOCK_MOUSE_DOWN:
            isSensingCondition = true;
            break;
        default:
            break;
    }
    if (isSensingCondition) {
        bool result = Block_evaluateCondition(block, s, sensing, stageRect, *globalVariables);
        block->lastResult = result ? "true" : "false";
        block->resultEndTime = SDL_GetTicks() + 2000;
    }
    switch (block->type) {
        case BLOCK_MOVE_STEPS: {
            float steps = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->x += steps * cos(s->angle * M_PI / 180.0);
            s->y += steps * sin(s->angle * M_PI / 180.0);
            s->x = max(0.0f, min(s->x, (float) stageRect.w));
            s->y = max(0.0f, min(s->y, (float) stageRect.h));
            if (s->isPenDown) {
                SDL_SetRenderTarget(renderer, penLayer);
                SDL_SetRenderDrawColor(renderer, s->penColor.r, s->penColor.g, s->penColor.b, 255);
                int ps = max(1, s->penSize);
                for (int dx = -ps / 2; dx <= ps / 2; dx++) {
                    for (int dy = -ps / 2; dy <= ps / 2; dy++) {
                        SDL_RenderDrawLine(renderer,
                                           (int) oldX + dx, (int) oldY + dy,
                                           (int) s->x + dx, (int) s->y + dy);
                    }
                }
                SDL_SetRenderTarget(renderer, NULL);
            }
            break;
        }
        case BLOCK_TURN_RIGHT: {
            float angle = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->angle += angle;
            break;
        }
        case BLOCK_TURN_LEFT: {
            float angle = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->angle -= angle;
            break;
        }
        case BLOCK_GOTO_MOUSE:
            s->x = sensing->mouseX - stageRect.x;
            s->y = sensing->mouseY - stageRect.y;
            s->x = max(0.0f, min(s->x, (float) stageRect.w));
            s->y = max(0.0f, min(s->y, (float) stageRect.h));
            break;
        case BLOCK_GOTO_X_Y: {
            float newX = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            float newY = strlen(block->inputText2) > 0 ? atof(block->inputText2) : 0.0f;
            if (block->children.size() >= 2) {
                newX = block->children[0]->value;
                newY = block->children[1]->value;
            }
            s->x = max(0.0f, min(newX, (float) stageRect.w));
            s->y = max(0.0f, min(newY, (float) stageRect.h));
            break;
        }
        case BLOCK_CHANGE_X: {
            float delta = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->x += delta;
            s->x = max(0.0f, min(s->x, (float) stageRect.w));
            break;
        }
        case BLOCK_CHANGE_Y: {
            float delta = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->y += delta;
            s->y = max(0.0f, min(s->y, (float) stageRect.h));
            break;
        }
        case BLOCK_SET_X: {
            float newX = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->x = newX;
            s->x = max(0.0f, min(s->x, (float) stageRect.w));
            break;
        }
        case BLOCK_SET_Y: {
            float newY = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->y = newY;
            s->y = max(0.0f, min(s->y, (float) stageRect.h));
            break;
        }
        case BLOCK_SET_ANGLE: {
            float newAngle = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->angle = newAngle;
            break;
        }
        case BLOCK_IF_ON_EDGE_BOUNCE: {
            bool bounced = false;
            if (s->x <= 10) {
                s->x = 10;
                s->angle = 180 - s->angle;
                bounced = true;
            }
            else if (s->x >= stageRect.w - 10) {
                s->x = stageRect.w - 10;
                s->angle = 180 - s->angle;
                bounced = true;
            }
            if (s->y <= 10) {
                s->y = 10;
                s->angle = -s->angle;
                bounced = true;
            }
            else if (s->y >= stageRect.h - 10) {
                s->y = stageRect.h - 10;
                s->angle = -s->angle;
                bounced = true;
            }
            (void) bounced;
            break;
        }
        case BLOCK_SAY: {
            string sayMsg = block->textData;
            if (sayMsg.substr(0, 4) == "say ") sayMsg = sayMsg.substr(4);
            if (sayMsg.empty()) sayMsg = "Hello";
            s->sayText = sayMsg;
            s->sayEndTime = SDL_GetTicks() + 99999999u;
            s->thinkText = "";
            s->thinkEndTime = 0;
            break;
        }
        case BLOCK_SAY_FOR_SECONDS: {
            float secs = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            if (secs <= 0) secs = 2.0f;
            string sayMsg = block->textData;
            if (sayMsg.substr(0, 4) == "say ") sayMsg = sayMsg.substr(4);
            size_t forPos = sayMsg.rfind(" for ");
            if (forPos != string::npos) sayMsg = sayMsg.substr(0, forPos);
            if (sayMsg.empty()) sayMsg = "Hello";
            s->sayText = sayMsg;
            s->thinkText = "";
            s->thinkEndTime = 0;
            Uint32 endTime = SDL_GetTicks() + (Uint32) (secs * 1000);
            s->sayEndTime = endTime;
            while (SDL_GetTicks() < endTime && !sensing->stopRequested) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                    if (ev.type == SDL_QUIT) {
                        sensing->stopRequested = true;
                        break;
                    }
                    if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) { sensing->stopRequested = true; }
                }
                SDL_Delay(16);
                if (g_engine) render(g_engine); else SDL_RenderPresent(renderer);
            }
            if (!sensing->stopRequested) {
                s->sayText = "";
                s->sayEndTime = 0;
            }
            break;
        }
        case BLOCK_THINK: {
            string thinkMsg = block->textData;
            if (thinkMsg.substr(0, 6) == "think ") thinkMsg = thinkMsg.substr(6);
            if (thinkMsg.empty()) thinkMsg = "Hmm...";
            s->thinkText = thinkMsg;
            s->thinkEndTime = SDL_GetTicks() + 99999999u;
            s->sayText = "";
            s->sayEndTime = 0;
            break;
        }
        case BLOCK_THINK_FOR_SECONDS: {
            float secs = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            if (secs <= 0) secs = 2.0f;
            string thinkMsg = block->textData;
            if (thinkMsg.substr(0, 6) == "think ") thinkMsg = thinkMsg.substr(6);
            size_t forPos = thinkMsg.rfind(" for ");
            if (forPos != string::npos) thinkMsg = thinkMsg.substr(0, forPos);
            if (thinkMsg.empty()) thinkMsg = "Hmm...";
            s->thinkText = thinkMsg;
            s->sayText = "";
            s->sayEndTime = 0;
            Uint32 endTime = SDL_GetTicks() + (Uint32) (secs * 1000);
            s->thinkEndTime = endTime;
            while (SDL_GetTicks() < endTime && !sensing->stopRequested) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                    if (ev.type == SDL_QUIT) {
                        sensing->stopRequested = true;
                        break;
                    }
                    if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) { sensing->stopRequested = true; }
                }
                SDL_Delay(16);
                if (g_engine) render(g_engine); else SDL_RenderPresent(renderer);
            }
            if (!sensing->stopRequested) {
                s->thinkText = "";
                s->thinkEndTime = 0;
            }
            break;
        }
        case BLOCK_SHOW:
            s->isVisible = true;
            break;
        case BLOCK_HIDE:
            s->isVisible = false;
            break;
        case BLOCK_SET_SIZE: {
            float newSize = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->size = newSize;
            break;
        }
        case BLOCK_CHANGE_SIZE: {
            float delta = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->size += delta;
            s->size = max(5.0f, s->size);
            break;
        }
        case BLOCK_GO_TO_FRONT:
            s->layerOrder = 1000;
            break;
        case BLOCK_GO_BACK_LAYERS: {
            int layers = (int) (strlen(block->inputText) > 0 ? atof(block->inputText) : block->value);
            s->layerOrder = max(0, s->layerOrder - layers);
            break;
        }
        case BLOCK_SET_COSTUME:
            s->currentCostume = block->textData;
            break;
        case BLOCK_NEXT_COSTUME:
            Sprite_nextCostume(s);
            break;
        case BLOCK_FLIP_HORIZONTAL:
            Sprite_flipHorizontal(s);
            break;
        case BLOCK_FLIP_VERTICAL:
            Sprite_flipVertical(s);
            break;
        case BLOCK_PLAY_SOUND:
            if (popSound) {
                Mix_VolumeChunk(popSound, (int) (s->volume * MIX_MAX_VOLUME / 100.0));
                Mix_PlayChannel(-1, popSound, 0);
            }
            break;
        case BLOCK_PLAY_SOUND_MEOW:
            if (meowSound) {
                Mix_VolumeChunk(meowSound, (int) (s->volume * MIX_MAX_VOLUME / 100.0));
                Mix_PlayChannel(-1, meowSound, 0);
            }
            break;
        case BLOCK_PLAY_SOUND_UNTIL_DONE:
            if (meowSound) {
                Mix_VolumeChunk(meowSound, (int) (s->volume * MIX_MAX_VOLUME / 100.0));
                int channel = Mix_PlayChannel(-1, meowSound, 0);
                while (Mix_Playing(channel) && !sensing->stopRequested) {
                    SDL_Delay(100);
                    SDL_RenderPresent(renderer);
                }
            }
            break;
        case BLOCK_STOP_ALL_SOUNDS:
            Mix_HaltChannel(-1);
            break;
        case BLOCK_CHANGE_VOLUME: {
            float delta = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->volume += delta;
            s->volume = max(0.0f, min(s->volume, 100.0f));
            break;
        }
        case BLOCK_SET_VOLUME: {
            float newVolume = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->volume = newVolume;
            s->volume = max(0.0f, min(s->volume, 100.0f));
            break;
        }
        case BLOCK_WHEN_GREEN_FLAG:
        case BLOCK_WHEN_KEY_PRESSED:
        case BLOCK_WHEN_SPRITE_CLICKED:
            break;
        case BLOCK_BROADCAST:
        case BLOCK_BROADCAST_AND_WAIT:
            break;
        case BLOCK_WAIT: {
            float seconds = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            if (seconds <= 0) seconds = 1.0f;
            Uint32 start = SDL_GetTicks();
            while (SDL_GetTicks() - start < (Uint32) (seconds * 1000) && !sensing->stopRequested) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                    if (ev.type == SDL_QUIT) {
                        sensing->stopRequested = true;
                        break;
                    }
                    if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT) {
                        int stopX = stageRect.x + 60, stopY = stageRect.y + 10;
                        if (ev.button.x >= stopX && ev.button.x <= stopX + 40 &&
                            ev.button.y >= stopY && ev.button.y <= stopY + 40) {
                            sensing->stopRequested = true;
                        }
                    }
                }
                SDL_Delay(10);
                SDL_RenderPresent(renderer);
            }
            break;
        }
        case BLOCK_WAIT_UNTIL: {
            while (!block->children.empty() &&
                   !Block_evaluateCondition(block->children[0], s, sensing, stageRect, *globalVariables) &&
                   !sensing->stopRequested) {
                SDL_Delay(10);
                SDL_RenderPresent(renderer);
            }
            break;
        }
        case BLOCK_REPEAT: {
            int count = (int) (strlen(block->inputText) > 0 ? atof(block->inputText) : block->value);
            if (count <= 0) count = 10;
            vector<Block *> bodyBlocks;
            if (!block->children.empty()) {
                bodyBlocks = block->children;
            } else if (block->next) {
                Block *cur = block->next;
                while (cur) {
                    if (cur->rect.x >= block->rect.x + 10 &&
                        cur->rect.x <= block->rect.x + block->rect.w) {
                        bodyBlocks.push_back(cur);
                        cur = cur->next;
                    } else {
                        break;
                    }
                }
            }
            for (int i = 0; i < count && !sensing->stopRequested; i++) {
                while (sensing->isPaused && !sensing->stopRequested) { SDL_Delay(16); }
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                    if (ev.type == SDL_QUIT) {
                        sensing->stopRequested = true;
                        break;
                    }
                    if (ev.type == SDL_MOUSEBUTTONDOWN) {
                        int stopX = stageRect.x + 60, stopY = stageRect.y + 10;
                        if (ev.button.x >= stopX && ev.button.x <= stopX + 40 &&
                            ev.button.y >= stopY && ev.button.y <= stopY + 40) {
                            sensing->stopRequested = true;
                        }
                    }
                    if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) { sensing->stopRequested = true; }
                }
                if (sensing->stopRequested) break;
                for (auto child: bodyBlocks) {
                    if (sensing->stopRequested) return false;
                    if (!Block_execute(child, s, renderer, penLayer, customFunctions,
                                       sensing, stageRect, meowSound, popSound, globalVariables)) {
                        return false;
                    }
                }
                if (g_engine) render(g_engine); else SDL_RenderPresent(renderer);
                SDL_Delay(16);
            }
            break;
        }
        case BLOCK_FOREVER: {
            vector<Block *> bodyBlocks;
            if (!block->children.empty()) {
                bodyBlocks = block->children;
            } else if (block->next) {
                Block *cur = block->next;
                while (cur) {
                    if (cur->rect.x >= block->rect.x + 10 &&
                        cur->rect.x <= block->rect.x + block->rect.w) {
                        bodyBlocks.push_back(cur);
                        cur = cur->next;
                    } else {
                        break;
                    }
                }
            }
            int watchdog = 0;
            while (!sensing->stopRequested) {
                while (sensing->isPaused && !sensing->stopRequested) { SDL_Delay(16); }
                watchdog++;
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                    if (ev.type == SDL_QUIT) {
                        sensing->stopRequested = true;
                        break;
                    }
                    if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT) {
                        int stopX = stageRect.x + 60, stopY = stageRect.y + 10;
                        if (ev.button.x >= stopX && ev.button.x <= stopX + 40 &&
                            ev.button.y >= stopY && ev.button.y <= stopY + 40) {
                            sensing->stopRequested = true;
                        }
                        int pauseX = stageRect.x + 110, pauseY = stageRect.y + 10;
                        if (ev.button.x >= pauseX && ev.button.x <= pauseX + 40 &&
                            ev.button.y >= pauseY && ev.button.y <= pauseY + 40) {
                            sensing->isPaused = !sensing->isPaused;
                        }
                    }
                    if (ev.type == SDL_KEYDOWN) {
                        if (ev.key.keysym.sym == SDLK_ESCAPE) sensing->stopRequested = true;
                        string key;
                        switch (ev.key.keysym.sym) {
                            case SDLK_SPACE:
                                key = "space";
                                break;
                            case SDLK_UP:
                                key = "up";
                                break;
                            case SDLK_DOWN:
                                key = "down";
                                break;
                            case SDLK_LEFT:
                                key = "left";
                                break;
                            case SDLK_RIGHT:
                                key = "right";
                                break;
                            default:
                                if (ev.key.keysym.sym >= SDLK_a && ev.key.keysym.sym <= SDLK_z)
                                    key = string(1, 'a' + (ev.key.keysym.sym - SDLK_a));
                                break;
                        }
                        if (!key.empty()) sensing->lastKey = key;
                    }
                }
                if (sensing->stopRequested) break;
                sensing->isMouseDown =
                        SDL_GetMouseState(&sensing->mouseX, &sensing->mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT);
                sensing->keys = SDL_GetKeyboardState(NULL);
                for (auto child: bodyBlocks) {
                    if (sensing->stopRequested) return false;
                    if (!Block_execute(child, s, renderer, penLayer, customFunctions,
                                       sensing, stageRect, meowSound, popSound, globalVariables)) {
                        return false;
                    }
                }
                if (watchdog % 2 == 0) { if (g_engine) render(g_engine); else SDL_RenderPresent(renderer); }
                SDL_Delay(16);
            }
            return false;
        }
        case BLOCK_IF_THEN: {
            bool condition = false;
            if (!block->children.empty()) {
                condition = Block_evaluateCondition(block->children[0], s, sensing, stageRect, *globalVariables);
            } else {
                condition = (block->value != 0);
            }
            if (condition) {
                int start = block->children.empty() ? 0 : 1;
                for (int i = start; i < (int) block->children.size() && !sensing->stopRequested; i++) {
                    if (!Block_execute(block->children[i], s, renderer, penLayer, customFunctions,
                                       sensing, stageRect, meowSound, popSound, globalVariables)) {
                        return false;
                    }
                }
            }
            break;
        }
        case BLOCK_IF_THEN_ELSE: {
            bool condition = false;
            if (!block->children.empty()) {
                condition = Block_evaluateCondition(block->children[0], s, sensing, stageRect, *globalVariables);
            } else {
                condition = (block->value != 0);
            }
            int elseIdx = (int) block->children.size();
            for (int i = 1; i < (int) block->children.size(); i++) {
                if (block->children[i]->type == -1) {
                    elseIdx = i;
                    break;
                }
            }
            int start = block->children.empty() ? 0 : 1;
            if (condition) {
                for (int i = start; i < elseIdx && !sensing->stopRequested; i++) {
                    if (!Block_execute(block->children[i], s, renderer, penLayer, customFunctions,
                                       sensing, stageRect, meowSound, popSound, globalVariables)) {
                        return false;
                    }
                }
            } else {
                for (int i = elseIdx + 1; i < (int) block->children.size() && !sensing->stopRequested; i++) {
                    if (!Block_execute(block->children[i], s, renderer, penLayer, customFunctions,
                                       sensing, stageRect, meowSound, popSound, globalVariables)) {
                        return false;
                    }
                }
            }
            break;
        }
        case BLOCK_STOP_ALL:
            sensing->stopRequested = true;
            Mix_HaltChannel(-1);
            return false;
        case BLOCK_STOP_THIS_SCRIPT:
            return false;
        case BLOCK_ASK_AND_WAIT: {
            string question = block->textData.empty() ? "What's your name?" : block->textData;
            s->sayText = question;
            s->sayEndTime = SDL_GetTicks() + 99999999u;
            sensing->waitingForAnswer = true;
            sensing->currentQuestion = question;
            sensing->answer = "";
            SDL_StartTextInput();
            while (sensing->waitingForAnswer && !sensing->stopRequested) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                    if (ev.type == SDL_QUIT) {
                        sensing->stopRequested = true;
                        break;
                    }
                    if (ev.type == SDL_TEXTINPUT) { sensing->answer += ev.text.text; }
                    if (ev.type == SDL_KEYDOWN) {
                        if (ev.key.keysym.sym == SDLK_RETURN || ev.key.keysym.sym == SDLK_KP_ENTER) {
                            sensing->waitingForAnswer = false;
                        } else if (ev.key.keysym.sym == SDLK_BACKSPACE && !sensing->answer.empty()) {
                            sensing->answer.pop_back();
                        }
                    }
                }
                s->sayText = question + "\n> " + sensing->answer + "_";
                SDL_RenderPresent(renderer);
                SDL_Delay(16);
            }
            SDL_StopTextInput();
            Sprite_clearSay(s);
            break;
        }
        case BLOCK_SET_DRAG_MODE:
        case BLOCK_DRAG_MODE: {
            float mode = block->value;
            s->draggable = (mode != 0);
            break;
        }
        case BLOCK_PEN_DOWN:
            s->isPenDown = true;
            break;
        case BLOCK_PEN_UP:
            s->isPenDown = false;
            break;
        case BLOCK_ERASE_ALL:
            SDL_SetRenderTarget(renderer, penLayer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, NULL);
            break;
        case BLOCK_SET_PEN_COLOR: {
            break;
        }
        case BLOCK_CHANGE_PEN_COLOR: {
            float delta = block->value;
            s->colorEffect += delta;
            s->penColor.r = (Uint8) ((int) s->colorEffect % 255);
            s->penColor.g = (Uint8) ((int) (s->colorEffect + 85) % 255);
            s->penColor.b = (Uint8) ((int) (s->colorEffect + 170) % 255);
            break;
        }
        case BLOCK_SET_PEN_SIZE: {
            int newSize = (int) (strlen(block->inputText) > 0 ? atof(block->inputText) : block->value);
            s->penSize = max(1, newSize);
            break;
        }
        case BLOCK_CHANGE_PEN_SIZE: {
            int delta = (int) (strlen(block->inputText) > 0 ? atof(block->inputText) : block->value);
            s->penSize = max(1, s->penSize + delta);
            break;
        }
        case BLOCK_STAMP: {
            SDL_SetRenderTarget(renderer, penLayer);
            if (s->costume) {
                int w, h;
                SDL_QueryTexture(s->costume, NULL, NULL, &w, &h);
                float scale = min((float) stageRect.w / w, (float) stageRect.h / h) * (s->size / 100.0f);
                int newW = (int) (w * scale);
                int newH = (int) (h * scale);
                SDL_Rect dest = {(int) s->x - newW / 2, (int) s->y - newH / 2, newW, newH};
                SDL_RenderCopyEx(renderer, s->costume, NULL, &dest, s->angle, NULL, s->flip);
            } else {
                SDL_SetRenderDrawColor(renderer, s->penColor.r, s->penColor.g, s->penColor.b, 200);
                int r = 20;
                for (int dy = -r; dy <= r; dy++)
                    for (int dx = -r; dx <= r; dx++)
                        if (dx * dx + dy * dy <= r * r)
                            SDL_RenderDrawPoint(renderer, (int) s->x + dx, (int) s->y + dy);
            }
            SDL_SetRenderTarget(renderer, NULL);
            break;
        }
        case BLOCK_CHANGE_BRIGHTNESS: {
            float delta = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->brightness += delta;
            s->brightness = max(0.0f, min(s->brightness, 200.0f));
            float bf = s->brightness / 100.0f;
            s->penColor.r = (Uint8) min(255.0f, s->penColor.r * bf);
            s->penColor.g = (Uint8) min(255.0f, s->penColor.g * bf);
            s->penColor.b = (Uint8) min(255.0f, s->penColor.b * bf);
            break;
        }
        case BLOCK_SET_BRIGHTNESS: {
            float newBrightness = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->brightness = max(0.0f, min(newBrightness, 200.0f));
            float bf = s->brightness / 100.0f;
            s->penColor.r = (Uint8) min(255.0f, s->penColor.r * bf);
            s->penColor.g = (Uint8) min(255.0f, s->penColor.g * bf);
            s->penColor.b = (Uint8) min(255.0f, s->penColor.b * bf);
            break;
        }
        case BLOCK_CHANGE_SATURATION: {
            float delta = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->saturation += delta;
            s->saturation = max(0.0f, min(s->saturation, 200.0f));
            float gray = 0.299f * s->penColor.r + 0.587f * s->penColor.g + 0.114f * s->penColor.b;
            float sf = s->saturation / 100.0f;
            s->penColor.r = (Uint8) min(255.0f, max(0.0f, gray + sf * (s->penColor.r - gray)));
            s->penColor.g = (Uint8) min(255.0f, max(0.0f, gray + sf * (s->penColor.g - gray)));
            s->penColor.b = (Uint8) min(255.0f, max(0.0f, gray + sf * (s->penColor.b - gray)));
            break;
        }
        case BLOCK_SET_SATURATION: {
            float newSaturation = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            s->saturation = max(0.0f, min(newSaturation, 200.0f));
            float gray = 0.299f * s->penColor.r + 0.587f * s->penColor.g + 0.114f * s->penColor.b;
            float sf = s->saturation / 100.0f;
            s->penColor.r = (Uint8) min(255.0f, max(0.0f, gray + sf * (s->penColor.r - gray)));
            s->penColor.g = (Uint8) min(255.0f, max(0.0f, gray + sf * (s->penColor.g - gray)));
            s->penColor.b = (Uint8) min(255.0f, max(0.0f, gray + sf * (s->penColor.b - gray)));
            break;
        }
        case BLOCK_SET_VARIABLE: {
            float val = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            string varName = block->textData;
            if (varName.substr(0, 4) == "set ") varName = varName.substr(4);
            size_t toPos = varName.rfind(" to");
            if (toPos != string::npos) varName = varName.substr(0, toPos);
            if (varName.empty()) varName = "my variable";
            (*globalVariables)[varName] = val;
            s->variables[varName] = val;
            break;
        }
        case BLOCK_CHANGE_VARIABLE: {
            float delta = strlen(block->inputText) > 0 ? atof(block->inputText) : block->value;
            string varName = block->textData;
            if (varName.substr(0, 7) == "change ") varName = varName.substr(7);
            size_t byPos = varName.rfind(" by");
            if (byPos != string::npos) varName = varName.substr(0, byPos);
            if (varName.empty()) varName = "my variable";
            (*globalVariables)[varName] += delta;
            s->variables[varName] += delta;
            break;
        }
        case BLOCK_SHOW_VARIABLE: {
            string varName = block->textData;
            if (varName.substr(0, 13) == "show variable") varName = varName.substr(13);
            if (!varName.empty() && varName[0] == ' ') varName = varName.substr(1);
            if (varName.empty()) varName = "my variable";
            s->variableVisible[varName] = true;
            (*globalVariables)[varName] = (*globalVariables).count(varName) ? (*globalVariables)[varName] : 0;
            break;
        }
        case BLOCK_HIDE_VARIABLE: {
            string varName = block->textData;
            if (varName.substr(0, 13) == "hide variable") varName = varName.substr(13);
            if (!varName.empty() && varName[0] == ' ') varName = varName.substr(1);
            if (varName.empty()) varName = "my variable";
            s->variableVisible[varName] = false;
            break;
        }
        case BLOCK_DEFINE_FUNC:
            customFunctions[block->textData] = block;
            break;
        case BLOCK_CALL_FUNC:
            if (customFunctions.find(block->textData) != customFunctions.end()) {
                Block *funcBody = customFunctions[block->textData];
                if (funcBody && !funcBody->children.empty()) {
                    for (auto child: funcBody->children) {
                        if (!Block_execute(child, s, renderer, penLayer, customFunctions,
                                           sensing, stageRect, meowSound, popSound, globalVariables)) {
                            return false;
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
    return true;
}
struct Button {
    SDL_Rect rect;
    string label;
    SDL_Color color;
    SDL_Color textColor;
    int actionID;
};
struct ScratchEngine {
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    SDL_Texture *penLayer;
    TTF_Font *m_font;
    Mix_Chunk *meowSound;
    Mix_Chunk *popSound;
    int winWidth;
    int winHeight;
    bool running;
    struct SensingData sensing;
    map<string, Block *> customFunctions;
    map<string, float> globalVariables;
    vector<struct Sprite> sprites;
    int activeSpriteIndex;
    vector<string> systemLog;
    int MAX_LOG_LINES;
    SDL_Color backgroundColor;
    string currentBackgroundName;
    vector<string> backgrounds;
    int currentBackgroundIndex;
    SDL_Texture *backgroundTexture;
    vector<SDL_Texture *> backgroundTextures;
    SDL_Rect blocksPanelRect;
    SDL_Rect blocksAreaRect;
    SDL_Rect codeAreaRect;
    SDL_Rect rightPanelRect;
    SDL_Rect catPanelRect;
    SDL_Rect spritesPanelRect;
    SDL_Rect fileMenuRect;
    bool showFileMenu;
    SDL_Rect greenFlagRect;
    SDL_Rect stopRect;
    SDL_Rect pauseRect;
    vector<Button> buttons;
    vector<Button> fileMenuButtons;
    int activeCategory;
    int blocksScrollOffset;
    int blocksMaxScroll;
    vector<Block *> codeBlocks;
    map<int, vector<Block *>> paletteBlocks;
    Block *draggingBlock;
    Block *selectedBlock;
    Block *targetBlock;
    int attachType;
    bool isDragging;
    Uint32 lastClickTime;
    Uint32 lastSpriteClickTime;
    Uint32 DOUBLE_CLICK_TIME;
    bool textInputActive;
    SDL_Rect spriteRects[10];
    bool isDebugMode;
    bool debugWaitingForStep;
    int editingParamField;
    char editParamText[64];
    SDL_Rect spriteThumbnailRects[10];
    bool showMakeBlockPopup;
    char makeBlockName[64];
    bool makeBlockEditingName;
    bool showMakeVariablePopup;
    char makeVarName[64];
    bool makeVarEditingName;
};

void showMakeBlockDialog(struct ScratchEngine *engine) {
    const int POPUP_W = 460;
    const int POPUP_H = 280;
    int popX = (engine->winWidth - POPUP_W) / 2;
    int popY = (engine->winHeight - POPUP_H) / 2;
    char blockName[64] = {0};
    bool addNumberParam = false;
    bool addTextParam = false;
    bool addBoolParam = false;
    bool editing = true;
    bool confirmed = false;
    SDL_StartTextInput();
    SDL_Event ev;
    while (editing) {
        SDL_SetRenderDrawBlendMode(engine->m_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 160);
        SDL_Rect overlay = {0, 0, engine->winWidth, engine->winHeight};
        SDL_RenderFillRect(engine->m_renderer, &overlay);
        SDL_SetRenderDrawColor(engine->m_renderer, 45, 45, 65, 255);
        SDL_Rect popup = {popX, popY, POPUP_W, POPUP_H};
        SDL_RenderFillRect(engine->m_renderer, &popup);
        SDL_SetRenderDrawColor(engine->m_renderer, 180, 100, 255, 255);
        for (int bw = 0; bw < 2; bw++) {
            SDL_Rect br = {popup.x - bw, popup.y - bw, popup.w + bw * 2, popup.h + bw * 2};
            SDL_RenderDrawRect(engine->m_renderer, &br);
        }
        drawText(engine, "Make a Block", popX + POPUP_W / 2, popY + 14, {255, 255, 255, 255}, true);
        drawText(engine, "Block name:", popX + 20, popY + 44, {200, 200, 200, 255});
        SDL_Rect inputRect = {popX + 20, popY + 62, POPUP_W - 40, 36};
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(engine->m_renderer, &inputRect);
        SDL_SetRenderDrawColor(engine->m_renderer, 180, 100, 255, 255);
        SDL_RenderDrawRect(engine->m_renderer, &inputRect);
        string nameDisplay = (strlen(blockName) > 0 ? string(blockName) : "") + "|";
        drawText(engine, nameDisplay, inputRect.x + 8, inputRect.y + 10, {30, 30, 30, 255});
        drawText(engine, "Preview:", popX + 20, popY + 108, {180, 180, 180, 255});
        {
            int bx = popX + 20, by = popY + 124;
            string label = strlen(blockName) > 0 ? string("define ") + blockName : "define my block";
            if (addNumberParam) label += " [num]";
            if (addTextParam) label += " [text]";
            if (addBoolParam) label += " <bool>";
            int bw2 = max(160, (int) label.size() * 8 + 20);
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 100, 50, 255);
            SDL_Rect previewBlock = {bx, by, bw2, 32};
            SDL_RenderFillRect(engine->m_renderer, &previewBlock);
            SDL_SetRenderDrawColor(engine->m_renderer, 220, 60, 20, 255);
            SDL_RenderDrawRect(engine->m_renderer, &previewBlock);
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 120, 70, 255);
            SDL_Rect notch = {bx + 10, by - 8, 30, 10};
            SDL_RenderFillRect(engine->m_renderer, &notch);
            drawText(engine, label, bx + bw2 / 2, by + 16, {255, 255, 255, 255}, true);
        }
        int cbY = popY + 168;
        drawText(engine, "Add parameter:", popX + 20, cbY, {190, 190, 190, 255});
        auto drawCheckbox = [&](int x, int y, bool checked, const char *label) -> SDL_Rect {
            SDL_Rect box = {x, y, 16, 16};
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(engine->m_renderer, &box);
            SDL_SetRenderDrawColor(engine->m_renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(engine->m_renderer, &box);
            if (checked) {
                SDL_SetRenderDrawColor(engine->m_renderer, 120, 60, 200, 255);
                SDL_Rect inner = {x + 3, y + 3, 10, 10};
                SDL_RenderFillRect(engine->m_renderer, &inner);
            }
            drawText(engine, label, x + 22, y, {220, 220, 220, 255});
            return box;
        };
        SDL_Rect cbNum = drawCheckbox(popX + 20, cbY + 20, addNumberParam, "Number/Text");
        SDL_Rect cbText = drawCheckbox(popX + 140, cbY + 20, addTextParam, "Label");
        SDL_Rect cbBool = drawCheckbox(popX + 250, cbY + 20, addBoolParam, "Boolean");
        SDL_Rect okBtn = {popX + 60, popY + POPUP_H - 44, 110, 32};
        SDL_Rect cancelBtn = {popX + 280, popY + POPUP_H - 44, 110, 32};
        SDL_SetRenderDrawColor(engine->m_renderer, 120, 60, 200, 255);
        SDL_RenderFillRect(engine->m_renderer, &okBtn);
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 200);
        SDL_RenderDrawRect(engine->m_renderer, &okBtn);
        drawText(engine, "OK", okBtn.x + okBtn.w / 2, okBtn.y + okBtn.h / 2, {255, 255, 255, 255}, true);
        SDL_SetRenderDrawColor(engine->m_renderer, 80, 80, 100, 255);
        SDL_RenderFillRect(engine->m_renderer, &cancelBtn);
        SDL_SetRenderDrawColor(engine->m_renderer, 200, 200, 200, 200);
        SDL_RenderDrawRect(engine->m_renderer, &cancelBtn);
        drawText(engine, "Cancel", cancelBtn.x + cancelBtn.w / 2, cancelBtn.y + cancelBtn.h / 2, {255, 255, 255, 255},
                 true);
        SDL_RenderPresent(engine->m_renderer);
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                engine->running = false;
                editing = false;
            }
            if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_ESCAPE) { editing = false; }
                else if (ev.key.keysym.sym == SDLK_RETURN) {
                    if (strlen(blockName) > 0) {
                        confirmed = true;
                        editing = false;
                    }
                }
                else if (ev.key.keysym.sym == SDLK_BACKSPACE) {
                    int len = strlen(blockName);
                    if (len > 0) blockName[len - 1] = '\0';
                }
            }
            if (ev.type == SDL_TEXTINPUT) {
                int len = strlen(blockName);
                if (len < 62) strncat(blockName, ev.text.text, 62 - len);
            }
            if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT) {
                int mx = ev.button.x, my = ev.button.y;
                auto inR = [](int x, int y, SDL_Rect r) {
                    return x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h;
                };
                if (inR(mx, my, okBtn)) {
                    if (strlen(blockName) > 0) {
                        confirmed = true;
                        editing = false;
                    }
                }
                if (inR(mx, my, cancelBtn)) { editing = false; }
                if (inR(mx, my, cbNum)) { addNumberParam = !addNumberParam; }
                if (inR(mx, my, cbText)) { addTextParam = !addTextParam; }
                if (inR(mx, my, cbBool)) { addBoolParam = !addBoolParam; }
            }
        }
        SDL_Delay(16);
    }
    SDL_StopTextInput();
    if (confirmed && strlen(blockName) > 0) {
        string funcName = string(blockName);
        string paramSuffix = "";
        if (addNumberParam) paramSuffix += " %n";
        if (addTextParam) paramSuffix += " %s";
        if (addBoolParam) paramSuffix += " %b";
        Block *defineBlock = Block_create(BLOCK_DEFINE_FUNC, 0, "define " + funcName + paramSuffix);
        defineBlock->textData = "define " + funcName + paramSuffix;
        int xOff = 20 + (int) (engine->codeBlocks.size() % 5) * 40;
        int yOff = 60 + (int) (engine->codeBlocks.size() % 8) * 50;
        defineBlock->rect.x = engine->codeAreaRect.x + xOff;
        defineBlock->rect.y = engine->codeAreaRect.y + yOff;
        defineBlock->rect.w = max(180, (int) (funcName.size() + paramSuffix.size()) * 8 + 24);
        engine->codeBlocks.push_back(defineBlock);
        engine->customFunctions[funcName] = defineBlock;
        Block *callBlock = Block_create(BLOCK_CALL_FUNC, 0, funcName + paramSuffix);
        callBlock->textData = funcName;
        callBlock->rect.w = max(160, (int) (funcName.size() + paramSuffix.size()) * 8 + 20);
        engine->paletteBlocks[BLOCK_CATEGORY_VARIABLES].push_back(callBlock);
    }
}

void showMakeVariableDialog(struct ScratchEngine *engine) {
    const int POPUP_W = 400;
    const int POPUP_H = 200;
    int popX = (engine->winWidth - POPUP_W) / 2;
    int popY = (engine->winHeight - POPUP_H) / 2;
    char varName[64] = {0};
    bool editing = true;
    bool confirmed = false;
    SDL_StartTextInput();
    SDL_Event ev;
    while (editing) {
        SDL_SetRenderDrawBlendMode(engine->m_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 160);
        SDL_Rect overlay = {0, 0, engine->winWidth, engine->winHeight};
        SDL_RenderFillRect(engine->m_renderer, &overlay);
        SDL_SetRenderDrawColor(engine->m_renderer, 50, 50, 70, 255);
        SDL_Rect popup = {popX, popY, POPUP_W, POPUP_H};
        SDL_RenderFillRect(engine->m_renderer, &popup);
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 180, 50, 255);
        SDL_RenderDrawRect(engine->m_renderer, &popup);
        drawText(engine, "New Variable", popX + POPUP_W / 2 - 45, popY + 12, {255, 255, 255, 255});
        drawText(engine, "Variable name:", popX + 20, popY + 50, {200, 200, 200, 255});
        SDL_Rect inputRect = {popX + 20, popY + 70, POPUP_W - 40, 36};
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(engine->m_renderer, &inputRect);
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 140, 0, 255);
        SDL_RenderDrawRect(engine->m_renderer, &inputRect);
        string nameDisplay = string(varName) + "_";
        drawText(engine, nameDisplay, inputRect.x + 8, inputRect.y + 10, {30, 30, 30, 255});
        SDL_Rect okBtn = {popX + 60, popY + 145, 100, 32};
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 140, 0, 255);
        SDL_RenderFillRect(engine->m_renderer, &okBtn);
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(engine->m_renderer, &okBtn);
        drawText(engine, "OK", okBtn.x + 35, okBtn.y + 8, {255, 255, 255, 255});
        SDL_Rect cancelBtn = {popX + 240, popY + 145, 100, 32};
        SDL_SetRenderDrawColor(engine->m_renderer, 100, 100, 120, 255);
        SDL_RenderFillRect(engine->m_renderer, &cancelBtn);
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(engine->m_renderer, &cancelBtn);
        drawText(engine, "Cancel", cancelBtn.x + 20, cancelBtn.y + 8, {255, 255, 255, 255});
        SDL_RenderPresent(engine->m_renderer);
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                engine->running = false;
                editing = false;
            }
            if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_ESCAPE) { editing = false; }
                else if (ev.key.keysym.sym == SDLK_RETURN) {
                    confirmed = true;
                    editing = false;
                }
                else if (ev.key.keysym.sym == SDLK_BACKSPACE) {
                    int len = strlen(varName);
                    if (len > 0) varName[len - 1] = '\0';
                }
            }
            if (ev.type == SDL_TEXTINPUT) {
                int len = strlen(varName);
                if (len < 63) { strncat(varName, ev.text.text, 63 - len); }
            }
            if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT) {
                int mx = ev.button.x, my = ev.button.y;
                auto inRect = [](int x, int y, SDL_Rect r) {
                    return x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h;
                };
                if (inRect(mx, my, okBtn)) {
                    confirmed = true;
                    editing = false;
                }
                if (inRect(mx, my, cancelBtn)) { editing = false; }
            }
        }
        SDL_Delay(16);
    }
    SDL_StopTextInput();
    if (confirmed && strlen(varName) > 0) {
        string vn = string(varName);
        engine->globalVariables[vn] = 0;
        if (!engine->sprites.empty()) {
            engine->sprites[engine->activeSpriteIndex].variables[vn] = 0;
            engine->sprites[engine->activeSpriteIndex].variableVisible[vn] = true;
        }
        Block *setBlock = Block_create(BLOCK_SET_VARIABLE, 0, "set " + vn + " to");
        setBlock->hasInput = true;
        engine->paletteBlocks[BLOCK_CATEGORY_VARIABLES].push_back(setBlock);
        Block *chgBlock = Block_create(BLOCK_CHANGE_VARIABLE, 1, "change " + vn + " by");
        chgBlock->hasInput = true;
        engine->paletteBlocks[BLOCK_CATEGORY_VARIABLES].push_back(chgBlock);
        Block *showBlock = Block_create(BLOCK_SHOW_VARIABLE, 0, "show variable " + vn);
        engine->paletteBlocks[BLOCK_CATEGORY_VARIABLES].push_back(showBlock);
        Block *hideBlock = Block_create(BLOCK_HIDE_VARIABLE, 0, "hide variable " + vn);
        engine->paletteBlocks[BLOCK_CATEGORY_VARIABLES].push_back(hideBlock);
    }
}

void ScratchEngine_init(struct ScratchEngine *engine) {
    engine->winWidth = 1200;
    engine->winHeight = 800;
    engine->running = true;
    engine->activeSpriteIndex = 0;
    engine->MAX_LOG_LINES = 10;
    engine->draggingBlock = nullptr;
    engine->selectedBlock = nullptr;
    engine->targetBlock = nullptr;
    engine->attachType = ATTACH_NONE;
    engine->isDragging = false;
    engine->lastClickTime = 0;
    engine->lastSpriteClickTime = 0;
    engine->DOUBLE_CLICK_TIME = 300;
    engine->meowSound = nullptr;
    engine->popSound = nullptr;
    engine->showFileMenu = false;
    engine->blocksScrollOffset = 0;
    engine->isDebugMode = false;
    engine->debugWaitingForStep = false;
    engine->editingParamField = 0;
    memset(engine->editParamText, 0, sizeof(engine->editParamText));
    engine->showMakeBlockPopup = false;
    memset(engine->makeBlockName, 0, sizeof(engine->makeBlockName));
    engine->makeBlockEditingName = false;
    engine->showMakeVariablePopup = false;
    memset(engine->makeVarName, 0, sizeof(engine->makeVarName));
    engine->makeVarEditingName = false;
    engine->currentBackgroundName = "Default";
    engine->currentBackgroundIndex = 0;
    engine->textInputActive = false;
    engine->activeCategory = BLOCK_CATEGORY_MOTION;
    engine->backgroundColor = {255, 255, 255, 255};
    engine->backgroundTexture = nullptr;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    engine->m_window = SDL_CreateWindow("Sharif Scratch Clone",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        engine->winWidth, engine->winHeight,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    engine->m_renderer = SDL_CreateRenderer(engine->m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(engine->m_renderer, SDL_BLENDMODE_BLEND);
    engine->m_font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 12);
    if (!engine->m_font) {
        engine->m_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 12);
    }
    engine->meowSound = Mix_LoadWAV("files/meow_sound.mp3");
    engine->popSound = Mix_LoadWAV("files/pop.wav");
    if (!engine->meowSound) { cout << "Warning: Failed to load meow_sound.mp3" << endl; }
    if (!engine->popSound) { cout << "Warning: Failed to load pop.wav" << endl; }
    for (int i = 1; i <= 4; i++) {
        string path = "files/bg" + to_string(i) + ".jpg";
        SDL_Surface *surf = IMG_Load(path.c_str());
        if (surf) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(engine->m_renderer, surf);
            SDL_FreeSurface(surf);
            engine->backgroundTextures.push_back(tex);
        } else {
            cout << "Warning: Failed to load " << path << endl;
            engine->backgroundTextures.push_back(nullptr);
        }
    }
    SDL_GetWindowSize(engine->m_window, &engine->winWidth, &engine->winHeight);
    int blocksPanelWidth = 280;
    int rightPanelWidth = 450;
    int codeAreaWidth = engine->winWidth - blocksPanelWidth - rightPanelWidth;
    engine->blocksPanelRect = {0, 0, blocksPanelWidth, engine->winHeight};
    engine->rightPanelRect = {blocksPanelWidth + codeAreaWidth, 0, rightPanelWidth, engine->winHeight};
    engine->codeAreaRect = {blocksPanelWidth, 0, codeAreaWidth, engine->winHeight};
    int catPanelHeight = 400;
    int spritesPanelHeight = engine->winHeight - catPanelHeight;
    engine->catPanelRect = {engine->rightPanelRect.x, 0, engine->rightPanelRect.w, catPanelHeight};
    engine->spritesPanelRect = {engine->rightPanelRect.x, catPanelHeight, engine->rightPanelRect.w, spritesPanelHeight};
    engine->fileMenuRect = {engine->blocksPanelRect.x + 10, engine->blocksPanelRect.y + 5, 100, 30};
    engine->greenFlagRect = {engine->catPanelRect.x + 10, engine->catPanelRect.y + 10, 40, 40};
    engine->stopRect = {engine->catPanelRect.x + 60, engine->catPanelRect.y + 10, 40, 40};
    engine->pauseRect = {engine->catPanelRect.x + 110, engine->catPanelRect.y + 10, 40, 40};
    engine->blocksAreaRect = {engine->blocksPanelRect.x + 45, engine->blocksPanelRect.y + 45,
                              blocksPanelWidth - 55, engine->winHeight - 200};
    Button newBtn;
    newBtn.rect = {engine->fileMenuRect.x, engine->fileMenuRect.y + 35, 150, 30};
    newBtn.label = "New Project";
    newBtn.color = {70, 70, 70, 255};
    newBtn.textColor = {255, 255, 255, 255};
    newBtn.actionID = 201;
    engine->fileMenuButtons.push_back(newBtn);
    Button saveBtn;
    saveBtn.rect = {engine->fileMenuRect.x, engine->fileMenuRect.y + 70, 150, 30};
    saveBtn.label = "Save";
    saveBtn.color = {70, 70, 70, 255};
    saveBtn.textColor = {255, 255, 255, 255};
    saveBtn.actionID = 202;
    engine->fileMenuButtons.push_back(saveBtn);
    Button loadBtn;
    loadBtn.rect = {engine->fileMenuRect.x, engine->fileMenuRect.y + 105, 150, 30};
    loadBtn.label = "Load";
    loadBtn.color = {70, 70, 70, 255};
    loadBtn.textColor = {255, 255, 255, 255};
    loadBtn.actionID = 203;
    engine->fileMenuButtons.push_back(loadBtn);
    Button addSpriteBtn;
    addSpriteBtn.rect = {engine->spritesPanelRect.x + 10, engine->spritesPanelRect.y + engine->spritesPanelRect.h - 40,
                         180, 30};
    addSpriteBtn.label = "Add Sprite";
    addSpriteBtn.color = {100, 200, 100, 255};
    addSpriteBtn.textColor = {255, 255, 255, 255};
    addSpriteBtn.actionID = 101;
    engine->buttons.push_back(addSpriteBtn);
    Button delSpriteBtn;
    delSpriteBtn.rect = {engine->spritesPanelRect.x + 10, engine->spritesPanelRect.y + engine->spritesPanelRect.h - 80,
                         180, 30};
    delSpriteBtn.label = "Delete Sprite";
    delSpriteBtn.color = {200, 100, 100, 255};
    delSpriteBtn.textColor = {255, 255, 255, 255};
    delSpriteBtn.actionID = 102;
    engine->buttons.push_back(delSpriteBtn);
    Button nextSpriteBtn;
    nextSpriteBtn.rect = {engine->spritesPanelRect.x + 10,
                          engine->spritesPanelRect.y + engine->spritesPanelRect.h - 120, 180, 30};
    nextSpriteBtn.label = "Next Sprite";
    nextSpriteBtn.color = {100, 100, 200, 255};
    nextSpriteBtn.textColor = {255, 255, 255, 255};
    nextSpriteBtn.actionID = 103;
    engine->buttons.push_back(nextSpriteBtn);
    Button bgBtn;
    bgBtn.rect = {engine->spritesPanelRect.x + 10, engine->spritesPanelRect.y + engine->spritesPanelRect.h - 160, 180,
                  30};
    bgBtn.label = "Change Background";
    bgBtn.color = {60, 140, 180, 255};
    bgBtn.textColor = {255, 255, 255, 255};
    bgBtn.actionID = 104;
    engine->buttons.push_back(bgBtn);
    int buttonSize = 25;
    int startX = engine->blocksPanelRect.x + 15;
    int startY = engine->blocksPanelRect.y + 45;
    int spacing = 65;
    for (size_t i = 0; i < categories.size(); i++) {
        categories[i].buttonRect = {
                startX,
                startY + (int) i * spacing,
                buttonSize,
                buttonSize
        };
    }
    engine->penLayer = SDL_CreateTexture(engine->m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 2000,
                                         2000);
    SDL_SetTextureBlendMode(engine->penLayer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(engine->m_renderer, engine->penLayer);
    SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 0);
    SDL_RenderClear(engine->m_renderer);
    SDL_SetRenderTarget(engine->m_renderer, NULL);
    SensingData_init(&engine->sensing);
    engine->sensing.startTime = SDL_GetTicks();
    struct Sprite cat;
    Sprite_init(&cat, "Cat", engine->catPanelRect.w / 2, engine->catPanelRect.h / 2);
    cat.size = 40;
    cat.costumes.push_back("cat1");
    cat.costumes.push_back("cat2");
    engine->sprites.push_back(cat);
    if (engine->sprites.size() > 0) {
        SDL_Surface *surface = IMG_Load("files/cat.png");
        if (surface) {
            if (engine->sprites[0].costume) { SDL_DestroyTexture(engine->sprites[0].costume); }
            engine->sprites[0].costume = SDL_CreateTextureFromSurface(engine->m_renderer, surface);
            SDL_SetTextureBlendMode(engine->sprites[0].costume, SDL_BLENDMODE_BLEND);
            SDL_FreeSurface(surface);
            engine->sprites[0].currentCostume = "files/cat.jpg";
        }
    }
    engine->backgrounds.push_back("Default");
    engine->backgrounds.push_back("Blue Sky");
    engine->backgrounds.push_back("Forest");
    vector<Block *> motionBlocks;
    motionBlocks.push_back(Block_create(BLOCK_MOVE_STEPS, 10, "move ... steps"));
    motionBlocks.push_back(Block_create(BLOCK_TURN_RIGHT, 15, "turn right "));
    motionBlocks.push_back(Block_create(BLOCK_TURN_LEFT, 15, "turn left "));
    motionBlocks.push_back(Block_create(BLOCK_GOTO_MOUSE, 0, "go to mouse"));
    motionBlocks.push_back(Block_create(BLOCK_GOTO_X_Y, 0, "go to x: y:"));
    motionBlocks.push_back(Block_create(BLOCK_CHANGE_X, 10, "change x by "));
    motionBlocks.push_back(Block_create(BLOCK_CHANGE_Y, 10, "change y by "));
    motionBlocks.push_back(Block_create(BLOCK_SET_X, 0, "set x to "));
    motionBlocks.push_back(Block_create(BLOCK_SET_Y, 0, "set y to "));
    motionBlocks.push_back(Block_create(BLOCK_SET_ANGLE, 90, "point in direction 90"));
    motionBlocks.push_back(Block_create(BLOCK_IF_ON_EDGE_BOUNCE, 0, "if on edge, bounce"));
    engine->paletteBlocks[BLOCK_CATEGORY_MOTION] = motionBlocks;
    vector<Block *> looksBlocks;
    looksBlocks.push_back(Block_create(BLOCK_SAY, 0, "say Hello"));
    looksBlocks.push_back(Block_create(BLOCK_SAY_FOR_SECONDS, 2, "say Hello for 2 secs"));
    looksBlocks.push_back(Block_create(BLOCK_THINK, 0, "think Hmm..."));
    looksBlocks.push_back(Block_create(BLOCK_THINK_FOR_SECONDS, 2, "think for 2 secs"));
    looksBlocks.push_back(Block_create(BLOCK_SHOW, 0, "show"));
    looksBlocks.push_back(Block_create(BLOCK_HIDE, 0, "hide"));
    looksBlocks.push_back(Block_create(BLOCK_SET_SIZE, 100, "set size to ...%"));
    looksBlocks.push_back(Block_create(BLOCK_CHANGE_SIZE, 10, "change size by "));
    looksBlocks.push_back(Block_create(BLOCK_SET_COSTUME, 0, "switch costume to"));
    looksBlocks.push_back(Block_create(BLOCK_NEXT_COSTUME, 0, "next costume"));
    looksBlocks.push_back(Block_create(BLOCK_SET_BACKDROP, 0, "switch backdrop to"));
    looksBlocks.push_back(Block_create(BLOCK_NEXT_BACKDROP, 0, "next backdrop"));
    looksBlocks.push_back(Block_create(BLOCK_FLIP_HORIZONTAL, 0, "flip horizontal"));
    looksBlocks.push_back(Block_create(BLOCK_FLIP_VERTICAL, 0, "flip vertical"));
    looksBlocks.push_back(Block_create(BLOCK_GO_TO_FRONT, 0, "go to front"));
    looksBlocks.push_back(Block_create(BLOCK_GO_BACK_LAYERS, 1, "go back 1 layer"));
    engine->paletteBlocks[BLOCK_CATEGORY_LOOKS] = looksBlocks;
    vector<Block *> soundBlocks;
    soundBlocks.push_back(Block_create(BLOCK_PLAY_SOUND, 0, "play sound pop"));
    soundBlocks.push_back(Block_create(BLOCK_PLAY_SOUND_MEOW, 0, "play sound meow"));
    soundBlocks.push_back(Block_create(BLOCK_PLAY_SOUND_UNTIL_DONE, 0, "play sound until done"));
    soundBlocks.push_back(Block_create(BLOCK_STOP_ALL_SOUNDS, 0, "stop all sounds"));
    soundBlocks.push_back(Block_create(BLOCK_CHANGE_VOLUME, 10, "change volume by "));
    soundBlocks.push_back(Block_create(BLOCK_SET_VOLUME, 50, "set volume to ...%"));
    engine->paletteBlocks[BLOCK_CATEGORY_SOUND] = soundBlocks;
    vector<Block *> eventsBlocks;
    eventsBlocks.push_back(Block_create(BLOCK_WHEN_GREEN_FLAG, 0, "when flag clicked"));
    eventsBlocks.push_back(Block_create(BLOCK_WHEN_KEY_PRESSED, 0, "when space pressed"));
    eventsBlocks.push_back(Block_create(BLOCK_WHEN_SPRITE_CLICKED, 0, "when this sprite clicked"));
    eventsBlocks.push_back(Block_create(BLOCK_BROADCAST, 0, "broadcast message"));
    eventsBlocks.push_back(Block_create(BLOCK_BROADCAST_AND_WAIT, 0, "broadcast and wait"));
    engine->paletteBlocks[BLOCK_CATEGORY_EVENTS] = eventsBlocks;
    vector<Block *> controlBlocks;
    controlBlocks.push_back(Block_create(BLOCK_REPEAT, 4, "repeat 4"));
    controlBlocks.push_back(Block_create(BLOCK_FOREVER, 0, "forever"));
    controlBlocks.push_back(Block_create(BLOCK_WAIT, 1, "wait 1 sec"));
    controlBlocks.push_back(Block_create(BLOCK_WAIT_UNTIL, 0, "wait until"));
    controlBlocks.push_back(Block_create(BLOCK_IF_THEN, 0, "if then"));
    controlBlocks.push_back(Block_create(BLOCK_IF_THEN_ELSE, 0, "if then else"));
    controlBlocks.push_back(Block_create(BLOCK_STOP_ALL, 0, "stop all"));
    controlBlocks.push_back(Block_create(BLOCK_STOP_THIS_SCRIPT, 0, "stop this script"));
    engine->paletteBlocks[BLOCK_CATEGORY_CONTROL] = controlBlocks;
    vector<Block *> sensingBlocks;
    sensingBlocks.push_back(Block_create(BLOCK_TOUCHING_MOUSE, 0, "touching mouse?"));
    sensingBlocks.push_back(Block_create(BLOCK_TOUCHING_EDGE, 0, "touching edge?"));
    sensingBlocks.push_back(Block_create(BLOCK_KEY_PRESSED, 0, "key space pressed?"));
    sensingBlocks.push_back(Block_create(BLOCK_MOUSE_X, 0, "mouse x"));
    sensingBlocks.push_back(Block_create(BLOCK_MOUSE_Y, 0, "mouse y"));
    sensingBlocks.push_back(Block_create(BLOCK_MOUSE_DOWN, 0, "mouse down?"));
    sensingBlocks.push_back(Block_create(BLOCK_TIMER, 0, "timer"));
    sensingBlocks.push_back(Block_create(BLOCK_RESET_TIMER, 0, "reset timer"));
    sensingBlocks.push_back(Block_create(BLOCK_DISTANCE_TO, 0, "distance to"));
    sensingBlocks.push_back(Block_create(BLOCK_ASK_AND_WAIT, 0, "ask and wait"));
    sensingBlocks.push_back(Block_create(BLOCK_ANSWER, 0, "answer"));
    sensingBlocks.push_back(Block_create(BLOCK_DRAG_MODE, 1, "set drag mode On"));
    sensingBlocks.push_back(Block_create(BLOCK_DRAG_MODE, 0, "set drag mode Off"));
    engine->paletteBlocks[BLOCK_CATEGORY_SENSING] = sensingBlocks;
    vector<Block *> operatorsBlocks;
    operatorsBlocks.push_back(Block_create(BLOCK_ADD, 0, "+"));
    operatorsBlocks.push_back(Block_create(BLOCK_SUBTRACT, 0, "-"));
    operatorsBlocks.push_back(Block_create(BLOCK_MULTIPLY, 0, "*"));
    operatorsBlocks.push_back(Block_create(BLOCK_DIVIDE, 0, "/"));
    operatorsBlocks.push_back(Block_create(BLOCK_RANDOM, 0, "random"));
    operatorsBlocks.push_back(Block_create(BLOCK_LESS_THAN, 0, "<"));
    operatorsBlocks.push_back(Block_create(BLOCK_EQUAL, 0, "="));
    operatorsBlocks.push_back(Block_create(BLOCK_GREATER_THAN, 0, ">"));
    operatorsBlocks.push_back(Block_create(BLOCK_AND, 0, "and"));
    operatorsBlocks.push_back(Block_create(BLOCK_OR, 0, "or"));
    operatorsBlocks.push_back(Block_create(BLOCK_NOT, 0, "not"));
    operatorsBlocks.push_back(Block_create(BLOCK_JOIN_STRINGS, 0, "join"));
    operatorsBlocks.push_back(Block_create(BLOCK_LENGTH_OF, 0, "length of"));
    operatorsBlocks.push_back(Block_create(BLOCK_MOD, 0, "mod"));
    operatorsBlocks.push_back(Block_create(BLOCK_ROUND, 0, "round"));
    operatorsBlocks.push_back(Block_create(BLOCK_ABS, 0, "abs"));
    operatorsBlocks.push_back(Block_create(BLOCK_SQRT, 0, "sqrt"));
    operatorsBlocks.push_back(Block_create(BLOCK_SIN, 0, "sin"));
    operatorsBlocks.push_back(Block_create(BLOCK_COS, 0, "cos"));
    engine->paletteBlocks[BLOCK_CATEGORY_OPERATORS] = operatorsBlocks;
    vector<Block *> variablesBlocks;
    variablesBlocks.push_back(Block_create(BLOCK_SET_VARIABLE, 0, "set my variable to"));
    variablesBlocks.back()->hasInput = true;
    variablesBlocks.push_back(Block_create(BLOCK_CHANGE_VARIABLE, 0, "change my variable by"));
    variablesBlocks.back()->hasInput = true;
    variablesBlocks.push_back(Block_create(BLOCK_SHOW_VARIABLE, 0, "show variable"));
    variablesBlocks.push_back(Block_create(BLOCK_HIDE_VARIABLE, 0, "hide variable"));
    engine->paletteBlocks[BLOCK_CATEGORY_VARIABLES] = variablesBlocks;
    vector<Block *> penBlocks;
    penBlocks.push_back(Block_create(BLOCK_PEN_DOWN, 0, "pen down"));
    penBlocks.push_back(Block_create(BLOCK_PEN_UP, 0, "pen up"));
    penBlocks.push_back(Block_create(BLOCK_ERASE_ALL, 0, "erase all"));
    penBlocks.push_back(Block_create(BLOCK_SET_PEN_COLOR, 0, "set pen color"));
    penBlocks.push_back(Block_create(BLOCK_CHANGE_PEN_COLOR, 10, "change pen color by 10"));
    penBlocks.push_back(Block_create(BLOCK_SET_PEN_SIZE, 1, "set pen size to 1"));
    penBlocks.push_back(Block_create(BLOCK_CHANGE_PEN_SIZE, 1, "change pen size by 1"));
    penBlocks.push_back(Block_create(BLOCK_SET_BRIGHTNESS, 50, "set brightness to 50"));
    penBlocks.push_back(Block_create(BLOCK_CHANGE_BRIGHTNESS, 10, "change brightness by 10"));
    penBlocks.push_back(Block_create(BLOCK_SET_SATURATION, 50, "set saturation to 50"));
    penBlocks.push_back(Block_create(BLOCK_CHANGE_SATURATION, 10, "change saturation by 10"));
    penBlocks.push_back(Block_create(BLOCK_STAMP, 0, "stamp"));
    engine->paletteBlocks[BLOCK_CATEGORY_PEN] = penBlocks;
    engine->blocksMaxScroll = 0;
    if (engine->paletteBlocks.find(engine->activeCategory) != engine->paletteBlocks.end()) {
        int totalHeight = 0;
        for (auto block: engine->paletteBlocks[engine->activeCategory]) { totalHeight += block->rect.h + 5; }
        engine->blocksMaxScroll = max(0, totalHeight - engine->blocksAreaRect.h + 20);
    }
    for (int i = 0; i < 10 && i < engine->sprites.size(); i++) {
        engine->spriteRects[i] = {engine->spritesPanelRect.x + 10, engine->spritesPanelRect.y + 30 + i * 20, 200, 20};
    }
    engine->globalVariables["my variable"] = 0;
}

void ScratchEngine_destroy(struct ScratchEngine *engine) {
    for (auto &pair: engine->paletteBlocks) {
        for (auto block: pair.second)
            Block_destroy(block);
    }
    for (auto block: engine->codeBlocks) {
        Block_destroy(block);
    }
    for (auto &s: engine->sprites) {
        Sprite_destroy(&s);
    }
    if (engine->meowSound)
        Mix_FreeChunk(engine->meowSound);
    if (engine->popSound)
        Mix_FreeChunk(engine->popSound);
    if (engine->m_font)
        TTF_CloseFont(engine->m_font);
    for (auto tex: engine->backgroundTextures) {
        if (tex)
            SDL_DestroyTexture(tex);
    }
    SDL_DestroyTexture(engine->penLayer);
    SDL_DestroyRenderer(engine->m_renderer);
    SDL_DestroyWindow(engine->m_window);
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}



void renderFileMenu(struct ScratchEngine *engine) {
    SDL_SetRenderDrawColor(engine->m_renderer, 80, 80, 90, 255);
    SDL_RenderFillRect(engine->m_renderer, &engine->fileMenuRect);
    SDL_SetRenderDrawColor(engine->m_renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(engine->m_renderer, &engine->fileMenuRect);
    {
        int ix = engine->fileMenuRect.x + 5;
        int iy = engine->fileMenuRect.y + 7;
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 210, 80, 255);
        SDL_Rect folderTab = {ix, iy, 8, 5};
        SDL_RenderFillRect(engine->m_renderer, &folderTab);
        SDL_Rect folderBody = {ix, iy + 4, 16, 12};
        SDL_RenderFillRect(engine->m_renderer, &folderBody);
        SDL_SetRenderDrawColor(engine->m_renderer, 220, 170, 40, 255);
        SDL_RenderDrawRect(engine->m_renderer, &folderBody);
    }
    drawText(engine, "File", engine->fileMenuRect.x + 24, engine->fileMenuRect.y + 5, {255, 255, 255, 255});
    if (engine->showFileMenu) {
        SDL_RenderSetClipRect(engine->m_renderer, NULL);
        for (auto &btn: engine->fileMenuButtons) {
            SDL_SetRenderDrawColor(engine->m_renderer, btn.color.r, btn.color.g, btn.color.b, btn.color.a);
            SDL_RenderFillRect(engine->m_renderer, &btn.rect);
            SDL_SetRenderDrawColor(engine->m_renderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(engine->m_renderer, &btn.rect);
            drawText(engine, btn.label, btn.rect.x + 10, btn.rect.y + 5, btn.textColor);
        }
    }
}

int main() {

    return 0;
}

