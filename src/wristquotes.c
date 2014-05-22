#include <pebble.h>
 
Window* window;
TextLayer *header, *quote;
char quote_buffer[500];

//key that connects javascript app to c app
enum {
	KEY_QUOTE = 0,
};

//processes the information sent from javascript app
void process_tuple(Tuple *t)
{
	int key = t->key;
	int value = t->value->int32;
	char string_value[500];
	strcpy(string_value, t->value->cstring);
	switch(key) {
		case KEY_QUOTE:
			snprintf(quote_buffer, 300, "%s", string_value);
			text_layer_set_text(quote, (char*) &quote_buffer);
			break;
	}
}

//receives information from javascript app
static void in_received_handler(DictionaryIterator *iter, void *context) 
{
	(void) context;	
	//gets the information
	Tuple *t = dict_read_first(iter);
	if(t)
	{
		process_tuple(t);
	}	
	//checks until all information has been passed
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)
		{
			process_tuple(t);
		}
	}
}

//initializes text layers
static TextLayer* init_text_layer(GRect quote, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
	TextLayer *layer = text_layer_create(quote);
	text_layer_set_text_color(layer, colour);
	text_layer_set_background_color(layer, background);
	text_layer_set_font(layer, fonts_get_system_font(res_id));
	text_layer_set_text_alignment(layer, alignment);
	return layer;
}

//function to communicate back to javascript app when new data should be passed
void send_int(uint8_t key, uint8_t cmd)
{
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
 	
 	Tuplet value = TupletInteger(key, cmd);
 	dict_write_tuplet(iter, &value);
 	
 	app_message_outbox_send();
}

//watch will communicate back to javascript app when middle button is pressed
void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
  send_int(5, 5);
}

//configures communication with buttons on watch
void click_config_provider(void *context)
{
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

//loads the watch app with text layers
void window_load(Window *window)
{
	header = init_text_layer(GRect(5, 0, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_DROID_18", GTextAlignmentLeft);
	text_layer_set_text(header, "QUICKQUOTES");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(header));

	quote = init_text_layer(GRect(5, 15, 144, 300), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_16", GTextAlignmentLeft);
	text_layer_set_text(quote, "quote: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(quote));
}
 
//method to unload watch app
void window_unload(Window *window)
{	
	text_layer_destroy(header);
	text_layer_destroy(quote);

}

//initializes and configures app and loads windows
void init()
{
	window = window_create();
	WindowHandlers handlers = {
		.load = window_load,
		.unload = window_unload
	};
	window_set_window_handlers(window, handlers);
	app_message_register_inbox_received(in_received_handler);					 
	app_message_open(512, 512);
  window_set_click_config_provider(window, click_config_provider);
	window_stack_push(window, true);
}
 
//deinitializes app
void deinit()
{
	window_destroy(window);
}

//main function
int main(void)
{
	init();
	app_event_loop();
	deinit();
}
