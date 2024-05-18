/**
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2023 furudbat
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

#ifndef RAYGUI_EXTRAS_HPP
#define RAYGUI_EXTRAS_HPP

#include <span>

namespace raygui {

    namespace styles::rdefault {
        inline static constexpr auto BorderColorNormal = 0x838383ff;
        inline static constexpr auto BaseColorNormal = 0xc9c9c9ff;
        inline static constexpr auto TextColorNormal = 0x686868ff;
        inline static constexpr auto BorderColorFocused = 0x5bb2d9ff;
        inline static constexpr auto BaseColorFocused = 0xc9effeff;
        inline static constexpr auto TextColorFocused = 0x6c9bbcff;
        inline static constexpr auto BorderColorPressed = 0x0492c7ff;
        inline static constexpr auto BaseColorPressed = 0x97e8ffff;
        inline static constexpr auto TextColorPressed = 0x368bafff;
        inline static constexpr auto BorderColorDisabled = 0xb5c1c2ff;
        inline static constexpr auto BaseColorDisabled = 0xe6e9e9ff;
        inline static constexpr auto TextColorDisabled = 0xaeb7b8ff;
        inline static constexpr auto BorderWidth = 1;
        inline static constexpr auto TextPadding = 0;
        inline static constexpr auto TextAlignment = TEXT_ALIGN_CENTER;

        inline static constexpr auto TextSize = 10;
        inline static constexpr auto TextSpacing = 1;
        inline static constexpr auto LineColor = 0x90abb5ff;
        inline static constexpr auto BackgroundColor = 0xf5f5f5ff;
        inline static constexpr auto TextLineSpacing = 15;
        inline static constexpr auto TextAlignmentVertical = TEXT_ALIGN_MIDDLE;

        namespace label {
            inline static constexpr auto TextAlignment = TEXT_ALIGN_LEFT;
        }
        namespace button {
            inline static constexpr auto BorderWidth = 2;
        }
        namespace slider {
            inline static constexpr auto TextPadding = 4;
            inline static constexpr auto SliderWidth = 16;
            inline static constexpr auto SliderPadding = 1;
        }
        namespace progressbar {
            inline static constexpr auto TextPadding = 4;
            inline static constexpr auto ProgressPadding = 1;
        }
        namespace checkbox {
            inline static constexpr auto TextPadding = 4;
            inline static constexpr auto TextAlignment = TEXT_ALIGN_RIGHT;
        }
        namespace dropdown {
            inline static constexpr auto TextPadding = 0;
            inline static constexpr auto TextAlignment = TEXT_ALIGN_CENTER;
        }
        namespace textbox {
            inline static constexpr auto TextPadding = 4;
            inline static constexpr auto TextAlignment = TEXT_ALIGN_LEFT;
        }
        namespace valuebox {
            inline static constexpr auto TextPadding = 0;
            inline static constexpr auto TextAlignment = TEXT_ALIGN_LEFT;
        }
        namespace spinner {
            inline static constexpr auto TextPadding = 0;
            inline static constexpr auto TextAlignment = TEXT_ALIGN_LEFT;
        }
        namespace statusbar {
            inline static constexpr auto TextPadding = 8;
            inline static constexpr auto TextAlignment = TEXT_ALIGN_LEFT;
        }
        namespace toggle {
            inline static constexpr auto GroupPadding = 2;
        }
        namespace list_view {
            inline static constexpr auto ListItemsHeight = 28;
            inline static constexpr auto ListItemsSpacing = 2;
            inline static constexpr auto ScrollbarWidth = 12;
            inline static constexpr auto ScrollbarSide = SCROLLBAR_RIGHT_SIDE;
        }
    }

}

[[nodiscard]] inline constexpr auto GuiDefaultListHeight(int items) {
    return (raygui::styles::rdefault::list_view::ListItemsHeight + 2*raygui::styles::rdefault::list_view::ListItemsSpacing) * items;
}

struct GuiListComponent {
    int scroll_index{-1};
    int selected{-1};
    int hover{-1}; // focused
    int clicked_on{-1};
};

RAYGUIAPI int GuiListView(Rectangle bounds, const char *text, GuiListComponent& menu);
RAYGUIAPI int GuiListViewEx(Rectangle bounds, const char **text, int count, GuiListComponent& menu);

enum class gui_list_view_selectable_t : uint8_t {
    KeepSelection,
    ToggleSelection,
    DisableSelection,
};
struct GuiListViewUpdateOptions {
    gui_list_view_selectable_t selectable {gui_list_view_selectable_t::ToggleSelection};
};
RAYGUIAPI int GuiListViewUpdate(Rectangle bounds, int count, GuiListComponent& menu, GuiListViewUpdateOptions options = {});

RAYGUIAPI int GuiMenuListViewUpdate(Rectangle bounds, int count, GuiListComponent& menu, GuiListViewUpdateOptions options = {
        .selectable = gui_list_view_selectable_t::DisableSelection,
});

RAYGUIAPI int GuiListViewRender(Rectangle bounds, const char *text, GuiListComponent& menu, GuiListViewRenderExOptions options = {});
RAYGUIAPI int GuiListViewRenderEx(Rectangle bounds, std::span<const char *> text, GuiListComponent& menu, GuiListViewRenderExOptions options = {});

RAYGUIAPI int GuiMenuListViewRender(Rectangle bounds, const char *text, GuiListComponent& menu, GuiListViewRenderExOptions options = {
        .show_focused = true,
        .show_selected = true,
        .only_show_focused_or_selected = true,
});

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
    int m_state;
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
    int m_state;
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
    bool m_locked;
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
    bool m_locked;
};

class GuiTempStyleChange {
public:
    GuiTempStyleChange(int control, int property, int value)
            : m_control(control), m_property(property), m_temp_style(GuiGetStyle(m_control, m_property)) {
        GuiSetStyle(m_control, m_property, value);
    }
    ~GuiTempStyleChange() { GuiSetStyle(m_control, m_property, m_temp_style); }

private:
    int m_control;
    int m_property;
    int m_temp_style;
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
    int m_temp_style;
#if !defined(RAYGUI_NO_ICONS)
    int m_temp_icon_scale;
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
    return GuiListViewEx(bounds, text, count, &menu.scroll_index, &menu.selected, &menu.hover);
}

int GuiListViewUpdate(Rectangle bounds, int count, GuiListComponent& menu, GuiListViewUpdateOptions options) {
    int result = 0;
    GuiState state = (GuiState)GuiGetState();

    int itemFocused = menu.hover;
    int itemSelected = menu.selected;

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

    int startIndex = (menu.scroll_index < 0)? 0 : menu.scroll_index;
    if ((startIndex < 0) || (startIndex > (count - visibleItems))) startIndex = 0;
    int endIndex = startIndex + visibleItems;

    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && !GuiIsLocked() && !GuiControlExclusiveMode())
    {
        Vector2 mousePoint = GetMousePosition();

        // Check mouse inside list view
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            state = STATE_FOCUSED;

            // Check focused and selected item
            for (int i = 0; i < visibleItems; i++)
            {
                if (CheckCollisionPointRec(mousePoint, itemBounds))
                {
                    itemFocused = startIndex + i;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        if (itemSelected == (startIndex + i) && options.selectable == gui_list_view_selectable_t::ToggleSelection) itemSelected = -1;
                        else itemSelected = startIndex + i;
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
        else itemFocused = -1;

        // Reset item rectangle y to [0]
        itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
    }
    //--------------------------------------------------------------------

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

    if (options.selectable != gui_list_view_selectable_t::DisableSelection)
    {
        menu.selected = itemSelected;
    }
    if (result) {
        menu.clicked_on = itemSelected;
    }
    menu.hover = itemFocused;
    menu.scroll_index = startIndex;

    return result;
}


int GuiMenuListViewUpdate(Rectangle bounds, int count, GuiListComponent& menu, GuiListViewUpdateOptions options) {
    return GuiListViewUpdate(bounds, count, menu, options);
}

int GuiListViewRender(Rectangle bounds, const char *text, GuiListComponent& menu, GuiListViewRenderExOptions options) {
  return GuiListViewRender(bounds, text, &menu.scroll_index, menu.selected, menu.hover, options);

}
int GuiListViewRenderEx(Rectangle bounds, std::span<const char *>text, GuiListComponent& menu, GuiListViewRenderExOptions options) {
  return GuiListViewRenderEx(bounds, text.data(), text.size(), &menu.scroll_index, menu.selected, menu.hover, options);
}


RAYGUIAPI int GuiMenuListViewRender(Rectangle bounds, const char *text, GuiListComponent& menu, GuiListViewRenderExOptions options) {
  return GuiListViewRender(bounds, text, menu, options);
}

#endif

#endif