/**
  You can run this on a socket like so:
    netcat -vnltp 4141 -s 127.0.0.1 -e ./challenge

  This will listen on 127.0.0.1:4141

  See this sockets tutorial: https://www.youtube.com/watch?v=dQw4w9WgXcQ
*/


#define _BSD_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define WELCOME  \
    "Welcome to CSE Lego Revue Totally Legit Ticketing System dot Com\n"  \
    "       It's heaps good: you'll shit 4x2 Lego bricks!!1\n"  \

#define MENU  \
    "\nChoose an option:\n"  \
    " 1. Print the seat map\n"  \
    " 2. Buy ticket\n"  \
    " 3. Print a pun\n"  \
    " 7. Log in as admin\n"  \
    " 8. Alter a booking\n"  \
    " 9. Get a refund\n"  \
    " 0. Quit\n"  \
    "> "

#define N 6

bool is_admin;

typedef struct __node node_t;
struct __node {
  node_t *prev, *next;
  size_t sz;
  char data[];
};

typedef struct __booking booking_t;
struct __booking {
  char *name;
  int row;
  int col;
  booking_t *next;
};

node_t *all_blocks;

booking_t *bookings_list;

bool seat_taken[N][N];

void moar_memory(void) {
  node_t *mem = (void*)sbrk(4096);
  mem->prev = NULL;
  mem->next = all_blocks;
  mem->sz = 4096 - sizeof(*mem);
  all_blocks = mem;
}

void __attribute__((constructor)) init_first_heap_block(void) {
  all_blocks = (node_t*)sbrk(4096);
  all_blocks->prev = NULL;
  all_blocks->next = all_blocks + 1;
  all_blocks->sz = sizeof(node_t);

  node_t *next = all_blocks->next;
  next->prev = all_blocks;
  next->next = NULL;
  next->sz = 4096 - sizeof(node_t);
}

void *allocate(size_t sz) {
  // Round up |sz| to a multiple of node size, and add the header size.
  if (sz % sizeof(node_t)) {
    sz += sizeof(node_t) - sz % sizeof(node_t);
  }
  sz += sizeof(node_t);

  // We can't allocate too-large blocks.
  assert(sz < 4000 && "you are doing it wrong!");

  // Find first large-enough free block.
  node_t *ptr = all_blocks;
  while (ptr && (ptr->sz < sz || (ptr->sz & 1))) {
    ptr = ptr->next;
  }

  // If failed, allocate some more mem and try again.
  if (!ptr) {
    moar_memory();
    return allocate(sz);
  }

  // Only split up the block if it is large enough.
  if (ptr->sz >= 4 * sizeof(node_t)) {
    node_t *new = (node_t*)((char*)ptr + sz);
    node_t *next = ptr->next;

    new->prev = ptr;
    new->next = next;
    new->sz = ptr->sz - sz;

    if (next) next->prev = new;
    ptr->next = new;
    ptr->sz = sz;
  }

  ptr->sz |= 1;  // Mark block as used.
  return ptr->data;
}

void deallocate(void* data) {
  node_t *ptr = data;
  ptr--;

  node_t *prev = ptr->prev;
  node_t *next = ptr->next;

  if (prev) prev->next = next;
  if (next) next->prev = prev;

  ptr->next = all_blocks->next;
  if (all_blocks->next)
    all_blocks->next->prev = ptr;
  all_blocks->next = ptr;

  ptr->sz &= ~1;
}


// Safe I/O primitives.
char* read_line_specified(char *buffer, size_t count) {
  assert(fgets(buffer, count, stdin));
  char *ptr = strchr(buffer, '\n');
  if (ptr) *ptr = 0;
  return buffer;
}

char* read_line(void) {
  static char buffer[100];
  return read_line_specified(buffer, sizeof(buffer));
}

int read_int(void) {
  return strtol(read_line(), NULL, 10);
}

size_t read_size(void) {
  return strtoul(read_line(), NULL, 10);
}

bool check_admin(void) {
  if (!is_admin) {
    puts("NOT ADMIN. HACK ATTEMPT DETECTED. DROPPING PRIVS. http://imgur.com/HwPyWcr");
    return false;
  }
  return true;
}

booking_t* find_booking(void) {
  printf("Enter name\n > ");
  char * const name = read_line();
  for (booking_t *ptr = bookings_list; ptr; ptr = ptr->next)
    if (!strcmp(ptr->name, name))
      return ptr;
  puts("Could not find a booking with this name");
  return NULL;
}

void handle_print(void) {
  for (int i = 0; i < N; i++) {
    printf("Row %c: ", 'A' + i);
    for (int k = 0; k < N; k++)
      printf("[%c] ", seat_taken[i][k] ? 'x' : ' ');
    printf("\n");
  }

  for (booking_t *ptr = bookings_list; ptr; ptr = ptr->next) {
    printf("Booking by [%s] at [%c%d]\n", ptr->name, ptr->row + 'A', ptr->col);
  }
}

void handle_buy(void) {
  printf("Enter your name\n > ");
  char * const tmp_buf = read_line();

  // tmp_buf is the shared input buffer (static char[] above), so we need to
  // copy it -- otherwise next read will clobber its contents.

  char * const name = allocate(strlen(tmp_buf) + 1);
  strcpy(name, tmp_buf);


  printf("Which seat? Enter your preference (e.g. A1)\n > ");
  char * const buf_seat = read_line();
  int row = buf_seat[0] - 'A';
  int col = buf_seat[1] - '0';

  if (row >= N || col >= N) {
    printf("HACKING ALERT !1!! ASSASSINS DEPLOYED. ONE STACK COOKIE USED AS PAYMENT.\n");
    return;
  }

  if (seat_taken[row][col]) {
    printf("Dude, someone's sitting there, not cool\n");
    return;
  }

  seat_taken[row][col] = true;

  booking_t *booking = allocate(sizeof(*booking));
  memset(booking, 0, sizeof(*booking));
  booking->name = name;
  booking->row = row;
  booking->col = col;
  booking->next = bookings_list;
  bookings_list = booking;
}

void handle_pun(void) {
  puts("I can't handle this shit");
}

void handle_login(void) {
  puts("Log on, log off. Wax on, wax off.");

  while (1) {
    printf("[sudo] password for %s: ", getenv("USER"));
    read_line();
    sleep(1);
    puts("Sorry, try again");
  }
}

void handle_alter(void) {
  if (!check_admin()) return;
  booking_t *booking = find_booking();
  if (!booking)
    return;

  printf("New seat\n > ");
  char * const buf_seat = read_line();
  int row = buf_seat[0] - 'A';
  int col = buf_seat[1] - '0';
  if (row >= N || col >= N) {
    puts("Just stop trying.");
    return;
  }

  if (seat_taken[row][col]) {
    puts("Seat already taken! Jerk.");
    return;
  }

  printf("New name\n > ");
  char *old = booking->name, *new = read_line();
  while (*new)
    *(old++) = *(new++);

  booking->row = row;
  booking->col = col;
}

void handle_refund(void) {
  if (!check_admin()) return;
  booking_t *booking = find_booking();
  if (!booking)
    return;

  puts("What a genital.");

  if (booking == bookings_list) {
    bookings_list = bookings_list->next;
    deallocate(booking->name);
    deallocate(booking);
  } else {
    for (booking_t *ptr = bookings_list; ptr; ptr = ptr->next) {
      if (ptr->next == booking) {
        ptr->next = ptr->next->next;
        deallocate(booking->name);
        deallocate(booking);
      }
    }
  }
}


int main() {
  // Setup
  setbuf(stdout, NULL);

  puts(WELCOME);

  while (1) {
    printf(MENU);

    switch (read_int()) {
      case 0: return 0;
      case 1: handle_print(); break;
      case 2: handle_buy(); break;
      case 3: handle_pun(); break;

      case 7: handle_login(); break;
      case 8: handle_alter(); break;
      case 9: handle_refund(); break;

      default:
        printf("Sorry Dave, I'm afraid I can't do that\n");
    }
  }

  return 0;
}
