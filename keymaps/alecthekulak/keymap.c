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
// #define _L_ONE 1
// #define _L_TWO 2

// Actual keyboard layout
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // [0] = LAYOUT(KC_LCTL, LT(1, KC_C), KC_V),
    // [_L_ONE] = LAYOUT(_______, _______, _______),
    // [1] = LAYOUT(LCTL(KC_X), _______, KC_A),
    [_BASE] = LAYOUT(SO_KEY, C_KEY, V_KEY),

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
      char log[6];
    } key_pressed;
    bool action_taken;
    deferred_token pending_action;
  }; 
} user_config_t; static user_config_t user_config;
unsigned short int n_pressed(void) {
  return (user_config.key_pressed.so + user_config.key_pressed.c + user_config.key_pressed.v);
}
void print_uconfig(void) {   //
  uprintf(" <<CONF>> so=%d; c=%d; v=%d; Action=%d; Pending=%d\n", user_config.key_pressed.so, user_config.key_pressed.c, user_config.key_pressed.v, user_config.action_taken, user_config.pending_action);  // ; raw:%u", user_config.raw
  // uprintf(" \\ - - layer state?  %u and %d\n", layer_state, layer_state);
}  
void log_key(char c_) {
  for (unsigned int i = sizeof(user_config.key_pressed.log) - 1; i > 0; i = i - 1) {
    user_config.key_pressed.log[i] = user_config.key_pressed.log[i - 1];
  }
  user_config.key_pressed.log[0] = c_; eeconfig_update_user(user_config.raw); 
  if (c_ != '-') {
    xprintf("Keystroke log: '%s'\n", user_config.key_pressed.log);
  }
}
void reset_config(void)  {
  // for (unsigned int i = 0; i < sizeof(user_config.key_pressed.log); i = i + 1) {   //  for (unsigned int i = sizeof(user_config.key_pressed.log); i > 0; i = i - 1) {
  //   user_config.key_pressed.log[i] = '_';
  // }
  log_key('-');
  user_config.pending_action = INVALID_DEFERRED_TOKEN;
  user_config.key_pressed.so = false;  
  user_config.key_pressed.c = false; 
  user_config.key_pressed.v = false; 
  user_config.action_taken = false;
  eeconfig_update_user(user_config.raw); // Write default value to EEPROM now
}
void eeconfig_init_user(void) {  // EEPROM is getting reset!
  user_config.raw = 0; 
  for (unsigned int i = 0; i < sizeof(user_config.key_pressed.log); i = i + 1) {   //  for (unsigned int i = sizeof(user_config.key_pressed.log); i > 0; i = i - 1) {
    user_config.key_pressed.log[i] = '-';
  }; reset_config();
}


// bool copy_search(user_config_t conf) {
void copy_search(void) {  // https://github.com/tmk/tmk_keyboard/issues/332#:~:text=No%20action-,WSCH,-Focus%20address%20bar
  timer = timer_read();  // Start it   // if (timer_elapsed(key_timer) < 100) {}
  // if (conf.key_pressed.so == false) {
  //   tap_code16(LCTL(KC_C));
  // } 
  // tap_code_delay(KC_LEFT_GUI, 1500);
  clear_mods();
  uprintf("%u Clicking windows button...\n", timer_elapsed(timer));  // KC_LEFT_GUI  
  // // KC_APPLICATION -> "Windows Context Menu" -> Right clicking on a webpage, etc. 
  // SEND_STRING(SS_TAP(X_WWW_SEARCH) SS_DELAY(400) "Chrome" SS_DELAY(225) SS_TAP(X_ENTER) SS_DELAY(500));
  // tap_code_delay(KC_RIGHT_GUI, 400);
  tap_code_delay(KC_WWW_SEARCH, 400);
  uprintf("%u Typing 'Google Chrome'...\n", timer_elapsed(timer));
  SEND_STRING("Chrome" SS_DELAY(225) SS_TAP(X_ENTER) "" SS_DELAY(500));
  // uprintf("%u Hitting 'ENTER'...\n", timer_elapsed(timer));
  // tap_code_delay(KC_ENTER, 500);
  uprintf("%u Hitting 'CTRL+V'...\n", timer_elapsed(timer));
  tap_code16(LCTL(KC_V)); 
  uprintf("%u Hitting 'ENTER'...\n", timer_elapsed(timer));
  tap_code(KC_ENTER);
  uprintf("%u Finishing up...\n", timer_elapsed(timer));
  // return true;
}
uint32_t _delay_copy_search(uint32_t trigger_time, void *cb_arg) {  //void *cb_arg
    // uprintf("in delay_copy_search, trigger_time: %u; cb_arg: %s\n", trigger_time, typeid(conf));  // KC_LEFT_GUI  // KC_APPLICATION
    // uprintf("  ####in delay_copy_search, trigger_time: %u\n", trigger_time);  // KC_LEFT_GUI  // KC_APPLICATION
    // // copy_search(cb_arg);
    // print("  ### DELAY ACTION OCCURS HERE ###\n");
    // print(" ####  in delay_copy_search, returning 0...\n");
    ////////////////////////////
    uprintln("\n --------- THIS IS THE DEFFERED ACTION ---------");
    // copy_search(user_config);
    copy_search();
    uprintln(" --------- FINISHED:   DEFFERED ACTION ---------\n");
    user_config.action_taken = true; user_config.pending_action = INVALID_DEFERRED_TOKEN;
    eeconfig_update_user(user_config.raw);
    print_uconfig();
    println("  --- about to return from _delay_copy_search ---  ");
    return 0;
}
uint32_t _delay_c(uint32_t trigger_time, void *cb_arg) {  //void *cb_arg
    tap_code16(LCTL(KC_C));  // tap_code(KC_C);
    user_config.pending_action = INVALID_DEFERRED_TOKEN;  //user_config.action_taken = true; 
    println(" -- _delay_c: COMPLETED");
    eeconfig_update_user(user_config.raw); return 0;
}
uint32_t _delay_v(uint32_t trigger_time, void *cb_arg) {  //void *cb_arg
    tap_code16(LCTL(KC_V));  // tap_code(KC_V);
    user_config.pending_action = INVALID_DEFERRED_TOKEN;  //user_config.action_taken = true; 
    println(" -- _delay_v: COMPLETED");
    eeconfig_update_user(user_config.raw); return 0;
}
void cancel_pending(void) {
  if (user_config.pending_action != INVALID_DEFERRED_TOKEN) {
    uprintln(" ~ ~ ~ Pending action exists, canceling it!"); 
    cancel_deferred_exec(user_config.pending_action); user_config.pending_action = INVALID_DEFERRED_TOKEN;
    eeconfig_update_user(user_config.raw);
  }
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
    if ( user_config.pending_action != INVALID_DEFERRED_TOKEN && (keycode  == C_KEY || keycode == V_KEY)) {
      extend_deferred_exec(user_config.pending_action, 250); uprintln("   Extending deferred action deadline by 0.2s");
    }  
    // xprintf("");
  } else {
    // print_uconfig();
    // println("");
    // if ( user_config.pending_action == 0 && user_config.action_taken == false && user_config.key_pressed.v == true && user_config.key_pressed.c == true) {
    if ( user_config.pending_action == INVALID_DEFERRED_TOKEN && user_config.action_taken == false && n_pressed() == 3) {
      deferred_token my_token = defer_exec(800, _delay_copy_search, NULL); user_config.pending_action = my_token; log_key('|');
      uprintln(" -- 'COPY_SEARCH' action queued to start in 0.8s..");
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
        // uprintf("char log contents: %s\n", user_config.key_pressed.log);
        user_config.key_pressed.so = true; //eeconfig_update_user(user_config.raw);
        register_code(KC_LCTL); log_key('*');
        println("/------ SO_KEY was pressed ------\\");
        // print_uconfig();
        // SEND_STRING(" sending string SO key \n");
      } else {
        unregister_code(KC_LCTL); 
        user_config.key_pressed.so = false; //eeconfig_update_user(user_config.raw);
        println("\\------ SO_KEY was released ------/\n"); 
      }  // return false; // Skip all further processing of this key
      break;   // return true; // Let QMK send the enter press/release events
    case C_KEY:
      if (record->event.pressed) {
        user_config.key_pressed.c = true; 
        (user_config.key_pressed.so == true)?log_key('C'):log_key('c');
      } else {

        if ( user_config.action_taken == false && user_config.pending_action == INVALID_DEFERRED_TOKEN ) {
          if (user_config.key_pressed.so) {
            deferred_token my_token = defer_exec(100, _delay_c, NULL); user_config.pending_action = my_token; log_key('|'); log_key('C'); println(" -- Deferred 'copy' added");
          } else {
            tap_code(KC_C); // log_key('c'); 
          }
          // (user_config.key_pressed.so == true)?log_key('C'):log_key('c');
        }
        user_config.key_pressed.c = false; //eeconfig_update_user(user_config.raw);
      }; break;   // return true; // Let QMK send the enter press/release events
    case V_KEY:
      if (record->event.pressed) {
        user_config.key_pressed.v = true; //log_key('v'); //eeconfig_update_user(user_config.raw);
        (user_config.key_pressed.so == true)?log_key('V'):log_key('v'); 
      } else {
        if ( user_config.action_taken == false && user_config.pending_action == INVALID_DEFERRED_TOKEN ) {
          if (user_config.key_pressed.so) {
            deferred_token my_token = defer_exec(250, _delay_v, NULL); user_config.pending_action = my_token; log_key('|'); log_key('V'); println(" -- Deferred 'paste' added");
          } else {
            tap_code(KC_V); // log_key('v'); 
          }
          // (user_config.key_pressed.so == true)?log_key('V'):log_key('v'); 
        }
        user_config.key_pressed.v = false; 
      }; break;   // return true; // Let QMK send the enter press/release events
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
    xprintf("");
  } else if (user_config.action_taken == false) {  // On release
    /*  Multi-press definitions   */ 
    char c0 = user_config.key_pressed.log[0];
    char c1 = user_config.key_pressed.log[1];
    char c2 = user_config.key_pressed.log[2]; 
    char c3 = user_config.key_pressed.log[3]; 
    char c4 = user_config.key_pressed.log[4]; 
    // if ((c0 == 'v' || c0 == 'V') && (c1 == 'v' || c1 == 'V') && (c2 == 'v' || c2 == 'V')) {
    // if ((c0 == 'V') && (c1 == 'V') && (c2 == 'V')) {
    if (c4 == 'V' && c3 == 'V' && c2 == 'C' && c1 == 'V' && c0=='C') {  // Code: CTRL+VVCVC
      cancel_pending(); uprintln(" ~ ~ ~ VVCVC combo activated, sending WINDOWS+'L'!"); 
      register_code(KC_RIGHT_GUI);
      // tap_code16(LWIN(KC_L)); user_config.action_taken = true;
      tap_code_delay(KC_L, 50); user_config.action_taken = true;
      unregister_code(KC_RIGHT_GUI);
      // tap_code_delay(KC_RIGHT_GUI, 400);
      // // tap_code(KC_SYSTEM_SLEEP);  // <- makes whole system sleep, undesirable, we want just screen lock
      uprintln(" ~ ~ ~ SENT!\n"); 
      user_config.action_taken = true; log_key('|');
    } else if (c2 == 'C' && c1 == 'C' && c0=='C') { 
      cancel_pending(); uprintln(" ~ ~ ~ Three C-combo activated, sending CTRL+Z!"); 
      tap_code16(LCTL(KC_Z)); user_config.action_taken = true;  // log_key('|');   <- dont 
      uprintln(" ~ ~ ~ SENT!\n"); 
    } 
    if (user_config.action_taken) { eeconfig_update_user(user_config.raw); }
    // if (user_config.action_taken && (user_config.pending_action != INVALID_DEFERRED_TOKEN)) {
    //     uprintln(" ~ ~ ~ Pending action exists, canceling it!"); 
    //     cancel_deferred_exec(user_config.pending_action); user_config.pending_action = INVALID_DEFERRED_TOKEN;
    //     eeconfig_update_user(user_config.raw);
    // }
  }
  /*   Cleanup Code   */
  if ( n_ == 0 && user_config.pending_action == INVALID_DEFERRED_TOKEN ) {
    print_uconfig();
    xprintf("    [No Buttons Pressed; No Pending Actions]\n\n"); // print_uconfig();
    reset_config(); 
  } else if ( n_ == 0 ) {  //  && record->event.pressed == false
      extend_deferred_exec(user_config.pending_action, 5);
      uprintln("Speeding up deferred execution (should fire in 5ms)");
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
