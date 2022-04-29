/**
 * Copyright (c) 2021 furudbat
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 * that you wrote the original software. If you use this software in a product,
 * an acknowledgment in the product documentation would be appreciated but is
 * not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#ifndef RAYGUI_EXTRAS_H
#define RAYGUI_EXTRAS_H

#include <string.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <raylib.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @TODO: load font from memory
RAYGUIAPI void GuiLoadStyleFromMemory(const char* buffer);

RAYGUIAPI bool GuiButtonEx(Rectangle bounds, const char* text, bool focused);
RAYGUIAPI bool GuiCheckBoxEx(Rectangle bounds, const char* text, bool checked, bool focused);

RAYGUIAPI int GuiNextDropdownItem(const char* text, int* active);
RAYGUIAPI int GuiPrevDropdownItem(const char* text, int* active);

RAYGUIAPI bool GuiLabelBox(Rectangle bounds, const char* text);

RAYGUIAPI bool GuiTextBoxReadOnly(Rectangle bounds, const char* text);
RAYGUIAPI bool GuiTextBoxMultiReadOnly(Rectangle bounds, const char* text);

/// value can be set with arrow keys
RAYGUIAPI bool GuiSpinnerEx(Rectangle bounds, const char* text, int* value, int minValue, int maxValue, bool editMode);

/// rotate value (when reach min/max)
RAYGUIAPI bool GuiSpinnerExRotate(Rectangle bounds, const char* text, int* value, int minValue, int maxValue,
                                  bool editMode, bool rotate);

RAYGUIAPI bool GuiTextBoxExEdit(Rectangle bounds, char* text, int textSize, bool editMode);
RAYGUIAPI bool GuiTextBoxExEditPlaceholder(Rectangle bounds, char* text, int textSize, bool editMode, const char* placeholder);

RAYGUIAPI void GuiDebugLayoutRecs(const Rectangle* layoutRecs, int layoutRecsSize);

#ifdef __cplusplus
}
#endif

#endif


#ifdef RAYGUI_EXTRAS_IMPLEMENTATION

void GuiLoadStyleFromMemory(const char* str) {
  #ifndef MAX_LINE_BUFFER_SIZE
  #define MAX_LINE_BUFFER_SIZE    256
  #endif

  if (str == NULL) {
    return;
  }

  char buffer[32 * MAX_LINE_BUFFER_SIZE] = {0};
  strncpy(buffer, str, 32 * MAX_LINE_BUFFER_SIZE);

  if (buffer[0] == '#') {
    int controlId = 0;
    int propertyId = 0;
    unsigned int propertyValue = 0;

    const char delim[2] = "\n";
    char* token;

    token = strtok(buffer, delim);

    while (token != NULL) {
      switch (token[0]) {
        case 'p': {
          // Style property: p <control_id> <property_id> <property_value> <property_name>

          sscanf(token, "p %d %d 0x%x", &controlId, &propertyId, &propertyValue);
          GuiSetStyle(controlId, propertyId, (int)propertyValue);
        } break;
        /// @FIXME: load font from memory ?
        /*
        case 'f':
        {
          // Style font: f <gen_font_size> <charmap_file> <font_file>

          int fontSize = 0;
          char charmapFileName[256] = { 0 };
          char fontFileName[256] = { 0 };
          sscanf(token, "f %d %s %[^\r\n]s", &fontSize, charmapFileName, fontFileName);

          Font font = { 0 };

          if (charmapFileName[0] != '0')
          {
            // Load characters from charmap file,
            // expected '\n' separated list of integer values
            char *charValues = LoadFileText(charmapFileName);
            if (charValues != NULL)
            {
              int glyphCount = 0;
              const char **chars = TextSplit(charValues, '\n', &glyphCount);

              int *values = (int *)RAYGUI_MALLOC(glyphCount*sizeof(int));
              for (int i = 0; i < glyphCount; i++) values[i] = TextToInteger(chars[i]);

              if (font.texture.id != GetFontDefault().texture.id) UnloadTexture(font.texture);
              font = LoadFontEx(TextFormat("%s/%s", GetDirectoryPath(fileName), fontFileName), fontSize, values, glyphCount);
              if (font.texture.id == 0) font = GetFontDefault();

              RAYGUI_FREE(values);
            }
          }
          else
          {
            if (font.texture.id != GetFontDefault().texture.id) UnloadTexture(font.texture);
            font = LoadFontEx(TextFormat("%s/%s", GetDirectoryPath(fileName), fontFileName), fontSize, NULL, 0);
            if (font.texture.id == 0) font = GetFontDefault();
          }

          if ((font.texture.id > 0) && (font.glyphCount > 0)) GuiSetFont(font);

        } break;
        */
        default:
          break;
      }

      token = strtok(NULL, delim);
    }
  }
}


// Button control, returns true when clicked
bool GuiButtonEx(Rectangle bounds, const char* text, bool focused) {
  GuiControlState state = (GuiControlState)GuiGetState();
  bool pressed = false;
  if (state != GUI_STATE_DISABLED && focused) {
    state = GUI_STATE_FOCUSED;
  }

  // Update control
  //--------------------------------------------------------------------
  if ((state != GUI_STATE_DISABLED) && !GuiIsLocked())
  {
    Vector2 mousePoint = GetMousePosition();

    // Check button state
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
      else state = GUI_STATE_FOCUSED;

      if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) pressed = true;
    }
  }
  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  GuiDrawRectangle(bounds, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, GUI_BORDER + (state*3))), GuiGetFade()), Fade(GetColor(GuiGetStyle(BUTTON, GUI_BASE + (state*3))), GuiGetFade()));
  GuiDrawText(text, GetTextBounds(BUTTON, bounds), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, GUI_TEXT + (state*3))), GuiGetFade()));
  //------------------------------------------------------------------

  return pressed;
}

// Check Box control, returns true when active
bool GuiCheckBoxEx(Rectangle bounds, const char* text, bool checked, bool focused) {
  GuiControlState state = (GuiControlState)GuiGetState();
  if (focused) {
    state = GUI_STATE_FOCUSED;
  }

  Rectangle textBounds = { 0 };

  if (text != NULL)
  {
    textBounds.width = (float)GetTextWidth(text);
    textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
    textBounds.x = bounds.x + bounds.width + GuiGetStyle(CHECKBOX, TEXT_PADDING);
    textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
    if (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(CHECKBOX, TEXT_PADDING);
  }

  // Update control
  //--------------------------------------------------------------------
  if ((state != GUI_STATE_DISABLED) && !GuiIsLocked())
  {
    Vector2 mousePoint = GetMousePosition();

    Rectangle totalBounds = {
        (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT)? textBounds.x : bounds.x,
        bounds.y,
        bounds.width + textBounds.width + GuiGetStyle(CHECKBOX, TEXT_PADDING),
        bounds.height,
    };

    // Check checkbox state
    if (CheckCollisionPointRec(mousePoint, totalBounds))
    {
      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
      else state = GUI_STATE_FOCUSED;

      if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) checked = !checked;
    }
  }
  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  GuiDrawRectangle(bounds, GuiGetStyle(CHECKBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(CHECKBOX, GUI_BORDER + (state*3))), GuiGetFade()), BLANK);

  if (checked)
  {
    Rectangle check = { bounds.x + GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING),
                       bounds.y + GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING),
                       bounds.width - 2*(GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING)),
                       bounds.height - 2*(GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING)) };
    GuiDrawRectangle(check, 0, BLANK, Fade(GetColor(GuiGetStyle(CHECKBOX, GUI_TEXT + state*3)), GuiGetFade()));
  }

  GuiDrawText(text, textBounds, (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT)? GUI_TEXT_ALIGN_LEFT : GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, GUI_TEXT + (state*3))), GuiGetFade()));
  //--------------------------------------------------------------------

  return checked;
}


int GuiNextDropdownItem(const char* text, int* active) {
  // Get substrings items from text (items pointers, lengths and count)
  int itemCount = 0;
  const char** items = GuiTextSplit(text, &itemCount, NULL);
  if (active && itemCount > 0) {
    *active = (*active + 1) % itemCount;
    return *active;
  }
  return -1;
}

int GuiPrevDropdownItem(const char* text, int* active) {
  // Get substrings items from text (items pointers, lengths and count)
  int itemCount = 0;
  const char** items = GuiTextSplit(text, &itemCount, NULL);
  if (active && itemCount > 0) {
    *active = (*active > 0) ? (*active - 1) % itemCount : 0;
    return *active;
  }
  return -1;
}

bool GuiTextBoxReadOnly(Rectangle bounds, const char* text) {
  GuiControlState state = (GuiControlState)GuiGetState();
  const float guiFade = GuiGetFade();
  bool pressed = false;

  // Update control
  //--------------------------------------------------------------------
  if ((state != GUI_STATE_DISABLED) && !guiLocked)
  {
    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, bounds))
    {
      state = GUI_STATE_FOCUSED;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;
    }
  }
  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  if (state == GUI_STATE_PRESSED)
  {
    GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, GUI_BORDER + (state*3))), guiFade), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), guiFade));
  }
  else if (state == GUI_STATE_DISABLED)
  {
    GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, GUI_BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiFade));
  }
  else GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(TEXTBOX, GUI_BORDER + (state*3))), guiFade), BLANK);

  GuiDrawText(text, GetTextBounds(TEXTBOX, bounds), GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TEXTBOX, GUI_TEXT + (state*3))), guiFade));

  //--------------------------------------------------------------------

  return pressed;
}

bool GuiLabelBox(Rectangle bounds, const char* text) {
  GuiControlState state = (GuiControlState)GuiGetState();
  const float guiFade = GuiGetFade();
  const bool guiIsLocked = GuiIsLocked();
  bool pressed = false;

  // Update control
  //--------------------------------------------------------------------
  if ((state != GUI_STATE_DISABLED) && !guiIsLocked) {
    Vector2 mousePoint = GetMousePosition();
    if (CheckCollisionPointRec(mousePoint, bounds)) {
      //state = GUI_STATE_FOCUSED;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        pressed = true;
    }
  }
  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  if (state == GUI_STATE_DISABLED) {
    GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH),
                     Fade(GetColor(GuiGetStyle(TEXTBOX, GUI_BORDER + (state * 3))), guiFade),
                     Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiFade));
  } else
    GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(TEXTBOX, GUI_BORDER + (state * 3))), guiFade), BLANK);


  GuiDrawText(text, bounds, GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT),
              Fade(GetColor(GuiGetStyle(TEXTBOX, GUI_TEXT + (state * 3))), guiFade));

  //--------------------------------------------------------------------

  return pressed;
}

// Text Box control with multiple lines
bool GuiTextBoxMultiReadOnly(Rectangle bounds, const char* text) {
  GuiControlState state = (GuiControlState)GuiGetState();
  bool pressed = false;

  Rectangle textAreaBounds = {
      bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING),
      bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING),
      bounds.width - 2*(GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)),
      bounds.height - 2*(GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING))
  };

  // Cursor position, [x, y] values should be updated
  Rectangle cursor = { 0, -1, 4, (float)GuiGetStyle(DEFAULT, TEXT_SIZE) + 2 };

  float scaleFactor = (float)GuiGetStyle(DEFAULT, TEXT_SIZE)/(float)guiFont.baseSize;     // Character rectangle scaling factor

  // Update control
  //--------------------------------------------------------------------
  if ((state != GUI_STATE_DISABLED) && !guiLocked)
  {
    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, bounds))
    {
      state = GUI_STATE_FOCUSED;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;
    }
  }
  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  if (state == GUI_STATE_PRESSED)
  {
    GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), guiAlpha));
  }
  else if (state == GUI_STATE_DISABLED)
  {
    GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiAlpha));
  }
  else GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), BLANK);

  int wrapMode = 1;      // 0-No wrap, 1-Char wrap, 2-Word wrap
  Vector2 cursorPos = { textAreaBounds.x, textAreaBounds.y };

  //int lastSpacePos = 0;
  //int lastSpaceWidth = 0;
  //int lastSpaceCursorPos = 0;

  for (int i = 0, codepointLength = 0; text[i] != '\0'; i += codepointLength)
  {
    int codepoint = GetCodepoint(text + i, &codepointLength);
    int index = GetGlyphIndex(guiFont, codepoint);      // If requested codepoint is not found, we get '?' (0x3f)
    Rectangle atlasRec = guiFont.recs[index];
    GlyphInfo glyphInfo = guiFont.glyphs[index];        // Glyph measures

    if ((codepointLength == 1) && (codepoint == '\n'))
    {
      cursorPos.y += (guiFont.baseSize*scaleFactor + GuiGetStyle(TEXTBOX, TEXT_LINES_SPACING));   // Line feed
      cursorPos.x = textAreaBounds.x;                 // Carriage return
    }
    else
    {
      if (wrapMode == 1)
      {
        int glyphWidth = 0;
        if (glyphInfo.advanceX != 0) glyphWidth += glyphInfo.advanceX;
        else glyphWidth += (int)(atlasRec.width + glyphInfo.offsetX);

        // Jump line if the end of the text box area has been reached
        if ((cursorPos.x + (glyphWidth*scaleFactor)) > (textAreaBounds.x + textAreaBounds.width))
        {
          cursorPos.y += (guiFont.baseSize*scaleFactor + GuiGetStyle(TEXTBOX, TEXT_LINES_SPACING));   // Line feed
          cursorPos.x = textAreaBounds.x;     // Carriage return
        }
      }
      else if (wrapMode == 2)
      {
        /*
        if ((codepointLength == 1) && (codepoint == ' '))
        {
            lastSpacePos = i;
            lastSpaceWidth = 0;
            lastSpaceCursorPos = cursorPos.x;
        }

        // Jump line if last word reaches end of text box area
        if ((lastSpaceCursorPos + lastSpaceWidth) > (textAreaBounds.x + textAreaBounds.width))
        {
            cursorPos.y += 12;               // Line feed
            cursorPos.x = textAreaBounds.x;  // Carriage return
        }
        */
      }

      // Draw current character glyph
      DrawTextCodepoint(guiFont, codepoint, cursorPos, (float)GuiGetStyle(DEFAULT, TEXT_SIZE), Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))), guiAlpha));

      int glyphWidth = 0;
      if (glyphInfo.advanceX != 0) glyphWidth += glyphInfo.advanceX;
      else glyphWidth += (int)(atlasRec.width + glyphInfo.offsetX);

      cursorPos.x += (glyphWidth*scaleFactor + (float)GuiGetStyle(DEFAULT, TEXT_SPACING));
      //if (i > lastSpacePos) lastSpaceWidth += (atlasRec.width + (float)GuiGetStyle(DEFAULT, TEXT_SPACING));
    }
  }

  cursor.x = cursorPos.x;
  cursor.y = cursorPos.y;

  //--------------------------------------------------------------------

  return pressed;
}

// Spinner control, returns selected value
bool GuiSpinnerEx(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode) {
  return GuiSpinnerExRotate(bounds, text, value, minValue, maxValue, editMode, false);
}
bool GuiSpinnerExRotate(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode, bool rotate)
{
    GuiControlState state = (GuiControlState)GuiGetState();

    bool pressed = false;
    int tempValue = *value;

    Rectangle spinner = { bounds.x + GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_SPACING), bounds.y,
                         bounds.width - 2*(GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_SPACING)), bounds.height };
    Rectangle leftButtonBound = { (float)bounds.x, (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };
    Rectangle rightButtonBound = { (float)bounds.x + bounds.width - GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };

    Rectangle textBounds = { 0 };
    if (text != NULL)
    {
      textBounds.width = (float)GetTextWidth(text);
      textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
      textBounds.x = bounds.x + bounds.width + GuiGetStyle(SPINNER, TEXT_PADDING);
      textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
      if (GuiGetStyle(SPINNER, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(SPINNER, TEXT_PADDING);
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
      Vector2 mousePoint = GetMousePosition();

      // Check spinner state
      if (CheckCollisionPointRec(mousePoint, bounds))
      {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
        else state = GUI_STATE_FOCUSED;
      }
    }

    #if defined(RAYGUI_NO_ICONS)
      if (GuiButton(leftButtonBound, "<")) tempValue--;
      if (GuiButton(rightButtonBound, ">")) tempValue++;
    #else
      if (GuiButton(leftButtonBound, GuiIconText(RAYGUI_ICON_ARROW_LEFT_FILL, NULL))) tempValue--;
      if (GuiButton(rightButtonBound, GuiIconText(RAYGUI_ICON_ARROW_RIGHT_FILL, NULL))) tempValue++;
    #endif

    if (!editMode)
    {
      if (tempValue < minValue && !rotate) {
        tempValue = minValue;
      }
      if (tempValue < minValue && rotate) {
        tempValue = maxValue;
      }
      if (tempValue > maxValue && !rotate) {
        tempValue = maxValue;
      }
      if (tempValue > maxValue && rotate) {
        tempValue = minValue;
      }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    // TODO: Set Spinner properties for ValueBox
    pressed = GuiValueBox(spinner, NULL, &tempValue, minValue, maxValue, editMode);

    // Draw value selector custom buttons
    // NOTE: BORDER_WIDTH and TEXT_ALIGNMENT forced values
    int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
    int tempTextAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, GuiGetStyle(SPINNER, BORDER_WIDTH));
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlign);
    GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);

    // Draw text label if provided
    GuiDrawText(text, textBounds, (GuiGetStyle(SPINNER, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT)? GUI_TEXT_ALIGN_LEFT : GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    *value = tempValue;
    return pressed;
}

bool GuiTextBoxExEdit(Rectangle bounds, char *text, int textSize, bool editMode)
{
    GuiControlState state = (GuiControlState)GuiGetState();
    bool pressed = false;

    const int CURSOR_WIDTH = GuiGetStyle(DEFAULT, TEXT_SIZE);
    const int textLength = (int)strlen(text);
    const int cursorPositionX = (textLength > 0)? GetTextWidth(text) - GetTextWidth(text + (sizeof(char) * (textLength - 1))) : 0;
    Rectangle cursor = {
        bounds.x + GuiGetStyle(TEXTBOX, TEXT_PADDING) + cursorPositionX - ((textLength > 0)? CURSOR_WIDTH/4 : 0),
        bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE),
        CURSOR_WIDTH,
        (float)GuiGetStyle(DEFAULT, TEXT_SIZE)*2
    };

    if (cursor.height >= bounds.height) cursor.height = bounds.height - GuiGetStyle(TEXTBOX, BORDER_WIDTH)*2;
    if (cursor.y < (bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH))) cursor.y = bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH);

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked) {
      Vector2 mousePoint = GetMousePosition();

      if (editMode) {
        state = GUI_STATE_PRESSED;

        int key = GetCharPressed(); // Returns codepoint as Unicode
        int keyCount = textLength;
        if (key == 0 && IsKeyPressed(KEY_SPACE)) {
          key = ' ';
        }

        // Only allow keys in range [32..125]
        if (keyCount < (textSize - 1)) {
          float maxWidth = (bounds.width - (GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING) * 2));

          if ((GetTextWidth(text) < (maxWidth - GuiGetStyle(DEFAULT, TEXT_SIZE))) && (key >= 32)) {
            int byteSize = 0;
            const char* textUTF8 = CodepointToUTF8(key, &byteSize);

            for (int i = 0; i < byteSize; i++) {
              text[keyCount] = textUTF8[i];
              keyCount++;
            }

            text[keyCount] = '\0';
          }
        }

        // Delete text
        if (keyCount > 0) {
          if (IsKeyPressed(KEY_BACKSPACE)) {
            keyCount--;
            text[keyCount] = '\0';
            if (keyCount < 0)
              keyCount = 0;
          }
        }

        if (IsKeyPressed(KEY_ENTER) ||
            (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
          pressed = true;
        } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
          keyCount--;
          if (keyCount < 0) {
            keyCount = 0;
          }
          char key = (keyCount < textSize) ? text[keyCount] : ' ';
          float maxWidth = (bounds.width - (GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING) * 2));
          if (IsKeyPressed(KEY_UP)) {
            if (key == ' ' || key == '\0') {
              key = 'a';
            } else if (key >= 'a' && key < 'z') {
              key++;
            } else if (key == 'z') {
              key = 'A';
            } else if (key >= 'A' && key < 'Z') {
              key++;
            } else if (key == 'Z') {
              key = ' ';
            }
          } else if (IsKeyPressed(KEY_DOWN)) {
            if (key == ' ' || key == '\0') {
              key = 'Z';
            } else if (key > 'a' && key <= 'z') {
              key--;
            } else if (key == 'a') {
              key = ' ';
            } else if (key > 'A' && key <= 'Z') {
              key--;
            } else if (key == 'A') {
              key = 'z';
            }
          }

          if ((GetTextWidth(text) < (maxWidth - GuiGetStyle(DEFAULT, TEXT_SIZE))) && (key >= 32)) {
            int byteSize = 0;
            const char* textUTF8 = CodepointToUTF8(key, &byteSize);

            for (int i = 0; i < byteSize; i++) {
              text[keyCount] = textUTF8[i];
              keyCount++;
            }

            text[keyCount] = '\0';
          }
          // Check text alignment to position cursor properly
          int textAlignment = GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT);
          if (textAlignment == GUI_TEXT_ALIGN_CENTER)
            cursor.x = bounds.x + GetTextWidth(text) / 2 + bounds.width / 2 + 1;
          else if (textAlignment == GUI_TEXT_ALIGN_RIGHT)
            cursor.x = bounds.x + bounds.width - GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING);
        }
      } else {
        if (CheckCollisionPointRec(mousePoint, bounds)) {
          state = GUI_STATE_FOCUSED;
          if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            pressed = true;
        }
      }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state == GUI_STATE_PRESSED)
    {
      GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), guiAlpha));
    }
    else if (state == GUI_STATE_DISABLED)
    {
      GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiAlpha));
    }
    else GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), BLANK);

    GuiDrawText(text, GetTextBounds(TEXTBOX, bounds), GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))), guiAlpha));

    // Draw cursor
    if (editMode) GuiDrawRectangle(cursor, 0, BLANK, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)), guiAlpha*0.5F));
    //--------------------------------------------------------------------

    return pressed;
}

bool GuiTextBoxExEditPlaceholder(Rectangle bounds, char *text, int textSize, bool editMode, const char* placeholder)
{
  const auto pressed = GuiTextBoxExEdit(bounds, text, textSize, editMode);
  GuiControlState state = (GuiControlState)GuiGetState();
  const int textLength = (int)strlen(text);
  if(textLength == 0)
  {
    GuiDrawText(placeholder, GetTextBounds(TEXTBOX, bounds), GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT),
                Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state * 3))), guiAlpha * 0.75F));
  }
  return pressed;
}

void GuiDebugLayoutRecs(const Rectangle* layoutRecs, int layoutRecsSize) {
  if (IsKeyDown(KEY_F3) || IsKeyDown(KEY_F4)) {
    const Vector2 mousePoint = GetMousePosition();
    const bool oldLock = GuiIsLocked();
    const int oldState = GuiGetState();
    const float oldFade = GuiGetFade();
    GuiLock();
    for (int i = 0;i < layoutRecsSize;i++) {
      const char* info = (IsKeyDown(KEY_F3))? TextFormat("%i", i) : TextFormat("%i: (%i, %i, %i, %i)", i, (int)layoutRecs[i].x, (int)layoutRecs[i].y, (int)layoutRecs[i].width, (int)layoutRecs[i].height);
      if (CheckCollisionPointRec(mousePoint, layoutRecs[i])) {
        GuiFade(1.0f);
        GuiSetState(GUI_STATE_FOCUSED);
        GuiPanel(layoutRecs[i], NULL);
        GuiTextBoxReadOnly(layoutRecs[i], info);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          TraceLog(LOG_DEBUG, TextFormat("GuiDebugLayoutRecs: %i: (%i, %i, %i, %i)", i, (int)layoutRecs[i].x, (int)layoutRecs[i].y, (int)layoutRecs[i].width, (int)layoutRecs[i].height));
        }
      } else {
        GuiFade(0.5f);
        GuiSetState(GUI_STATE_DISABLED);
        GuiPanel(layoutRecs[i], NULL);
      }
    }
    if (oldLock) {
      GuiLock();
    } else {
      GuiUnlock();
    }
    GuiSetState(oldState);
    GuiFade(oldFade);
  }
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#endif

#endif
