/* widgets.c
 *
 * Widgets app concept
 *
 * RebbleOS
 *
 * Author: Taylor <taylor@stanivision.com>
 */
 
#include "ngfxwrap.h"
#include "widgets.h"
#include "platform_config.h"
#include "platform_res.h"

extern void graphics_draw_bitmap_in_rect(GContext *, GBitmap *, GRect);

//const char *widgetssapp_name = "Widgets";

static void simple_update_proc(Layer *layer, GContext *nGContext);

static Window *s_widgetsMain_window;
static TextLayer *s_title_layer, *s_subTitle_layer;
static Layer *s_widgetsCanvas_layer;

uint8_t s_color_channels[3];

int current_widget = 0;

static void refresh(){
	layer_mark_dirty(s_widgetsCanvas_layer);
}

void widgets_up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	
  if(current_widget == 0){
	  //appmanager_load_homescreen(); 
  }else{
	  current_widget--;
	  refresh();
  }
}

void widgets_select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	
	appmanager_app_start(widgets[current_widget].app_attached);
}

void widgets_down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	
	current_widget++;
	refresh();
  
}

void widgets_back_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	
	current_widget = 0;
	//appmanager_load_homescreen();
  
}

void widgets_config_provider(Window *window) {
	window_single_click_subscribe(BUTTON_ID_UP, widgets_up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, widgets_select_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, widgets_down_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_BACK, widgets_back_single_click_handler);
}

static void widgetssapp_window_load(Window *window, GContext *context)
{
    Layer *window_layer = window_get_root_layer(s_widgetsMain_window);
    GRect bounds = layer_get_bounds(window_layer);
	window_set_background_color(s_widgetsMain_window, GColorLightGray);
	
	//Dummy values for widgets
	widgets[0].titles = "Music";
	widgets[0].subtitles = "Heaven Can Wait - Meatloaf";
	widgets[0].background_color = GColorOrange;
	widgets[0].icon = gbitmap_create_with_resource(RESOURCE_ID_MUSIC_PAUSE);
	widgets[0].app_attached = "easteregg";
	
	widgets[1].titles = "Weather";
	widgets[1].subtitles = "70° - Cloudy";
	widgets[1].background_color = GColorPictonBlue;
	widgets[1].icon = gbitmap_create_with_resource(RESOURCE_ID_UNKNOWN);
	
	widgets[2].titles = "Notifications";
	widgets[2].subtitles = "Ben - You got mil...";
	widgets[2].background_color = GColorSunsetOrange;
	widgets[2].icon = gbitmap_create_with_resource(RESOURCE_ID_ALARM_BELL_RINGING);
	
	// Create canvas layer
	s_widgetsCanvas_layer = layer_create(bounds);
    layer_set_update_proc(s_widgetsCanvas_layer, simple_update_proc);
    layer_add_child(window_layer, s_widgetsCanvas_layer);
	
	layer_mark_dirty(s_widgetsCanvas_layer);
		
}

static void simple_update_proc(Layer *layer, GContext *nGContext)
{   
    GRect bounds = layer_get_unobstructed_bounds(layer);
    graphics_context_set_fill_color(nGContext, GColorWhite);
	
	//Wid-gets
	graphics_context_set_fill_color(nGContext, widgets[current_widget].background_color);
    graphics_fill_rect(nGContext, GRect(0, 0, bounds.size.w, 150), 0, GCornerNone);
	
	graphics_context_set_fill_color(nGContext, widgets[current_widget+1].background_color);
    graphics_fill_rect(nGContext, GRect(0, 150, bounds.size.w, 40), 0, GCornerNone);
    
    graphics_context_set_stroke_color(nGContext, GColorBlack);
    graphics_context_set_stroke_width(nGContext, 5);
    //graphics_context_set_antialiased(nGContext, ANTIALIASING);
	
	graphics_context_set_stroke_color(nGContext, GColorWhite);
	for(int i = 0; i < bounds.size.w; i++)
	{
		n_graphics_draw_pixel(nGContext, n_GPoint(i, 149));
		n_graphics_draw_pixel(nGContext, n_GPoint(i, 150));
	}
	
	graphics_context_set_text_color(nGContext, GColorBlack);
	
	graphics_draw_text(nGContext, widgets[current_widget].titles, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(0, 20, bounds.size.w, 20),
                               GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
	graphics_draw_text(nGContext, widgets[current_widget].subtitles, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(0, 120, bounds.size.w, 20),
                               GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
	graphics_draw_text(nGContext, widgets[current_widget+1].titles, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(0, bounds.size.h - 18, bounds.size.w, 20),
                               GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
	
	if (widgets[current_widget].icon != NULL)
    	{
		GSize icon_size = widgets[current_widget].icon->raw_bitmap_size;
        	graphics_draw_bitmap_in_rect(nGContext, widgets[current_widget].icon, GRect((bounds.size.w / 2)-14, (bounds.size.h / 2)-10, bounds.size.w, bounds.size.h));
	}


}

static void widgetssapp_window_unload(Window *window)
{
	text_layer_destroy(s_title_layer);
	text_layer_destroy(s_subTitle_layer);
	layer_destroy(s_widgetsCanvas_layer);
}

void widgetssapp_init(void)
{
    printf("init\n");
    s_widgetsMain_window = window_create();

    window_set_window_handlers(s_widgetsMain_window, (WindowHandlers) {
        .load = widgetssapp_window_load,
        .unload = widgetssapp_window_unload,
    });
	
	window_set_click_config_provider(s_widgetsMain_window, (ClickConfigProvider) widgets_config_provider);

    window_stack_push(s_widgetsMain_window, true);

}

void widgetssapp_deinit(void)
{
    window_destroy(s_widgetsMain_window);
}

void widgetsapp_main(void)
{
    widgetssapp_init();
    app_event_loop();
    widgetssapp_deinit();
}