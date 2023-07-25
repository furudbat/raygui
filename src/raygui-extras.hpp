#ifndef RAYGUI_EXTRAS_HPP
#define RAYGUI_EXTRAS_HPP

#include "raygui.h"
#include <span>

namespace raygui {

    inline static constexpr auto DefaultBorderWidth = 0x1;
    inline static constexpr auto DefaultTextPadding = 0x0;
    inline static constexpr auto DefaultTextAlignment = 0x1;
    inline static constexpr auto DefaultTextSize = 0xa;
    inline static constexpr auto DefaultTextSpacing = 0x1;

    namespace styles::rdefault {
        inline static constexpr auto ButtonBorderWidth = 0x2;
        inline static constexpr auto ToggleGroupPadding = 0x2;
        inline static constexpr auto SliderSliderWidth = 0xf;
        inline static constexpr auto SliderTextPadding = 0x5;
        inline static constexpr auto ProgressbarProgressPadding = 0x1;
        inline static constexpr auto CheckboxTextPadding = 0x5;
        inline static constexpr auto CheckboxTextAlignment = 0x2;
        inline static constexpr auto CheckboxCheckPadding = 0x1;
        inline static constexpr auto ComboboxComboButtonWidth = 0x1e;
        inline static constexpr auto ComboboxComboButtonPadding = 0x2;
        inline static constexpr auto DropdownboxArrowPadding = 0x10;
        inline static constexpr auto DropdownboxDropdownItemsPadding = 0x2;
        inline static constexpr auto TextboxTextPadding = 0x5;
        inline static constexpr auto TextboxTextAlignment = 0x0;
        inline static constexpr auto TextboxTextLinesPadding = 0x5;

        inline static constexpr auto ListviewListItemsHeight = 0x1e;
        inline static constexpr auto ListviewListItemsPadding = 0x2;
        inline static constexpr auto ListviewScrollbarWidth = 0xa;
        inline static constexpr auto ListviewScrollbarSide = 0x2;
    }

}

[[nodiscard]] inline constexpr auto GuiDefaultListHeight(int items) {
    return (raygui::styles::rdefault::ListviewListItemsHeight + raygui::styles::rdefault::ListviewListItemsPadding) * items;
}

struct GuiListComponent {
    int scroll_index{-1};
    int active{-1};
    int hover{-1};
};

RAYGUIAPI int GuiListView(Rectangle bounds, const char *text, GuiListComponent& menu);
RAYGUIAPI int GuiListViewEx(Rectangle bounds, const char **text, int count, GuiListComponent& menu);

RAYGUIAPI int GuiListViewUpdate(Rectangle bounds, int count, GuiListComponent& menu, bool unselectable = false);

RAYGUIAPI int GuiListViewRender(Rectangle bounds, const char *text, GuiListComponent& menu);
RAYGUIAPI int GuiListViewRenderEx(Rectangle bounds, std::span<const char *> text, GuiListComponent& menu);

RAYGUIAPI void GuiDebugLayoutRecs(std::span<const Rectangle> layoutRecs);

class GuiEnabler {
public:
    explicit GuiEnabler() : m_state(GuiGetState()) { GuiEnable(); }
    explicit GuiEnabler(bool enable) : m_state(GuiGetState()) {
        if (enable) {
            GuiEnable();
        }
    }

    ~GuiEnabler() {
        GuiSetState(m_state);
    }

private:
    const int m_state;
};
class GuiDisabler {
public:
    explicit GuiDisabler() : m_state(GuiGetState()) { GuiDisable(); }
    explicit GuiDisabler(bool disable) : m_state(GuiGetState()) {
        if (disable) {
            GuiDisable();
        }
    }

    ~GuiDisabler() {
        GuiSetState(m_state);
    }

private:
    const int m_state;
};


class GuiLocker {
public:
    explicit GuiLocker() : m_locked(GuiIsLocked()) { GuiLock(); }
    explicit GuiLocker(bool lock) : m_locked(GuiIsLocked()) {
        if (lock) {
            GuiLock();
        }
    }

    ~GuiLocker() {
        if (m_locked) GuiLock();
        else GuiUnlock();
    }

private:
    const bool m_locked;
};
class GuiUnlocker {
public:
    explicit GuiUnlocker() : m_locked(GuiIsLocked()) { GuiUnlock(); }
    explicit GuiUnlocker(bool unlock) : m_locked(GuiIsLocked()) {
        if (unlock) {
            GuiUnlock();
        }
    }

    ~GuiUnlocker() {
        if (m_locked) GuiLock();
        else GuiUnlock();
    }

private:
    const bool m_locked;
};

class GuiTempStyleChange {
public:
    GuiTempStyleChange(int control, int property, int value)
            : m_control(control), m_property(property), m_temp_style(GuiGetStyle(m_control, m_property)) {
        GuiSetStyle(m_control, m_property, value);
    }
    ~GuiTempStyleChange() { GuiSetStyle(m_control, m_property, m_temp_style); }

private:
    const int m_control;
    const int m_property;
    const int m_temp_style;
};

class GuiTempFontScaleChange {
public:
    explicit GuiTempFontScaleChange(int scale)
            : m_temp_style(GuiGetStyle(DEFAULT, TEXT_SIZE))
#if !defined(RAYGUI_NO_ICONS)
            , m_temp_icon_scale(GuiGetIconScale())
#endif
    {
        GuiSetStyle(DEFAULT, TEXT_SIZE, GuiGetStyle(DEFAULT, TEXT_SIZE) * scale);
#if !defined(RAYGUI_NO_ICONS)
        GuiSetIconScale(scale);
#endif
    }
    ~GuiTempFontScaleChange() {
        GuiSetStyle(DEFAULT, TEXT_SIZE, m_temp_style);
#if !defined(RAYGUI_NO_ICONS)
        GuiSetIconScale(m_temp_icon_scale);
#endif
    }

private:
    const int m_temp_style;
#if !defined(RAYGUI_NO_ICONS)
    const int m_temp_icon_scale;
#endif
};

#ifdef RAYGUI_EXTRAS_IMPLEMENTATION

void GuiDebugLayoutRecs(std::span<const Rectangle> layoutRecs) {
  GuiDebugLayoutRecs(layoutRecs.data(), layoutRecs.size());
}

int GuiListView(Rectangle bounds, const char *text, GuiListComponent& menu) {
    int result = 0;
    int itemCount = 0;
    const char **items = NULL;

    if (text != NULL) items = GuiTextSplit(text, ';', &itemCount, NULL);

    result = GuiListViewEx(bounds, items, itemCount, menu);

    return result;
}
int GuiListViewEx(Rectangle bounds, const char **text, int count, GuiListComponent& menu) {
  return GuiListViewEx(bounds, text, count, &menu.scroll_index, &menu.active, &menu.hover);
}

int GuiListViewUpdate(Rectangle bounds, int count, GuiListComponent& menu, bool unselectable) {
  GuiState state = (GuiState)GuiGetState();
  menu.hover = -1;
  int result = 0;

  // Check if we need a scroll bar
  const bool useScrollBar = (GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING))*count > bounds.height;

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

  int startIndex = menu.scroll_index;
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
          if (menu.hover == (startIndex + i)) menu.hover = i;
          else menu.hover = startIndex + i;
          if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
          {
            if (menu.active == (startIndex + i) && unselectable) menu.active = -1;
            else menu.active = startIndex + i;
            result = 1;
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

  return result;
}


int GuiListViewRender(Rectangle bounds, const char *text, GuiListComponent& menu) {
  return GuiListViewRender(bounds, text, &menu.scroll_index, menu.active, menu.hover);

}
int GuiListViewRenderEx(Rectangle bounds, std::span<const char *>text, GuiListComponent& menu) {
  return GuiListViewRenderEx(bounds, text.data(), text.size(), &menu.scroll_index, menu.active, menu.hover);
}

#endif

#endif