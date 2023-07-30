/**
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2021-2023 furudbat
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
*/

#ifndef RAYGUI_EXTRAS_H
#define RAYGUI_EXTRAS_H

#include "raygui.h"
#include <cstring>

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

typedef enum {
    GUI_WINDOW_BOX_NONE = 0,
    GUI_WINDOW_BOX_CLOSE = 1,
    GUI_WINDOW_BOX_LEFT,
    GUI_WINDOW_BOX_RIGHT,
} GuiWindowBoxClicked;

// Gui control property style color element, same as GuiPropertyElement (for public use)
typedef enum { GUI_BORDER = 0, GUI_BASE, GUI_TEXT, GUI_OTHER } PublicGuiPropertyElement;

RAYGUIAPI int GuiWindowBoxEx(Rectangle bounds, const char *title);

RAYGUIAPI void GuiLoadStyleTextFromMemory(const char* buffer);

RAYGUIAPI int GuiButtonEx(Rectangle bounds, const char* text, bool focused);
RAYGUIAPI int GuiCheckBoxEx(Rectangle bounds, const char* text, bool* checked, bool focused);

RAYGUIAPI int GuiLabelButtonEx(Rectangle bounds, const char *text, bool focused);
RAYGUIAPI int GuiLabelButtonExActive(Rectangle bounds, const char *text, bool focused, bool active);

RAYGUIAPI int GuiNextDropdownItem(const char* text, int* active);
RAYGUIAPI int GuiPrevDropdownItem(const char* text, int* active);

RAYGUIAPI int GuiTextBoxReadOnly(Rectangle bounds, const char* text);
RAYGUIAPI int GuiTextBoxMultiReadOnly(Rectangle bounds, const char *text);

/// value can be set with arrow keys
RAYGUIAPI int GuiSpinnerEx(Rectangle bounds, const char* text, int* value, int minValue, int maxValue, bool editMode);

/// rotate value (when reach min/max)
RAYGUIAPI int GuiSpinnerExRotate(Rectangle bounds, const char* text, int* value, int minValue, int maxValue,
                                 bool editMode, bool rotate);

RAYGUIAPI int GuiTextBoxPlaceholder(Rectangle bounds, char *text, int textSize, bool editMode, const char* placeholder);

RAYGUIAPI void GuiDebugLayoutRecs(const Rectangle* layoutRecs, unsigned int layoutRecsSize);

RAYGUIAPI int GuiListViewHover(Rectangle bounds, const char* text, const int* scrollIndex);
RAYGUIAPI int GuiListViewExHover(Rectangle bounds, int count, const int* scrollIndex);

RAYGUIAPI int GuiListViewClicked(Rectangle bounds, const char* text, const int* scrollIndex);
RAYGUIAPI int GuiListViewExClicked(Rectangle bounds, int count, const int* scrollIndex);

RAYGUIAPI int GuiListViewRender(Rectangle bounds, const char *text, int *scrollIndex, int itemSelected, int itemFocused);
RAYGUIAPI int GuiListViewRenderEx(Rectangle bounds, const char **text, int count, int *scrollIndex, int itemSelected, int itemFocused);

RAYGUIAPI bool GuiClickOnBox(Rectangle bounds);
RAYGUIAPI bool GuiClickOutsideBox(Rectangle bounds);

#ifdef __cplusplus
}            // Prevents name mangling of functions
#endif

#endif // RAYGUI_EXTRAS_H

/***********************************************************************************
*
*   RAYGUI EXTRAS IMPLEMENTATION
*
************************************************************************************/

#ifdef RAYGUI_EXTRAS_IMPLEMENTATION

// Window Box control
int GuiWindowBoxEx(Rectangle bounds, const char *title)
{
  // Window title bar height (including borders)
  // NOTE: This define is also used by GuiMessageBox() and GuiTextInputBox()
  #if !defined(RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
      #define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT        24
  #endif

  int result = GUI_WINDOW_BOX_NONE;
  //GuiState state = guiState;

  int statusBarHeight = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;

  Rectangle statusBar = { bounds.x, bounds.y, bounds.width, (float)statusBarHeight };
  if (bounds.height < statusBarHeight*2.0f) bounds.height = statusBarHeight*2.0f;

  Rectangle windowPanel = { bounds.x, bounds.y + (float)statusBarHeight - 1, bounds.width, bounds.height - (float)statusBarHeight + 1 };
  Rectangle closeButtonRec = { statusBar.x + statusBar.width - GuiGetStyle(STATUSBAR, BORDER_WIDTH) - 1*20,
                               statusBar.y + statusBarHeight/2.0f - 18.0f/2.0f, 18, 18 };
  Rectangle rightButtonRec = { statusBar.x + statusBar.width - GuiGetStyle(STATUSBAR, BORDER_WIDTH) - 2*20,
                               statusBar.y + statusBarHeight/2.0f - 18.0f/2.0f, 18, 18 };
  Rectangle leftButtonRec = { statusBar.x + statusBar.width - GuiGetStyle(STATUSBAR, BORDER_WIDTH) - 3*20,
                              statusBar.y + statusBarHeight/2.0f - 18.0f/2.0f, 18, 18 };

  // Update control
  //--------------------------------------------------------------------
  // NOTE: Logic is directly managed by button
  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  GuiStatusBar(statusBar, title); // Draw window header as status bar
  GuiPanel(windowPanel, NULL);    // Draw window base

  // Draw window close button
  int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
  int tempTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
  GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
  GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
#if defined(RAYGUI_NO_ICONS)
  if(GuiButton(closeButtonRec, "x"))
    result = GUI_WINDOW_BOX_CLOSE;
  if(GuiButton(rightButtonRec, ">"))
    result = GUI_WINDOW_BOX_LEFT;
  if(GuiButton(leftButtonRec, "<"))
    result = GUI_WINDOW_BOX_RIGHT;
#else
  if(GuiButton(closeButtonRec, GuiIconText(ICON_CROSS_SMALL, NULL)))
    result = GUI_WINDOW_BOX_CLOSE;
  if(GuiButton(rightButtonRec, GuiIconText(ICON_ARROW_RIGHT, NULL)))
    result = GUI_WINDOW_BOX_RIGHT;
  if(GuiButton(leftButtonRec, GuiIconText(ICON_ARROW_LEFT, NULL)))
    result = GUI_WINDOW_BOX_LEFT;
#endif
  GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
  GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlignment);
  //--------------------------------------------------------------------

  return result;      // Window close button clicked: result = 1
}

void GuiLoadStyleTextFromMemory(const char* str) {
  #ifndef MAX_LINE_BUFFER_SIZE
  #define MAX_LINE_BUFFER_SIZE    256
  #endif

  if (str == NULL) {
    return;
  }

  char buffer[32 * MAX_LINE_BUFFER_SIZE] = {0};
  strncpy(buffer, str, 32 * MAX_LINE_BUFFER_SIZE);

  if (buffer[0] == '#')
  {
      int controlId = 0;
      int propertyId = 0;
      unsigned int propertyValue = 0;

      const char delim[2] = "\n";
      char* token;

      token = strtok(buffer, delim);


      while (token != NULL)
      {
          switch (token[0])
          {
              case 'p':
              {
                  // Style property: p <control_id> <property_id> <property_value> <property_name>

                  sscanf(token, "p %d %d 0x%x", &controlId, &propertyId, &propertyValue);
                  GuiSetStyle(controlId, propertyId, (int)propertyValue);

              } break;
              default: break;
          }

          token = strtok(NULL, delim);
      }
  }
}


// Button control, returns true when clicked
int GuiButtonEx(Rectangle bounds, const char* text, bool focused)
{
  int result = 0;
  GuiState state = (GuiState)GuiGetState();
  if (state != STATE_DISABLED && focused) {
    state = STATE_FOCUSED;
  }

  // Update control
  //--------------------------------------------------------------------
  if ((state != STATE_DISABLED) && !GuiIsLocked() && !GuiSliderIsDragging())
  {
      Vector2 mousePoint = GetMousePosition();

      // Check button state
      if (CheckCollisionPointRec(mousePoint, bounds))
      {
          if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = STATE_PRESSED;
          else state = STATE_FOCUSED;

          if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) result = 1;
      }
  }
  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  GuiDrawRectangle(bounds, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, GUI_BORDER + (state*3))), GuiGetAlpha()), Fade(GetColor(GuiGetStyle(BUTTON, GUI_BASE + (state*3))), GuiGetAlpha()));
  GuiDrawText(text, GetTextBounds(BUTTON, bounds), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, GUI_TEXT + (state*3))), GuiGetAlpha()));

  if (state == STATE_FOCUSED) GuiTooltip(bounds);
  //------------------------------------------------------------------

  return result;      // Button pressed: result = 1
}

// Check Box control, returns true when active
int GuiCheckBoxEx(Rectangle bounds, const char* text, bool* checked, bool focused)
{
  int result = 0;
  GuiState state = (GuiState)GuiGetState();
  if (focused) {
    state = STATE_FOCUSED;
  }

  bool temp = false;
  if (checked == NULL) checked = &temp;

  Rectangle textBounds = { 0 };

  if (text != NULL)
  {
      textBounds.width = (float)GetTextWidth(text) + 2;
      textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
      textBounds.x = bounds.x + bounds.width + GuiGetStyle(CHECKBOX, TEXT_PADDING);
      textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
      if (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(CHECKBOX, TEXT_PADDING);
  }

  // Update control
  //--------------------------------------------------------------------
  if ((state != STATE_DISABLED) && !GuiIsLocked() && !GuiSliderIsDragging())
  {
      Vector2 mousePoint = GetMousePosition();

      Rectangle totalBounds = {
          (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == TEXT_ALIGN_LEFT)? textBounds.x : bounds.x,
          bounds.y,
          bounds.width + textBounds.width + GuiGetStyle(CHECKBOX, TEXT_PADDING),
          bounds.height,
      };

      // Check checkbox state
      if (CheckCollisionPointRec(mousePoint, totalBounds))
      {
          if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = STATE_PRESSED;
          else state = STATE_FOCUSED;

          if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) *checked = !(*checked);
      }
  }
  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  GuiDrawRectangle(bounds, GuiGetStyle(CHECKBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(CHECKBOX, GUI_BORDER + (state*3))), GuiGetAlpha()), BLANK);

  if (*checked)
  {
      Rectangle check = { bounds.x + GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING),
                          bounds.y + GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING),
                          bounds.width - 2*(GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING)),
                          bounds.height - 2*(GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING)) };
      GuiDrawRectangle(check, 0, BLANK, Fade(GetColor(GuiGetStyle(CHECKBOX, GUI_TEXT + state*3)), GuiGetAlpha()));
      result = 1;
  }

  GuiDrawText(text, textBounds, (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == TEXT_ALIGN_RIGHT)? TEXT_ALIGN_LEFT : TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, GUI_TEXT + (state*3))), GuiGetAlpha()));
  //--------------------------------------------------------------------

  return result;
}


int GuiNextDropdownItem(const char* text, int* active) {
  // Get substrings items from text (items pointers, lengths and count)
  int itemCount = 0;
  /*const char** items = */
  GuiTextSplit(text, ';', &itemCount, NULL);
  if (active && itemCount > 0) {
    *active = (*active + 1) % itemCount;
    return *active;
  }
  return -1;
}

int GuiPrevDropdownItem(const char* text, int* active) {
  // Get substrings items from text (items pointers, lengths and count)
  int itemCount = 0;
  /*const char** items = */
  GuiTextSplit(text, ';', &itemCount, NULL);
  if (active && itemCount > 0) {
    *active = (*active > 0) ? (*active - 1) % itemCount : 0;
    return *active;
  }
  return -1;
}

int GuiTextBoxReadOnly(Rectangle bounds, const char* text) {
  int result = 0;
  GuiState state = (GuiState)GuiGetState();
  float guiAlpha = GuiGetAlpha();

  Rectangle textBounds = GetTextBounds(TEXTBOX, bounds);
  int textWidth = GetTextWidth(text) - GetTextWidth(text + textBoxCursorIndex);
  int textIndexOffset = 0;        // Text index offset to start drawing in the box

  int alignmentVertical = GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT_VERTICAL);

  // Cursor rectangle
  // NOTE: Position X value should be updated
  Rectangle cursor = {
      textBounds.x + textWidth + GuiGetStyle(DEFAULT, TEXT_SPACING),
      textBounds.y + textBounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE),
      2,
      (float)GuiGetStyle(DEFAULT, TEXT_SIZE)*2
  };

  switch (alignmentVertical)
  {
      case 0: cursor.y = textBounds.y + textBounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE); break;  // CENTERED
      case 1: cursor.y = textBounds.y - GuiGetStyle(DEFAULT, TEXT_SIZE)/2; break;  // UP
      case 2: cursor.y = textBounds.y + textBounds.height; break;  // DOWN
      default: break;
  }

  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  if (state == STATE_PRESSED)
  {
      GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), guiAlpha));
  }
  else if (state == STATE_DISABLED)
  {
      GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiAlpha));
  }
  else GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), BLANK);

  // Draw text considering index offset if required
  // NOTE: Text index offset depends on cursor position
  GuiDrawText(text + textIndexOffset, textBounds, GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))), guiAlpha));

  if (state == STATE_FOCUSED) GuiTooltip(bounds);
  //--------------------------------------------------------------------

  return result;      // Mouse button pressed: result = 1
}
// Text box control with multiple lines
int GuiTextBoxMultiReadOnly(Rectangle bounds, const char *text)
{
    int result = 0;

    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT_VERTICAL, 1);
    GuiSetStyle(TEXTBOX, TEXT_MULTILINE, 1);

    // TODO: Implement methods to calculate cursor position properly
    result = GuiTextBoxReadOnly(bounds, text);

    GuiSetStyle(TEXTBOX, TEXT_MULTILINE, 0);
    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT_VERTICAL, 0);

    return result;
}


// Spinner control, returns selected value
int GuiSpinnerEx(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode) {
  return GuiSpinnerExRotate(bounds, text, value, minValue, maxValue, editMode, false);
}
int GuiSpinnerExRotate(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode, bool rotate)
{
  int result = 1;
  GuiState state = (GuiState)GuiGetState();

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
      if (GuiGetStyle(SPINNER, TEXT_ALIGNMENT) == TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(SPINNER, TEXT_PADDING);
  }

  // Update control
  //--------------------------------------------------------------------
  if ((state != STATE_DISABLED) && !GuiIsLocked() && !GuiSliderIsDragging())
  {
      Vector2 mousePoint = GetMousePosition();

      // Check spinner state
      if (CheckCollisionPointRec(mousePoint, bounds))
      {
          if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = STATE_PRESSED;
          else state = STATE_FOCUSED;
      }
  }

  #if defined(RAYGUI_NO_ICONS)
  if (GuiButton(leftButtonBound, "<")) tempValue--;
  if (GuiButton(rightButtonBound, ">")) tempValue++;
  #else
  if (GuiButton(leftButtonBound, GuiIconText(ICON_ARROW_LEFT_FILL, NULL))) tempValue--;
  if (GuiButton(rightButtonBound, GuiIconText(ICON_ARROW_RIGHT_FILL, NULL))) tempValue++;
  #endif

  if (!editMode)
  {
    if (tempValue < minValue && !rotate)
      tempValue = minValue;
    if (tempValue < minValue && rotate)
      tempValue = maxValue;

    if (tempValue > maxValue && !rotate)
      tempValue = maxValue;
    if (tempValue > maxValue && rotate)
      tempValue = minValue;
  }
  //--------------------------------------------------------------------

  // Draw control
  //--------------------------------------------------------------------
  result = GuiValueBox(spinner, NULL, &tempValue, minValue, maxValue, editMode);

  // Draw value selector custom buttons
  // NOTE: BORDER_WIDTH and TEXT_ALIGNMENT forced values
  int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
  int tempTextAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
  GuiSetStyle(BUTTON, BORDER_WIDTH, GuiGetStyle(SPINNER, BORDER_WIDTH));
  GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

  GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlign);
  GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);

  // Draw text label if provided
  GuiDrawText(text, textBounds, (GuiGetStyle(SPINNER, TEXT_ALIGNMENT) == TEXT_ALIGN_RIGHT)? TEXT_ALIGN_LEFT : TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
  //--------------------------------------------------------------------

  *value = tempValue;
  return result;
}

int GuiTextBoxPlaceholder(Rectangle bounds, char *text, int textSize, bool editMode, const char* placeholder)
{
  const auto result = GuiTextBox(bounds, text, textSize, editMode);
  GuiState state = (GuiState)GuiGetState();
  //const int textLength = (int)strlen(text);
  //const bool empty_text = textLength == 0;
  const bool empty_text = textSize == 0 || text[0] == '\0';
  if(empty_text)
  {
    GuiDrawText(placeholder, GetTextBounds(TEXTBOX, bounds), GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT),
                Fade(GetColor(GuiGetStyle(TEXTBOX, GUI_TEXT + (state * 3))), GuiGetAlpha() * 0.75F));
  }
  return result;
}

void GuiDebugLayoutRecs(const Rectangle* layoutRecs, unsigned int layoutRecsSize) {
  if (IsKeyDown(KEY_F3) || IsKeyDown(KEY_F4)) {
    const Vector2 mousePoint = GetMousePosition();
    const bool oldLock = GuiIsLocked();
    const int oldState = GuiGetState();
    const float oldFade = GuiGetAlpha();
    const bool isLongInfo = IsKeyDown(KEY_F4);
    GuiLock();
    for (unsigned int i = 0; i < layoutRecsSize; i++) {
      const char* info = (isLongInfo)? TextFormat("%i: (%i, %i, %i, %i)", i, (int)layoutRecs[i].x, (int)layoutRecs[i].y, (int)layoutRecs[i].width, (int)layoutRecs[i].height) : TextFormat("%i", i);
      if (CheckCollisionPointRec(mousePoint, layoutRecs[i])) {
        GuiSetAlpha(1.0f);
        GuiSetState(STATE_FOCUSED);
        GuiDrawRectangle(layoutRecs[i], 0, WHITE, Fade(RED, 0.4f));
        GuiTextBoxReadOnly(layoutRecs[i], info);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          TraceLog(LOG_DEBUG, TextFormat("GuiDebugLayoutRecs: %i: (%i, %i, %i, %i)", i, (int)layoutRecs[i].x, (int)layoutRecs[i].y, (int)layoutRecs[i].width, (int)layoutRecs[i].height));
        }
      } else {
        GuiSetAlpha(0.5f);
        GuiSetState(STATE_DISABLED);
        GuiPanel(layoutRecs[i], NULL);
        if (!isLongInfo) {
          GuiTextBoxReadOnly(layoutRecs[i], info);
        }
      }
    }
    if (oldLock) {
      GuiLock();
    } else {
      GuiUnlock();
    }
    GuiSetState(oldState);
    GuiSetAlpha(oldFade);
  }
}

int GuiListViewHover(Rectangle bounds, const char *text, const int *scrollIndex) {
  int itemCount = 0;

  if (text != NULL) GuiTextSplit(text, ';', &itemCount, NULL);

  return GuiListViewExHover(bounds, itemCount, scrollIndex);
}
int GuiListViewExHover(Rectangle bounds, int count, const int *scrollIndex) {
  GuiState state = (GuiState)GuiGetState();
  int itemSelected = -1;

  // Check if we need a scroll bar
  bool useScrollBar = false;
  if ((GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING))*count > bounds.height) useScrollBar = true;

  // Define base item rectangle [0]
  Rectangle itemBounds = { 0 };
  itemBounds.x = bounds.x + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING);
  itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
  itemBounds.width = bounds.width - 2*GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) - GuiGetStyle(DEFAULT, BORDER_WIDTH);
  itemBounds.height = (float)GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
  if (useScrollBar) itemBounds.width -= GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);

  // Get items on the list
  int visibleItems = (int)bounds.height/(GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING));
  if (visibleItems > count) visibleItems = count;

  int startIndex = (scrollIndex == NULL)? 0 : *scrollIndex;
  if ((startIndex < 0) || (startIndex > (count - visibleItems))) startIndex = 0;
  int endIndex = startIndex + visibleItems;

  // Update control
  //--------------------------------------------------------------------
  if ((state != STATE_DISABLED) && !GuiIsLocked() && !GuiSliderIsDragging())
  {
    Vector2 mousePoint = GetMousePosition();

    // Check mouse inside list view
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
      // Check focused and selected item
      for (int i = 0; i < visibleItems; i++)
      {
        if (CheckCollisionPointRec(mousePoint, itemBounds))
        {
          if (itemSelected == (startIndex + i)) itemSelected = -1;
          else itemSelected = startIndex + i;
          break;
        }

        // Update item rectangle y position for next item
        itemBounds.y += (GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING));
      }

      if (useScrollBar)
      {
          int wheelMove = (int)GetMouseWheelMove();
          startIndex -= wheelMove;

          if (startIndex < 0) startIndex = 0;
          else if (startIndex > (count - visibleItems)) startIndex = count - visibleItems;

          endIndex = startIndex + visibleItems;
          if (endIndex > count) endIndex = count;
      }
    }

    // Reset item rectangle y to [0]
    itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
  }
  //--------------------------------------------------------------------

  return itemSelected;
}

int GuiListViewClicked(Rectangle bounds, const char *text, const int *scrollIndex) {
  int itemCount = 0;
  const char** items = NULL;

  if (text != NULL) items = GuiTextSplit(text, ';', &itemCount, NULL);

  return GuiListViewExClicked(bounds, itemCount, scrollIndex);
}
int GuiListViewExClicked(Rectangle bounds, int count, const int *scrollIndex) {
  GuiState state = (GuiState)GuiGetState();
  int itemSelected = -1;

  // Check if we need a scroll bar
  bool useScrollBar = false;
  if ((GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING))*count > bounds.height) useScrollBar = true;

  // Define base item rectangle [0]
  Rectangle itemBounds = { 0 };
  itemBounds.x = bounds.x + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING);
  itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
  itemBounds.width = bounds.width - 2*GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) - GuiGetStyle(DEFAULT, BORDER_WIDTH);
  itemBounds.height = (float)GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
  if (useScrollBar) itemBounds.width -= GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);

  // Get items on the list
  int visibleItems = (int)bounds.height/(GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING));
  if (visibleItems > count) visibleItems = count;

  int startIndex = (scrollIndex == NULL)? 0 : *scrollIndex;
  if ((startIndex < 0) || (startIndex > (count - visibleItems))) startIndex = 0;
  int endIndex = startIndex + visibleItems;

  // Update control
  //--------------------------------------------------------------------
  if ((state != STATE_DISABLED) && !GuiIsLocked() && !GuiSliderIsDragging())
  {
    Vector2 mousePoint = GetMousePosition();

    // Check mouse inside list view
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
      // Check focused and selected item
      for (int i = 0; i < visibleItems; i++)
      {
        if (CheckCollisionPointRec(mousePoint, itemBounds))
        {
          if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
          {
            if (itemSelected == (startIndex + i)) itemSelected = -1;
            else itemSelected = startIndex + i;
          }
          break;
        }

        // Update item rectangle y position for next item
        itemBounds.y += (GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING));
      }

      if (useScrollBar)
      {
          int wheelMove = (int)GetMouseWheelMove();
          startIndex -= wheelMove;

          if (startIndex < 0) startIndex = 0;
          else if (startIndex > (count - visibleItems)) startIndex = count - visibleItems;

          endIndex = startIndex + visibleItems;
          if (endIndex > count) endIndex = count;
      }
    }

    // Reset item rectangle y to [0]
    itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
  }
  //--------------------------------------------------------------------

  return itemSelected;
}


// List View control (Draw controls)
int GuiListViewRender(Rectangle bounds, const char *text, int *scrollIndex, int itemSelected, int itemFocused)
{
    int result = 0;
    int itemCount = 0;
    const char **items = NULL;

    if (text != NULL) items = GuiTextSplit(text, ';', &itemCount, NULL);

    result = GuiListViewRenderEx(bounds, items, itemCount, scrollIndex, itemSelected, itemFocused);

    return result;
}

// List View control with extended parameters (Draw controls)
int GuiListViewRenderEx(Rectangle bounds, const char **text, int count, int *scrollIndex, int itemSelected, int itemFocused)
{
    int result = 0;
    GuiState state = (GuiState)GuiGetState();
    const float guiAlpha = GuiGetAlpha();

    // Check if we need a scroll bar
    bool useScrollBar = false;
    if ((GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING))*count > bounds.height) useScrollBar = true;

    // Define base item rectangle [0]
    Rectangle itemBounds = { 0 };
    itemBounds.x = bounds.x + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING);
    itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
    itemBounds.width = bounds.width - 2*GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) - GuiGetStyle(DEFAULT, BORDER_WIDTH);
    itemBounds.height = (float)GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
    if (useScrollBar) itemBounds.width -= GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);

    // Get items on the list
    int visibleItems = (int)bounds.height/(GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING));
    if (visibleItems > count) visibleItems = count;

    int startIndex = (scrollIndex == NULL)? 0 : *scrollIndex;
    if ((startIndex < 0) || (startIndex > (count - visibleItems))) startIndex = 0;
    int endIndex = startIndex + visibleItems;

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER + state*3)), guiAlpha), GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));     // Draw background

    // Draw visible items
    for (int i = 0; ((i < visibleItems) && (text != NULL)); i++)
    {
        if (state == STATE_DISABLED)
        {
            if ((startIndex + i) == itemSelected) GuiDrawRectangle(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_DISABLED)), guiAlpha), Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_DISABLED)), guiAlpha));

            GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_DISABLED)), guiAlpha));
        }
        else
        {
            if ((startIndex + i) == itemSelected)
            {
                // Draw item selected
                GuiDrawRectangle(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_PRESSED)), guiAlpha), Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_PRESSED)), guiAlpha));
                GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_PRESSED)), guiAlpha));
            }
            else if ((startIndex + i) == itemFocused)
            {
                // Draw item focused
                GuiDrawRectangle(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_FOCUSED)), guiAlpha), Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_FOCUSED)), guiAlpha));
                GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_FOCUSED)), guiAlpha));
            }
            else
            {
                // Draw item normal
                GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_NORMAL)), guiAlpha));
            }
        }

        // Update item rectangle y position for next item
        itemBounds.y += (GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING));
    }

    if (useScrollBar)
    {
        Rectangle scrollBarBounds = {
            bounds.x + bounds.width - GuiGetStyle(LISTVIEW, BORDER_WIDTH) - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH),
            bounds.y + GuiGetStyle(LISTVIEW, BORDER_WIDTH), (float)GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH),
            bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH)
        };

        // Calculate percentage of visible items and apply same percentage to scrollbar
        float percentVisible = (float)(endIndex - startIndex)/count;
        float sliderSize = bounds.height*percentVisible;

        int prevSliderSize = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE);   // Save default slider size
        int prevScrollSpeed = GuiGetStyle(SCROLLBAR, SCROLL_SPEED); // Save default scroll speed
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, (int)sliderSize);            // Change slider size
        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, count - visibleItems); // Change scroll speed

        startIndex = GuiScrollBar(scrollBarBounds, startIndex, 0, count - visibleItems);

        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, prevScrollSpeed); // Reset scroll speed to default
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, prevSliderSize); // Reset slider size to default
    }
    //--------------------------------------------------------------------

    return result;
}

// Label button control
int GuiLabelButtonEx(Rectangle bounds, const char *text, bool focused)
{
    GuiState state = (GuiState)GuiGetState();
    const float guiAlpha = GuiGetAlpha();
    int result = 0;

    // NOTE: We force bounds.width to be all text
    float textWidth = (float)GetTextWidth(text);
    if ((bounds.width - 2*GuiGetStyle(LABEL, BORDER_WIDTH) - 2*GuiGetStyle(LABEL, TEXT_PADDING)) < textWidth) bounds.width = textWidth + 2*GuiGetStyle(LABEL, BORDER_WIDTH) + 2*GuiGetStyle(LABEL, TEXT_PADDING) + 2;

    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && !GuiIsLocked() && !GuiSliderIsDragging())
    {
        Vector2 mousePoint = GetMousePosition();
        if (focused) state = STATE_FOCUSED;

        // Check checkbox state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = STATE_PRESSED;
            else state = STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) result = 1;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return result;
}


// Label button control
int GuiLabelButtonExActive(Rectangle bounds, const char *text, bool focused, bool active)
{
    GuiState state = (GuiState)GuiGetState();
    float guiAlpha = GuiGetAlpha();
    int result = 0;

    // NOTE: We force bounds.width to be all text
    float textWidth = (float)GetTextWidth(text);
    if ((bounds.width - 2*GuiGetStyle(LABEL, BORDER_WIDTH) - 2*GuiGetStyle(LABEL, TEXT_PADDING)) < textWidth) bounds.width = textWidth + 2*GuiGetStyle(LABEL, BORDER_WIDTH) + 2*GuiGetStyle(LABEL, TEXT_PADDING) + 2;

    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && !GuiIsLocked() && !GuiSliderIsDragging())
    {
        Vector2 mousePoint = GetMousePosition();
        if (focused) state = STATE_FOCUSED;

        // Check checkbox state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = STATE_PRESSED;
            else state = STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) result = 1;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state != STATE_DISABLED) {
      if (active || result == 1) {
        GuiDrawRectangle(bounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH),
                         Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_PRESSED)), guiAlpha),
                         Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_PRESSED)), guiAlpha));
      } else if ((focused || state == STATE_FOCUSED) && !GuiIsLocked()) {
        GuiDrawRectangle(bounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH),
                         Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_FOCUSED)), guiAlpha),
                         Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_FOCUSED)), guiAlpha));
      }
    }

    if (active || state == STATE_PRESSED || state == STATE_FOCUSED || state == STATE_NORMAL) guiAlpha = 1.0F;
    else if (state == STATE_DISABLED) guiAlpha = 0.6F;
    else guiAlpha = 0.25F;
    GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));

    if (state == STATE_FOCUSED) GuiTooltip(bounds);
    //--------------------------------------------------------------------

    return result;
}

bool GuiClickOnBox(Rectangle bounds) {
  Vector2 mousePoint = GetMousePosition();
  return CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
bool GuiClickOutsideBox(Rectangle bounds) {
  Vector2 mousePoint = GetMousePosition();
  return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mousePoint, bounds);
}

#endif      // RAYGUI_EXTRAS_IMPLEMENTATION
