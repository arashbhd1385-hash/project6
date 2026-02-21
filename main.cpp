void drawText(struct ScratchEngine *engine, const string &text, int x, int y, SDL_Color color, bool center) {
    if (!engine->m_font) return;
    SDL_Surface *surface = TTF_RenderUTF8_Blended(engine->m_font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(engine->m_renderer, surface);
    SDL_Rect dest;
    if (center) {
        dest = {x - surface->w / 2, y - surface->h / 2, surface->w, surface->h};
    } else {
        dest = {x, y, surface->w, surface->h};
    }
    SDL_RenderCopy(engine->m_renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void drawRoundedRect(struct ScratchEngine *engine, SDL_Rect rect, SDL_Color color, int radius) {
    SDL_SetRenderDrawColor(engine->m_renderer, color.r, color.g, color.b, color.a);
    SDL_Rect mainRect = {
            rect.x + radius,
            rect.y,
            rect.w - 2 * radius,
            rect.h
    };
    SDL_RenderFillRect(engine->m_renderer, &mainRect);
    SDL_Rect leftRect = {
            rect.x,
            rect.y + radius,
            radius,
            rect.h - 2 * radius
    };
    SDL_RenderFillRect(engine->m_renderer, &leftRect);
    SDL_Rect rightRect = {
            rect.x + rect.w - radius,
            rect.y + radius,
            radius,
            rect.h - 2 * radius
    };
    SDL_RenderFillRect(engine->m_renderer, &rightRect);
    auto drawCirclePoints = [&](int cx, int cy, int r) {
        for (int dy = -r; dy <= r; dy++) {
            for (int dx = -r; dx <= r; dx++) {
                if (dx * dx + dy * dy <= r * r) { SDL_RenderDrawPoint(engine->m_renderer, cx + dx, cy + dy); }
            }
        }
    };
    drawCirclePoints(rect.x + radius, rect.y + radius, radius);
    drawCirclePoints(rect.x + rect.w - radius, rect.y + radius, radius);
    drawCirclePoints(rect.x + radius, rect.y + rect.h - radius, radius);
    drawCirclePoints(rect.x + rect.w - radius, rect.y + rect.h - radius, radius);
}

void drawCircle(struct ScratchEngine *engine, SDL_Rect rect, SDL_Color color) {
    SDL_SetRenderDrawColor(engine->m_renderer, color.r, color.g, color.b, color.a);
    int radius = rect.w / 2;
    int cx = rect.x + radius;
    int cy = rect.y + radius;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) { SDL_RenderDrawPoint(engine->m_renderer, cx + dx, cy + dy); }
        }
    }
}

void drawBlock(struct ScratchEngine *engine, Block *block, int offsetX = 0, int offsetY = 0) {
    SDL_Color blockColor = getBlockColor(block->type);
    if (block->isSelected) {
        blockColor.r = min(255, blockColor.r + 50);
        blockColor.g = min(255, blockColor.g + 50);
        blockColor.b = min(255, blockColor.b + 50);
    }
    SDL_Rect renderRect = {block->rect.x + offsetX, block->rect.y + offsetY,
                           block->rect.w, block->rect.h};
    drawRoundedRect(engine, renderRect, blockColor, 8);
    if (block->isActive) {
        SDL_Color darkBorder = {(Uint8) max(0, (int) blockColor.r - 70),
                                (Uint8) max(0, (int) blockColor.g - 70),
                                (Uint8) max(0, (int) blockColor.b - 70), 255};
        SDL_SetRenderDrawColor(engine->m_renderer, darkBorder.r, darkBorder.g, darkBorder.b, 255);
        for (int bw = 0; bw < 3; bw++) {
            SDL_Rect bRect = {renderRect.x - bw, renderRect.y - bw, renderRect.w + bw * 2, renderRect.h + bw * 2};
            SDL_RenderDrawRect(engine->m_renderer, &bRect);
        }
    } else {
        SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(engine->m_renderer, &renderRect);
    }
    if (block->isLoop) {
        SDL_Color loopColor = blockColor;
        SDL_Rect headerRect = {renderRect.x, renderRect.y, renderRect.w, 40};
        drawRoundedRect(engine, headerRect, loopColor, 8);
        SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 180);
        SDL_RenderDrawRect(engine->m_renderer, &headerRect);
        int bodyH = 50;
        int startChildIdx = 0;
        if (block->type == BLOCK_IF_THEN || block->type == BLOCK_IF_THEN_ELSE) { startChildIdx = 1; }
        int bodyChildCount = 0;
        for (int i = startChildIdx; i < (int) block->children.size(); i++) {
            bodyH += block->children[i]->rect.h + 4;
            bodyChildCount++;
        }
        bodyH = max(bodyH, 50);
        SDL_Rect bodyRect = {renderRect.x + 20, renderRect.y + 40, renderRect.w - 20, bodyH};
        SDL_SetRenderDrawColor(engine->m_renderer, loopColor.r, loopColor.g, loopColor.b, 60);
        SDL_RenderFillRect(engine->m_renderer, &bodyRect);
        SDL_SetRenderDrawColor(engine->m_renderer, loopColor.r, loopColor.g, loopColor.b, 200);
        SDL_RenderDrawLine(engine->m_renderer,
                           renderRect.x, renderRect.y + 40,
                           renderRect.x, renderRect.y + 40 + bodyH);
        SDL_Rect bottomRect = {renderRect.x, renderRect.y + 40 + bodyH, renderRect.w, 14};
        drawRoundedRect(engine, bottomRect, loopColor, 5);
        SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 180);
        SDL_RenderDrawRect(engine->m_renderer, &bottomRect);
        block->loopBodyRect = {renderRect.x + 10, renderRect.y + 40, renderRect.w - 15, bodyH};
        if (block->type == BLOCK_REPEAT) {
            drawText(engine, "repeat", renderRect.x + 8, renderRect.y + 20, {255, 255, 255, 255}, false);
            SDL_Rect numBox = {renderRect.x + 70, renderRect.y + 7, 46, 26};
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(engine->m_renderer, &numBox);
            SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(engine->m_renderer, &numBox);
            if (block->isEditingInput) {
                SDL_SetRenderDrawColor(engine->m_renderer, 255, 220, 0, 255);
                SDL_RenderDrawRect(engine->m_renderer, &numBox);
            }
            block->inputRect = numBox;
            drawText(engine, block->inputText,
                     numBox.x + numBox.w / 2, numBox.y + numBox.h / 2, {0, 0, 0, 255}, true);
            drawText(engine, "times", renderRect.x + 122, renderRect.y + 20, {255, 255, 255, 255}, false);
        } else if (block->type == BLOCK_FOREVER) {
            drawText(engine, "forever", renderRect.x + renderRect.w / 2,
                     renderRect.y + 20, {255, 255, 255, 255}, true);
        } else if (block->type == BLOCK_IF_THEN) {
            drawText(engine, "if", renderRect.x + 8, renderRect.y + 20, {255, 255, 255, 255}, false);
            SDL_Rect condBox = {renderRect.x + 28, renderRect.y + 7, 100, 26};
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 80);
            SDL_RenderFillRect(engine->m_renderer, &condBox);
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 200);
            SDL_RenderDrawRect(engine->m_renderer, &condBox);
            if (!block->children.empty() && block->children[0] != nullptr) {
                Block *cond = block->children[0];
                string condTxt = cond->textData;
                drawText(engine, condTxt, condBox.x + condBox.w / 2, condBox.y + condBox.h / 2,
                         {0, 0, 0, 255}, true);
            } else {
                drawText(engine, "condition", condBox.x + condBox.w / 2, condBox.y + condBox.h / 2,
                         {180, 180, 180, 255}, true);
            }
            drawText(engine, "then", renderRect.x + 136, renderRect.y + 20, {255, 255, 255, 255}, false);
        } else if (block->type == BLOCK_IF_THEN_ELSE) {
            drawText(engine, "if", renderRect.x + 8, renderRect.y + 20, {255, 255, 255, 255}, false);
            SDL_Rect condBox = {renderRect.x + 28, renderRect.y + 7, 100, 26};
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 80);
            SDL_RenderFillRect(engine->m_renderer, &condBox);
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 200);
            SDL_RenderDrawRect(engine->m_renderer, &condBox);
            if (!block->children.empty()) {
                drawText(engine, block->children[0]->textData,
                         condBox.x + condBox.w / 2, condBox.y + condBox.h / 2, {0, 0, 0, 255}, true);
            }
            drawText(engine, "then", renderRect.x + 136, renderRect.y + 20, {255, 255, 255, 255}, false);
        }
        int childY = renderRect.y + 45;
        for (int i = startChildIdx; i < (int) block->children.size(); i++) {
            Block *child = block->children[i];
            child->rect.x = renderRect.x + 22;
            child->rect.y = childY;
            drawBlock(engine, child, offsetX, 0);
            childY += child->rect.h + 4;
        }
        if (bodyChildCount == 0) {
            drawText(engine, "drop here",
                     bodyRect.x + bodyRect.w / 2, bodyRect.y + bodyRect.h / 2,
                     {255, 255, 255, 100}, true);
        }
        return;
    } else if (block->isOperator) {
        int inputWidth = 60;
        int spacing = 10;
        int startX = renderRect.x + 10;
        if (block->isBinaryOperator) {
            Block *input1 = block->operatorInputs[0];
            input1->rect.x = startX;
            input1->rect.y = renderRect.y + 5;
            input1->rect.w = inputWidth;
            input1->rect.h = 30;
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(engine->m_renderer, &input1->rect);
            SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(engine->m_renderer, &input1->rect);
            drawText(engine, input1->inputText,
                     input1->rect.x + input1->rect.w / 2,
                     input1->rect.y + input1->rect.h / 2,
                     {0, 0, 0, 255}, true);
            int midX = renderRect.x + renderRect.w / 2;
            drawText(engine, block->textData, midX, renderRect.y + renderRect.h / 2,
                     {255, 255, 255, 255}, true);
            Block *input2 = block->operatorInputs[1];
            input2->rect.x = renderRect.x + renderRect.w - inputWidth - 10;
            input2->rect.y = renderRect.y + 5;
            input2->rect.w = inputWidth;
            input2->rect.h = 30;
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(engine->m_renderer, &input2->rect);
            SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(engine->m_renderer, &input2->rect);
            drawText(engine, input2->inputText,
                     input2->rect.x + input2->rect.w / 2,
                     input2->rect.y + input2->rect.h / 2,
                     {0, 0, 0, 255}, true);
        } else {
            Block *input = block->operatorInputs[0];
            input->rect.x = startX;
            input->rect.y = renderRect.y + 5;
            input->rect.w = inputWidth;
            input->rect.h = 30;
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(engine->m_renderer, &input->rect);
            SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(engine->m_renderer, &input->rect);
            drawText(engine, input->inputText,
                     input->rect.x + input->rect.w / 2,
                     input->rect.y + input->rect.h / 2,
                     {0, 0, 0, 255}, true);
            drawText(engine, block->textData, renderRect.x + renderRect.w / 2,
                     renderRect.y + renderRect.h / 2, {255, 255, 255, 255}, true);
        }
    } else if (block->hasPenColorPicker) {
        drawText(engine, "set pen color", renderRect.x + 8,
                 renderRect.y + renderRect.h / 2, {255, 255, 255, 255}, false);
        block->colorPickerRect.x = block->rect.x + block->rect.w - 28;
        block->colorPickerRect.y = block->rect.y + 8;
        block->colorPickerRect.w = 22;
        block->colorPickerRect.h = 22;
        if (!engine->sprites.empty()) {
            SDL_Color pc = engine->sprites[engine->activeSpriteIndex].penColor;
            SDL_SetRenderDrawColor(engine->m_renderer, pc.r, pc.g, pc.b, 255);
        } else {
            SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
        }
        SDL_RenderFillRect(engine->m_renderer, &block->colorPickerRect);
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 200);
        SDL_RenderDrawRect(engine->m_renderer, &block->colorPickerRect);
    } else if (block->hasSecondInput) {
        drawText(engine, "go to x:", renderRect.x + 4,
                 renderRect.y + renderRect.h / 2, {255, 255, 255, 255}, false);
        block->inputRect.x = block->rect.x + 60;
        block->inputRect.y = block->rect.y + 7;
        block->inputRect.w = 40;
        block->inputRect.h = 26;
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(engine->m_renderer, &block->inputRect);
        SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(engine->m_renderer, &block->inputRect);
        if (block->isEditingInput) {
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(engine->m_renderer, &block->inputRect);
        }
        drawText(engine, block->inputText,
                 block->inputRect.x + block->inputRect.w / 2,
                 block->inputRect.y + block->inputRect.h / 2, {0, 0, 0, 255}, true);
        drawText(engine, "y:", renderRect.x + 108,
                 renderRect.y + renderRect.h / 2, {255, 255, 255, 255}, false);
        block->inputRect2.x = block->rect.x + 122;
        block->inputRect2.y = block->rect.y + 7;
        block->inputRect2.w = 40;
        block->inputRect2.h = 26;
        SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(engine->m_renderer, &block->inputRect2);
        SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(engine->m_renderer, &block->inputRect2);
        if (block->isEditingInput2) {
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(engine->m_renderer, &block->inputRect2);
        }
        drawText(engine, block->inputText2,
                 block->inputRect2.x + block->inputRect2.w / 2,
                 block->inputRect2.y + block->inputRect2.h / 2, {0, 0, 0, 255}, true);
    } else if (block->hasInput) {
        if (block->type == BLOCK_WHEN_KEY_PRESSED) {
            drawText(engine, "when", renderRect.x + 4,
                     renderRect.y + renderRect.h / 2, {255, 255, 255, 255}, false);
            block->inputRect.x = block->rect.x + 40;
            block->inputRect.y = block->rect.y + 7;
            block->inputRect.w = 60;
            block->inputRect.h = 26;
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(engine->m_renderer, &block->inputRect);
            SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(engine->m_renderer, &block->inputRect);
            if (block->isEditingInput) {
                SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 0, 255);
                SDL_RenderDrawRect(engine->m_renderer, &block->inputRect);
            }
            drawText(engine, block->inputText,
                     block->inputRect.x + block->inputRect.w / 2,
                     block->inputRect.y + block->inputRect.h / 2, {0, 0, 0, 255}, true);
            drawText(engine, "pressed", renderRect.x + 106,
                     renderRect.y + renderRect.h / 2, {255, 255, 255, 255}, false);
        } else {
            drawText(engine, block->textData, renderRect.x + (renderRect.w - 55) / 2,
                     renderRect.y + renderRect.h / 2, {255, 255, 255, 255}, true);
            block->inputRect.x = block->rect.x + block->rect.w - 55;
            block->inputRect.y = block->rect.y + 5;
            SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(engine->m_renderer, &block->inputRect);
            SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(engine->m_renderer, &block->inputRect);
            if (block->isEditingInput) {
                SDL_SetRenderDrawColor(engine->m_renderer, 255, 255, 0, 255);
                SDL_RenderDrawRect(engine->m_renderer, &block->inputRect);
            }
            drawText(engine, block->inputText,
                     block->inputRect.x + block->inputRect.w / 2,
                     block->inputRect.y + block->inputRect.h / 2,
                     {0, 0, 0, 255}, true);
        }
    } else {
        drawText(engine, block->textData, renderRect.x + renderRect.w / 2,
                 renderRect.y + renderRect.h / 2, {255, 255, 255, 255}, true);
    }
    if (Block_hasResult(block)) {
        int resW = max(60, (int) block->lastResult.size() * 10 + 10);
        block->resultRect = {renderRect.x + renderRect.w / 2 - resW / 2,
                             renderRect.y + renderRect.h + 3, resW, 20};
        SDL_Color resColor = (block->lastResult == "true") ? SDL_Color{0, 200, 0, 220} :
                             (block->lastResult == "false") ? SDL_Color{200, 0, 0, 220} :
                             SDL_Color{255, 220, 0, 220};
        SDL_SetRenderDrawColor(engine->m_renderer, resColor.r, resColor.g, resColor.b, resColor.a);
        SDL_RenderFillRect(engine->m_renderer, &block->resultRect);
        SDL_SetRenderDrawColor(engine->m_renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(engine->m_renderer, &block->resultRect);
        drawText(engine, block->lastResult,
                 block->resultRect.x + block->resultRect.w / 2,
                 block->resultRect.y + block->resultRect.h / 2,
                 {0, 0, 0, 255}, true);
    }
    if (!block->isLoop) {
        int childY = renderRect.y + renderRect.h;
        for (auto child: block->children) {
            child->rect.x = block->rect.x + 20;
            child->rect.y = childY;
            drawBlock(engine, child, offsetX, 0);
            childY += child->rect.h + 5;
        }
    }
}