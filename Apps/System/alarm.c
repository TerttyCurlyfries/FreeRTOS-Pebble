/* alarm.c
 * 
 * RebbleOS
 *
 * Author: Taylor E. <taylor@stanivision.com>
 */

#include "rebbleos.h"
#include "alarmapp.h"
#include "menu.h"
#include "status_bar_layer.h"
#include "platform_config.h"
#include "platform_res.h"

static Window *s_main_window;
static Menu *alarm_menu;

int alarms[9]; //fight me
int activeAlarms = 0; //seriously, fight me

static Window *s_alarm_select_window;
static Layer *s_aboutCanvas_layer;
static ScrollLayer *s_about_scroll;
static void about_update_proc(Layer *layer, GContext *nGContext);

StatusBarLayer *status_bar;

typedef struct {
    uint8_t hours;
    uint8_t minutes;
} Time;

static Time s_last_time;

static MenuItems* add_alarm_item_selected(const MenuItem *item)
{
    //Open time selection menu
	//Note: Pebble selection format is HH-MM-AM/PM
	return NULL;
}

static MenuItems* edit_alarm_item_selected(const MenuItem *item)
{
    appmanager_app_start(item->text);
    return NULL;
}

static MenuItems* watch_list_item_selected(const MenuItem *item) {
    MenuItems *items = menu_items_create(16);
    // loop through all apps
    App *node = app_manager_get_apps_head();
    while(node)
    {
        if ((!strcmp(node->name, "System")) ||
            //             (!strcmp(node->name, "91 Dub 4.0")) ||
            (!strcmp(node->name, "watchface")))
        {
            node = node->next;
            continue;
        }
        menu_items_add(items, MenuItem(node->name, NULL, RESOURCE_ID_CLOCK, app_item_selected));

        node = node->next;
    }
    return items;
}

static void exit_to_watchface(struct Menu *menu, void *context)
{
    // Exit to watchface
    appmanager_app_start("Simple");
}

static void alarmapp_window_load(Window *window)
{
    Layer *window_layer = window_get_root_layer(s_main_window);
#ifdef PBL_RECT
    alarm_menu = menu_create(GRect(0, 16, DISPLAY_COLS, DISPLAY_ROWS - 16));
#else
    // Let the menu draw behind the statusbar so it is perfectly centered
    alarm_menu = menu_create(GRect(0, 0, DISPLAY_COLS, DISPLAY_ROWS));
#endif
    menu_set_callbacks(alarm_menu, alarm_menu, (MenuCallbacks) {
        .on_menu_exit = exit_to_watchface
    });
    layer_add_child(window_layer, menu_get_layer(alarm_menu));

    menu_set_click_config_onto_window(alarm_menu, window);

    MenuItems *items = menu_items_create(11);
    menu_items_add(items, MenuItem("+", NULL, RESOURCE_ID_CLOCK, add_alarm_item_selected));
	
	for(int i = 0; i < activeAlarms; i++){
		//add buffer for num to be converted to string
		menu_items_add(items, MenuItem(ARRAY BUFF, NULL, RESOURCE_ID_SPANNER, edit_alarm_item_selected));
	}
	
    menu_set_items(alarm_menu, items);

#ifdef PBL_RECT
    // Status bar is only on rectangular pebbles
    status_bar = status_bar_layer_create();
    layer_add_child(menu_get_layer(alarm_menu), status_bar_layer_get_layer(status_bar));
#endif

    //tick_timer_service_subscribe(MINUTE_UNIT, prv_tick_handler);
}

static void alarmapp_window_unload(Window *window)
{
    menu_destroy(alarm_menu);
}

static void alarm_select_window_load(Window *window)
{
    
    Layer *window_layer = window_get_root_layer(s_alarm_select_window);
    GRect bounds = layer_get_bounds(window_layer);
    
    status_bar = status_bar_layer_create();
    status_bar_layer_set_separator_mode(status_bar, StatusBarLayerSeparatorModeDotted);
    status_bar_layer_set_colors(status_bar, GColorRed, GColorWhite);
    status_bar_layer_set_text(status_bar, "About RebbleOS");
    
    s_about_scroll = scroll_layer_create(bounds);
    scroll_layer_set_click_config_onto_window(s_about_scroll, window);
	
	s_aboutCanvas_layer = layer_create(bounds);
    layer_set_update_proc(s_aboutCanvas_layer, about_update_proc);
	scroll_layer_add_child(s_about_scroll, s_aboutCanvas_layer);	
	layer_mark_dirty(s_aboutCanvas_layer);
	
	layer_add_child(window_layer, scroll_layer_get_layer(s_about_scroll));
    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));

}

static void about_update_proc(Layer *layer, GContext *nGContext)
{  
	
	GRect bounds = layer_get_unobstructed_bounds(layer);
	graphics_context_set_text_color(nGContext, GColorBlack);
	
	graphics_draw_text(nGContext, "Version:", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect((bounds.size.w/2)-70, (bounds.size.h/2)-10, 140, 20),
                               GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
	
	graphics_draw_text(nGContext, git_version, fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect((bounds.size.w/2)-70, (bounds.size.h/2)+5, 140, 20),
                               GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
	
	graphics_draw_text(nGContext, "Join us!", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect((bounds.size.w/2)-70, (bounds.size.h/2)+20, 140, 20),
                               GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
	
	graphics_draw_text(nGContext, "discord.gg/aRUAYFN", fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect((bounds.size.w/2)-70, (bounds.size.h/2)+35, 140, 20),
                               GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
	
    graphics_draw_bitmap_in_rect(nGContext, gbitmap_create_with_resource(RESOURCE_ID_REBBLE_LOGO_DARK), GRect((bounds.size.w/2)-17, (bounds.size.h/2)-63, 34, 53));
	graphics_draw_bitmap_in_rect(nGContext, gbitmap_create_with_resource(RESOURCE_ID_TO_MOON), GRect((bounds.size.w/2)-8, (bounds.size.h/2)+60, 19, 19));
	
}

static void alarm_select_window_unload(Window *window)
{

}

void alarmapp_init(void)
{
    s_main_window = window_create();

    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = alarmapp_window_load,
        .unload = alarmapp_window_unload,
    });

    s_alarm_select_window = window_create();
    
    window_set_window_handlers(s_alarm_select_window, (WindowHandlers) {
        .load = alarm_select_window_load,
        .unload = alarm_select_window_unload,
    });
    
    window_stack_push(s_main_window, true);
}

void alarmapp_deinit(void)
{
    window_destroy(s_main_window);
    window_destroy(s_alarm_select_window);
}

void alarmapp_main(void)
{
    alarmapp_init();
    app_event_loop();
    alarmapp_deinit();
}

void alarmapp_tick(void)
{
    struct tm *tick_time = rbl_get_tm();

    // Store time
    s_last_time.hours = tick_time->tm_hour;
    s_last_time.hours -= (s_last_time.hours > 12) ? 12 : 0;
    s_last_time.minutes = tick_time->tm_min;
}