/* Copyright 2021 Joe Maples <joe@maples.dev>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * NOTE: all _kb() functions should have the _user() function run at it's top!!
 */

#include QMK_KEYBOARD_H
#include "print.h"

// Enumeration of custom keycodes
enum custom_keycodes {
  SO_KEY = SAFE_RANGE,
  C_KEY, 
  V_KEY,
};
#define _BASE 0
#define _L_ONE 1
#define _L_TWO 2

// Actual keyboard layout
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // [0] = LAYOUT(KC_LCTL, LT(1, KC_C), KC_V),
    [_BASE] = LAYOUT(SO_KEY, C_KEY, V_KEY),
    [_L_ONE] = LAYOUT(_______, _______, _______),
    // [1] = LAYOUT(LCTL(KC_X), _______, KC_A),

};

/*          Memory / Storage functionality          */
static uint16_t timer;  // uint64_t
typedef union {
  uint_fast64_t raw;
  struct {
    struct {
      bool so;
      bool c;
      bool v;
      char log[4];
    } key_pressed;
    bool action_taken;
    deferred_token pending_action;
  }; 
} user_config_t; user_config_t user_config;
unsigned short int n_pressed(void) {
  return (user_config.key_pressed.so + user_config.key_pressed.c + user_config.key_pressed.v);
}
void print_uconfig(user_config_t conf) {   //
  uprintf(" / - - Config: so=%d; c=%d; v=%d; Action=%d; Pending=%d; raw:%u\n", conf.key_pressed.so, conf.key_pressed.c, conf.key_pressed.v, conf.action_taken, conf.pending_action, conf.raw);
  // uprintf(" \\ - - Pending:%d\n",  conf.pending_action);
  // uprintf(" \\ - - layer state?  %u and %d\n", layer_state, layer_state);
}  
void log_key(char c_) {
  for (unsigned int i = sizeof(user_config.key_pressed.log) - 1; i > 0; i = i - 1) {
    user_config.key_pressed.log[i] = user_config.key_pressed.log[i - 1];
  }
  user_config.key_pressed.log[0] = c_;
  xprintf("Keystroke log: '%s'\n", user_config.key_pressed.log);
  eeconfig_update_user(user_config.raw); 
}
void reset_config(void)  {
  // for (unsigned int i = 0; i < sizeof(user_config.key_pressed.log); i = i + 1) {   //  for (unsigned int i = sizeof(user_config.key_pressed.log); i > 0; i = i - 1) {
  //   user_config.key_pressed.log[i] = '_';
  // }
  log_key('_');
  user_config.pending_action = 0;
  user_config.key_pressed.so = false;  
  user_config.key_pressed.c = false; 
  user_config.key_pressed.v = false; 
  user_config.action_taken = false;
  // user_config.char_log[] = "----------";
  eeconfig_update_user(user_config.raw); // Write default value to EEPROM now
}
void eeconfig_init_user(void) {  // EEPROM is getting reset!
  user_config.raw = 0; 
  for (unsigned int i = 0; i < sizeof(user_config.key_pressed.log); i = i + 1) {   //  for (unsigned int i = sizeof(user_config.key_pressed.log); i > 0; i = i - 1) {
    user_config.key_pressed.log[i] = '_';
  }
  reset_config(user_config);
}



uint32_t delay_copy_search(uint32_t trigger_time, void *cb_arg) {  //void *cb_arg
    // uprintf("in delay_copy_search, trigger_time: %u; cb_arg: %s\n", trigger_time, typeid(conf));  // KC_LEFT_GUI  // KC_APPLICATION
    // uprintf("  ####in delay_copy_search, trigger_time: %u\n", trigger_time);  // KC_LEFT_GUI  // KC_APPLICATION
    // // copy_search(cb_arg);
    // print("  ### DELAY ACTION OCCURS HERE ###\n");
    // print_uconfig();
    // print(" ####  in delay_copy_search, returning 0...\n");
    ////////////////////////////
    print("\n --------- THIS IS THE DEFFERED ACTION ---------\n");
    // copy_search(user_config);
    user_config.pending_action = 0;
    user_config.action_taken = true;
    eeconfig_update_user(user_config.raw);
    return 0;
}

bool copy_search(user_config_t conf) {
  timer = timer_read();  // Start it   // if (timer_elapsed(key_timer) < 100) {}
  if (conf.key_pressed.so) {
    tap_code16(LCTL(KC_C));
  } 
  // tap_code_delay(KC_LEFT_GUI, 1500);
  clear_mods();
  uprintf("%u Clicking windows button...\n", timer_elapsed(timer));  // KC_LEFT_GUI  // KC_APPLICATION
  SEND_STRING(SS_TAP(X_WWW_SEARCH) SS_DELAY(400) "Chrome" SS_DELAY(225) SS_TAP(X_ENTER) SS_DELAY(500));
  // tap_code_delay(KC_RIGHT_GUI, 400);
  // uprintf("%u Typing 'Google Chrome'...\n", timer_elapsed(timer));
  // SEND_STRING("Chrome" SS_DELAY(225) );
  // uprintf("%u Hitting 'ENTER'...\n", timer_elapsed(timer));
  // tap_code_delay(KC_ENTER, 500);
  uprintf("%u Hitting 'CTRL+V'...\n", timer_elapsed(timer));
  tap_code16(LCTL(KC_V)); tap_code(KC_ENTER);
  return true;
}



/*          Event handling functions          */

// Called when a key is pressed before it's handled, if return is `false`, then it will skip the normal evaluation of the key (i.e. it won't process that as a press of a 'C' character, etc.) also: process_record_kb()?
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // print("in process_record_user print\n");
  // println("in process_record_user println");
  // xprintf("in process_record_user xprint\n");
  // uprintf("in post init user uprintf str:'%s' \n", user_config.key_pressed.log);

  // If console is enabled, it will print the matrix position and status of each key pressed
  // #ifdef CONSOLE_ENABLE
  //   uprintf("KL: kc: 0x%04X, CxR: %ux%u; pressed: %b, time: %u, interrupt: %b, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
  // #endif 
 
  if (record->event.pressed) {
    xprintf("");
  } else {
    // println("Key released, config stuff:");
    print_uconfig();
    println("");
    // print("\n");
    if ( user_config.pending_action == 0 && user_config.action_taken == false && user_config.key_pressed.v == true && user_config.key_pressed.c == true) {
      deferred_token my_token = defer_exec(1500, delay_copy_search, NULL);
      uprintln("Deferred action queued to start in 1.5S...");
      user_config.pending_action = my_token; log_key('|');
    }
  }
  // if ( (record->event.pressed) == false && user_config.action_taken == false && user_config.key_pressed.v == true && user_config.key_pressed.c == true) {
  //   uprintf("char log contents: %s\n", user_config.key_pressed.log);
  //   uprintf("char log size: %u\n", sizeof(user_config.key_pressed.log));
    
  //   deferred_token my_token = defer_exec(1500, delay_copy_search, NULL);

  //   print("\n\n\n timer started; print_uconfig: \n");
  //   print_uconfig();
  //   print("starting the  delay_copy_search body of code\n");
  //   timer = timer_read();  // Start it  
  //   // uprintf("user_config type: %s\n", user_config);

  //   uprintf("\n%u Calling `delay_copy_search`...\n/----------------------------\\\n", timer_elapsed(timer));  // record->event.pressed && 
  //   // delay_copy_search(1500, user_config);
  //   // user_config.pending_action = true;   //KC_WWW_SEARCH
  //   // deferred_token my_token = defer_exec(1500, delay_copy_search, *user_config);
  //   user_config.pending_action = true;
  //   deferred_token my_token = defer_exec(1500, delay_copy_search, NULL);
  //   uprintf("%u defer statement init finished here\n", timer_elapsed(timer));
  //   extend_deferred_exec(my_token, 500);
  //   uprintf("%u extend deferred statement here\n", timer_elapsed(timer));
    
  //   tap_code_delay(KC_LEFT_CTRL, 2000);
  //   uprintf("%u finished waiting 2000ms\n", timer_elapsed(timer));
  //   tap_code_delay(KC_LEFT_CTRL, 500);
  //   uprintf("%u finished waiting 500 additional ms\n", timer_elapsed(timer));

  //   // uprintf("returned value from  defer_exec, my_token=%u", my_token);
  //   uprintf("\\----------------------------/\n%u Finished running `delay_copy_search`...\n", timer_elapsed(timer));
  //   tap_code_delay(KC_LEFT_CTRL, 1900);


  // } 
  
  /*     Key Presses   */
  switch (keycode) {
    case SO_KEY:
      if (record->event.pressed) {
        uprintf("char log contents: %s\n", user_config.key_pressed.log);
        // print_uconfig();
        // unsigned short int before = n_pressed();
        user_config.key_pressed.so = true; //eeconfig_update_user(user_config.raw);
        register_code(KC_LCTL); log_key('*');
        // unsigned short int after = n_pressed();
        // uprintf("  (( pressed before: %u, after: %u \n", before, after);
        println("/------ SO_KEY was pressed ------\\");
        // print_uconfig();
        // SEND_STRING(" sending string SO key \n");
      } else {
        unregister_code(KC_LCTL); 
        user_config.key_pressed.so = false; //eeconfig_update_user(user_config.raw);
        println("\\------ SO_KEY was released ------/");
        // print_uconfig();
      }
      // return false; // Skip all further processing of this key
      break;   // return true; // Let QMK send the enter press/release events
    case C_KEY:
      if (record->event.pressed) {
        user_config.key_pressed.c = true; 
        (user_config.key_pressed.so == true)?log_key('C'):log_key('c');
        // log_key('c'); //eeconfig_update_user(user_config.raw);
        // register_code(KC_C); print("KC_C was pressed\n");
      } else {
        if ( user_config.action_taken == false && user_config.pending_action == 0) {
          tap_code(KC_C); //user_config.action_taken = true;  // unregister_code(KC_C); 
          println("  - KC_C was *tapped*");
        }
        user_config.key_pressed.c = false; //eeconfig_update_user(user_config.raw);
        // print("      \\_ KC_C was released\n");
      }
      break;   // return true; // Let QMK send the enter press/release events
    case V_KEY:
      if (record->event.pressed) {
        user_config.key_pressed.v = true; //log_key('v'); //eeconfig_update_user(user_config.raw);
        (user_config.key_pressed.so == true)?log_key('V'):log_key('v'); 
        // register_code(KC_V); print("KC_V was pressed\n");
      } else {
        if ( user_config.action_taken == false && user_config.pending_action == 0 ) {
          // if user_config.key_pressed.
          tap_code(KC_V); //user_config.action_taken = true;  // unregister_code(KC_C); 
          println("  - KC_V was *tapped*");
        }
        user_config.key_pressed.v = false; // print("      \\_ KC_V was released\n");
      }
      break;   // return true; // Let QMK send the enter press/release events
    default:
      if (record->event.pressed) {
        uprintf(" !!!!!!!!! key pressed not defined for switch, keycode: 0x%04X\n", keycode);
      }
      // eeconfig_update_user(user_config.raw);
      // return true; // Process all other keycodes normally
      break;   
  }; eeconfig_update_user(user_config.raw);
  unsigned short int n_ = n_pressed();

  /*   Combos / Behaviors   */
  if (record->event.pressed) {   // on click 
    print("");
  } else {  // On release
    // uprintf("char log contents: %s\n", user_config.key_pressed.log);
    /*  Multi-press definitions   */ 
    char c0 = user_config.key_pressed.log[0];
    char c1 = user_config.key_pressed.log[1];
    char c2 = user_config.key_pressed.log[2];
    // char c3 = user_config.key_pressed.log[3];
    if ( user_config.pending_action != 0 ) {
      extend_deferred_exec(user_config.pending_action, 500);
      uprintln("   Extending deferred action deadline by 0.5s");
    }  
    // if ((c0 == 'v' || c0 == 'V') && (c1 == 'v' || c1 == 'V') && (c2 == 'v' || c2 == 'V')) {
    if ((c0 == 'V') && (c1 == 'V') && (c2 == 'V')) {
      cancel_deferred_exec(user_config.pending_action); user_config.pending_action = 0;
      uprintln(" ~ ~ ~ Three V-combo activated!"); user_config.action_taken = true;
    } 
  }
  // // if (user_config.action_taken == false && n_ == 3) {
  // if (user_config.action_taken == false && user_config.key_pressed.v == true && user_config.key_pressed.c == true) {
  //   print("\nAbout to start `copy_search`...\n/----------------------------\\\n");  // record->event.pressed && 
  //   copy_search();
  //   print("\\----------------------------/\nFinished running `copy_search`...\n");
  //   eeconfig_update_user(user_config.raw);
  // } else 
  /*   Cleanup Code   */
  if ( n_ == 0 && user_config.pending_action == 0 ) {
    // print("    [No Buttons Pressed]\n");
    xprintf("    [No Buttons Pressed; No Pending Actions]\n"); print_uconfig();
    reset_config(user_config); // print_uconfig();
    xprintf("    Resetting user_config...\n"); print_uconfig();
  }
  // eeconfig_update_user(user_config.raw)
  // print("  !!! Here, about to return false \n");
  return false;
}

// Enable debugging by default at beginning of run
void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  debug_enable=true;
  //debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;

  // Read the user config from EEPROM
  user_config.raw = eeconfig_read_user();
  // reset_config();
}
