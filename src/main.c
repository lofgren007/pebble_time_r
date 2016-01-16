#include <pebble.h>
#define TIMECOLOR GColorWhite
#define BG_COLOR GColorBlack

#define vDOTS 48
#define DOTS_RADIUS 2
#define DOTS_COLOR1 GColorVividViolet
#define DOTS_COLOR2 GColorCyan
#define DOTS_COLOR3 GColorGreen
#define DOTS_COLOR4 GColorYellow
#define DOTS_COLOR5 GColorOrange
#define DOTS_COLOR6 GColorRed


//weather
  //#define KEY_TEMPERATURE 0
  //#define KEY_CONDITIONS 1


//Main Window
static Window *s_main_window;
//background
static Layer *s_canvas;
//time
static TextLayer *s_time_layer;
static GFont s_time_font;
//bitmap
// static BitmapLayer *s_background_layer;
// static GBitmap *s_background_bitmap;

//information 
static TextLayer *s_weather_layer;
static GFont s_weather_font;

// Calculate position [via angle] of circles to print - return result
static int32_t get_angle_for_dots(int dot_place) {
  // Progress through 12 hours, out of 360 degrees
  return (dot_place * 360) / vDOTS;
}


//time
static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%l:%M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


//draw dots
static void layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GRect frame = grect_inset(bounds, GEdgeInsets(3));
  
// Adjust geometry variables for inner ring
  frame = grect_inset(frame, GEdgeInsets(3 * DOTS_RADIUS));
  
// For loop that determines dot placement, color and prints dots
  for(int i = 0; i < vDOTS; i++) {
    int dot_angle = get_angle_for_dots(i);
    GPoint pos = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(dot_angle));
        if (dot_angle <= 60){ 
          graphics_context_set_fill_color(ctx, DOTS_COLOR1);
        }
        else if (dot_angle <= 120){ 
          graphics_context_set_fill_color(ctx, DOTS_COLOR2);
        }
        else if (dot_angle <= 180){ 
          graphics_context_set_fill_color(ctx, DOTS_COLOR3);
        }
        else if (dot_angle <= 240){ 
          graphics_context_set_fill_color(ctx, DOTS_COLOR4);
        }
        else if (dot_angle <= 300){ 
          graphics_context_set_fill_color(ctx, DOTS_COLOR5);
        }
        else { 
          graphics_context_set_fill_color(ctx, DOTS_COLOR6);
        }
    graphics_fill_circle(ctx, pos, DOTS_RADIUS);
  }
}


// Load Main Window Layer
static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //bitmap
  //-- Create GBitmap
  //    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SAMPLE_BACKGROUND);
  //-- Create BitmapLayer to display the GBitmap
  //    s_background_layer = bitmap_layer_create(bounds);
  //-- Set the bitmap onto the layer and add to the window
  //    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  //    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  //background
  //Create Canvas Layer
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, layer_update_proc);
  layer_add_child(window_layer, s_canvas);

  //time
  // Create the TextLayer with specific bounds
  // Set text box position and size
  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(45, 52), bounds.size.w, 85));
  // Create GFont and set variable
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HIGHERUP_REG_80));
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, TIMECOLOR);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  //--
  // [UNUSED] -- information layer
  //    s_weather_layer = text_layer_create(
  //      GRect(0, PBL_IF_ROUND_ELSE(125, 120), bounds.size.w, 25));
  // Style the text
  //    text_layer_set_background_color(s_weather_layer, GColorClear);
  //    text_layer_set_text_color(s_weather_layer, GColorBlack);
  //    text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  //    text_layer_set_text(s_weather_layer, "Loading...");
  // Create second custom font, apply it and add to Window
  //    s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DBJ_REGULAR_20));
  //    text_layer_set_font(s_weather_layer, s_weather_font);
  //    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  
}


// Unload and destroy layers
static void main_window_unload(Window *window) {
  // time
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  //Unload time fonts
  fonts_unload_custom_font(s_time_font);
  //background
  // Destroy Canvas layer
  layer_destroy(s_canvas);
  
  // Destroy GBitmap
  //   gbitmap_destroy(s_background_bitmap);
  // Destroy BitmapLayer
  //   bitmap_layer_destroy(s_background_layer);
  
  // Destroy information elements
  text_layer_destroy(s_weather_layer);
  fonts_unload_custom_font(s_weather_font);
}


//communications-callbacks
//    initially set to log to APP_LOG
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set main background colour
  window_set_background_color(s_main_window, BG_COLOR);
    
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  //time
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Make sure the time is displayed from the start
  update_time();
  
  //communications
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}


static void deinit() {
  // Destroy Main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}