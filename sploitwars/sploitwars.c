#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// non-executable stack, ASLR on - you need your own leak

const char * products[] = {
   "black hoodies", "ROPchainz", "rootkits", "botnets", "0days"
};

const char * locations[] = {
    "k17 basement", "silicon road", "nsa coffee shop", "irc.ruxcon.org.au #9447"
};

int fib[20] = {0};

#define FIB_LIMIT (sizeof(fib) / sizeof(int))
#define PRODUCT_COUNT (sizeof(products) / sizeof(const char *))
#define LOCATION_COUNT (sizeof(locations) / sizeof(const char *))

struct loc {
    int qty[PRODUCT_COUNT];
    double price[PRODUCT_COUNT];
} g_locs[LOCATION_COUNT];

struct player {
    int loc;
    int turn;
    int health;
    char name[256];
    double currency;
    int count[PRODUCT_COUNT];
} g_player;

int random(int min, int max) {
    int range = max - min;
    int divisor = RAND_MAX / (range + 1);
    int retval;
    do {
        retval = rand() / divisor;
    } while (retval > range);
    retval += min;
    return retval;
}

double frandom(double min, double max) {
    double f = (double)rand() / RAND_MAX;
    return min + f * (max - min);
}

int int_cmp(const void *a, const void *b) {
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return *ia  - *ib;
}

char getinput(void) {
    char ch;
    char input = getchar();
    while ((ch = getchar()) && ch != EOF && ch != '\n');
    return input;
}

void getstr(char * buf, int n) {
    char ch;
    char fmt[1024];
    sprintf(fmt, "%%%ds", n);
    scanf(fmt, buf);
    while ((ch = getchar()) && ch != EOF && ch != '\n');
}

int getint(void) {
    int n;
    char ch;
    scanf("%d", &n);
    while ((ch = getchar()) && ch != EOF && ch != '\n');
    return n;
}

int getdigit(void) {
    char ch;
    char input = getchar();
    while ((ch = getchar()) && ch != EOF && ch != '\n');
    return input - '0';
}

int nonfib(void) {
    int n;
    while (1) {
        n = random(fib[0], fib[FIB_LIMIT-1]);
        for (int i = 0; i < FIB_LIMIT; i++) {
            if (n == fib[i]) {
                n = -1;
                break;
            }
        }
        if (n != -1) {
            break;
        }
    }
    return n;
}

void intro(void) {
    printf(":: Welcome to [sploitwarz] v1.0\n\n");
    printf(">> The shady 0day dealer stands over you... <<\n");
    printf("She says, \"You think you can pwn binaries on my turf?\"\n");
    printf("Her fist sinks into your gut, it hurts. \"You owe me 9447449 respect!\"\n");
    printf("\"Thirty days rookie, you better be ready.\"\n\n");
    printf(">> You forget how you even got here, but one thing is clear.\n");
    printf("Before three days is up, you better have obtained enough respect!\n\n");
}

void init(void) {
    srand(time(NULL));
    g_player.loc = 0;
    g_player.health = 10;
    g_player.currency = 1337.0;
    g_player.count[0] = 1;
    g_player.turn = 0;
    for (int i = 0; i < PRODUCT_COUNT; i++) {
        g_player.count[i] = 0;
    }
    for (int i = 0; i < LOCATION_COUNT; i++) {
        for (int j = 0; j < PRODUCT_COUNT; j++) {
            int min = (j*j*j*j)*1337;
            int max = (j+1)*(j+1)*(j+1)*(j+1) * 1337;
            g_locs[i].price[j] = frandom(min, max);
            g_locs[i].qty[j] = random(0, 1337);
        }
    }

    int a = 0;
    int b = 1;
    int x = 0;
    for (int i = 1; i < FIB_LIMIT; i++) {
        x = b;
        b = a + b;
        a = x;
        fib[i] = b;
    }
}

void do_status(struct player * p) {
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf(":: DAY %d\n", p->turn);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Location: %s\n", locations[p->loc]);
    printf("Respect: %.2f\n\n", p->currency);
    for (int i = 0; i < PRODUCT_COUNT; i++) {
        printf("(%d) %s\n", p->count[i], products[i]);
    }
    printf("\n");
    printf("%-32s %4s %11s\n", "Product", "Qty", "Price");
    printf("=================================================\n");
    for (int i = 0; i < PRODUCT_COUNT; i++) {
        printf("%-32s %4d %11.2f\n", products[i],
                g_locs[p->loc].qty[i],
                g_locs[p->loc].price[i]);
    }
    printf("\n");
    printf("> OPTIONS:\n\n");
    printf(" (t)ravel\n");
    printf(" (b)uy\n");
    printf(" (s)ell\n");
    printf(" (g)amble\n");
    printf(" (c)hange name\n");
    printf(" (q)uit\n\n");
    printf("What will you do? ");
}

void do_travel(struct player * p) {
    printf("Select destination:\n");
    for (int i = 0; i < LOCATION_COUNT; i++) {
        printf("%d) %s\n", i+1, locations[i]);
    }
    int dst = -1;
    while (dst < 0 || dst >= LOCATION_COUNT) {
        printf("> ");
        dst = getdigit() - 1;
    }
    if (dst != p->loc) p->turn++;
    p->loc = dst;
}

void do_buy(struct player * p) {
    printf("Select product:\n");
    for (int i = 0; i < PRODUCT_COUNT; i++) {
        printf("%d) %s\n", i+1, products[i]);
    }
    int choice = -1;
    while (choice < 0 || choice >= PRODUCT_COUNT) {
        printf("> ");
        choice = getdigit() - 1;
    }
    double price = g_locs[p->loc].price[choice];
    int qty = g_locs[p->loc].qty[choice];
    int max = p->currency / price;
    int maxqty = max < qty ? max : qty;
    qty = -1;
    while (qty < 0 || qty > maxqty) {
        printf("Choose QTY (max %d): ", maxqty);
        qty = getint();
    }
    struct loc * loc = &g_locs[p->loc];
    p->currency -= qty * price;
    p->count[choice] += qty;
    loc->qty[choice] -= qty;
    for (int i = 0; i < qty; i++) {
        loc->price[choice] *= 1.01;
    }
}

void do_sell(struct player * p) {
    printf("Select product:\n");
    for (int i = 0; i < PRODUCT_COUNT; i++) {
        printf("%d) %s\n", i+1, products[i]);
    }
    int choice = -1;
    while (choice < 0 || choice >= PRODUCT_COUNT) {
        printf("> ");
        choice = getdigit() - 1;
    }
    int qty = -1;
    while (qty < 0 || qty > p->count[choice]) {
        printf("Choose QTY (max %d): ", p->count[choice]);
        qty = getint();
    }
    struct loc * loc = &g_locs[p->loc];
    p->currency += qty * loc->price[choice];
    p->count[choice] -= qty;
    loc->qty[choice] += qty;
    for (int i = 0; i < qty; i++) {
        loc->price[choice] *= 0.99;
    }
}

void do_gamble(struct player * p) {
    printf("How much respect do you wish to gamble? (max %.0f): ", p->currency);
    int pot = 0;
    while (pot < 0 || pot > (int)(p->currency)) {
        pot = getint();
    }
    printf("\n");
    printf("*** GAMBLE ***\nPick the odd one out:\n");
    int nums[5];
    int wrong = nonfib();
    nums[0] = wrong;
    for (int i = 1; i < 5; i++) {
        nums[i] = fib[random(0, FIB_LIMIT)];
    }
    qsort(nums, 5, sizeof(int), int_cmp);
    for (int i = 0; i < 5; i++) {
        printf("%d) %d\n", i+1, nums[i]);
    }
    int choice = -1;
    while (choice < 0 || choice >= 5) {
        choice = getdigit() - 1;
    }
    printf("\n");
    if (nums[choice] == wrong) {
        printf("Well done, ");
        printf(p->name);
        printf("! You win %d respect.\n", pot);
        p->currency += pot;
    } else {
        printf("Wrong! You lose %d respect.\n", pot);
        p->currency -= pot;
    }
}

int gameloop(void) {
    do_status(&g_player);
    char action = getinput();
    printf("\n");
    switch (action) {
        case 'q': return 0;
        case 't':
            do_travel(&g_player);
            break;
        case 'b':
            do_buy(&g_player);
            break;
        case 's':
            do_sell(&g_player);
            break;
        case 'g':
            do_gamble(&g_player);
            break;
        case 'c':
            printf("What is your name? ");
	    fflush(stdout);
            getstr(g_player.name, 255);
            break;
    }
    printf("\n");

    if (g_player.turn == 30) {
        printf(">> The shady 0day dealer returns! <<\n");
        printf("She says, \"Did you gain enough respect to satisfy me?\"\n\n");
        if (g_player.currency > 9447449) {
            printf("\"Yes, I can see it in your eyes. I embrace you as one of us.\"\n\n");
            printf("-- YOU WIN --\n");
        } else {
            printf("\"No, you disgust me script kiddie. I banish thee to 4chan.\"\n\n");
            printf("-- YOU LOSE --\n");
        }
        return 0;
    }

    return 1;
}

int main(int argc, char ** argv) {
    setbuf(stdout, NULL);
    intro();
    init();
    printf("What is your name? ");
    getstr(g_player.name, 255);
    while (gameloop());
    return 0;
}
